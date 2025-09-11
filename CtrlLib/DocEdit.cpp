#include "CtrlLib.h"

namespace drx {

void DocEdit::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	sb.Wheel(zdelta);
}

void DocEdit::ClearLines()
{
	para.Clear();
	ASSERT(GetLineCount() == para.GetCount());
}

void DocEdit::InsertLines(i32 line, i32 count)
{
	para.Insert(line, Para(), count);
	ASSERT(GetLineCount() == para.GetCount());
}

void DocEdit::RemoveLines(i32 line, i32 count)
{
	para.Remove(line, count);
	ASSERT(GetLineCount() == para.GetCount());
}

DocEdit::Fmt DocEdit::Format(const WTxt& text) const
{
	FontInfo fi = font.Info();
	Fmt fmt;
	i32 tcx = fi['x'] * 4;
	fmt.len = text.GetLength();
	fmt.text.Alloc(text.GetLength());
	memcpy(fmt.text, text, text.GetLength() * sizeof(wchar));
	fmt.width.Alloc(text.GetLength());
	fmt.line.Add(0);
	i32 *w = fmt.width;
	i32 x = 0;
	wtukk space = NULL;
	i32 spacex = 0;
	for(wtuk s = fmt.text; s < fmt.text + fmt.len; s++) {
		i32 cw;
		if(*s == '\t')
			cw = (x + tcx) / tcx * tcx - x;
		else
			cw = fi[*s];
		*w++ = cw;
		if(*s == ' ' || *s == '\t') {
			space = s;
			spacex = x + cw;
			*s = ' ';
		}
		x += cw;
		if(x > cx) {
			if(space && space <= s) {
				space++;
				fmt.line.Add(i32(space - fmt.text));
				space = NULL;
				x -= spacex;
			}
			else {
				fmt.line.Add(i32(s - fmt.text));
				x = cw;
			}
		}
	}
	fmt.fi = fi;
	return fmt;
}

i32  DocEdit::GetHeight(i32 i) {
	Para& p = para[i];
	if(p.cx == cx) return p.cy;
	Fmt fmt = Format(GetWLine(i));
	p.cx = cx;
	p.cy = fmt.line.GetCount() * (fmt.fi.GetHeight()) + after;
	return p.cy;
}

i32 DocEdit::GetY(i32 parai) {
	i32 y = 1;
	for(i32 i = 0; i < parai; i++)
		y += GetHeight(i);
	return y;
}

void DocEdit::InvalidateLine(i32 i)
{
	para[i].cx = -1;
}

void DocEdit::RefreshLine(i32 i) {
	i32 q = para[i].cx >= 0 ? para[i].cy : -1;
	Refresh(1, GetY(i) - sb, cx, GetHeight(i));
	if(q < 0 || q != para[i].cy)
		Refresh();
}

i32 sSum(i32k *w, i32 n)
{
	i32 m = 0;
	while(n--)
		m += *w++;
	return m;
}

void DocEdit::Paint(Draw& w) {
	Size sz = GetSize();
	Color bg =  color[IsShowEnabled() && !IsReadOnly() ? PAPER_NORMAL : PAPER_READONLY];
	if(nobg)
		bg = Null;
	i32 y = -sb + 1;
	i32 pos = 0;
	i32 sell, selh;
	GetSelection32(sell, selh);
	for(i32 i = 0; i < para.GetCount() && y < sz.cy; i++) {
		i32 h = GetHeight(i);
		if(y + h >= 0) {
			WTxt text = GetWLine(i);
			Fmt fmt = Format(text);
			i32 p = pos;
			for(i32 i = 0; i < fmt.line.GetCount(); i++) {
				i32 n = fmt.LineEnd(i) - fmt.line[i];
				i32 a = minmax(sell - p, 0, n);
				i32 b = minmax(selh - p, 0, n) - a;
				i32 c = n - a - b;
				i32 *wa = fmt.width + fmt.line[i];
				i32 *wb = fmt.width + fmt.line[i] + a;
				i32 *wc = fmt.width + fmt.line[i] + a + b;
				i32 acx = sSum(wa, a);
				i32 bcx = sSum(wb, b);
				i32 ccx = sSum(wc, c);
				w.DrawRect(1, y, acx, fmt.fi.GetHeight(), bg);
				w.DrawText(1, y, ~fmt.text + fmt.line[i], font,
				           IsShowEnabled() ? color[INK_NORMAL] : color[INK_DISABLED], a, wa);
				w.DrawRect(1 + acx, y, bcx, fmt.fi.GetHeight(), color[PAPER_SELECTED]);
				w.DrawText(1 + acx, y, ~fmt.text + fmt.line[i] + a, font, color[INK_SELECTED], b, wb);
				w.DrawRect(1 + acx + bcx, y, ccx, fmt.fi.GetHeight(), bg);
				w.DrawText(1 + acx + bcx, y, ~fmt.text + fmt.line[i] + a + b, font, color[INK_NORMAL], c, wc);
				p += n;
				w.DrawRect(1 + acx + bcx + ccx, y, cx - (acx + bcx + ccx), fmt.fi.GetHeight(),
				           p >= sell && p < selh ? color[PAPER_SELECTED] : bg);
				y += fmt.fi.GetHeight();
			}
			w.DrawRect(1, y, cx, after, color[PAPER_NORMAL]);
			y += after;
		}
		else
			y += h;
		pos += GetLineLength(i) + 1;
	}
	w.DrawRect(0, -sb, sz.cx, 1, bg);
	w.DrawRect(0, 0, 1, sz.cy, bg);
	w.DrawRect(sz.cx - 1, 0, 1, sz.cy, bg);
	if(eofline)
		w.DrawRect(1, y++, cx, 1, SColorShadow);
	if(y < sz.cy)
		w.DrawRect(1, y, cx, sz.cy - y, bg);
	DrawTiles(w, DropCaret(), CtrlImg::checkers());
}

