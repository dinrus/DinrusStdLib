#include "CtrlLib.h"

namespace drx {

CH_VALUE(ViewEdge, CtrlsImg::VE());

Val EditFieldEdge()
{
	return EditField::StyleDefault().edge[0];
}

CtrlFrame& EditFieldFrame()
{
	static LookFrame m(EditFieldEdge);
	return m;
}

CtrlFrame& ViewFrame()
{
	static LookFrame m(ViewEdge);
	return m;
}

bool IsWCh(i32 c)
{
	return IsLeNum(c) || c == '_';
}

bool TextArrayOps::GetWordSelection(z64 c, z64& l, z64& h)
{
	if(IsWCh(GetCharAt(c))) {
		l = h = c;
		while(l > 0 && IsWCh(GetCharAt(l - 1)))
			l--;
		while(h < GetTotal() && IsWCh(GetCharAt(h)))
			h++;
		if(h != c)
			while(h < GetTotal() && GetCharAt(h) == ' ')
				h++;
		return true;
	}
	return false;
}

z64 TextArrayOps::GetNextWord(z64 cursor)
{
	bool a = IsWCh(GetCharAt(cursor));
	i32 n = 0;
	z64 c = cursor;
	while(c <= GetTotal() && IsWCh(GetCharAt(c)) == a) {
		if(++n > 10000) return cursor;
		c++;
	}
	return c;
}

z64 TextArrayOps::GetPrevWord(z64 cursor)
{
	i32 n = 0;
	z64 c = cursor;
	if(c == 0) return 0;
	bool a = IsWCh(GetCharAt(c - 1));
	while(c > 0 && IsWCh(GetCharAt(c - 1)) == a) {
		if(++n > 10000) return cursor;
		c--;
	}
	return c;
}

void LookFrame::FrameLayout(Rect& r)
{
	Rect m = LookMargins(r, Get());
	r.left += m.left;
	r.right -= m.right;
	r.top += m.top;
	r.bottom -= m.bottom;
}

void LookFrame::FramePaint(Draw& w, const Rect& r)
{
	ChPaintEdge(w, r, Get());
}

void LookFrame::FrameAddSize(Size& sz)
{
	Rect m = ChMargins(Get());
	sz.cx += m.left + m.right;
	sz.cy += m.top + m.bottom;
}

CH_STYLE(EditField, Style, StyleDefault)
{
	paper = SColorPaper();
	disabled = SColorFace();
	focus = paper;
	invalid = Blend(paper, Color(255, 0, 0), 32);
	text = SColorText();
	textdisabled = SColorDisabled();
	selected = SColorHighlight();
	selectedtext = SColorHighlightText();
	selected0 = SColorDkShadow();
	selectedtext0 = SColorHighlightText();
	for(i32 i = 0; i < 4; i++)
		edge[i] = CtrlsImg::EFE();
	activeedge = false;
	vfm = 2;
	coloredge = Null;
}

bool EditField::FrameIsEdge()
{
	return &GetFrame() == &edge;
}

void EditField::RefreshAll()
{
	Color paper = GetPaper();
	edge.SetColor(style->coloredge, paper);
	WhenPaper(paper);
	RefreshFrame();
}

void EditField::MouseEnter(Point p, dword keyflags)
{
	edge.Mouse(true);
	RefreshAll();
}

void EditField::MouseLeave()
{
	edge.Mouse(false);
	RefreshAll();
}

EditField& EditField::SetStyle(const Style& s)
{
	style = &s;
	edge.Set(this, style->edge, style->activeedge);
	RefreshLayout();
	RefreshAll();
	return *this;
}

void EditField::CancelMode()
{
	keep_selection = false;
	selclick = false;
	dropcaret.Clear();
}

i32 EditField::GetTextCx(wtukk txt, i32 n, bool password, Font fnt) const
{
	if(password)
		return n * font['*'];
	wtukk s = txt;
	i32 x = 0;
	while(n--)
		x += GetCharWidth(*s++);
	return x;
}

i32  EditField::GetCaret(i32 cursor) const
{
	return GetTextCx(text, cursor, password, font);
}

i32  EditField::GetViewHeight(Font font)
{
	Size sz(0, 0);
	EditFieldFrame().FrameAddSize(sz);
	return font.GetCy() + (sz.cy <= 2 ? 4 : sz.cy <= 4 ? 2 : 0);
}

i32  EditField::GetStdHeight(Font font)
{
	Size sz = Size(10, GetViewHeight());
	EditFieldFrame().FrameAddSize(sz);
	return sz.cy;
}

Size EditField::GetMinSize() const
{
	return AddFrameSize(10 + GetSpaceLeft() + GetSpaceRight(), font.GetCy() + (no_internal_margin ? 0 : 4));
}

void EditField::PaintSpace(Draw& w)
{
}

i32 EditField::GetSpaceLeft() const
{
	return 0;
}

i32 EditField::GetSpaceRight() const
{
	return 0;
}

void EditField::EditCapture()
{
}

bool EditField::HasEditCapture()
{
	return HasCapture();
}

i32  EditField::GetCursor(i32 posx)
{
	posx -= GetSpaceLeft();
	if(!no_internal_margin)
		posx -= 2;
	if(posx <= 0) return 0;

	i32 count = text.GetLength();
	if(password)
		return min((posx + font['*'] / 2) / font['*'], count);

	i32 x = 0;
	wtukk s = text;
	i32 i = 0;
	while(i < count) {
		i32 witdh = GetCharWidth(*s);
		if(posx < x + witdh / 2)
			break;
		x += witdh;
		s++; i++;
	}
	return i;
}

Image EditField::CursorImage(Point, dword)
{
	return Image::IBeam();
}

i32  EditField::GetTy() const
{
	return (GetSize().cy + 1 - font.GetCy()) / 2;
}

void EditField::HighlightText(Vec<Highlight>& hl)
{
	WhenHighlight(hl);
}

void EditField::Paints(Draw& w, i32& x, i32 fcy, wtukk &txt,
					   Color ink, Color paper, i32 n, bool password, Font fnt, Color underline,
					   bool showspaces)
{
	if(n < 0) return;
	i32 cx = GetTextCx(txt, n, password, font);
	w.DrawRect(x, 0, cx, fcy, paper);
	if(password) {
		Txt h;
		h.Cat('*', n);
		w.DrawText(x, 0, ~h, fnt, ink, n);
	}
	else {
		wtukk txts = txt;
		Buffer<wchar> h;
		wtukk e = txt + n;
		for(wtukk q = txt; q < e; q++)
			if(*q < 32) {
				h.Alloc(n);
				wtuk t = ~h;
				for(wtukk q = txt; q < e; q++)
					*t++ = *q < 32 ? LowChar(*q) : *q;
				txts = ~h;
			}
		if(!IsNull(underline))
			w.DrawRect(x, fnt.GetAscent() + 1, cx, 1, underline);
		w.DrawText(x, 0, txts, fnt, ink, n);
		if(showspaces) {
			i32 xx = x;
			Size sz = GetTextSize(" ", fnt) / 2;
			e = txts + n;
			for(wtukk q = txts; q < e; q++) {
				if(*q == ' ')
					w.DrawRect(xx + sz.cx, sz.cy, 2, 2, Blend(SColorHighlight(), SColorPaper()));
				xx += fnt[*q];
			}
		}
	}
	txt += n;
	x += cx;
}

void EditField::State(i32)
{
	RefreshAll();
}

Color EditField::GetPaper()
{
	bool enabled = IsShowEnabled();
	Color paper = GetColorAttr(ATTR_BACKGROUND);
	if(IsNull(paper))
		paper = enabled && !IsReadOnly() ? (HasFocus() ? style->focus
	                                                   : style->paper)
	                                     : style->disabled;
	if(nobg)
		paper = Null;
	if(enabled && (convert && convert->Scan(text).IsError() || errorbg))
		paper = style->invalid;
	return paper;
}

void EditField::Paint(Draw& w)
{
	i32 lspace = GetSpaceLeft();
	i32 rspace = GetSpaceRight();
	Size sz = GetSize();
	bool enabled = IsShowEnabled();
	Color paper = GetPaper();
	Color textcolor = GetColorAttr(ATTR_TEXTCOLOR);
	Color ink = enabled ? Nvl(textcolor, style->text) : style->textdisabled;
	i32 fcy = font.GetCy();
	i32 yy = GetTy();
	w.DrawRect(sz, paper);
	PaintSpace(w);
	if(!no_internal_margin) {
		lspace += 2;
		rspace += 2;
	}
	if(lspace || rspace)
		w.Clipoff(lspace, no_internal_margin ? 0 : yy, sz.cx - lspace - rspace, no_internal_margin ? 0 : fcy);
	i32 x = -sc;
	Txt nulltext = GetTextAttr(ATTR_NULLTEXT);
	Image nullicon = GetAttr<Image>(ATTR_NULLICON);
	if(IsNull(text) && (!IsNull(nulltext) || !IsNull(nullicon))) {
		x = 0;
		WTxt nt = nulltext.ToWTxt();
		wtukk txt = nt;
		if(!IsNull(nullicon)) {
			i32 icx = nullicon.GetWidth();
			w.DrawRect(x, 0, icx + 4, fcy, paper);
			w.DrawImage(x, (fcy - nullicon.GetHeight()) / 2, nullicon);
			x += icx + 4;
		}
		Paints(w, x, fcy, txt, Nvl(GetColorAttr(ATTR_NULLINK), SColorDisabled()),
		       paper, nt.GetLength(), false, Nvl(GetFontAttr(ATTR_NULLFONT), StdFont().Italic()), Null, false);
	}
	else {
		wtukk txt = text;
		i32 len = GetLength();
		Vec<Highlight> hl;
		hl.SetCount(len);
		for(i32 i = 0; i < len; i++) {
			hl[i].ink = ink;
			hl[i].paper = paper;
			hl[i].underline = Null;
		}
		HighlightText(hl);
		len = hl.GetCount();
		i32 l, h;
		if(GetSelection(l, h)) {
			h = min(h, len);
			for(i32 i = l; i < h; i++) {
				hl[i].ink = enabled ? HasFocus() ? style->selectedtext : style->selectedtext0 : paper;
				hl[i].paper = enabled ? HasFocus() ? style->selected : style->selected0 : ink;
			}
		}
		i32 b = 0;
		for(i32 i = 0; i <= len; i++)
			if((i == len || hl[i] != hl[b]) && b < len) {
				Paints(w, x, fcy, txt, hl[b].ink, hl[b].paper, i - b, password, font, hl[b].underline, showspaces);
				b = i;
			}
	}
	DrawTiles(w, dropcaret, CtrlImg::checkers());
	if(lspace || rspace)
		w.End();
}

bool EditField::GetSelection(i32& l, i32& h) const
{
	if(anchor < 0 || anchor == cursor) {
		l = h = cursor;
		return false;
	}
	if(anchor < cursor) {
		l = anchor;
		h = cursor;
	}
	else {
		l = cursor;
		h = anchor;
	}
	return true;
}

bool EditField::IsSelection() const
{
	return anchor >= 0 && anchor != cursor;
}

Rect EditField::GetCaretRect(i32 pos) const
{
	return RectC(GetCaret(pos) - sc + 2 * !no_internal_margin + GetSpaceLeft()
	               - font.GetRightSpace('o') + font.GetLeftSpace('o'), GetTy(),
	             DPI(1), min(GetSize().cy - 2 * GetTy(), font.GetCy()));
}

Rect EditField::GetCaret() const
{
	return GetCaretRect(cursor);
}

void EditField::Finish(bool refresh)
{
	if(anchor > text.GetLength()) anchor = text.GetLength();
	if(cursor > text.GetLength()) cursor = text.GetLength();
	if(cursor < 0) cursor = 0;
	Size sz = GetSize();
	if(autosize) {
		Rect r = GetRect();
		i32 mw = min(r.Width(), Draw::GetStdFontSize().cx);
		sz.cx = GetCaret(text.GetLength()) + 4;
		sz = AddFrameSize(sz);
		if(GetParent())
			sz.cx = min(sz.cx, GetParent()->GetSize().cx - r.left);
		sz.cx = minmax(sz.cx, mw, autosize);
		if(sz.cx != r.Width())
			LeftPos(r.left, sz.cx);
		sz = GetSize();
	}
	if(!no_internal_margin)
		sz.cx -= 2;
	sz.cx -= GetSpaceLeft() + GetSpaceRight();
	if(sz.cx <= 0) return;
	i32 x = GetCaret(cursor);
	i32 rspc = max(font.GetRightSpace('o'), font.GetCy() / 5); // sometimes RightSpace is not implemented (0)
	i32 wx = x + rspc;
	if(alignright) {
		i32 cx = GetCaret(text.GetCount());
		sc = cx - sz.cx + rspc + 2;
		RefreshAll();
	}
	if(wx > sz.cx + sc - 1) {
		sc = wx - sz.cx + 1;
		RefreshAll();
	}
	if(x < sc) {
		sc = x;
		RefreshAll();
	}
	if(refresh)
		RefreshAll();
}

void EditField::Layout()
{
	Ctrl::Layout();
	sc = 0;
	Finish();
}

void EditField::SelSource()
{
	if(GetSelection(fsell, fselh))
		SetSelectionSource(ClipFmtsText());
	else
		fsell = fselh = -1;
	CancelMyPreedit();
}

void EditField::GotFocus()
{
	auto inactive_convert = (const Convert *)GetVoidPtrAttr(ATTR_INACTIVE_CONVERT);
	if(autoformat && IsEditable() && !IsNull(text) && inactive_convert) {
		Val v = convert->Scan(text);
		if(!v.IsError()) {
			WTxt s = convert->Format(v);
			if(s != text) text = s;
		}
	}
	if(!keep_selection && !IsSelection()) {
		anchor = 0;
		cursor = text.GetLength();
	}
	SelSource();
	Finish();
	RefreshAll();
}

void EditField::LostFocus()
{
	if(autoformat && IsEditable() && !IsNull(text) && !IsDragAndDropSource()) {
		Val v = convert->Scan(text);
		if(!v.IsError()) {
			auto inactive_convert = (const Convert *)GetVoidPtrAttr(ATTR_INACTIVE_CONVERT);
			const Convert * cv = inactive_convert ? inactive_convert : convert;
			WTxt s = cv->Format(v);
			if(s != text) text = s;
		}
	}
	if(!keep_selection) {
		anchor = -1;
		cursor = sc = 0;
		if(alignright)
			Finish();
	}
	RefreshAll();
}

void EditField::LeftDown(Point p, dword flags)
{
	i32 c = GetCursor(p.x + sc);
	if(!HasFocus()) {
		SetFocus();
		if(clickselect) {
			SetSelection();
			Finish();
			return;
		}
		sc = 0;
		Move(c);
	}
	i32 l, h;
	selclick = false;
	if(GetSelection(l, h) && c >= l && c < h) {
		selclick = true;
		return;
	}
	SetCapture();
	EditCapture();
	Move(c, flags & K_SHIFT);
	Finish();
}

void EditField::MiddleDown(Point p, dword flags)
{
	if(IsReadOnly())
		return;
	if(AcceptText(Selection())) {
		WTxt w = GetWTxt(Selection());
		selclick = false;
		LeftDown(p, flags);
		Insert(w);
		Action();
		Finish();
	}
}

void EditField::LeftUp(Point p, dword flags)
{
	i32 c = GetCursor(p.x + sc);
	i32 l, h;
	if(GetSelection(l, h) && c >= l && c < h && !HasEditCapture() && selclick)
		Move(c, false);
	Finish();
	selclick = false;
}

void EditField::LeftDouble(Point p, dword flags)
{
	z64 l, h;
	if(GetWordSelection(cursor, l, h))
		SetSelection((i32)l, (i32)h);
}

void EditField::LeftTriple(Point p, dword keyflags)
{
	anchor = 0;
	cursor = text.GetLength();
	Finish();
}

void EditField::MouseMove(Point p, dword flags)
{
	if(!HasEditCapture()) return;
	Move(GetCursor(p.x + sc), true);
	Finish();
}

void EditField::SaveUndo()
{
	undotext = text;
	undoanchor = anchor;
	undocursor = cursor;
}

void EditField::Move(i32 newpos, bool select)
{
	bool refresh = anchor >= 0;
	if(select) {
		if(anchor < 0) anchor = cursor;
		refresh = true;
	}
	else
		anchor = -1;
	cursor = newpos;
	Finish(refresh);
	SelSource();
}

void EditField::SetSelection(i32 l, i32 h)
{
	if(l < h) {
		anchor = max(l, 0);
		cursor = min(h, text.GetLength());
	}
	else {
		cursor = l;
		anchor = -1;
	}
	SelSource();
	Finish();
}

void EditField::CancelSelection()
{
	i32 l, h;
	if(GetSelection(l, h)) {
		cursor = l;
		anchor = -1;
		fsell = fselh = -1;
		sc = 0;
		Finish();
	}
}

bool EditField::RemoveSelection()
{
	i32 l, h;
	if(!GetSelection(l, h)) {
		anchor = -1;
		return false;
	}
	SaveUndo();
	Remove(l, h - l);
	cursor = l;
	anchor = -1;
	sc = 0;
	return true;
}

void EditField::Copy()
{
	i32 l, h;
	if(password) return;
	if(!GetSelection(l, h)) {
		l = 0;
		h = text.GetLength();
	}
	WriteClipboardUnicodeText(text.Mid(l, h - l));
}

i32 EditField::Insert(i32 pos, const WTxt& itext)
{
	if(IsReadOnly()) return 0;
	WTxt ins;
	wtukk s = itext;
	for(;;) {
		wchar chr = *s++;
		i32 count = 1;
		if(chr == '\t') {
			count = 4;
			chr = ' ';
		}
		if(chr >= ' ') {
			chr = (*filter)(chr);
			if(chr) {
				chr = convert->Filter(chr);
				if(chr && (charset == CHARSET_UTF8 || FromUnicode(chr, charset, 0)))
					ins.Cat(chr, count);
			}
		}
		else
			break;
	}
	if(ins.GetCount() + text.GetCount() > maxlen) {
		BeepExclamation();
		return 0;
	}
	text.Insert(pos, ins);
	Update();
	return ins.GetLength();
}

void EditField::Remove(i32 pos, i32 n)
{
	if(IsReadOnly()) return;
	text.Remove(pos, n);
	if(cursor >= text.GetLength())
		cursor = text.GetLength();
	Update();
}

void EditField::Insert(i32 chr)
{
	if(IsReadOnly()) return;
	if(initcaps && cursor == 0 && text.GetCount() == 0)
		chr = ToUpper(chr);
	cursor += Insert(cursor, WTxt(chr, 1));
	Finish();
}

void EditField::Insert(const WTxt& s)
{
	if(!RemoveSelection()) SaveUndo();
	cursor += Insert(cursor, s);
	Finish();
}

void EditField::DragAndDrop(Point p, PasteClip& d)
{
	if(IsReadOnly()) return;
	i32 c = GetCursor(p.x + sc);
	if(AcceptText(d)) {
		SaveUndo();
		i32 sell, selh;
		WTxt txt = GetWTxt(d);
		if(GetSelection(sell, selh)) {
			if(c >= sell && c < selh) {
				if(!IsReadOnly())
					RemoveSelection();
				if(IsDragAndDropSource())
					d.SetAction(DND_COPY);
				c = sell;
			}
			else
			if(d.GetAction() == DND_MOVE && IsDragAndDropSource()) {
				if(c > sell)
					c -= selh - sell;
				if(!IsReadOnly())
					RemoveSelection();
				d.SetAction(DND_COPY);
			}
		}
		i32 count = Insert(c, txt);
		SetFocus();
		SetSelection(c, c + count);
		Action();
		return;
	}
	if(!d.IsAccepted()) return;
	Rect dc(0, 0, 0, 0);
	if(c >= 0) {
		i32 x = GetCaret(c);
		dc = RectC(x - sc + 2 - font.GetRightSpace('o'), GetTy(),
		           1, min(GetSize().cy - 2 * GetTy(), font.GetCy()));
	}
	if((Rect16)dc != dropcaret) {
		Refresh(dropcaret);
		dropcaret = dc;
		Refresh(dropcaret);
	}
}

void EditField::DragRepeat(Point p)
{
	if(IsReadOnly())
		return;
	Size sz = GetSize();
	i32 sd = min(sz.cx / 6, 16);
	i32 d = 0;
	if(p.x < sd)
		d = -3;
	if(p.x > sz.cx - sd)
		d = 3;
	i32 a = minmax((i32)sc + minmax(d, -16, 16), 0, max(0, GetCaret(GetLength()) - sz.cx + 2));
	if(a != sc) {
		sc = a;
		Refresh();
	}
}

void EditField::DragLeave()
{
	Refresh(dropcaret);
	dropcaret.Clear();
}

void EditField::LeftDrag(Point p, dword flags)
{
	if(password)
		return;
	i32 c = GetCursor(p.x + sc);
	Size ssz = StdSampleSize();
	i32 sell, selh;
	if(!HasEditCapture() && GetSelection(sell, selh) && c >= sell && c <= selh) {
		WTxt sel = text.Mid(sell, selh - sell);
		ImageDraw iw(ssz);
		iw.DrawText(0, 0, sel);
		iw.Alpha().DrawRect(ssz, Black);
		iw.Alpha().DrawText(0, 0, sel, StdFont(), White);
		VecMap<Txt, ClipData> data;
		Append(data, sel);
		bool oks = keep_selection;
		keep_selection = true;
		if(DoDragAndDrop(data, iw) == DND_MOVE && !IsReadOnly()) {
			CancelSelection();
			SaveUndo();
			Remove(sell, selh - sell);
			sc = 0;
			Finish();
			Action();
		}
		keep_selection = oks;
	}
}

Txt EditField::GetSelectionData(const Txt& fmt) const
{
	if(password) return Txt();
	if(fsell >= 0 && fselh >= 0 && fsell <= text.GetCount() && fselh <= text.GetCount())
		return GetTextClip(text.Mid(fsell, fselh - fsell), fmt);
	return Txt();
}

void EditField::Undo()
{
	if(!IsEditable())
		return;
	Swap(undotext, text);
	Swap(undoanchor, anchor);
	Swap(undocursor, cursor);
	anchor = -1;
	UpdateAction();
	Finish();
}

void EditField::Cut()
{
	if(!IsEditable())
		return;
	Copy();
	RemoveSelection();
	Action();
	Finish();
}

void EditField::StdPasteFilter(WTxt&)
{
}

void EditField::Paste()
{
	if(!IsEditable())
		return;
	WTxt w = ReadClipboardUnicodeText();
	WhenPasteFilter(w);
	Insert(w);
	Action();
	Finish();
}

void EditField::Erase()
{
	if(!IsEditable())
		return;
	if(!IsSelection())
		SelectAll();
	RemoveSelection();
	Finish();
}

void EditField::SelectAll()
{
	SetSelection();
	Finish();
}

void EditField::StdBar(Bar& menu) {
	menu.Add(IsEditable(), t_("Отменить"), OTVET(Undo))
		.Key(K_ALT_BACKSPACE)
		.Key(K_CTRL_Z);
	menu.Separator();
	menu.Add(IsEditable() && IsSelection(), t_("Вырезать"), CtrlImg::cut(), OTVET(Cut))
		.Key(K_SHIFT_DELETE)
		.Key(K_CTRL_X);
	menu.Add(IsSelection(), t_("Копировать"), CtrlImg::copy(), OTVET(Copy))
		.Key(K_CTRL_INSERT)
		.Key(K_CTRL_C);
	menu.Add(IsEditable() && IsClipboardAvailableText(), t_("Вставить"), CtrlImg::paste(), OTVET(Paste))
		.Key(K_SHIFT_INSERT)
		.Key(K_CTRL_V);
	menu.Add(IsEditable(), t_("Стереть"), CtrlImg::remove(), OTVET(Erase))
		.Key(K_DELETE);
	menu.Separator();
	menu.Add(GetLength(), t_("Выделить Все"), CtrlImg::select_all(), OTVET(SelectAll))
		.Key(K_CTRL_A);
}

void EditField::RightDown(Point p, dword keyflags)
{
	keep_selection = true;
	Ptr<EditField> self = this;
	MenuBar::Execute(WhenBar);
	if(self) { // protect from destruction when in menu modal loop
		SetFocus();
		keep_selection = false;
	}
}

bool EditField::Key(dword key, i32 rep)
{
	i32 q;
	bool h;
	Txt s;
	bool select = key & K_SHIFT;
	switch(key & ~K_SHIFT) {
	case K_LEFT:
		Move(cursor - 1, select);
		return true;
	case K_CTRL_LEFT:
		Move((i32)GetPrevWord(cursor), select);
		return true;
	case K_CTRL_RIGHT:
		Move((i32)GetNextWord(cursor), select);
		return true;
	case K_RIGHT:
		Move(cursor + 1, select);
		return true;
	case K_HOME:
		Move(0, select);
		return true;
	case K_END:
		Move(text.GetLength(), select);
		return true;
	}
	if(!IsEditable())
		return MenuBar::Scan(WhenBar, key);
	switch(key) {
	case K_BACKSPACE:
	case K_SHIFT|K_BACKSPACE:
		if(RemoveSelection()) {
			Action();
			break;
		}
		if(cursor == 0 || IsReadOnly()) return true;
		SaveUndo();
		cursor--;
		Remove(cursor, 1);
		Action();
		break;
	case K_CTRL_BACKSPACE:
		if(RemoveSelection()) {
			Action();
			break;
		}
		if(cursor == 0 || IsReadOnly()) return true;
		SaveUndo();
		q = cursor;
		h = IsWCh(text[--cursor]);
		while(cursor > 0 && IsWCh(text[cursor - 1]) == h)
			cursor--;
		Remove(cursor, q - cursor);
		Action();
		break;
	case K_DELETE:
		if(RemoveSelection()) {
			Action();
			break;
		}
		if(cursor >= text.GetLength()) return true;
		SaveUndo();
		Remove(cursor, 1);
		Action();
		break;
	case K_CTRL_DELETE:
		if(RemoveSelection()) {
			Action();
			break;
		}
		if(cursor >= text.GetLength()) return true;
		q = cursor;
		h = IsWCh(text[q]);
		while(IsWCh(text[q]) == h && q < text.GetLength()) q++;
		SaveUndo();
		Remove(cursor, q - cursor);
		Action();
		break;
	case K_ENTER:
		if(WhenEnter) {
			WhenEnter();
			return true;
		}
		return false;
	default:
		if(key >= ' ' && key < K_CHAR_LIM || key == K_SHIFT_SPACE) {
			if(!RemoveSelection()) SaveUndo();
			while(rep--)
				Insert(key == K_SHIFT_SPACE ? ' ' : key);
			Action();
			return true;
		}
		else
			return MenuBar::Scan(WhenBar, key);
	}
	Finish();
	return true;
}

void EditField::SetText(const WTxt& txt)
{
	if(text == txt) {
		Update();
		return;
	}
	text = txt;
	sc = 0;
	if(HasFocus()) {
		cursor = txt.GetLength();
		anchor = 0;
	}
	else {
		cursor = 0;
		anchor = -1;
	}
	Update();
	Finish();
}

void EditField::SetData(const Val& data)
{
	auto inactive_convert = (const Convert *)GetVoidPtrAttr(ATTR_INACTIVE_CONVERT);
	const Convert * cv = convert;
	if(!HasFocus() && inactive_convert)
		cv = inactive_convert;
	SetText((WTxt) cv->Format(data));
}

Val EditField::GetData() const
{
	return convert->Scan(text);
}

void EditField::Clear()
{
	SetText(WTxt());
	sc = cursor = 0;
}

void EditField::Reset()
{
	Clear();
	ClearModify();
	sc = 0;
	cursor = 0;
	anchor = -1;
	password = false;
	autoformat = true;
	clickselect = false;
	filter = CharFilterUnicode;
	convert = &NoConvert();
	initcaps = false;
	maxlen = INT_MAX;
	autosize = false;
	keep_selection = false;
	errorbg = nobg = false;
	charset = CHARSET_UTF8;
	alignright = false;
	showspaces = false;
	no_internal_margin = false;
	SetStyle(StyleDefault());
	SetFrame(edge);
	font = StdFont();
	fsell = fselh = -1;
	DeleteAttr<Image>(ATTR_NULLICON);
}

EditField& EditField::SetFont(Font _font)
{
	font = _font;
	Finish(true);
	return *this;
}

EditField& EditField::SetColor(Color c)
{
	if(GetColorAttr(ATTR_TEXTCOLOR) != c) {
		SetColorAttr(ATTR_TEXTCOLOR, c);
		Refresh();
	}
	return *this;
}

EditField& EditField::SetBackground(Color c)
{
	if(GetColorAttr(ATTR_BACKGROUND) != c) {
		SetColorAttr(ATTR_BACKGROUND, c);
		Refresh();
	}
	return *this;
}

EditField& EditField::NullText(const Image& icon, tukk text, Font fnt, Color ink)
{
	if(!IsNull(icon))
		CreateAttr<Image>(ATTR_NULLICON) = icon;
	Txt h = text;
	h << " ";
	SetTextAttr(ATTR_NULLTEXT, h);
	SetColorAttr(ATTR_NULLINK, ink);
	SetFontAttr(ATTR_NULLFONT, fnt);
	Refresh();
	return *this;
}

EditField& EditField::NullText(const Image& icon, tukk text, Color ink)
{
	return NullText(icon, text, Null, ink);
}

EditField& EditField::NullText(tukk text, Font fnt, Color ink)
{
	return NullText(Null, text, fnt, ink);
}

EditField& EditField::NullText(tukk text, Color ink)
{
	return NullText(text, Null, ink);
}

EditField::EditField()
{
	dropcaret = Rect(0, 0, 0, 0);
	Unicode();
	Reset();
	WhenBar = OTVET(StdBar);
}

EditField::~EditField()
{
	DeleteAttr<Image>(ATTR_NULLICON);
}

}