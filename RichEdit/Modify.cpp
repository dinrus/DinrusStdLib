#include "RichEdit.h"

namespace drx {


void RichEdit::PasteFilter(RichText& txt, const Txt&) { Filter(txt); }
void RichEdit::Filter(RichText& txt) {}

void BegSelFixRaw(RichText& text)
{
	RichPos p = text.GetRichPos(0, 1);
	ASSERT(p.table == 1);
	if(p.table != 1)
		return;
	RichPara::Format fmt;
	text.InsertParaSpecial(1, true, fmt);
}

void BegSelUnFixRaw(RichText& text)
{
	ASSERT(text.GetLength() > 0);
	RichPos p = text.GetRichPos(1, 1);
	ASSERT(p.table == 1);
	if(p.table != 1)
		return;
	text.RemoveParaSpecial(1, true);
}

void RichEdit::UndoBegSelFix::Apply(RichText& txt)
{
	BegSelUnFixRaw(txt);
}

One<RichEdit::UndoRec> RichEdit::UndoBegSelFix::GetRedo(const RichText& txt)
{
	return MakeOne<RichEdit::UndoBegSelUnFix>();
}

void RichEdit::UndoBegSelUnFix::Apply(RichText& text)
{
	BegSelFixRaw(text);
}

One<RichEdit::UndoRec> RichEdit::UndoBegSelUnFix::GetRedo(const RichText& txt)
{
	return MakeOne<RichEdit::UndoBegSelFix>();
}

bool RichEdit::BegSelTabFix(i32& count)
{
	if(begtabsel) { // If selection starts with first table which is the first element in the text
		i32 c = cursor;
		AddUndo(MakeOne<UndoBegSelFix>());
		BegSelFixRaw(text); // adds an empty paragraph at the start
		Move(0);
		Move(c + 1, true); // and changes the selection
		count++;
		begtabsel = false;
		return true;
	}
	return false;
}

void RichEdit::BegSelTabFixEnd(bool fix)
{ // removes empty paragraph added by BegSelTabFix
	if(fix && GetLength() > 0) {
		i32 c = cursor;
		AddUndo(MakeOne<UndoBegSelUnFix>());
		BegSelUnFixRaw(text);
		Move(0);
		Move(c - 1, true);
		begtabsel = true;
	}
}

bool RichEdit::InvalidRange(i32 l, i32 h)
{
	return !InSameTxt(text.GetRichPos(min(l, h)), text.GetRichPos(max(l, h)));
}

void RichEdit::AddUndo(One<UndoRec>&& ur)
{
	redo.Clear();
	SetModify();
	modified = true;
	incundoserial = true;
	while(undo.GetCount() > undosteps)
		undo.DropHead();
	found = false;
	ur->cursor = cursor;
	ur->serial = undoserial;
	undo.AddTail(pick(ur));
}

void RichEdit::SaveStylesUndo()
{
	AddUndo(MakeOne<UndoStyles>(text));
}

void RichEdit::SaveStyleUndo(const Uuid& id)
{
	AddUndo(MakeOne<UndoStyle>(text, id));
}

void RichEdit::SaveFormat(i32 pos, i32 count)
{
	Limit(pos, count);
	AddUndo(MakeOne<UndoFormat>(text, pos, count));
}

void RichEdit::SaveFormat()
{
	i32 pos, count;
	if(IsSelection()) {
		if(tablesel) {
			SaveTable(tablesel);
			return;
		}
		pos = min(cursor, anchor);
		count = abs(cursor - anchor);
	}
	else {
		pos = cursor;
		count = 0;
	}
	bool b = BegSelTabFix(count);
	SaveFormat(pos, count);
	BegSelTabFixEnd(b);
}

void RichEdit::Limit(i32& pos, i32& count)
{
	i32 h = pos + count;
	pos = min(GetLength(), pos);
	count = min(GetLength(), h) - pos;
}

void RichEdit::ModifyFormat(i32 pos, const RichText::FormatInfo& fi, i32 count)
{
	if(IsReadOnly())
		return;
	bool b = BegSelTabFix(count);
	Limit(pos, count);
	SaveFormat(pos, count);
	text.ApplyFormatInfo(pos, fi, count);
	BegSelTabFixEnd(b);
}

void RichEdit::Remove(i32 pos, i32 len, bool forward)
{
	if(IsReadOnly())
		return;
	Limit(pos, len);
	if(InvalidRange(pos, pos + len))
		return;
	RichTxt::FormatInfo fi;
	if(forward)
		fi = text.GetFormatInfo(pos, 0);
	AddUndo(MakeOne<UndoRemove>(text, pos, len));
	text.Remove(pos, len);
	if(forward) {
		SaveFormat(pos, 0);
		text.ReplaceStyle(pos, fi.styleid);
		fi.paravalid &= ~RichText::STYLE;
		text.ApplyFormatInfo(pos, fi, 0);
	}
	SetModify();
	modified = true;
}

void RichEdit::Insert(i32 pos, const RichText& txt, bool typing)
{
	if(IsReadOnly())
		return;
	Index<i32> lng;
	for(i32 i = 0; i < language.GetCount(); i++)
		lng.Add(language.GetKey(i));
	Vec<i32> lngn = txt.GetAllLanguages();
	for(i32 i = 0; i < lngn.GetCount(); i++)
		lng.FindAdd(lngn[i]);
	SetupLanguage(lng.PickKeys());
	i32 l = text.GetLength();
	text.Insert(pos, txt);
	l = text.GetLength() - l;
	SetModify();
	modified = true;
	if(undo.GetCount()) {
		UndoRec& u = undo.Tail();
		if(typing) {
			UndoInsert *ui = dynamic_cast<UndoInsert *>(&u);
			if(ui && ui->length > 0 && ui->typing && ui->pos + ui->length == pos) {
				ui->length += l;
				return;
			}
		}
	}
	AddUndo(MakeOne<UndoInsert>(pos, l, typing));
}

void RichEdit::Undo()
{
	if(IsReadOnly())
		return;
	if(undo.IsEmpty()) return;
	CancelSelection();
	i32 serial = undo.Tail().serial;
	i32 c = cursor;
	while(undo.GetCount()) {
		UndoRec& u = undo.Tail();
		if(u.serial != serial) break;
		One<UndoRec> r = u.GetRedo(text);
		r->serial = u.serial;
		r->cursor = cursor;
		redo.Add(pick(r));
		u.Apply(text);
		c = u.cursor;
		undo.DropTail();
		modified = true;
	}
	ReadStyles();
	Move(c);	
}

void RichEdit::Redo()
{
	if(IsReadOnly())
		return;
	if(redo.IsEmpty()) return;
	NextUndo();
	CancelSelection();
	i32 serial = redo.Top().serial;
	i32 c = cursor;
	while(redo.GetCount()) {
		UndoRec& r = redo.Top();
		if(r.serial != serial) break;
		One<UndoRec> u = r.GetRedo(text);
		u->serial = r.serial;
		u->cursor = cursor;
		undo.AddTail(pick(u));
		r.Apply(text);
		c = r.cursor;
		redo.Drop();
		modified = true;
	}
	ReadStyles();
	Move(c);
}

#ifdef TARGET_WIN32
#define RTFS "Rich Text Format"
#else
#define RTFS "text/richtext"
#endif

RichText RichEdit::GetSelection(i32 maxcount) const
{
	RichText clip;
	if(tablesel) {
		RichTable tab = text.CopyTable(tablesel, cells);
		clip.SetStyles(text.GetStyles());
		clip.CatPick(pick(tab));
	}
	else {
		if(begtabsel) {
			i32 pos = 0;
			RichPos p = text.GetRichPos(0, 1);
			if(p.table) {
				clip.SetStyles(text.GetStyles());
				do {
					RichTable tab = text.CopyTable(p.table);
					clip.CatPick(pick(tab));
					pos += p.tablen + 1;
					p = text.GetRichPos(pos, 1);
				}
				while(p.table);
				clip.CatPick(text.Copy(pos, minmax(abs(cursor - pos), 0, maxcount)));
			}
		}
		else
			clip = text.Copy(min(cursor, anchor), min(maxcount, abs(cursor - anchor)));
	}
	return clip;
}

void RichEdit::Cut()
{
	if(IsReadOnly())
		return;
	Copy();
	if(IsSelection())
		RemoveSelection();
	else
	if(objectpos >= 0) {
		Remove(cursor, 1);
		Move(cursor, false);
	}
}

void RichEdit::PasteText(const RichText& text)
{
	SetModify();
	modified = true;
	RemoveSelection();
	i32 n = text.GetPartCount() - 1;
	if(!text.IsPara(0) || !text.IsPara(n)) { // inserted section must start/end with para
		RichText pp = clone(text);
		pp.Normalize();
		Insert(cursor, pp, false);
		ReadStyles();
		Move(cursor + pp.GetLength(), false);
	}
	else {
		Insert(cursor, text, false);
		ReadStyles();
		Move(cursor + text.GetLength(), false);
	}
}

struct ToParaIterator : RichText::Iterator {
	RichPara para;
	bool     space;

