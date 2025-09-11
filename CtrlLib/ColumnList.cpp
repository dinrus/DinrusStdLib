#include "CtrlLib.h"

namespace drx {

void ColumnList::MouseWheel(Point p, i32 zdelta, dword keyflags) {
	sb.Wheel(zdelta);
}

i32  ColumnList::GetColumnCx(i32 i) const {
	if (mode == MODE_ROWS)
		return cx;
	i32 szcx = GetSize().cx;
	i32 cx = max(szcx / max(ncl, 1), Draw::GetStdFontCy());
	return i == ncl - 1 ? szcx - i * cx : cx;
}

i32 ColumnList::GetColumnItems() const {
	if(cy == 0) return 0;
	return GetSize().cy / cy;
}

i32  ColumnList::GetDragColumn(i32 x) const {
	if (mode == MODE_ROWS)
		return -1;
	i32 cx = GetColumnCx();
	i32 i = 0;
	if(cx == 0) return -1;
	for(;;) {
		x -= GetColumnCx(i);
		if(x >= -3 && x < 4)
			return i;
		if(x < 0) return -1;
		i++;
	}
}

void ColumnList::ShiftSelect() {
	if(anchor < 0)
		anchor = cursor;
	i32 l = min(anchor, cursor);
	i32 h = max(anchor, cursor);
	for(i32 i = 0; i < GetCount(); i++) {
		bool sel = i >= l && i <= h;
		if(item[i].canselect && item[i].sel != sel) {
			item[i].sel = sel;
			RefreshItem(i);
		}
	}
	UpdateSelect();
}

Image ColumnList::CursorImage(Point p, dword)
{
	return HasCapture() || GetDragColumn(p.x) >= 0 ? Image::SizeHorz() : Image::Arrow();
}

i32 ColumnList::GetItem(Point p)
{
	i32 i;
	i32 max = GetPageItems();
	if (mode == MODE_ROWS) {
		i = ((p.y + sb % cy) / cy) * ncl + (p.x / cx);
		max += ncl*2;
	}
	else
		i = p.y / cy + GetColumnItems() * (p.x / GetColumnCx(0));
	if(i < 0 || i >= max)
		return -1;
	i += GetSbPos();
	return i >= 0 && i < GetCount() ? i : -1;
}

void ColumnList::PointDown(Point p)
{
	i32 i = GetItem(p);
	if(i >= 0 && i < GetCount())
		SetCursor0(i, false);
	else
	if(clickkill)
		KillCursor();
}

void ColumnList::DoClick(Point p, dword flags)
{
	SetWantFocus();
	PointDown(p);
	p.y %= cy;
	p.x %= GetColumnCx(0);
	if(multi && cursor >= 0) {
		if(flags & K_SHIFT)
			ShiftSelect();
		else
		if(flags & K_CTRL)
			SelectOne(cursor, !IsSelected(cursor));
		else {
			ClearSelection();
			SelectOne(cursor, true);
			anchor = cursor;
		}
	}
	WhenLeftClick();
	WhenLeftClickPos(p);
}

void ColumnList::DoLeftDown(Point p, dword flags) {
	selclick = false;
	i32 i = GetDragColumn(p.x);
	if(i >= 0) {
		ci = i;
		dx = p.x - GetColumnCx(0) * (i + 1);
		mpos = p.x;
		SetCapture();
		Refresh(mpos - dx, 0, 1, GetSize().cy);
		SyncInfo();
	}
	else {
		i32 i = GetItem(p);
		if(i >= 0 && i < GetCount() && multi && IsSel(i))
			selclick = true;
		else
			DoClick(p, flags);
	}
}

void ColumnList::LeftDown(Point p, dword flags) {
	DoLeftDown(p, flags);
}

void ColumnList::LeftUp(Point p, dword flags)
{
	if(selclick)
		DoClick(p, flags);
	selclick = false;
	if(HasCapture()) {
		Refresh();
		SyncInfo();
	}
	ReleaseCapture();
}

void ColumnList::LeftDrag(Point p, dword keyflags)
{
	if(!HasCapture())
		WhenDrag();
}

void ColumnList::RightDown(Point p, dword flags) {
	i32 i = GetItem(p);
	if(!(i >= 0 && multi && IsSel(i))) // Do not change selection if clicking on it
		DoClick(p, 0);
	MenuBar::Execute(WhenBar);
}

void ColumnList::LeftDouble(Point p, dword flags) {
	DoLeftDown(p, flags);
	WhenLeftDouble();
}

void ColumnList::SyncInfo()
{
	if((HasMouse() || info.HasMouse()) && !HasCapture() && popupex) {
		Point p = GetMouseViewPos();
		i32 i = GetItem(p);
		if(i >= 0) {
			Rect r = GetItemRect(i);
			if(p.x > r.left + 3 && p.x < r.right - 2) {
				Color ink, paper;
				dword style;
				GetItemStyle(i, ink, paper, style);
				Item& m = item[i];
				info.Set(this, r, m.value, m.display ? m.display : display, ink, paper, style);
				return;
			}
		}
	}
	info.Cancel();
}

void ColumnList::MouseMove(Point p, dword)
{
	if(HasCapture()) {
		i32 cx = sb.GetViewSize().cx;
		Refresh(mpos - dx, 0, 1, GetSize().cy);
		mpos = p.x;
		Refresh(mpos - dx, 0, 1, GetSize().cy);
		i32 ocl = ncl;
		ncl = max(1, cx * (ci + 1) / max(p.x, Draw::GetStdFontCy()));
		if(ncl != ocl)
			Refresh();
		SetSb();
	}
	SyncInfo();
}

void ColumnList::CancelMode()
{
	info.Cancel();
	dropitem = -1;
	anchor = -1;
	selclick = false;
}

void ColumnList::MouseLeave()
{
	if(!info.IsOpen())
		info.Cancel();
}

Rect ColumnList::GetItemRect(i32 i) const {
	if (mode == MODE_ROWS)
		return (i >= 0) ? RectC( (i % ncl) * cx, (i / ncl) * cy - sb, cx, cy) : Rect(0, 0, 0, 0);
	i -= GetSbPos();
	i32 n = GetColumnItems();
	if(i < 0 || n == 0)
		return Rect(0, 0, 0, 0);
	i32 cl = i / n;
	i32 x = 0;
	for(i32 q = 0; q < cl; q++)
		x += GetColumnCx(q);
	return RectC(x, i % n * cy, GetColumnCx(cl), cy);
}

void ColumnList::Page(bool down) {
	i32 q = GetSbPos();
	if(down)
		sb.NextPage();
	else
		sb.PrevPage();
	if(q == sb)
		SetCursor(down ? GetCount() - 1 : 0);
	else
		SetCursor(cursor - q + GetSbPos());
}

dword ColumnList::SwapKey(dword key)
{
	if (mode == MODE_ROWS) {
		switch(key) {
		case K_UP:
			return K_LEFT;
		case K_DOWN:
			return K_RIGHT;
		case K_LEFT:
			return K_UP;
		case K_RIGHT:
			return K_DOWN;
		}
	}
	return key;
}

bool ColumnList::Key(dword _key, i32 count) {
	i32 c = cursor;
	bool sel = (_key & K_SHIFT) && multi;
	i32 key = _key & ~K_SHIFT;
	key = SwapKey(key);
	switch(key) {
	case K_UP:
		if(c < 0)
			c = GetCount() - 1;
		else
		if(c > 0)
			c--;
		break;
	case K_DOWN:
		if(c < 0)
			c = 0;
		else
		if(c < GetCount() - 1)
			c++;
		break;
	case K_LEFT:
		c = max(c - ((mode == MODE_ROWS) ? ncl : GetColumnItems()), 0);
		break;
	case K_RIGHT:
		c = min(c + ((mode == MODE_ROWS) ? ncl : GetColumnItems()), GetCount() - 1);
		break;
	case K_PAGEUP:
		Page(false);
		return true;
	case K_PAGEDOWN:
		Page(true);
		return true;
	case K_HOME:
		c = 0;
		break;
	case K_END:
		c = GetCount() - 1;
		break;
	default:
		return MenuBar::Scan(WhenBar, _key);
	}
	if(sel) {
		SetCursor0(c, false);
		ShiftSelect();
	}
	else {
		SetCursor(c);
		anchor = c;
	}
	return true;
}

void ColumnList::RefreshSel()
{
	for(i32 i = 0; i < item.GetCount(); i++)
		if(item[i].sel)
			RefreshItem(i);
}

void ColumnList::ClearSelection() {
	bool upd = false;
	for(i32 i = 0; i < item.GetCount(); i++)
		if(item[i].sel) {
			item[i].sel = false;
			upd = true;
			RefreshItem(i);
		}
	if(upd)
		UpdateSelect();
}

void ColumnList::SelectOne(i32 i, bool sel) {
	if(!multi) return;
	if(item[i].canselect)
		item[i].sel = sel;
	UpdateSelect();
	RefreshItem(i);
}

bool ColumnList::IsSelected(i32 i) const {
	return item[i].sel;
}

bool ColumnList::IsSel(i32 i) const
{
	return IsMultiSelect() ? IsSelected(i) : GetCursor() == i;
}

void ColumnList::UpdateSelect() {
	selcount = 0;
	for(i32 i = 0; i < item.GetCount(); i++)
		if(item[i].sel)
			selcount++;
	SyncInfo();
	Action();
	WhenSelection();
	WhenSel();
}

void ColumnList::GetItemStyle(i32 i, Color& fg, Color& bg, dword& st)
{
	st = 0;
	bool hasfocus = HasFocusDeep() && !IsDragAndDropTarget();
	bool drop = i == dropitem && !insert;
	if(IsReadOnly())
		st |= Display::READONLY;
	if(IsSelected(i))
		st |= Display::SELECT;
	if(i == cursor)
		st |= Display::CURSOR;
	if(drop) {
		hasfocus = true;
		st |= Display::CURSOR;
	}
	if(hasfocus)
		st |= Display::FOCUS;
	fg = SColorText;
	bg = SColorPaper;
	if(nobg)
		bg = Null;
	if((st & Display::SELECT) ||
	   (!multi || !item[i].canselect && selcount == 0) && (st & Display::CURSOR) ||
	   drop) {
		fg = hasfocus ? SColorHighlightText : SColorText;
		bg = hasfocus ? SColorHighlight : Blend(SColorDisabled, SColorPaper);
	}
}

dword ColumnList::PaintItem(Draw& w, i32 i, const Rect& r)
{
	Color ink, paper;
	dword style;
	GetItemStyle(i, ink, paper, style);
	const Item& m = item[i];
	w.Clip(r);
	(m.display ? m.display : display)->Paint(w, r, m.value, ink, paper, style);
	w.End();
	return style;
}

void ColumnList::Paint(Draw& w) {
	Size sz = GetSize();
	if(!GetColumnCx(0) || !cy) return;
	if (mode == MODE_ROWS)
		return PaintRows(w, sz);
	else {
		i32 x = 0;
		i32 i = GetSbPos(sz);
		i32 coli = 0;
		while(x < sz.cx) {
			i32 cx = GetColumnCx(coli++);
			i32 y = 0;
			while(y + cy <= sz.cy) {
				Rect rect = RectC(x, y, cx, cy);
				if(i < GetCount()) {
					if(w.IsPainting(rect)) {
						Rect r = rect;
						r.right--;
						dword style = PaintItem(w, i, r);
						w.DrawRect(rect.right - 1, rect.top, 1, rect.Height(),
						           x + cx < sz.cx ? SColorDisabled : SColorPaper);
						if(i == cursor && selcount != 1 && multi && item[i].canselect)
							DrawFocus(w, r, style & Display::SELECT ? SColorPaper() : SColorText());
					}
				}
				else
					w.DrawRect(rect, SColorPaper);
				if(i == dropitem && insert)
					DrawHorzDrop(w, x, y, cx);
				if(i + 1 == dropitem && insert && y + 2 * cy > sz.cy)
					DrawHorzDrop(w, x, y + cy - 2, cx);
				y += cy;
				i++;
			}
			w.DrawRect(x, y, cx, sz.cy - y, SColorPaper);
			x += cx;
		}
	}
	if(HasCapture())
		w.DrawRect(mpos - dx, 0, 1, sz.cy, Blend(SColorHighlight, SColorFace));
	(mode == MODE_COLUMN) ?
		scroller.Set(Point(sb, 0)) :
		scroller.Set(sb);
}

void ColumnList::PaintRows(Draw &w, Size &sz)
{
	i32 pos = sb % cy;
	i32 y = -pos;
	i32 i = GetSbPos(sz);
	while(y < sz.cy-pos+cy) {
		i32 x = 0;
		while (x + cx <= sz.cx) {
			Rect rect = RectC(x, y, cx, cy);
			if(i < GetCount()) {
				if(w.IsPainting(rect)) {
					Rect r = rect;
					dword style = PaintItem(w, i, r);
//					w.DrawRect(rect.right - 1, rect.top, 1, rect.Height(),
//					           x + cx < sz.cx ? SColorDisabled : SColorPaper);
					if(i == cursor && selcount != 1 && multi && item[i].canselect)
						DrawFocus(w, r, style & Display::SELECT ? SColorPaper() : SColorText());
				}
			}
			else
				w.DrawRect(rect, SColorPaper);
			if(i == dropitem && insert)
				DrawHorzDrop(w, x, y, cx);
			if(i + 1 == dropitem && insert && y + 2 * cy > sz.cy)
				DrawHorzDrop(w, x, y + cy - 2, cx);
			x += cx;
			i++;
		}
		w.DrawRect(x, y, cx, sz.cy - y, SColorPaper);
		y += cy;
	}
}

Image ColumnList::GetDragSample()
{
	Size sz = StdSampleSize();
	ImageDraw iw(sz);
	i32 y = 0;
	for(i32 i = 0; i < GetCount() && y < sz.cy; i++)
		if(IsSel(i)) {
			PaintItem(iw, i, RectC(0, y, sz.cx, cy));
			y += cy;
		}
	return Crop(iw, 0, 0, sz.cx, y);;
}

i32  ColumnList::GetPageItems() const
{
	return ncl * GetColumnItems();
}

void ColumnList::SetSb()
{
	switch (mode) {
	case MODE_LIST:
		sb.SetTotal(GetCount());
		sb.SetPage(GetPageItems());
		sb.SetLine(1);
		break;
	case MODE_COLUMN: {
		i32 icnt = max(1, GetColumnItems());
		i32 ccnt = GetCount()/icnt;
		ccnt += (GetCount() % icnt) ? 1 : 0;
		sb.SetTotal(ccnt);
		sb.SetPage(ncl);
		sb.SetLine(1);
		break;
		}
	case MODE_ROWS:	{
		i32 rcnt = GetCount()/ncl;
		rcnt += (GetCount() % ncl) ? 1 : 0;
		sb.SetTotal(rcnt*cy);
		sb.SetPage((GetSize().cy/cy)*cy);
		sb.SetLine(cy);
		break;
		}
	}
}

void ColumnList::ScrollInto(i32 pos)
{
	switch(mode) {
	case MODE_ROWS:
		sb.ScrollInto((pos / ncl) * cy, max(0, sb.GetLine() - (GetSize().cy - sb.GetPage())));
		return;
	case MODE_COLUMN:
		sb.ScrollInto(pos / max(1, GetColumnItems()));
		return;
	case MODE_LIST:
		sb.ScrollInto(pos);
		return;
	}
}

void ColumnList::Layout()
{
	if (mode == MODE_ROWS)
		ncl = max(GetSize().cx / cx, 1);
	SetSb();
}

void ColumnList::Scroll()
{
	Size sz = GetSize();
	switch (mode) {
	case MODE_LIST:
		sz.cy = sz.cy / cy * cy;
		scroller.Scroll(*this, sz, sb, cy);
		break;
	case MODE_COLUMN:
		sz.cy = sz.cy / cy * cy;
		scroller.Scroll(*this, sz, Point(sb, 0), Size(GetColumnCx(0), 0));
		break;
	case MODE_ROWS:
		scroller.Scroll(*this, sz, sb, 1);
		break;
	}
	info.Cancel();
}

void ColumnList::RefreshItem(i32 i, i32 ex)
{
	if(i >= 0) {
		Refresh(GetItemRect(i).InflatedVert(ex));
		SyncInfo();
	}
}

void ColumnList::SetCursor0(i32 c, bool sel)
{
	i32 c0 = cursor;
	c = minmax(c, 0, GetCount() - 1);
	if(c < 0 || cursor < 0)
		Refresh();
	else
		RefreshCursor();
	cursor = c;
	i32 q = sb;
	ScrollInto(cursor);
	if(q != sb)
		Refresh();
	else
		RefreshCursor();
	if(sel && multi) {
		ClearSelection();
		if(cursor >= 0) {
			SelectOne(cursor, true);
			anchor = cursor;
		}
	}
	if(c0 != cursor) {
		if(cursor >= 0)
			WhenEnterItem();
		else
			WhenKillCursor();
		WhenSel();
	}
	SyncInfo();
	Action();
}

void ColumnList::SetCursor(i32 c)
{
	SetCursor0(c, true);
}

i32 ColumnList::GetSbPos(const Size& sz) const
{
	switch (mode) {
	case MODE_ROWS:
		return (sb / cy) * ncl;
	case MODE_COLUMN:
		return (cy ? sb * (sz.cy / cy) : 0);
	case MODE_LIST:
		return sb;
	}
	return sb;
}

void ColumnList::SetSbPos(i32 y)
{
	SetSb();
	sb = minmax(y, 0, GetCount() - GetPageItems());
}

void ColumnList::KillCursor()
{
	if(cursor < 0) return;
	Refresh();
	SyncInfo();
	cursor = -1;
	sb.Begin();
	WhenKillCursor();
	WhenSel();
	Action();
}

void ColumnList::GotFocus()
{
	if(cursor < 0 && GetCount())
		SetCursor(GetSbPos());
	Refresh();
	SyncInfo();
}

void ColumnList::LostFocus()
{
	Refresh();
	SyncInfo();
}

i32 ColumnList::RoundedCy()
{
	if (mode != MODE_LIST)
	    return GetRect().GetHeight();
	Rect r = GetRect();
	Rect rr = r;
	frame->FrameLayout(r);
	return (cy ? r.Height() / cy * cy : 0) + rr.Height() - r.Height();
}

void ColumnList::FrameLayout(Rect& r)
{
	r.bottom = r.top + RoundedCy();
	frame->FrameLayout(r);
}

void ColumnList::FrameAddSize(Size& sz)
{
	frame->FrameAddSize(sz);
}

void ColumnList::FramePaint(Draw& draw, const Rect& r)
{
	i32 ry = RoundedCy();
	Rect rr = r;
	rr.bottom = ry;
	frame->FramePaint(draw, rr);
	rr = r;
	rr.top = ry;
	draw.DrawRect(rr, SColorFace);
}

ColumnList& ColumnList::RoundSize(bool b)
{
	if(b)
		Ctrl::SetFrame(0, *this);
	else
		Ctrl::SetFrame(0, *frame);
	RefreshLayout();
	Refresh();
	SyncInfo();
	return *this;
}

void ColumnList::SetFrame(CtrlFrame& _frame)
{
	frame = &_frame;
	RefreshLayout();
	Refresh();
	SyncInfo();
}

struct ColumnList::ItemOrder {
	const ValOrder *order;
	bool operator()(const Item& a, const Item& b) const {
		return (*order)(a.value, b.value);
	}
};

void ColumnList::Sort(const ValOrder& order)
{
	ItemOrder itemorder;
	itemorder.order = &order;
	KillCursor();
	DRX::Sort(item, itemorder);
	sb.Begin();
	SyncInfo();
}

Val ColumnList::GetData() const
{
	if (!multi)
		return (cursor >= 0) ? Get(cursor) : Val();
	for (i32 i = 0; i < item.GetCount(); i++)
		if (item[i].sel)
			return item[i].key;
	return Val();
}

void ColumnList::SetData(const Val& key)
{
	i32 ii = Find(key);
	if (ii >= 0) {
		SelectOne(ii, true);
		SetCursor(ii);
	}
}

void ColumnList::Clear() {
	CancelMode();
	KillCursor();
	item.Clear();
	selcount = 0;
	Update();
	Refresh();
	SyncInfo();
	SetSb();
}

void ColumnList::Insert(i32 ii, const Val& val, bool canselect)
{
	Insert(ii, val, val, canselect);
}

void ColumnList::Insert(i32 ii, const Val& val, const Display& display, bool canselect)
{
	Insert(ii, val, val, display, canselect);
}

void ColumnList::Insert(i32 ii, const Val& key, const Val& val, bool canselect)
{
	i32 c = -1;
	if(cursor >= ii) {
		c = cursor + 1;
		KillCursor();
	}
	Item& m = item.Insert(ii);
	m.key = key;
	m.value = val;
	m.sel = false;
	m.canselect = canselect;
	m.display = NULL;
	Refresh();
	SyncInfo();
	SetSb();
	if(c >= 0)
		SetCursor(c);
}

void ColumnList::Insert(i32 ii, const Val& key, const Val& val, const Display& display, bool canselect)
{
	Insert(ii, key, val, canselect);
	item[ii].display = &display;
	SyncInfo();
}

void ColumnList::Set(i32 ii, const Val& val, bool canselect)
{
	Set(ii, val, val, canselect);
}

void ColumnList::Set(i32 ii, const Val& val, const Display& display, bool canselect)
{
	Set(ii, val, val, display, canselect);
}

void ColumnList::Set(i32 ii, const Val& key, const Val& val, bool canselect)
{
	Item& m = item[ii];
	m.value = val;
	m.key = key;
	m.sel = false;
	m.canselect = canselect;
	m.display = NULL;
	RefreshItem(ii);
	SyncInfo();
	SetSb();
}

void ColumnList::Set(i32 ii, const Val& key, const Val& val, const Display& display, bool canselect)
{
	Set(ii, key, val, canselect);
	item[ii].display = &display;
	SyncInfo();
}

void ColumnList::Set(const Val &key, const Val& val, const Display& display, bool canselect)
{
	i32 ii = Find(key);
	if (ii >= 0)
		Set(ii, key, val, display, canselect);
}

void ColumnList::Set(const Val &key, const Val& val, bool canselect)
{
	i32 ii = Find(key);
	if (ii >= 0)
		Set(ii, key, val, canselect);
}

void ColumnList::Remove(i32 ii)
{
	i32 c = -1;
	if(cursor >= ii) {
		c = max(ii, cursor - 1);
		KillCursor();
	}
	item.Remove(ii);
	Refresh();
	SyncInfo();
	SetSb();
	if(c >= 0 && c < GetCount())
		SetCursor(c);
}

void ColumnList::Add(const Val& val, bool canselect)
{
	Insert(item.GetCount(), val, val, canselect);
}

void ColumnList::Add(const Val& val, const Display& display, bool canselect)
{
	Add(val, val, display, canselect);
}

void ColumnList::Add(const Val& key, const Val& val, bool canselect)
{
	Insert(item.GetCount(), key, val, canselect);
}

void ColumnList::Add(const Val& key, const Val& val, const Display& display, bool canselect)
{
	Add(key, val, canselect);
	item.Top().display = &display;
}

void ColumnList::DnD(i32 _drop, bool _insert)
{
	if(dropitem != _drop || insert != _insert) {
		RefreshItem(dropitem - 1);
		RefreshItem(dropitem);
		dropitem = _drop;
		insert = _insert;
		RefreshItem(dropitem - 1);
		RefreshItem(dropitem);
	}
}

bool ColumnList::DnDInsert(i32 i, i32 py, PasteClip& d, i32 q)
{
	Rect r = GetItemRect(i);
	i32 cy = r.GetHeight();
	if(py < r.top + cy / q) {
		WhenDropInsert(i, d);
		if(d.IsAccepted()) {
			DnD(i, true);
			return true;
		}
	}
	if(py >= r.top + (q - 1) * cy / q && i < GetCount()) {
		WhenDropInsert(i + 1, d);
		if(d.IsAccepted()) {
			DnD(i + 1, true);
			return true;
		}
	}
	return false;
}

void ColumnList::DragAndDrop(Point p, PasteClip& d)
{
	i32 i = GetItem(p);
	if(i >= 0) {
		if(DnDInsert(i, p.y, d, 4))
			return;
		WhenDropItem(i, d);
		if(d.IsAccepted()) {
			DnD(i, false);
			return;
		}
		if(DnDInsert(i, p.y, d, 2))
			return;
	}
	if(GetCount() == 0 && p.y < 4 || !WhenDrop) {
		WhenDropInsert(GetCount(), d);
		if(d.IsAccepted()) {
			DnD(GetCount(), true);
			return;
		}
	}
	WhenDrop(d);
	DnD(-1, false);
}

void ColumnList::DragRepeat(Point p)
{
	sb = sb + ((mode == MODE_COLUMN) ? GetDragScroll(this, p, 1).x : GetDragScroll(this, p, 1).y);
}

void ColumnList::DragEnter()
{
	RefreshSel();
}

void ColumnList::DragLeave()
{
	DnD(-1, false);
	RefreshSel();
}

void ColumnList::RemoveSelection()
{
	KillCursor();
	for(i32 i = GetCount() - 1; i >= 0; i--)
		if(IsSel(i))
			Remove(i);
	selcount = 0;
}

i32 ColumnList::Find(const Val &key) const
{
	for (i32 i = 0; i < item.GetCount(); i++)
		if (item[i].key == key)
			return i;
	return -1;
}

void ColumnList::InsertDrop(i32 ii, const Vec<Val>& keys, const Vec<Val>& data, PasteClip& d, bool self)
{
	if(data.GetCount() == 0)
		return;
	if(d.GetAction() == DND_MOVE && self) {
		for(i32 i = GetCount() - 1; i >= 0; i--) {
			if(IsSel(i)) {
				Remove(i); // Optimize!
				if(i < ii)
					ii--;
			}
		}
		KillCursor();
		d.SetAction(DND_COPY);
	}
	item.InsertN(ii, data.GetCount());
	for(i32 i = 0; i < data.GetCount(); i++) {
		Item& m = item[ii + i];
		m.value = data[i];
		m.key = keys[i];
		m.sel = false;
		m.canselect = true;
		m.display = NULL;
	}
	Refresh();
	SyncInfo();
	SetSb();
	ClearSelection();
	SetCursor(ii + data.GetCount() - 1);
	if(IsMultiSelect())
		for(i32 i = 0; i < data.GetCount(); i++)
			SelectOne(ii + i, true);
}

void ColumnList::InsertDrop(i32 ii, const Vec<Val>& data, PasteClip& d, bool self)
{
	InsertDrop(ii, data, data, d, self);
}

void ColumnList::InsertDrop(i32 ii, const ColumnList& src, PasteClip& d)
{
	Vec<Val> keys;
	Vec<Val> data;
	for(i32 i = 0; i < src.GetCount(); i++)
		if(src.IsSel(i)) {
			data.Add(src.GetVal(i));
			keys.Add(src[i]);
		}
	InsertDrop(ii, keys, data, d, &src == this);
}

void ColumnList::InsertDrop(i32 ii, PasteClip& d)
{
	InsertDrop(ii, GetInternal<ColumnList>(d), d);
}

void ColumnList::SerializeSettings(Stream& s) {
	i32 version = 0;
	s / version;
	s / ncl;
	Refresh();
	SyncInfo();
}

/* Error-prone!
void ColumnList::Serialize(Stream& s) {
	i32 version = 1;
	s / version / ncl;
	if(version >= 1) {
		i32 cnt = item.GetCount();
		s.Magic();
		s / cnt;
		item.SetCount(cnt);
		for(i32 i = 0; i < cnt; i++) {
			Item &q = item[i];
			s % q.key % q.value % q.canselect;
			if (s.IsLoading()) {
				q.display = NULL;
				q.sel = false;
			}
		}
		s.Magic();
	}
	Refresh();
	SyncInfo();
}
*/

ColumnList& ColumnList::Mode(i32 m)
{
	mode = m;
	scroller.Clear();
	if(m == MODE_COLUMN)
		sb.Horz();
	else
		sb.Vert();
	return *this;
}

ColumnList::ColumnList() {
	clickkill = false;
	ncl = 1;
	cx = 50;
	cy = max(DPI(18), Draw::GetStdFontCy());
	cursor = -1;
	ListMode();
	AddFrame(sb);
	sb.WhenScroll = OTVET(Scroll);
	sb.AutoHide();
	Ctrl::SetFrame(ViewFrame());
	frame = &ViewFrame();
	RoundSize();
	display = &StdDisplay();
	multi = false;
	selcount = 0;
	nobg = false;
	popupex = true;
}

ColumnList::~ColumnList() {}

}
