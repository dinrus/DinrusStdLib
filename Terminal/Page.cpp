#include "Page.h"

#define LLOG(x)		// RLOG("VTPage [#" << this << "]: " << x)
#define LTIMING(x)	// RTIMING(x)

namespace drx {

VTLine::VTLine()
: invalid(true)
, wrapped(false)
{
}

void VTLine::Adjust(i32 cx, const VTCell& filler)
{
	if(cx < GetCount())
		wrapped = false;
	SetCount(cx, filler);
	invalid = true;
}

void VTLine::ShiftLeft(i32 begin, i32 end, i32 n, const VTCell& filler)
{
	Insert(end, filler, n);
	Remove(begin - 1, n);
	wrapped = false;
	invalid = true;
}

void VTLine::ShiftRight(i32 begin, i32 end, i32 n, const VTCell& filler)
{
	Insert(begin - 1, filler, n);
	Remove(end, n);
	wrapped = false;
	invalid = true;
}

bool VTLine::FillLeft(i32 begin, const VTCell& filler, dword flags)
{
	for(i32 i = 1; i <= clamp(begin, 1, GetCount()); i++)
		At(i - 1).Fill(filler, flags);
	invalid = true;
	return true;
}

bool VTLine::FillRight(i32 begin, const VTCell& filler, dword flags)
{
	for(i32 i = max(1, begin); i <= GetCount(); i++)
		At(i - 1).Fill(filler, flags);
	invalid = true;
	return true;
}

bool VTLine::Fill(i32 begin, i32 end, const VTCell& filler, dword flags)
{
	i32 n = GetCount();
	i32 b = clamp(begin, 1, n);
	i32 e = clamp(end,   1, n);

	for(i32 i = b; i <= e; i++)
		At(i - 1).Fill(filler, flags);

	bool done = b <= e;
	if(done)
		invalid = true;
	return done;
}

bool VTLine::FillLine(const VTCell& filler, dword flags)
{
	for(VTCell& l : static_cast<Vec<VTCell>&>(*this))
		l.Fill(filler, flags);
	invalid = true;
	return true;
}

const VTLine& VTLine::Void()
{
	static VTLine line;
	return line;
}

Txt VTLine::ToTxt() const
{
	return ToWTxt().ToTxt();
}

WTxt VTLine::ToWTxt() const
{
	return AsWTxt(SubRange(Begin(), End()));
}

WTxt AsWTxt(VTLine::ConstRange& cellrange, bool tspaces)
{
	WTxt txt;
	i32 j = 0;
	for(const VTCell& cell : cellrange) {
		if((cell.chr == 0 && tspaces) || cell.IsImage())
			j++;
		else {
			if(j) {
				txt.Cat(' ', j);
				j = 0;
			}
			if(cell.chr >= 32)
				txt.Cat(cell.chr, 1);
		}
	}
	return pick(txt);
}

VTPage::VTPage()
: tabsize(8)
, autowrap(false)
, reversewrap(false)
, history(false)
, historysize(1024)
, size(2, 2)
, margins(Null)
{
	Reset();
}

VTPage& VTPage::Reset()
{
	cellattrs = VTCell();
	cursor = Null;
	backup = Null;
	tabsync = false;
	SetTabs(tabsize);
	SetSize(size);
	AutoWrap(autowrap);
	Displaced(false);
	ErasePage();
	EraseHistory();
	MoveTopLeft();
	return *this;
}

VTPage& VTPage::Backup()
{
	backup = cursor;

	LLOG("Backup() -> " << cursor);
	return *this;
}

VTPage& VTPage::Discard()
{
	LLOG("Discard() -> " << backup);

	backup = Null;
	return *this;
}

VTPage& VTPage::Restore()
{
	cursor = backup;
	if(!autowrap)
		ClearEol();

	LLOG("Restore() -> " << cursor);
	return *this;
}

VTPage& VTPage::Displaced(bool b)
{
	LLOG("Displaced(" << b << ")");

	cursor.displaced = b;
	if(!b) ResetMargins();
	ClearEol();
	return *this;
}

VTPage& VTPage::AutoWrap(bool b)
{
	// Clear EOL flag only on reset
	// See: DEC STD 070-D, p. D-13

	LLOG("AutoWrap(" << b << ")");

	autowrap = b;
	if(!b) ClearEol();
	return *this;
}

VTPage& VTPage::ReverseWrap(bool b)
{
	// TODO
	LLOG("ReverseWrap(" << b << ")");

	reversewrap = b;
	if(!b) ClearEol();
	return *this;
}

VTPage& VTPage::History(bool b)
{
	LLOG("History(" << b << ")");

	history = b;
	return *this;
}

void VTPage::EraseHistory()
{
	saved.Clear();
	saved.Shrink();
	lines.Shrink();
	WhenUpdate();
}

void VTPage::SetHistorySize(i32 sz)
{
	LLOG("HistorySize(" << sz << ")");

	historysize = max(1, sz);
	AdjustHistorySize();
}

void VTPage::AdjustHistorySize()
{
	i32 count = saved.GetCount();
	if(count > historysize) {
		saved.DropHead(count - historysize);
		LLOG("AdjustHistorySize() -> Before: " << count << ", after: " << saved.GetCount());
	}
}

bool VTPage::SaveToHistory(i32 pos)
{
	if(margins != GetView())
		return false;
	AdjustHistorySize();
	saved.AddTail(pick(lines[pos - 1]));
	return true;
}

void VTPage::UnwindHistory(const Size& prevsize)
{
	i32 delta =  min(size.cy - prevsize.cy, saved.GetCount());
	while(delta-- > 0) {
		lines.Insert(0, pick(saved.Tail()));
		saved.DropTail();
		cursor.y++;
	}
}

void VTPage::RewindHistory(const Size& prevsize)
{
	i32 delta = min(cursor.y - size.cy, lines.GetCount());
	while(delta-- > 0) {
		saved.AddTail(pick(lines[0]));
		lines.Remove(0, 1);
	}
}

VTPage& VTPage::SetSize(Size sz)
{
	Size oldsize = GetSize();
	size = Nvl(sz, Size(2, 2));
	if(oldsize != size || HorzMarginsExist() || VertMarginsExist())
		ResetMargins();
	if(lines.IsEmpty())
		cursor.Clear();
	if(HasHistory()) {
		if(oldsize.cy < size.cy)
			UnwindHistory(oldsize);
		else
		if(oldsize.cy > size.cy)
			RewindHistory(oldsize);
		AdjustHistorySize();
	}
	lines.SetCount(size.cy);
	for(VTLine& line : lines) {
		if(line.GetCount() < size.cx)
			line.Adjust(size.cx, cellattrs);
		line.Invalidate();
	}
	if(tabsync)
		SetTabs(tabsize);
	return MoveTo(cursor);
}

Point VTPage::GetPos() const
{
	Point pt(cursor);
	pt.y += saved.GetCount();

	LLOG("GetPos() -> " << pt);
	return pt;
}

Point VTPage::GetRelPos() const
{
	Point pt(cursor);

	if(cursor.displaced) {
		pt.x += 1 - margins.left;
		pt.y += 1 - margins.top;
	}

	LLOG("GetRelPos() -> " << pt);
	return pt;
}

inline Point VTPage::Bind(const Rect& r, const Point& pt) const
{
	return Point(pt.x < r.left ? r.left : pt.x > r.right ? r.right : pt.x,
                 pt.y < r.top ? r.top : pt.y > r.bottom ? r.bottom : pt.y);
}

inline bool VTPage::ViewContains(const Point& pt) const
{
	Rect view = GetView();
	return view.left   <= pt.x
		&& view.right  >= pt.x
		&& view.top    <= pt.y
		&& view.bottom >= pt.y;
}

inline bool VTPage::MarginsContain(const Point& pt) const
{
	return margins.left   <= pt.x
		&& margins.right  >= pt.x
		&& margins.top    <= pt.y
		&& margins.bottom >= pt.y;
}

void VTPage::Invalidate(i32 begin, i32 end)
{
	i32 b = min(begin, end);
	i32 e = max(begin, end);
	for(i32 i = b; i < e; i++)
		lines[i].Invalidate();
}

VTPage& VTPage::SetCell(i32 x, i32 y, const VTCell& cell)
{
	if(ViewContains(Point(x, y)))
	{
		VTLine& line = lines[y - 1];
		line[x - 1] = cell;
		line.Invalidate();
	}
	return *this;
}

const VTCell& VTPage::GetCell(i32 x, i32 y) const
{
	return ViewContains(Point(x, y)) ? lines[y - 1][x - 1] : VTCell::Void();
}

i32 VTPage::CellAdd(const VTCell& cell, i32 width)
{
	if(width <= 0)
		return cursor.x;

	if(autowrap && cursor.eol)
	{
		lines[cursor.y - 1].Wrap();
		NewLine();
	}

	SetCell(cell);
	i32 next = cursor.x + 1;

	if(next <= margins.right)
	{
		MoveRight();
		if(width == 2)
		{
			VTCell ext = cell;
			ext.chr = 1;
			next = CellAdd(ext);
		}
	}
	else
		SetEol();

	return next;
}

VTPage& VTPage::InsertCell(const VTCell& cell)
{
	LLOG("InsertCell()");

	i32 width = cell.GetWidth();
	if(width > 0) InsertCells(cursor.x, width);
	CellAdd(cell, width);
	return *this;
}

VTPage& VTPage::RepeatCell(i32 n)
{
	LLOG("RepeatCell(" << n << ")");

	const VTCell& cell = GetCell(cursor.x - 1, cursor.y);
	for(i32 i = 0, w = cell.GetWidth(); i < n; i++)
		CellAdd(cell, w);
	return *this;
}

VTPage& VTPage::RewrapCursor(i32 n)
{
	LLOG("RewrapCursor(" << n << ")");

	i32 maxrewrap = margins.Width() * margins.Height() * 4;

	return margins.top == cursor.y
		? MoveBottomRight().MoveLeft(min(n - 1, maxrewrap))
		: MoveEnd().MoveUp().MoveLeft(min(n - 1, maxrewrap));
}

VTPage& VTPage::MoveHorz(i32 pos, dword flags)
{
	Rect view = GetView();
	i32  offset = 1;
	i32  left   = 1;
	i32  right  = 1;
	bool scrollable = flags & Cursor::Scroller;

	LTIMING("VTPage::MoveHorz");

	if(flags & Cursor::Displaceable && cursor.displaced) {
		offset = left = margins.left;
		right  = margins.right;
	}
	else
	if(flags & Cursor::Marginal && HorzMarginsExist()) {
		if(HorzMarginsContain(cursor.x)) {
			offset = left = margins.left;
			right  = margins.right;
		}
		else {
			if(cursor.x < margins.left) {
				offset = left = view.left;
				right  = margins.right;
				
			}
			else
			if(cursor.x > margins.right) {
				left   = margins.left;
				right  = view.right;
				offset = view.left;
			}
			if(scrollable)
				scrollable = false;
		}
	}
	else { // flags == Cursor::Absolute
		offset = left = view.left;
		right  = view.right;
	}

	pos = GetNextColPos(pos, offset, flags & Cursor::Relative);
	
	if(reversewrap && (flags & Cursor::ReWrapper) && pos < left) {
		return RewrapCursor(left - pos);
	}
	
	cursor.x = clamp(pos, left, right);

	if(scrollable) {
		if(pos < left)
			CellInsert(left, left - pos, cellattrs, true);
		else
		if(pos > right)
			CellRemove(left, pos - right, cellattrs, true);
	}

	ClearEol();
	return *this;
}

VTPage& VTPage::MoveVert(i32 pos, dword flags)
{
	Rect view = GetView();
	i32  offset = 1;
	i32  top    = 1;
	i32  bottom = 1;
	bool scrollable = flags & Cursor::Scroller;

	LTIMING("VTPage::MoveVert");

	if(flags & Cursor::Displaceable && cursor.displaced) {
		offset = top = margins.top;
		bottom = margins.bottom;
	}
	else
	if(flags & Cursor::Marginal && VertMarginsExist()) {
		if(VertMarginsContain(cursor.y)) {
			offset = top = margins.top;
			bottom = margins.bottom;
		}
		else {
			if(cursor.y < margins.top) {
				offset = top = view.top;
				bottom = margins.bottom;
			}
			else
			if(cursor.y > margins.bottom) {
				top    = margins.top;
				bottom = view.bottom;
				offset = view.top;
			}
			if(scrollable)
				scrollable = false;
		}
	}
	else { // flags == Cursor::Absolute
		offset = top = view.top;
		bottom = view.bottom;
	}

	pos = GetNextRowPos(pos, offset, flags & Cursor::Relative);
	cursor.y = clamp(pos, top, bottom);

	if(scrollable) {
		if(pos < top) {
			if(LineInsert(top, top - pos, cellattrs) > 0)
				WhenUpdate();
		}
		else
		if(pos > bottom) {
			if(LineRemove(top, pos - bottom, cellattrs) > 0)
				WhenUpdate();
		}
	}

	ClearEol();
	return *this;
}

VTPage& VTPage::MoveTo(i32 x, i32 y)
{
	LLOG("MoveTo(" << x << ", " << y << ")");

	return MoveVert(y, Cursor::Displaceable)
          .MoveHorz(x, Cursor::Displaceable);
}

VTPage& VTPage::MoveToLine(i32 n, bool relative)
{
	LLOG("MoveToLine(" << n <<")");

	return MoveVert(n,
		Cursor::Displaceable
		| (relative ? Cursor::Relative : 0));
}

VTPage& VTPage::MoveToColumn(i32 n, bool relative)
{
	LLOG("MoveToColumn(" << n << ")");
;
	return MoveHorz(n,
		Cursor::Displaceable
		| (relative ? Cursor::Relative : 0));
}

VTPage& VTPage::MoveUp(i32 n)
{
	LLOG("MoveUp(" << n << ")");

	return MoveVert(-n,
		Cursor::Marginal
		| Cursor::Relative
		| Cursor::Displaceable);
}

VTPage& VTPage::MoveDown(i32 n)
{
	LLOG("MoveDown(" << n << ")");

	return MoveVert(n,
		Cursor::Marginal
		| Cursor::Relative
		| Cursor::Displaceable);
}

VTPage& VTPage::MoveRight(i32 n)
{
	LLOG("MoveRight(" << n << ")");

	return MoveHorz(n,
		Cursor::Marginal
		| Cursor::Relative
		| Cursor::Displaceable);
}

VTPage& VTPage::MoveLeft(i32 n)
{
	LLOG("MoveLeft(" << n << ")");

	return MoveHorz(-n,
		Cursor::Marginal
		| Cursor::Relative
		| Cursor::ReWrapper
		| Cursor::Displaceable);
}

VTPage& VTPage::MoveHome()
{
	LLOG("MoveHome()");

	return MoveHorz(1,
		Cursor::Marginal
		| Cursor::Displaceable);
}

VTPage& VTPage::MoveEnd()
{
	LLOG("MoveEnd()");

	return MoveHorz(size.cx,
		Cursor::Marginal
		| Cursor::Displaceable);
}

VTPage& VTPage::MoveTopLeft()
{
	LLOG("MoveTopLeft()");

	dword flags = Cursor::Marginal
                | Cursor::Displaceable;

	return MoveHorz(1, flags)
		  .MoveVert(1, flags);
}

VTPage& VTPage::MoveBottomRight()
{
	LLOG("MoveBottomRight()");

	dword flags = Cursor::Marginal
                | Cursor::Displaceable;

	return MoveHorz(size.cx, flags)
		  .MoveVert(size.cy, flags);
}

VTPage& VTPage::NextLine(i32 n)
{
	LLOG("NextLine(" << n << ")");

	return MoveVert(n,
		Cursor::Marginal
		| Cursor::Relative
		| Cursor::Scroller
		| Cursor::Displaceable);
}

VTPage& VTPage::PrevLine(i32 n)
{
	LLOG("PrevLine(" << n << ")");

	return MoveVert(-n,
		Cursor::Marginal
		| Cursor::Relative
		| Cursor::Scroller
		| Cursor::Displaceable);
}

VTPage& VTPage::NextColumn(i32 n)
{
	LLOG("NextColumn(" << n << ")");

	return MoveHorz(n,
		Cursor::Marginal
		| Cursor::Relative
		| Cursor::Scroller
		| Cursor::Displaceable);
}

VTPage& VTPage::PrevColumn(i32 n)
{
	LLOG("PrevColumn(" << n << ")");

	return MoveHorz(-n,
		Cursor::Marginal
		| Cursor::Relative
		| Cursor::Scroller
		| Cursor::Displaceable);
}

i32 VTPage::LineInsert(i32 pos, i32 n, const VTCell& attrs)
{
	i32 scrolled = 0;

	n = clamp(n, 0, margins.bottom - pos + 1);

	LTIMING("VTPage::LineInsert");

	if(n > 0)
	{
		LLOG("LineInsert(" << pos << ") -> scrolling " << n << " lines up");

		if(HorzMarginsExist())
		{
			Rect r = margins;
			r.top = pos;
			r.bottom -= n;
			RectCopy(Point(r.left, pos + n), r, margins);
			r.top = pos;
			r.bottom = pos + n - 1;
			RectFill(r, cellattrs, VTCell::FILL_NORMAL);
			scrolled++;
		}
		else {
			for(i32 i = 0; i < n; i++)
			{
				VTLine& line = lines.Insert(pos - 1);
				line.Adjust(size.cx, attrs);
				lines.Remove(margins.bottom);
				scrolled++;
			}
		}

		Invalidate(pos - 1, margins.bottom);
		ClearEol();
	}

	return scrolled;
}

i32 VTPage::LineRemove(i32 pos, i32 n, const VTCell& attrs)
{
	i32 scrolled = 0;

	n = clamp(n, 0, margins.bottom - pos + 1);

	LTIMING("VTPage::LineRemove");

	if(n > 0)
	{
		LLOG("LineRemove(" << pos << ") -> scrolling " << n << " lines down");

		if(HorzMarginsExist())
		{
			Rect r = margins;
			r.top  = pos + n;
			RectCopy(margins.TopLeft(), r, margins);
			r.top = margins.bottom - n + 1;
			RectFill(r, cellattrs, VTCell::FILL_NORMAL);
			scrolled++;
		}
		else {
			for(i32 i = 0; i < n; i++)
			{
				VTLine& line = lines.Insert(margins.bottom);
				line.Adjust(size.cx, attrs);
				if(history && GetAbsRow(pos) == 1)
				{
					SaveToHistory(pos);
				}
				lines.Remove(pos - 1);
				scrolled++;
			}
		}

		Invalidate(pos - 1, margins.bottom);
		ClearEol();
	}

	return scrolled;
}

VTPage& VTPage::ScrollUp(i32 n)
{
	if(LineInsert(margins.top, n, cellattrs) > 0)
		WhenUpdate();
	return *this;
}

VTPage& VTPage::ScrollDown(i32 n)
{
	if(LineRemove(margins.top, n, cellattrs) > 0)
		WhenUpdate();
	return *this;
}

i32 VTPage::CellInsert(i32 pos, i32 n, const VTCell& attrs, bool pan)
{
	i32 shifted = 0;

	n = clamp(n, 0, margins.right - pos + 1);

	LTIMING("VTPage::CellInsert");

	if(n > 0)
	{
		if(pan)
		{
			for(i32 row = margins.top; row <= margins.bottom; row++)
			{
				lines[row - 1].ShiftRight(pos, margins.right, n, attrs);
				shifted++;
			}
		}
		else
		{
			i32 row = cursor.y - 1;
			lines[row].ShiftRight(pos, margins.right, n, attrs);
			shifted++;
		}

		ClearEol();
	}

	return shifted;
}

i32 VTPage::CellRemove(i32 pos, i32 n, const VTCell& attrs, bool pan)
{
	i32 shifted = 0;

	n = clamp(n, 0, margins.right - pos + 1);

	LTIMING("VTPage::CellRemove");

	if(n > 0)
	{
		if(pan)
		{
			for(i32 row = margins.top; row <= margins.bottom; row++)
			{
				lines[row - 1].ShiftLeft(pos, margins.right, n, attrs);
				shifted++;
			}
		}
		else
		{
			i32 row = cursor.y - 1;
			lines[row].ShiftLeft(pos, margins.right, n, attrs);
			shifted++;
		}

		ClearEol();
	}

	return shifted;
}

VTPage& VTPage::ScrollLeft(i32 n)
{
	if(MarginsContain(cursor))
		CellInsert(margins.left, n, cellattrs, true);
	return *this;
}

VTPage& VTPage::ScrollRight(i32 n)
{
	if(MarginsContain(cursor))
		CellRemove(margins.left, n, cellattrs, true);
	return *this;
}

VTPage& VTPage::PanLeft(i32 n)
{
	if(MarginsContain(cursor))
		CellRemove(cursor.x, n, cellattrs, true);
	return *this;
}

VTPage& VTPage::PanRight(i32 n)
{
	if(MarginsContain(cursor))
		CellInsert(cursor.x, n, cellattrs, true);
	return *this;
}

VTPage& VTPage::InsertLines(i32 pos, i32 n)
{
	LLOG("InsertLines(" << pos << ", " << n << ")");

	if(MarginsContain(cursor) && LineInsert(pos, n, cellattrs) > 0) {
		MoveHome();
		WhenUpdate();
	}
	return *this;
}

VTPage& VTPage::RemoveLines(i32 pos, i32 n)
{
	LLOG("RemoveLines(" << pos << ", " << n << ")");

	if(MarginsContain(cursor) && LineRemove(pos, n, cellattrs) > 0) {
		MoveHome();
		WhenUpdate();
	}
	return *this;
}

VTPage& VTPage::InsertCells(i32 pos, i32 n)
{
	LLOG("InsertCells(" << pos << ", " << n << ")");

	if(HorzMarginsContain(cursor.x))
		CellInsert(pos, n, cellattrs, false);
	return *this;
}

VTPage& VTPage::RemoveCells(i32 pos, i32 n)
{
	LLOG("RemoveCells(" << pos << ", " << n << ")");

	if(HorzMarginsContain(cursor.x))
		CellRemove(pos, n, cellattrs, false);
	return *this;
}

VTPage& VTPage::EraseCells(i32 n, dword flags)
{
	LLOG("EraseCells(" << n << ")");

	lines[cursor.y - 1].Fill(cursor.x, cursor.x + n - 1, cellattrs, flags);
	ClearEol();
	return *this;
}

VTPage& VTPage::EraseLine(dword flags)
{
	LLOG("EraseLine(" << flags << ")");
	
	lines[cursor.y - 1].FillLine(cellattrs, flags);
	ClearEol();
	return *this;
}

VTPage& VTPage::EraseLeft(dword flags)
{
	LLOG("EraseLeft(" << flags << ")");
	
	lines[cursor.y - 1].FillLeft(cursor.x, cellattrs, flags);
	ClearEol();
	return *this;
}

VTPage& VTPage::EraseRight(dword flags)
{
	LLOG("EraseRight(" << flags << ")");
	
	lines[cursor.y - 1].FillRight(cursor.x, cellattrs, flags);
	ClearEol();
	return *this;
}

VTPage& VTPage::ErasePage(dword flags)
{
	LLOG("ErasePage(" << flags << ")");
	
	Rect r = GetView();
	for(i32 i = r.top; i <= r.bottom; i++)
		lines[i - 1].FillLine(cellattrs, flags);
	ClearEol();
	return *this;
}

VTPage& VTPage::EraseBefore(dword flags)
{
	LLOG("EraseBefore(" << flags << ")");
	
	for(i32 i = 1; i < cursor.y; i++)
		lines[i - 1].FillLine(cellattrs, flags);
	return EraseLeft(flags);
}

VTPage& VTPage::EraseAfter(dword flags)
{
	LLOG("EraseAfter(" << flags << ")");
	
	for(i32 i = cursor.y + 1; i <= size.cy; i++)
		lines[i - 1].FillLine(cellattrs, flags);
	return EraseRight(flags);
}

i32 VTPage::SetTabStop(i32 col, bool b)
{
	LLOG("SetTabStop(" << col << ", "<< b << ")");

	tabs.Set(col, b);
	return ++col;
}

VTPage& VTPage::SetTabs(i32 tsz)
{
	LLOG("SetTabs(" << tsz << ")");

	tabsize = tsz;
	tabs.Clear();
	for(i32 i = 1; i <= size.cx; i += tabsize)
		tabs.Set(i, true);
	tabsync = true;
	return *this;
}

void VTPage::GetTabs(Vec<i32>& tabstops)
{
	for(i32 i = 1; i <= size.cx; i++)
		if(IsTabStop(i))
			tabstops.Add(i);

	LLOG("GetTabs() -> " << tabstops);
}

VTPage& VTPage::NextTab(i32 n)
{
	for(i32 count = 0, i = cursor.x + 1; i <= margins.right && count < n; i++)
	{
		if(IsTabStop(i))
		{
			MoveToColumn(GetAbsCol(i));
			count++;
		}
		else
		if(i == margins.right)
		{
			MoveEnd();
		}
	}

	return *this;
}

VTPage& VTPage::PrevTab(i32 n)
{
	i32 left = cursor.displaced ? margins.left : GetView().left;

	for(i32 count = 0, i = cursor.x - 1; i >= left && count < n; i--)
	{
		if(IsTabStop(i))
		{
			MoveToColumn(GetAbsCol(i));
			count++;
		}
		else
		if(i == left)
		{
			MoveHome();
		}
	}

	return *this;
}

VTPage& VTPage::SetHorzMargins(i32 l, i32 r)
{
	Rect view = GetView();

	if(l < r
	&& l >= view.left
	&& l <= view.right
	&& r >= view.left
	&& r <= view.right
	)
	{
		margins.left  = l;
		margins.right = r;
		
		LLOG("Horizontal margins: " << Point(l, r));
	}
	else
	{
		margins.left = view.left;
		margins.right = view.right;
	}
	return MoveTo(1, 1);
}

VTPage& VTPage::SetVertMargins(i32 t, i32 b)
{
	Rect view = GetView();

	if(t < b
	&& t >= view.top
	&& t <= view.bottom
	&& b >= view.top
	&& b <= view.bottom
	)
	{
		margins.top    = t;
		margins.bottom = b;
		
		LLOG("Vertical margins: " << Point(t, b));
	}
	else
	{
		margins.top = view.top;
		margins.bottom = view.bottom;
	}

	return MoveTo(1, 1);
}

VTPage& VTPage::SetMargins(const Rect& r)
{
	if(IsNull(r))
		ResetMargins();
	else {
		SetHorzMargins(r.left, r.right);
		SetVertMargins(r.top, r.bottom);
	}
	return *this;
}

VTPage& VTPage::ResetMargins()
{
	margins = GetView();
	ClearEol();
	return *this;
}

const VTCell& VTPage::FetchCell(const Point& pt) const
{
	const VTLine& line = FetchLine(pt.y);
	return !line.IsVoid()
		&& pt.x >= 0
		&& pt.x < line.GetCount()
			? line[pt.x]
			: VTCell::Void();
}

const VTLine& VTPage::FetchLine(i32 i) const
{
	i32 count = GetLineCount();
	
	if(0 <= i && i < count)
	{
		i32 slen = saved.GetCount();
		i32 llen = lines.GetCount();
	
		if(slen && i < slen)
			return saved[i];
		else
		if(llen && i >= slen)
			return lines[i - slen];
	}
	
	return VTLine::Void();
}

bool VTPage::FetchRange(const Rect& r, Gate<const VTLine&, VTLine::ConstRange&> consumer, bool rect) const
{
	Rect rr = Rect(0, 0, size.cx, GetLineCount());
	if(IsNull(r) || !rr.Contains(r) || !consumer)
		return false;

	for(i32 i = r.top; i <= r.bottom; i++) {
		const VTLine& line = FetchLine(i);
		if(!line.IsVoid()) {
			i32 length = line.GetCount();
			i32 b = 0, e = length;
			if(r.top == r.bottom || rect) {
				b = r.left;
				e = min(length, r.right - r.left);
			}
			else
			if(r.top == i) {
				b = r.left;
				e = min(length, length - r.left);
			}
			else
			if(r.bottom == i) {
				b = 0;
				e = min(length, r.right);
			}
			auto range  = SubRange(line, b, e);
			if(consumer(line, range))
				return false;
		}
	}

	return true;
}

void VTPage::LineFill(i32 pos, i32 begin, i32 end, const VTCell& filler, dword flags)
{
	if(lines[pos - 1].Fill(begin, end, filler, flags))
		ClearEol();
}

void VTPage::RectCopy(const Point &p, const Rect& r, const Rect& rr, dword flags)
{
	LTIMING("VTPage::RectCopy");

	Rect src(Bind(rr, r.TopLeft()), Bind(rr, r.BottomRight()));
	Rect dest(p, src.GetSize());
	dest.Set(Bind(rr, dest.TopLeft()), Bind(rr, dest.BottomRight()));

	Buffer<VTCell> temp((src.Height() + 1) * (src.Width() + 1));

	for(i32 pass = 0; pass < 2; pass++)
	{
		const Rect& rx = pass == 0 ? src : dest;
		for(i32 i = rx.top, pos = 0; i <= rx.bottom; i++)
		{
			VTLine& line = lines[i - 1];
			for(i32 j = rx.left; j <= rx.right; j++, pos++)
			{
				VTCell& a = line[j - 1];
				VTCell& b = *(temp + pos);
				if(pass == 0)
					b.Fill(a, flags);
				else
					a.Fill(b, flags);
			}
			if(pass == 1)
				line.Invalidate();
		}
	}
}

void VTPage::RectFill(const Rect& r, const VTCell& filler, dword flags)
{
	for(i32 i = r.top; i <= r.bottom; i++)
		if(lines[i - 1].Fill(r.left, r.right, filler, flags))
			ClearEol();
}

Rect VTPage::AdjustRect(const Rect& r, bool displaced)
{
	if(r.top > r.bottom || r.left > r.right)
		return Null;

	if(displaced && cursor.displaced) {
		Rect rr = r.Offseted(margins.TopLeft() - 1);
		return Rect(Bind(margins, rr.TopLeft()),
                    Bind(margins, rr.BottomRight()));
	}

	Rect view = GetView();
	return Rect(Bind(view, r.TopLeft()),
                Bind(view, r.BottomRight()));
}

VTPage& VTPage::FillRect(const Rect& r, const VTCell& filler, dword flags)
{
	Rect rr = AdjustRect(r);
	if(!IsNull(rr))
		RectFill(rr, filler, flags);
	return *this;
}

VTPage& VTPage::FillRect(const Rect& r, dword chr)
{
	VTCell filler = cellattrs;
	filler.chr = chr;
	return FillRect(r, filler, VTCell::FILL_NORMAL);
}

VTPage& VTPage::EraseRect(const Rect& r, dword flags)
{
	Rect rr = AdjustRect(r);
	if(!IsNull(rr))
		RectFill(rr, cellattrs, flags);
	return *this;
}

VTPage& VTPage::CopyRect(const Point& pt, const Rect& r, dword flags)
{
	Rect rr = AdjustRect(r);
	if(!IsNull(rr)) {
		Point pp = cursor.displaced ? margins.TopLeft() + pt - 1 : pt;
		RectCopy(pp, rr, cursor.displaced ? GetMargins() : GetView());
	}
	return *this;
}

void VTPage::GetRectArea(const Rect& r, Event<Point> consumer, bool displaced)
{
	if(r.top <= r.bottom && r.left <= r.right) {
		Rect rr = AdjustRect(r, displaced);
		for(i32 y = rr.top; y <= rr.bottom; y++)
		for(i32 x = rr.left; x <= rr.right; x++)
			consumer(Point(x, y));
	}
}

VTPage& VTPage::FillStream(const Rect& r, const VTCell& filler, dword flags)
{
	LTIMING("VTPage::FillStream");

	Rect rr = AdjustRect(r);
	if(!IsNull(rr)) {
		Rect pr = cursor.displaced ? GetMargins() : GetView();
		for(i32 i = rr.top; i <= rr.bottom; i++) {
			if(rr.top == rr.bottom)
				LineFill(i, rr.left, rr.right, filler, flags);
			else {
				if(i == rr.top)
					LineFill(i, rr.left, pr.right, filler, flags);
				else
				if(i == rr.bottom)
					LineFill(i, pr.left, rr.right, filler, flags);
				else
					LineFill(i, pr.left, pr.right, filler, flags);
			}
		}
	}
	return *this;
}

VTPage& VTPage::FillStream(const Rect& r, dword chr)
{
	VTCell filler = cellattrs;
	filler.chr = chr;
	return FillStream(r, filler, VTCell::FILL_NORMAL);
}

VTPage& VTPage::AddImage(Size sz, dword imageid, bool scroll, bool relpos)
{
	LTIMING("VTPage::AddImage");

	if(!scroll) sz = min(sz, size);
	for(i32 i = 0; i < sz.cy; i++) {
		Point pt = scroll || relpos ? cursor - 1 : Point(0, i);
		VTLine& line = lines[pt.y];
		for(i32 j = 0; j < sz.cx; j++) {
			VTCell& cell = line[min(pt.x + j, size.cx - 1)];
			cell.Image();
			cell.chr = imageid;
			cell.object.col = j;
			cell.object.row = i;
		}
		line.Invalidate();
		if(scroll)
			NextLine();
		else
		if(relpos)
			MoveDown();
	}

	return *this;
}

void VTPage::Serialize(Stream& s)
{
	i32 version = 1;
	s / version;
	if(version >= 1) {
		s % tabsize;
		s % history;
		s % historysize;
	}
	
	if(s.IsLoading()) {
		historysize = max(1, historysize);
		SetTabs(tabsize);
	}
}

void VTPage::Jsonize(JsonIO& jio)
{
	jio ("TabSize", tabsize)
		("HistoryBuffer", history)
		("HistoryBufferMaxSize", historysize);

	if(jio.IsLoading()) {
		historysize = max(1, historysize);
		SetTabs(tabsize);
	}
}

void VTPage::Xmlize(XmlIO& xio)
{
	XmlizeByJsonize(xio, *this);
}

Txt VTPage::Cursor::ToTxt() const
{
	return Format(
		"[%d:%d] - Flags: displaced: %`, EOL: %",
			x, y, displaced, eol);
}

WTxt AsWTxt(const VTPage& page, const Rect& r, bool rectsel, bool tspaces)
{
	Vec<WTxt> v;
	bool wrapped = false;

	auto RangeToWTxt = [&](const VTLine& line, VTLine::ConstRange& range) -> bool
	{
		WTxt s = AsWTxt(range, tspaces);
		if(!rectsel && (v.GetCount() && wrapped))
			v.Top() << s;
		else
			v.Add() << s;
		wrapped = line.IsWrapped();
		return false;
	};

	#ifdef TARGET_WIN32
	constexpr tukk VT_EOL = "\r\n";
	#else
	constexpr tukk VT_EOL = "\n";
	#endif

	page.FetchRange(r, RangeToWTxt, rectsel);
	return pick(Join(v, VT_EOL));
}
}