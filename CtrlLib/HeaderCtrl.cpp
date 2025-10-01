#include "CtrlLib.h"

namespace drx {

HeaderCtrl::Column::Column()
{
	ratio = 1;
	visible = true;
	min = 0;
	max = INT_MAX;
	margin = 4;
	header = NULL;
	SetAlign(ALIGN_LEFT);
	paper = Null;
	index = Null;
	NoWrap();
}

HeaderCtrl::Column&  HeaderCtrl::Column::SetMargin(i32 m)
{
	if(m != margin) {
		margin = m;
		LabelUpdate();
	}
	return *this;
}

HeaderCtrl::Column&  HeaderCtrl::Column::SetRatio(double wd)
{
	ratio = wd ? wd : 1;
	LabelUpdate();
	return *this;
}

void  HeaderCtrl::Column::LabelUpdate()
{
	if(header) {
		header->RefreshParentLayout();
		header->Refresh();
		header->WhenLayout();
		header->SbTotal();
	}
}

CH_STYLE(HeaderCtrl, Style, StyleDefault)
{
	CtrlsImageLook(look, CtrlsImg::I_HTB);
	gridadjustment = 0;
	Point p = Button::StyleNormal().pressoffset;
	pressoffset = p.x || p.y;
}

void HeaderCtrl::Column::Paint(bool& first, Draw& w,
                               i32 x, i32 y, i32 cx, i32 cy, bool disabled, bool push, bool hl)
{
	const HeaderCtrl::Style *style = header ? header->style : &HeaderCtrl::StyleDefault();;
	if(first) {
		if(cx >= -style->gridadjustment) {
			cx -= style->gridadjustment;
			if(cx < 0)
				cx = 0;
			first = false;
		}
	}
	else
		x -= style->gridadjustment;
	bool p = push && style->pressoffset;
	i32 q = CTRL_NORMAL;
	if(hl)
		q = CTRL_HOT;
	if(push)
		q = CTRL_PRESSED;
	if(disabled)
		q = CTRL_DISABLED;
	ChPaint(w, x, y, cx, cy, IsNull(paper) ? style->look[q] : paper.operator Val());
	x += margin;
	cx -= 2 * margin;
	w.Clip(x + 2, y, cx - 4, cy);
	PaintLabel(w, x + 2, y + 1, cx - 4, cy - 4, disabled, p);
	w.End();
}

HeaderCtrl::Column& HeaderCtrl::Tab(i32 i) {
	if(i >= col.GetCount())
		for(i32 j = col.GetCount(); j <= i; j++) {
			Column& c = col.Add();
			c.header = this;
			c.index = i;
		}
	return col[i];
}

void HeaderCtrl::SetHeight(i32 cy)
{
	height = cy;
	RefreshParentLayout();
}

i32  HeaderCtrl::GetHeight() const
{
	i32 cy = 0;
	for(i32 i = 0; i < col.GetCount(); i++)
		cy = max(cy, col[i].GetLabelSize().cy);
	return max(height, cy + 4);
}

HeaderCtrl::Column& HeaderCtrl::Add()
{
	oszcx = -1;
	return Tab(col.GetCount());
}

HeaderCtrl::Column& HeaderCtrl::Add(tukk text, double ratio)
{
	HeaderCtrl::Column& c = Add();
	c.ratio = ratio ? Zxf(ratio) : 1;
	c.SetText(text);
	SbTotal();
	return c;
}

void HeaderCtrl::SetTabRatio(i32 i, double ratio)
{
	col[i].ratio = Zxf(ratio);
	oszcx = -1;
	SbTotal();
	Refresh();
	WhenLayout();
}

void HeaderCtrl::SwapTabs(i32 first, i32 second)
{
	col.Swap(first, second);
	oszcx = -1;
	Refresh();
	WhenLayout();
}

void HeaderCtrl::MoveTab(i32 from, i32 to)
{
	col.Move(from, to);
	oszcx = -1;
	Refresh();
	WhenLayout();
}

double HeaderCtrl::Denominator() const {
	double rs = 0;
	for(i32 i = 0; i < col.GetCount(); i++)
		if(col[i].visible)
			rs += col[i].ratio;
	return rs;
}

void HeaderCtrl::DoSbTotal()
{
	if(mode == SCROLL) {
		i32 cx = 0;
		for(i32 i = 0; i < col.GetCount(); i++)
			if(col[i].visible)
				cx += (i32)col[i].ratio;
		sb.AutoHide(autohidesb);
		sb.SetTotal(cx);
	}
	else {
		sb.AutoHide();
		sb.SetTotal(0);
	}
}

void HeaderCtrl::SbTotal()
{
	if(HasCapture())
		return;
	DoSbTotal();
}

HeaderCtrl& HeaderCtrl::Proportional() { mode = PROPORTIONAL; SbTotal(); return *this; }
HeaderCtrl& HeaderCtrl::ReduceNext()   { mode = REDUCENEXT; SbTotal(); return *this; }
HeaderCtrl& HeaderCtrl::ReduceLast()   { mode = REDUCELAST; SbTotal(); return *this; }
HeaderCtrl& HeaderCtrl::Absolute()     { mode = SCROLL; SbTotal(); return *this; }
HeaderCtrl& HeaderCtrl::Fixed()        { mode = FIXED; SbTotal(); return *this; }

i32  HeaderCtrl::SumMin(i32 from)
{
	i32 mincx = 0;
	for(i32 i = from; i < col.GetCount(); i++)
		if(col[i].visible)
			mincx += col[i].min;
	return mincx;
}

i32  HeaderCtrl::SumMax(i32 from)
{
	i32 maxcx = 0;
	for(i32 i = from; i < col.GetCount(); i++)
		if(col[i].visible) {
			if(col[i].max == INT_MAX)
				return INT_MAX;
			maxcx += col[i].max;
		}
	return maxcx;
}

void HeaderCtrl::ReCompute()
{
	i32 szcx = GetSize().cx;
	tabrect.Clear();
	Rect r;
	r.top = 0;
	r.bottom = GetSize().cy;
	r.right = 0;
	if(mode == SCROLL)
		for(i32 i = 0; i < col.GetCount(); i++) {
			r.left = r.right;
			if(col[i].visible)
				r.right += (i32)col[i].ratio;
			tabrect.Add(r);
		}
	else {
		double rs = Denominator();
		double rr = 0;
		double eps = rs / 2.0e5;
		Vec<i32> ci = GetVisibleCi(0);
		for(i32 i = 0; i < ci.GetCount() - 1; i++) {
			i32 cii = ci[i];
			r.left = r.right;
			while(tabrect.GetCount() < cii)
				tabrect.Add(r);
			rr += rs ? col[cii].ratio : 1;
			r.right = i32(rr * szcx / (rs ? rs : ci.GetCount()) + eps);
			tabrect.Add(r);
		}
		r.left = r.right;
		if(!ci.IsEmpty()) {
			i32 cii = ci.Top();
			while(tabrect.GetCount() < cii)
				tabrect.Add(r);
			r.right = szcx;
			tabrect.Add(r);
		}
		r.left = r.right;
		tabrect.SetCount(col.GetCount(), r);
	}
}

void HeaderCtrl::Distribute(const Vec<i32>& sci, double delta)
{
	Vec<i32> ci(sci, 1);
	i32 szcx = GetSize().cx;
	if(szcx == 0)
		return;
	double rs = Denominator();
	double eps = max(1.0e-6, rs / 1.0e6);
	double epsr = max(1.0e-4, rs / 1.0e4);
	bool checkmin = SumMin(0) < szcx;
	for(;;) {
		double psm = 0;
		for(i32 i = 0; i < ci.GetCount(); i++)
			psm += col[ci[i]].ratio;
		if(fabs(psm) < eps)
			return;
		double q = 1 + delta / psm;
		double x = 0;
		Vec<i32> nci;
		for(i32 i = 0; i < ci.GetCount(); i++) {
			Column& c = col[ci[i]];
			c.ratio *= q;
			if(c.ratio < epsr)
				c.ratio = epsr;
			else
			if(c.ratio < c.min * rs / szcx && checkmin) {
				c.ratio = c.min * rs / szcx;
				if(delta > 0)
					nci.Add(ci[i]);
			}
			else
			if(c.ratio > c.max * rs / szcx) {
				c.ratio = c.max * rs / szcx;
				if(delta < 0)
					nci.Add(ci[i]);
			}
			else
				nci.Add(ci[i]);
			x += c.ratio;
		}

		delta = (psm + delta) - x;

		if(fabs(delta) < eps)
			break;
		ci = pick(nci);
		if(ci.GetCount() == 0) {
			double psm = 0;
			for(i32 i = 0; i < sci.GetCount(); i++)
				psm += col[sci[i]].ratio;
			if(fabs(psm) < eps)
				return;
			double q = 1 + delta / psm;
			for(i32 i = 0; i < sci.GetCount(); i++)
				col[sci[i]].ratio *= q;
			return;
		}
	}
}

Vec<i32> HeaderCtrl::GetVisibleCi(i32 from)
{
	Vec<i32> sci;
	for(i32 i = from; i < col.GetCount(); i++)
		if(col[i].visible)
			sci.Add(i);
	return sci;
}

void HeaderCtrl::RefreshDistribution()
{
	i32 szcx = GetSize().cx;
	if(oszcx != szcx) {
		if(mode == SCROLL)
			for(i32 i = 0; i < col.GetCount(); i++) {
				Column& c = col[i];
				c.ratio = minmax((i32)c.ratio, c.min, c.max);
			}
		else {
			Distribute(GetVisibleCi(0), 0);
		}
		oszcx = szcx;
		ReCompute();
	}
}

Rect HeaderCtrl::GetTabRect(i32 q)
{
	RefreshDistribution();
	return tabrect[q];
}

i32  HeaderCtrl::GetTabWidth(i32 ci)
{
	return GetTabRect(ci).Width();
}

void HeaderCtrl::SetTabWidth0(i32 i, i32 cx)
{
	Column& c = col[i];
	i32 szcx = GetSize().cx;
	Rect ir = GetTabRect(i);
	bool checkmin = SumMin(0) < szcx;
	cx = checkmin ? minmax(cx, c.min, c.max) : min(cx, c.max);
	if(mode != SCROLL) {
		if(checkmin)
			cx = min(cx, szcx - SumMin(i + 1) - ir.left);
		cx = max(cx, szcx - SumMax(i + 1) - ir.left);
	}
	if(cx < 0)
		cx = 0;

	double rs = Denominator();
	i32 ocx = ir.Width();
	if(szcx == 0) return;

	double delta = rs * (cx - ocx) / szcx;
	if(ocx == cx) return;
	col[i].ratio += delta;

	switch(mode) {
	case PROPORTIONAL:
		Distribute(GetVisibleCi(i + 1), -delta);
		break;
	case REDUCELAST:
		for(i32 q = col.GetCount() - 1; q >= i; q--)
			Reduce(q, delta, rs, szcx, checkmin);
		break;
	case REDUCENEXT:
		for(i32 q = i + 1; q < col.GetCount(); q++)
			Reduce(q, delta, rs, szcx, checkmin);
		if(delta > 0)
			Reduce(i, delta, rs, szcx, checkmin);
		break;
	case SCROLL:
		col[i].ratio = cx;
		SbTotal();
		break;
	}

	ReCompute();
	Refresh();
}

void HeaderCtrl::Reduce(i32 i, double& delta, double rs, i32 szcx, bool checkmin)
{
	if(col[i].visible) {
		Column& c = col[i];
		double q = minmax(c.ratio - delta, checkmin ? c.min * rs / szcx : 0, c.max * rs / szcx);
		if(q < 0)
			q = 0;
		delta -= c.ratio - q;
		c.ratio = q;
	}
}

void HeaderCtrl::SetTabWidth(i32 i, i32 cx) {
	SetTabWidth0(i, cx);
	WhenLayout();
}

void HeaderCtrl::Paint(Draw& w) {
	RefreshDistribution();
	Size sz = GetSize();
	w.DrawRect(sz, SColorFace());
	bool ds = !IsShowEnabled();
	double rs = Denominator();
	double rr = 0;
	i32 x = -sb;
	light = -1;
	bool first = true;
	i32 dx = Null;
	for(i32 i = 0; i < col.GetCount(); i++) {
		if(col[i].visible) {
			Rect r;
			if(mode == SCROLL) {
				i32 cx = (i32)col[i].ratio;
				r = RectC(x, 0, cx, sz.cy);
				x += cx;
			}
			else {
				rr += rs ? col[i].ratio : 1;
				i32 xx = i32(rr * sz.cx / (rs ? rs : col.GetCount()));
				r = RectC(x, 0, i == col.GetCount() - 1 ? sz.cx - x : xx - x, sz.cy);
				x = xx;
			}
			bool mousein = HasMouseIn(r.Deflated(1, 0)) && col[i].WhenAction && pushi < 0 &&
			               !isdrag;
			if(mousein)
				light = i;
			col[i].Paint(first, w,
			             r.left, r.top, r.Width(), r.Height(), ds, push && i == pushi, mousein);
			if(isdrag && ti == i)
				dx = r.left;
		}
		if(x >= sz.cx) break;
	}
	Column h;
	h.header = this;
	h.Paint(first, w, x, 0, sz.cx - x + 5, sz.cy, false, false, false);
	if(isdrag) {
		w.DrawImage(dragx + dragd, 0, dragtab);
		DrawVertDrop(w, IsNull(dx) ? sz.cx - 2 : dx - (dx > 0), 0, sz.cy);
	}
}

void HeaderCtrl::Layout()
{
	sb.SetPage(GetSize().cx);
}

i32 HeaderCtrl::GetNextTabWidth(i32 i) {
	while(++i < col.GetCount())
		if(col[i].visible)
			return GetTabWidth(i);
	return 0;
}

i32 HeaderCtrl::GetLastVisibleTab() {
	i32 i = col.GetCount();
	while(--i >= 0)
		if(col[i].visible) return i;
	return -1;
}

i32 HeaderCtrl::GetSplit(i32 px) {
	if(!IsEnabled())
		return Null;
	RefreshDistribution();
	px += sb;
	i32 cx = GetSize().cx;
	double rs = Denominator();
	i32 n = col.GetCount();
	i32 l = GetLastVisibleTab();
	i32 D3 = DPI(3);
	i32 D4 = DPI(4);
	if(mode != SCROLL && abs(px - cx) <= D4 && n > 0 && l >= 0 && GetTabWidth(l) < D4)
		while(--n >= 0)
			if(GetTabWidth(n) >= D4)
				return n;
	double rr = 0;
	i32 x = 0;
	i32 first_move = 0;
	while(first_move < n && (!col[first_move].visible || col[first_move].min == col[first_move].max))
		first_move++;
	i32 last_move = n - 1;
	while(last_move >= 0 && (!col[last_move].visible || col[last_move].min == col[last_move].max))
		last_move--;
	if(mode == PROPORTIONAL)
		last_move--;
	for(i32 i = 0; i < n; i++) {
		Column& cl = col[i];
		if(cl.visible) {
			bool canmove = (i >= first_move && i <= last_move);
			if(cl.min == cl.max) {
				i32 j = i;
				while(++j < n && !col[j].visible)
					;
				if(j >= n || col[j].min == col[j].max)
					canmove = false;
			}
			if(mode == SCROLL) {
				x += (i32)col[i].ratio;
				if(canmove && px >= x - D3 && px < x + D3 && (i >= n - 1 || GetNextTabWidth(i) >= D4 || px < x))
					return i;
			}
			else {
				rr += rs ? col[i].ratio : 1;
				x = i32(rr * cx / (rs ? rs : col.GetCount()));
				if(canmove && px >= x - D3 && px < x + D3 && i < n - 1 && (i >= n - 1 || GetNextTabWidth(i) >= D4 || px < x))
					return i;
			}
			if(px < x)
				return -1 - i;
		}
	}
	return Null;
}

Image HeaderCtrl::CursorImage(Point p, dword) {
	if(mode == FIXED)
		return Image::Arrow();
	if(HasCapture())
		return split >= 0 ? CtrlImg::HorzPos() : Image::Arrow();
	i32 q = GetSplit(p.x);
	return q < 0 ? Image::Arrow()
	             : GetTabWidth(q) < 4 ? CtrlImg::HorzSplit()
	                                  : CtrlImg::HorzPos();
}

void HeaderCtrl::LeftDown(Point p, dword keyflags) {
#ifdef _DEBUG
	if(keyflags & K_CTRL) {
		Txt text;
		for(i32 i = 0; i < col.GetCount(); i++)
			text += Format(i ? " %d" : "%d", InvZx(GetTabWidth(i)));
		WriteClipboardText(".ColumnWidths(\"" + text + "\");");
		BeepExclamation();
	}
#endif
	split = GetSplit(p.x);
	if(IsNull(split)) return;
	SetCapture();
	if(split >= 0) {
		colRect = GetTabRect(split);
		return;
	}
	li = pushi = -1 - split;
	col[pushi].WhenLeftClick();
#ifdef _DEBUG
	if((keyflags & K_ALT) && pushi >= 0)
		WriteClipboardText(AsTxt(GetTabWidth(pushi)));
#endif
	if(pushi >= 0) {
		if(!col[pushi].WhenAction) {
			pushi = -1;
			return;
		}
		colRect = GetTabRect(pushi);
		push = true;
	}
	Refresh();
}

void HeaderCtrl::LeftDouble(Point p, dword keyflags)
{
	i32 q = GetSplit(p.x);
	if(q >= 0 || IsNull(q))
		return;
	col[-1 - q].WhenLeftDouble();
}

void HeaderCtrl::RightDown(Point p, dword)
{
	i32 q = GetSplit(p.x);
	if(q >= 0 || IsNull(q))
		return;
	q = -1 - q;
	if(col[q].WhenBar)
		MenuBar::Execute(col[q].WhenBar);
}

void HeaderCtrl::StartSplitDrag(i32 s)
{
	split = s;
	colRect = GetTabRect(split);
	SetCapture();
}

void HeaderCtrl::MouseMove(Point p, dword keyflags) {
	if(isdrag) {
		ti = GetLastVisibleTab() + 1;
		for(i32 i = 0; i < GetCount(); i++)
			if(col[i].visible) {
				Rect r = GetTabRect(i).OffsetedHorz(-sb);
				if(p.x < r.left + r.Width() / 2) {
					ti = i;
					break;
				}
			}
		dragx = p.x;
		Refresh();
		return;
	}
	i32 q = GetSplit(p.x);
	i32 cx = ~q;
	if(cx >= 0 && cx < col.GetCount() && !IsNull(col[cx].tip))
		Tip(col[cx].tip);
	if(mode == FIXED)
		return;
	q = IsNull(q) || q >= 0 ? -1 : -1 - q;
	if(q != light)
		Refresh();
	if(!HasCapture())
		return;
	Size sz = GetSize();
	i32 x = mode == SCROLL ? p.x + sb : min(sz.cx, p.x);
	if(split >= 0) {
		i32 w = x - colRect.left;
		if(w < 0) w = 0;
		if(w != GetTabWidth(split)) {
			SetTabWidth0(split, w);
			Refresh();
			if(track) {
				Sync();
				Action();
				WhenLayout();
			}
		}
	}
}

void HeaderCtrl::LeftDrag(Point p, dword keyflags)
{
	if(li < 0 || !moving) return;
	i32 n = 0;
	for(i32 i = 0; i < col.GetCount(); i++)
		if(col[i].visible)
			n++;
	if(n < 2)
		return;
	push = false;
	ti = li;
	pushi = -1;
	Refresh();
	Rect r = GetTabRect(li).OffsetedHorz(-sb);
	Size sz = r.GetSize();
	ImageDraw iw(sz.cx, sz.cy);
	bool first = true;
	col[li].Paint(first, iw, 0, 0, sz.cx, sz.cy, false, false, false);
	DrawFrame(iw, sz, SColorText());
	dragtab = iw;
	dragx = p.x;
	dragd = r.left - p.x;
	ImageBuffer ib(dragtab);
	Unmultiply(ib);
	RGBA *s = ~ib;
	RGBA *e = s + ib.GetLength();
	while(s < e) {
		s->a >>= 1;
		s++;
	}
	Premultiply(ib);
	dragtab = ib;
	isdrag = true;
}

void HeaderCtrl::MouseLeave()
{
	Refresh();
}

void HeaderCtrl::LeftUp(Point, dword) {
	if(isdrag) {
		if(li >= 0 && ti >= 0)
			MoveTab(li, ti);
		li = ti = -1;
		Refresh();
	}
	else
	if(pushi >= 0 && push)
		col[pushi].WhenAction();
	push = false;
	ti = li = pushi = -1;
	isdrag = false;
	Refresh();
	if(split >= 0 && !track) {
		Action();
		WhenLayout();
	}
	DoSbTotal();
}

void HeaderCtrl::CancelMode() {
	ti = li = split = pushi = -1;
	isdrag = push = false;
}

void HeaderCtrl::ShowTab(i32 i, bool show) {
	Column& cm = Tab(i);
	if(cm.visible == show) return;
	cm.visible = show;
	if(mode == PROPORTIONAL)
		InvalidateDistribution();
	ReCompute();
	Refresh();
	WhenLayout();
	SbTotal();
}

void HeaderCtrl::Column::Show(bool b)
{
	if(!header)
		return;
	i32 q = header->FindIndex(GetIndex());
	if(q >= 0)
		header->ShowTab(q, b);
}

i32 HeaderCtrl::FindIndex(i32 ndx) const
{
	if(ndx >= 0 && ndx < col.GetCount() && col[ndx].index == ndx) return ndx;
	for(i32 i = 0; i < col.GetCount(); i++)
		if(col[i].index == ndx)
			return i;
	return -1;
}

#ifdef COMPILER_MSC
#pragma warning(push)
#pragma warning(disable: 4700) // MSVC6 complaint about n having not been initialized
#endif

void HeaderCtrl::Serialize(Stream& s) {
	i32 version = 0x04;
	s / version;
	if(version >= 0x04) {
		i32 n = col.GetCount();
		s / n;
		if(n < 0 || n > 10000)
			s.LoadError();
		Array<Column> col2 = clone(col);
		if(s.IsLoading())
			col2.InsertN(0, n);
		for(i32 i = 0; i < n; i++) {
			i32 ndx = col2[i].index;
			s % ndx;
			if(s.IsLoading())
				for(i32 j = n; j < col2.GetCount(); j++)
					if(col2[j].index == ndx) {
						col2.Swap(i, j);
						break;
					}
			col2[i].index = ndx;
			s % col2[i].ratio;
			s % col2[i].visible;
		}
		if(s.IsLoading() && n == col.GetCount()) {
			col2.SetCount(n);
			col = pick(col2);
		}
	}
	else
	if(version < 0x01) {
		i32 n = col.GetCount();
		s / n;
		for(i32 i = 0; i < n; i++)
			if(i < col.GetCount()) {
				i32 n = 1;
				s / n;
				col[i].ratio = n;
			}
			else {
				i32 dummy = 0;
				s / dummy;
			}
	}
	else {
		i32 n = col.GetCount();
		s / n;
		if(version < 0x02)
			for(i32 i = 0; i < n; i++)
				if(i < col.GetCount())
					s % col[i].ratio;
				else {
					i32 dummy = 0;
					s % dummy;
				}
		else {
			i32 t = 0;
			for(i32 i = 0; i < n; i++) {
				if(n == col.GetCount()) {
					i32 ndx = col[i].index;
					double r = col[i].ratio;
					s % ndx;
					s % r;
					i32 q = FindIndex(ndx);
					if(q >= 0) {
						col[q].ratio = r;
						col.Swap(t++, q);
					}
					if(version >= 0x03) {
						bool visible = IsTabVisible(i);
						s % visible;
						if(i < GetCount())
							ShowTab(i, visible);
					}
				}
				else {
					i32 dummy = 0;
					double dummy2 = 1.0;
					bool dummy3 = false;
					s % dummy;
					s % dummy2;
					if(version >= 0x03)
						s % dummy3;
				}
			}
		}
	}
	if(s.IsLoading()) {
		ReCompute();
		Refresh();
		WhenLayout();
	}
}

#ifdef COMPILER_MSC
#pragma warning(pop)
#endif

void HeaderCtrl::FrameAdd(Ctrl& parent)
{
	parent.Add(*this);
	parent.Add(sb);
}

void HeaderCtrl::FrameRemove()
{
	Remove();
	sb.Remove();
}

void HeaderCtrl::FrameLayout(Rect& r)
{
	LayoutFrameTop(r, this, invisible ? 0 : GetHeight());
	LayoutFrameBottom(r, &sb, sb.IsShown() ? ScrollBarSize() : 0);
}

void HeaderCtrl::FrameAddSize(Size& sz)
{
	if(!invisible)
		sz.cy += GetHeight();
	if(sb.IsVisible())
		sz.cy += ScrollBarSize();
}

HeaderCtrl& HeaderCtrl::Invisible(bool inv)
{
	invisible = inv;
	if(InFrame() && GetParent())
		RefreshParentLayout();
	return *this;
}

void HeaderCtrl::Reset()
{
	col.Clear();
	track = true;
	mode = PROPORTIONAL;
	oszcx = -1;
	invisible = false;
	height = 0;
	style = &StyleDefault();
	Refresh();
	moving = false;
}

void HeaderCtrl::WScroll()
{
	WhenLayout();
}

void HeaderCtrl::Scroll()
{
	Refresh();
	WhenScroll();
}

void HeaderCtrl::ScrollVisibility()
{
	WhenScrollVisibility();
}

HeaderCtrl::HeaderCtrl() {
	Reset();
	NoWantFocus();
	sb.AutoHide();
	sb.SetLine(StdFont()['Y']);
	autohidesb = true;
	sb.WhenScroll = OTVET(Scroll);
	WhenScroll = OTVET(WScroll);
	sb.WhenVisibility = OTVET(ScrollVisibility);
}

HeaderCtrl::~HeaderCtrl() {}

}