void DocEdit::SetSb()
{
	Size sz = GetSize();
	cx = max(Draw::GetStdFontCy(), sz.cx - 2);
	sb.SetPage(GetSize().cy);
	sb.SetTotal(GetY(para.GetCount()) + 2);
	PlaceCaret(false);
}

void DocEdit::Layout()
{
	SetSb();
	Invalidate();
}

Point DocEdit::GetCaret(i32 pos) {
	i32 i = GetLinePos32(pos);
	Fmt fmt = Format(GetWLine(i));
	i32 l;
	for(l = 0; l < fmt.line.GetCount(); l++)
		if(pos < fmt.line[l])
			break;
	l--;
	i32k *w = fmt.width + fmt.line[l];
	pos -= fmt.line[l];
	i32 x = 0;
	while(pos-- > 0)
		x += *w++;
	return Point(x, GetY(i) + l * fmt.fi.GetHeight());
}

i32  DocEdit::GetCursorPos(Point p) {
	i32 pos = 0;
	for(i32 i = 0; i < para.GetCount(); i++) {
		i32 h = GetHeight(i);
		if(p.y < h) {
			WTxt text = GetWLine(i);
			Fmt fmt = Format(text);
			i32 x = 0;
			i32 l = p.y / fmt.fi.GetHeight();
			if(l < 0)
				return pos;
			if(l >= fmt.line.GetCount())
				return pos + text.GetLength();
			i32k *w = fmt.width + fmt.line[l];
			i32k *e = fmt.width + fmt.LineEnd(l);
			while(w < e) {
				if(p.x < x + *w / 2)
					return i32(w - fmt.width) + pos;
				x += *w++;
			}
			i32 p = i32(e - fmt.width);
			if(p > 0 && text[p - 1] == ' ' && l < fmt.line.GetCount() - 1)
			   p--;
			return p + pos;
		}
		p.y -= h;
		pos += GetLineLength(i) + 1;
	}
	return GetLength32();
}

Rect DocEdit::GetCaret() const
{
	return caret;
}

void DocEdit::PlaceCaret(bool scroll) {
	Point cr = GetCaret((i32)cursor);
	i32 fy = font.Info().GetLineHeight();
	if(scroll) {
		if(cursor == GetLength32())
			sb.End();
		else
			sb.ScrollInto(cr.y, fy + 2);
	}
	caret = RectC(cr.x + 1, cr.y - sb, 1, fy);
	WhenSel();
}

void DocEdit::PlaceCaret(z64 newpos, bool select) {
	if(newpos > GetLength32())
		newpos = GetLength32();
	i32 z = GetLine(newpos);
	if(select) {
		if(anchor < 0)
			anchor = cursor;
		RefreshLines(z, GetLine(cursor));
	}
	else
		if(anchor >= 0) {
			RefreshLines(GetLine(cursor), GetLine(anchor));
			anchor = -1;
		}
	cursor = newpos;
	PlaceCaret(true);
	SelectionChanged();
	if(IsSelection())
		SetSelectionSource(ClipFmtsText());
	CancelMyPreedit();
}

i32 DocEdit::GetMousePos(Point p)
{
	return GetCursorPos(Point(p.x - 1, p.y + sb - 1));
}

void DocEdit::LeftDown(Point p, dword flags) {
	SetFocus();
	i32 c = GetMousePos(p);
	i32 l, h;
	if(GetSelection32(l, h) && c >= l && c < h) {
		selclick = true;
		return;
	}
	PlaceCaret(c, flags & K_SHIFT);
	SetCapture();
}

