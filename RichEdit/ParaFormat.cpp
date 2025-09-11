#include "RichEdit.h"

namespace drx {

struct ParaFormatDlg : public WithParaFormatLayout<TopWindow> {
	ParaFormatting para;

	ParaFormatDlg() {
		CtrlLayoutOKCancel(*this, t_("Формат параграфа"));
		ActiveFocus(para.before);
	}
};

void RichEdit::ParaFormat()
{
	ParaFormatDlg d;
	d.para.Set(unit, formatinfo, !IsSelection() && cursorp.level == 0);
	if(d.Execute() != IDOK || !d.para.IsChanged())
		return;
	dword v = d.para.Get(formatinfo);
	if(v) ApplyFormat(0, v);
}

struct sCompareLess {
	bool operator()(const Val& a, const Val& b) const {
		return CompareNoCase(Txt(a), Txt(b)) < 0;
	}
};

void RichEdit::ReadStyles()
{
	i32 i;
	style.Clear();
	Vec<Uuid> id;
	Vec<Txt> name;
	for(i = 0; i < text.GetStyleCount(); i++) {
		id.Add(text.GetStyleId(i));
		name.Add(text.GetStyle(i).name);
	}
	IndexSort(name, id, sCompareLess());
	for(i = 0; i < id.GetCount(); i++)
		style.Add(id[i], name[i]);
}

i32 RichEdit::CompareStyle(const Val& a, const Val& b)
{
	return CompareNoCase(Txt(a), Txt(b));
}

void RichEdit::SetStyle()
{
	if(!IsSelection()) {
		NextUndo();
		WithSetStyleLayout<TopWindow> d;
		CtrlLayoutOKCancel(d, t_("Установить стиль"));
		d.newstyle <<= d.Breaker(IDYES);
		d.style.AddKey();
		d.style.AddColumn();
		d.style.NoHeader().NoGrid();
		for(i32 i = 0; i < text.GetStyleCount(); i++)
			d.style.Add(text.GetStyleId(i), text.GetStyle(i).name);
		d.style.Sort(1, CompareStyle);
		i32 q = d.style.Find(RichStyle::GetDefaultId());
		if(q >= 0)
			d.style.SetDisplay(q, 0, Single<DisplayDefault>());
		d.style.FindSetCursor(formatinfo.styleid);
		RichStyle cs;
		cs.format = formatinfo;
		cs.format.sscript = 0;
		cs.format.link.Clear();
		cs.format.indexentry.Clear();
		cs.format.language = LNG_RUSSIAN;
		cs.format.label.Clear();

		Uuid id;
		switch(d.Run()) {
		case IDCANCEL:
			return;
		case IDOK:
			if(d.style.IsCursor()) {
				id = d.style.GetKey();
				const RichStyle& st = text.GetStyle(id);
				cs.name = st.name;
				cs.next = st.next;
				SaveStyleUndo(id);
				break;
			}
			return;
		case IDYES:
			Txt newname;
			if(EditText(newname, Format(t_("Новый стиль ном. %d"), text.GetStyleCount()),
			            "Имя", CharFilterAscii128)) {
				cs.name = newname;
				id = Uuid::Create();
				cs.next = id;
				SaveStylesUndo();
				break;
			}
			return;
		}
		text.SetStyle(id, cs);
		ReadStyles();
		formatinfo.styleid = id;
		SaveFormat(GetCursor(), 0);
		text.ReStyle(GetCursor(), id);
		Finish();
	}
}

void RichEdit::Styles()
{
	NextUndo();
	StyleManager s;
	s.Setup(ffs, unit);
	s.Set(text, formatinfo.styleid);
	if(s.Execute() != IDOK || !s.IsChanged())
		return;
	SaveStylesUndo();
	SetModify();
	s.Get(text);
	ReadStyles();
	Finish();
}

void RichEdit::ApplyStylesheet(const RichText& r)
{
	NextUndo();
	SaveStylesUndo();
	text.OverrideStyles(r.GetStyles(), false, false);
	ReadStyles();
	Finish();
}

}