	virtual bool operator()(i32 pos, const RichPara& p) {
		for(i32 i = 0; i < p.GetCount(); i++) {
			const RichPara::Part& part = p[i];
			if(part.IsText()) {
				wtukk s = part.text;
				while(*s) {
					while(*s && *s <= ' ') {
						space = true;
						s++;
					}
					wtukk t = s;
					while(*s > ' ') s++;
					if(s > t) {
						if(space)
							para.Cat(" ", part.format);
						para.Cat(WTxt(t, s), part.format);
						space = false;
					}
				}
			}
			else if(!part.field.IsNull()) {
				para.Cat(part.field, part.fieldparam, part.format);
				space = false;
			}
			else if(part.object) {
				para.Cat(part.object, part.format);
				space = false;
			}
		}
		space = true;
		return false;
	}

	ToParaIterator() { space = false; }
};

void RichEdit::ToPara()
{
	if(IsReadOnly())
		return;
	if(!IsSelection() || tablesel)
		return;
	NextUndo();
	RichText txt = text.Copy(min(cursor, anchor), abs(cursor - anchor));
	ToParaIterator it;
	txt.Iterate(it);
	RichText h;
	h.SetStyles(txt.GetStyles());
	h.Cat(it.para);
	PasteText(h);
}

void RichEdit::RemoveText(i32 count)
{
	CancelSelection();
	Remove(cursor, count);
	Finish();
}

RichText RichEdit::CopyText(i32 pos, i32 count) const
{
	return text.Copy(pos, count);
}

void RichEdit::InsertObject(i32 type)
{
	RichObjectType& richtype = RichObj::GetType(type);
	RichObj object = RichObj(&richtype, Val());
	RichObj o = object;
	o.DefaultAction(context);
	if(o.GetSerialId() != object.GetSerialId()) {
		RichText::FormatInfo finfo = GetFormatInfo();
		RemoveSelection();
		RichPara p;
		p.Cat(o, finfo);
		RichText clip;
		clip.Cat(p);
		Insert(GetCursor(), clip, false);
		Finish();
	}
}

void RichEdit::ReplaceObject(const RichObj& obj)
{
	Remove(objectpos, 1);
	RichPara p;
	p.Cat(obj, formatinfo);
	RichText clip;
	clip.Cat(p);
	Insert(objectpos, clip, false);
	Finish();
	objectrect = GetObjectRect(objectpos);
}

RichObj RichEdit::GetObject() const
{
	return text.GetRichPos(objectpos).object;
}

void RichEdit::Select(i32 pos, i32 count)
{
	found = false;
	Move(pos);
	Move(pos + count, true);
}

void RichEdit::InsertLine()
{
	if(IsReadOnly())
		return;
	RichText::FormatInfo b = formatinfo;
	RichText h;
	h.SetStyles(text.GetStyles());
	RichPara p;
	p.format = formatinfo;
	h.Cat(p);
	h.Cat(p);
	bool st = cursorp.paralen == cursorp.posinpara;
	bool f = cursorp.posinpara == 0 && formatinfo.label.GetCount();
	Insert(cursor, h, false);
	if(f) {
		Txt lbl = formatinfo.label;
		formatinfo.label.Clear();
		ApplyFormat(0, RichText::LABEL);
		formatinfo.label = lbl;
	}
	anchor = cursor = cursor + 1;
	begtabsel = false;
	formatinfo.firstonpage = formatinfo.newpage = formatinfo.newhdrftr = false;
	if(st) {
		Uuid next = text.GetStyle(b.styleid).next;
		if(next != formatinfo.styleid) {
			formatinfo.label.Clear();
			formatinfo.styleid = next;
			ApplyFormat(0, RichText::STYLE|RichText::NEWPAGE|RichText::LABEL|RichText::NEWHDRFTR);
			return;
		}
	}
	ApplyFormat(0, RichText::NEWPAGE|RichText::LABEL|RichText::NEWHDRFTR);
	objectpos = -1;
}

}