void DocEdit::LeftUp(Point p, dword flags)
{
	if(!HasCapture() && selclick) {
		i32 c = GetMousePos(p);
		PlaceCaret(c, flags & K_SHIFT);
		SetFocus();
	}
	selclick = false;
	ReleaseCapture();
}

void DocEdit::MouseMove(Point p, dword flags) {
	if(!HasCapture()) return;
	PlaceCaret(GetMousePos(p), true);
}

void DocEdit::LeftDouble(Point, dword)
{
	z64 l, h;
	if(GetWordSelection(cursor, l, h))
		SetSelection(l, h);
}

void DocEdit::LeftTriple(Point, dword)
{
	i32 q = (i32)cursor;
	i32 i = GetLinePos32(q);
	q = (i32)cursor - q;
	SetSelection(q, q + GetLineLength(i) + 1);
}

Image DocEdit::CursorImage(Point, dword) {
	return Image::IBeam();
}

void DocEdit::GotFocus() {
	Refresh();
}

void DocEdit::LostFocus() {
	Refresh();
}

void DocEdit::VertMove(i32 delta, bool select, bool scs) {
	i32 hy = GetY(para.GetCount());
	Point p = GetCaret((i32)cursor);
	i32 yy = p.y;
	for(;;) {
		p.y += delta;
		if(p.y > hy) p.y = hy - 1;
		if(p.y < 0) p.y = 0;
		i32 q = GetCursorPos(p);
		if(q >= 0 && q != cursor && (delta < 0) == (q < cursor) && GetCaret(q).y != yy) {
			PlaceCaret(q, select);
			break;
		}
		if(p.y == 0 || p.y >= hy - 1) {
			PlaceCaret(delta > 0 ? GetLength32() : 0, select);
			break;
		}
		delta = sgn(delta) * 4;
	}
	if(scs)
		sb = GetCaret((i32)cursor).y - (yy - sb);
	PlaceCaret(true);
}

void DocEdit::HomeEnd(i32 x, bool select) {
	Point p = GetCaret((i32)cursor);
	p.x = x;
	PlaceCaret(GetCursorPos(p), select);
}

bool DocEdit::Key(dword key, i32 cnt)
{
	NextUndo();
	bool h;
	i32 q;
	bool select = key & K_SHIFT;
	i32 pgsk = max(8, 6 * GetSize().cy / 8);
	switch(key & ~K_SHIFT) {
	case K_CTRL_LEFT:
		PlaceCaret(GetPrevWord(cursor), select);
		break;
	case K_CTRL_RIGHT:
		PlaceCaret(GetNextWord(cursor), select);
		break;
	case K_HOME:
		HomeEnd(0, select);
		break;
	case K_END:
		HomeEnd(cx, select);
		break;
	case K_CTRL_HOME:
	case K_CTRL_PAGEUP:
		PlaceCaret(0, select);
		break;
	case K_CTRL_END:
	case K_CTRL_PAGEDOWN:
		PlaceCaret(GetLength32(), select);
		break;
	case K_UP:
		if(GetCursor() == 0)
			return !updownleave;
		VertMove(-8, select, false);
		return true;
	case K_DOWN:
		if(GetCursor32() == GetLength32())
			return !updownleave;
		VertMove(8, select, false);
		return true;
	case K_PAGEUP:
		VertMove(-pgsk, select, true);
		return true;
	case K_PAGEDOWN:
		VertMove(pgsk, select, true);
		return true;
	case K_LEFT:
		if(cursor)
			PlaceCaret(cursor - 1, select);
		break;
	case K_RIGHT:
		if(cursor < GetLength32())
			PlaceCaret(cursor + 1, select);
		break;
	default:
		if(IsReadOnly()) return MenuBar::Scan(WhenBar, key);
		switch(key) {
		case K_BACKSPACE:
		case K_SHIFT|K_BACKSPACE:
			if(RemoveSelection()) break;
			if(cursor == 0) return true;
			cursor--;
			Remove((i32)cursor, 1);
			break;
		case K_CTRL_BACKSPACE:
			if(RemoveSelection()) break;
			if(cursor <= 0) return true;
			q = (i32)cursor - 1;
			h = IsLetter(GetChar(q));
			while(q > 0 && IsLetter(GetChar(q - 1)) == h) q--;
			Remove(q, (i32)cursor - q);
			SetCursor(q);
			break;
		case K_DELETE:
			if(RemoveSelection()) break;
			if(cursor >= GetLength32()) return true;
			if(cursor < GetLength32())
				Remove((i32)cursor, 1);
			break;
		case K_CTRL_DELETE:
			if(RemoveSelection()) break;
			if(cursor >= GetLength32()) return true;
			q = (i32)cursor;
			h = IsLetter(GetChar(q));
			while(IsLetter(GetChar(q)) == h && q < GetLength32()) q++;
			Remove((i32)cursor, q - (i32)cursor);
			break;
		case K_ENTER:
			if(!processenter)
				return true;
			key = '\n';
		default:
			if(filter && key >= 32 && key < 65535)
				key = (*filter)(key);
			if(key >= ' ' && key < K_CHAR_LIM || key == '\n' || key == '\t' || key == K_SHIFT_SPACE) {
				if(key == K_TAB && !processtab)
					return false;
				if(key >= 128 && key < K_CHAR_LIM && (charset != CHARSET_UTF8 && charset != CHARSET_UTF8_BOM)
				   && FromUnicode((wchar)key, charset) == DEFAULTCHAR)
					return true;
				RemoveSelection();
				Insert((i32)cursor, WTxt(key == K_SHIFT_SPACE ? ' ' : key, cnt), true);
				cursor += cnt;
				break;
			}
			return MenuBar::Scan(WhenBar, key);
		}
		UpdateAction();
	}
	PlaceCaret(true);
	return true;
}

void DocEdit::Scroll()
{
	PlaceCaret(false);
	Refresh();
}

void DocEdit::Invalidate()
{
	for(i32 i = 0; i < para.GetCount(); i++)
		para[i].cx = -1;
	PlaceCaret(false);
}

void  DocEdit::RefreshStyle()
{
	cursor = 0;
	sb = 0;
	ClearSelection();
	Invalidate();
	Layout();
	Refresh();
}

void DocEdit::RightDown(Point p, dword w)
{
	SetFocus();
	i32 c = GetMousePos(p);
	i32 l, h;
	if(!GetSelection32(l, h) || c < l || c >= h)
		PlaceCaret(c, false);
	MenuBar::Execute(WhenBar);
}

DocEdit::DocEdit()
{
	updownleave = false;
	cx = 0;
	filter = NULL;
	after = 0;
	font = StdFont();
	AutoHideSb();
	SetFrame(ViewFrame());
	AddFrame(sb);
	sb.SetLine(8);
	sb.WhenScroll = OTVET(Scroll);
	InsertLines(0, 1);
	eofline = true;
	PlaceCaret(false);
}

DocEdit::~DocEdit() {}

void DocEdit::DragAndDrop(Point p, PasteClip& d)
{
	if(IsReadOnly()) return;
	i32 c = GetMousePos(p);
	if(AcceptText(d)) {
		NextUndo();
		i32 a = sb;
		i32 sell, selh;
		WTxt txt = GetWTxt(d);
		if(GetSelection32(sell, selh)) {
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
		sb = a;
		SetFocus();
		SetSelection(c, c + count);
		Action();
		return;
	}
	if(!d.IsAccepted()) return;
	Point dc = Null;
	if(c >= 0) {
		Point cr = GetCaret(c);
		dc = Point(cr.x + 1, cr.y);
	}
	if(dc != dropcaret) {
		RefreshDropCaret();
		dropcaret = dc;
		RefreshDropCaret();
	}
}

Rect DocEdit::DropCaret()
{
	if(IsNull(dropcaret))
		return Rect(0, 0, 0, 0);
	return RectC(dropcaret.x, dropcaret.y - sb, 1, font.Info().GetLineHeight());
}

void DocEdit::RefreshDropCaret()
{
	Refresh(DropCaret());
}

void DocEdit::DragRepeat(Point p)
{
	sb = (i32)sb + GetDragScroll(this, p, 16).y;
}

void DocEdit::DragLeave()
{
	RefreshDropCaret();
	dropcaret = Null;
	isdrag = false;
	Layout();
}

void DocEdit::LeftDrag(Point p, dword flags)
{
	i32 c = GetMousePos(p);
	i32 l, h;
	if(!HasCapture() && GetSelection32(l, h) && c >= l && c < h) {
		WTxt sample = GetW(l, min(h - l, 3000));
		Size ssz = StdSampleSize();
		ImageDraw iw(ssz);
		iw.DrawRect(ssz, Black());
		iw.Alpha().DrawRect(ssz, Black());
		DrawTLText(iw.Alpha(), 0, 0, ssz.cx, sample, StdFont(), White());
		NextUndo();
		if(DoDragAndDrop(ClipFmtsText(), iw) == DND_MOVE && !IsReadOnly()) {
			RemoveSelection();
			Action();
		}
	}
}

TextCtrl::EditPos DocEdit::GetEditPos() const {
	EditPos pos;
	pos.sby = sb.Get();
	pos.cursor = cursor;
	return pos;
}

void DocEdit::SetEditPos(const TextCtrl::EditPos& pos) {
	sb.Set(minmax(pos.sby, 0, GetY(para.GetCount()) + 2));
	SetCursor(pos.cursor);
}

}
