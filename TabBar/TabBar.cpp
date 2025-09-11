#include "TabBar.h"

#define TFILE <drx/TabBar/TabBar.t>
#include <drx/Core/t.h>

#define IMAGECLASS TabBarImg
#define IMAGEFILE <drx/TabBar/TabBar.iml>
#include <drx/Draw/iml_source.h>

namespace drx {

// AlignedFrame
void AlignedFrame::FrameLayout(Rect &r)
{
	switch(layout)
	{
		case LEFT:
			LayoutFrameLeft(r, this, framesize);
			break;
		case TOP:
			LayoutFrameTop(r, this, framesize);
			break;
		case RIGHT:
			LayoutFrameRight(r, this, framesize);
			break;
		case BOTTOM:
			LayoutFrameBottom(r, this, framesize);
			break;
	}
	r.top += border;
	r.left += border;
	r.right -= border;
	r.bottom -= border;
}

void AlignedFrame::FrameAddSize(Size& sz)
{
	sz += border * 2;
	IsVert() ? sz.cx += framesize : sz.cy += framesize;
}

void AlignedFrame::FramePaint(Draw& w, const Rect& r)
{
	if(border > 0)
	{
		Rect n = r;
		switch(layout)
		{
			case LEFT:
				n.left += framesize;
				break;
			case TOP:
				n.top += framesize;
				break;
			case RIGHT:
				n.right -= framesize;
				break;
			case BOTTOM:
				n.bottom -= framesize;
				break;
		}
		ViewFrame().FramePaint(w, n);
	}
	else
		FrameCtrl<Ctrl>::FramePaint(w, r);
}

AlignedFrame& AlignedFrame::SetFrameSize(i32 sz, bool refresh)
{
	framesize = sz;
	if (refresh) RefreshParentLayout();
	return *this;
}

void AlignedFrame::Fix(Size& sz)
{
	if(IsVert())
		Swap(sz.cx, sz.cy);
}

void AlignedFrame::Fix(Point& p)
{
	if(IsVert())
		Swap(p.x, p.y);
}

Size AlignedFrame::Fixed(const Size& sz)
{
	return IsVert() ? Size(sz.cy, sz.cx) : Size(sz.cx, sz.cy);
}

Point AlignedFrame::Fixed(const Point& p)
{
	return IsVert() ? Point(p.y, p.x) : Point(p.x, p.y);
}

// TabScrollBar
TabScrollBar::TabScrollBar()
{
	Clear();
}

void TabScrollBar::Clear()
{
	total = 0;
	pos = 0;
	ps = 0;
	start_pos = 0;
	new_pos = 0;
	old_pos = 0;
	sz.Clear();
	ready = false;
}

void TabScrollBar::UpdatePos(bool update)
{
	sz = GetSize();
	Fix(sz);
	if(total <= 0 || sz.cx <= 0)
		cs = ics = 0;
	else
	{
		cs = sz.cx / ((double) total + 0.5);
		ics = total / ((double) sz.cx);
	}
	size = sz.cx * cs;
	if(update)
		pos = new_pos - start_pos;
	if(pos < 0)
		pos = 0;
	else if(pos + size > sz.cx)
		pos = sz.cx - size;

	ps = total > sz.cx ? pos * ics : 0;
}

void TabScrollBar::Paint(Draw &w)
{
	if(!ready)
	{
		UpdatePos();
		ready = true;
	}
	Size rsz = GetSize();
	#ifdef TABBAR_DEBUG
	w.DrawRect(rsz, Red);
	#else
	w.DrawRect(rsz, White);
	#endif
	Point p;

	if(total > sz.cx) {
		p = Point(ffloor(pos), 1);
		rsz = Size(fceil(size), (IsVert() ? rsz.cx : rsz.cy) - 2);
	}
	else {
		p = Point(0, 1);
		rsz = Size(sz.cx, (IsVert() ? rsz.cx : rsz.cy) - 2);
	}
	Fix(p);
	Fix(rsz);
	w.DrawRect(p.x, p.y, rsz.cx, rsz.cy, Blue);
}

void TabScrollBar::Layout()
{
	UpdatePos(false);
}

void TabScrollBar::LeftDown(Point p, dword keyflags)
{
	SetCapture();
	Fix(p);
	old_pos = new_pos = p.x;
	if(p.x < pos || p.x > pos + size)
		start_pos = size / 2;
	else
		start_pos = tabs(p.x - pos);
	UpdatePos();
	UpdateActionRefresh();
}

void TabScrollBar::LeftUp(Point p, dword keyflags)
{
	ReleaseCapture();
	Fix(p);
	old_pos = p.x;
}

void TabScrollBar::MouseMove(Point p, dword keyflags)
{
	if(!HasCapture())
		return;

	Fix(p);
	new_pos = p.x;
	UpdatePos();
	UpdateActionRefresh();
}

void TabScrollBar::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	AddPos(-zdelta / 4, true);
	UpdateActionRefresh();
}

i32 TabScrollBar::GetPos() const
{
	return ffloor(ps);
}

void TabScrollBar::SetPos(i32 p, bool dontscale)
{
	pos = total > 0 ? dontscale ? p : iscale(p, sz.cx, total) : 0;
	UpdatePos(false);
	Refresh();
}

void TabScrollBar::AddPos(i32 p, bool dontscale)
{
	pos += total > 0 ? dontscale ? p : iscale(p, sz.cx, total) : 0;
	UpdatePos(false);
	Refresh();
}

i32 TabScrollBar::GetTotal() const
{
	return total;
}

void TabScrollBar::SetTotal(i32 t)
{
	bool upd = total < t;
	total = t;
	UpdatePos(upd);
	Refresh();
}

void TabScrollBar::AddTotal(i32 t)
{
	sz = GetSize();
	Fix(sz);
	total += t;
	if(total <= 0 || sz.cx <= 0)
		cs = ics = 0;
	else
		cs = sz.cx / ((double) total + 0.5);
	size = sz.cx * cs;
	ps = min(ps, (double)(total-sz.cx));
	pos = (i32)(ps * cs);
	old_pos = new_pos = (i32)(pos - start_pos);

	Refresh();
}

void TabScrollBar::GoEnd()
{
	pos = total;
	UpdatePos(false);
	Refresh();
}

void TabScrollBar::GoBegin()
{
	pos = 0;
	UpdatePos(false);
	Refresh();
}

void TabScrollBar::Set(const TabScrollBar& t)
{
	total = t.total;
	pos = t.pos;
	ps = t.ps;
	Refresh();
}

bool TabScrollBar::IsScrollable() const
{
	// Note: sz already 'fixed'
	return total > sz.cx && sz.cx > 0;
}

// Group

void TabBar::Group::Serialize(Stream& s)
{
	s % name % active % count % first % last;
}


// TabBar

TabBar::TabBar()
{
	Clear();

	id = 0;
	display = NULL;
	crosses = true;
	crosses_side = RIGHT;
	grouping = true;
	isctrl = false;
	isdrag = false;
	inactivedisabled = false;
	autoscrollhide = true;
	stacking = false;
	groupseps = false;
	allownullcursor = false;
	icons = true;
	mintabcount = 1;
	scrollbar_sz = TB_SBHEIGHT;
	allowreorder = true;
	style = &StyleDefault();

	// Init sorting
	groupsort = false;
	tabsort = false;
	stacksort = true;
	contextmenu = true;
	keysorter_inst.vo = &Single<StdValOrder>();
	valuesorter_inst.vo = &Single<StdValOrder>();
	stacksorter_inst.vo = &Single<StdValOrder>();
	tabsorter = &keysorter_inst;
	groupsorter = &Single<TabGroupSort>();
	stacksorter = &stacksorter_inst;

	SetAlign(TOP);
	SetFrameSize(GetHeight(false));
	BackPaint();

	ConfirmClose = [](Val) { return true; };
	ConfirmCloseAll = []() { return true; };
	ConfirmCloseSome = [](ValArray) { return true; };
}

i32 TabBar::GetLR( i32 c, i32 jd )
{
	i32 new_tab;
	if ( jd == JumpDirLeft )
		new_tab = GetPrev( c );
	else
		new_tab = GetNext( c );
	return new_tab;
}

i32 TabBar::GetTabStackLR( i32 jd )
{
	i32 nt = -1;
	if ( HasCursor() ) {
		i32 c = GetCursor();

		if ( IsStacking() ) {
			i32 c_stack = tabs[ c ].stack;
			if ( jd == JumpDirLeft )
				nt = FindStackTail( c_stack );
			else
				nt = c + 1;
		}
	}
	return nt;
}

i32 TabBar::GetTabLR( i32 jd )
{
	i32 lt = -1;
	bool js_NeedReset = true;

	if ( HasCursor() ) {
	    i32 c = GetCursor();
	    i32 tc = GetCount();

		if ( IsStacking() ) {
			i32 c_stack = tabs[ c ].stack;

			if ( jd == JumpDirRight && jump_stack.IsReset() ) {

				i32 c_stack_count = GetStackCount( c_stack );

		        if ( c_stack_count > 1 ) {
					jump_stack.Activate( c_stack_count - 1, jd );
					js_NeedReset = false;
				}
			}

			if ( jump_stack.IsReset() || ( jump_stack.Rest == 0 ) ) {
				lt = GetLR( c, jd );
				if ( ( lt >= 0 ) && ( lt < tc ) ) {

					i32 lt_stack = tabs[ lt ].stack;
					i32 lt_stack_count = GetStackCount( lt_stack );

					if ( lt_stack_count > 1 ) {
						lt = FindStackHead( lt_stack );
						jump_stack.Activate( lt_stack_count - 1, jd );
						js_NeedReset = false;
					}
				}
			} else {
				if ( jump_stack.Rest > 0 ) {
					if ( jd == jump_stack.jump_direct ) {
						lt = c + 1;
						--jump_stack.Rest;
						js_NeedReset = false;
					} else  {
						if ( jump_stack.IsFull() ) {
							lt = GetLR( c, jd );
						} else {
							lt = FindStackTail( c_stack );
							++jump_stack.Rest;
							js_NeedReset = false;
						}
					}
				}
			}
		} else /* !IsStacking() */ {
			lt = GetLR( c, jd );
		}
	}

	if ( js_NeedReset )
		jump_stack.Reset();
	return lt;
}

void TabBar::Set(const TabBar& t)
{
	CopyBaseSettings(t);

	id = t.id;

	tabs.Clear();
	tabs <<= t.tabs;
	groups.Clear();
	groups <<= t.groups;
	separators.Clear();
	separators <<= t.separators;

	group = t.group;
	stackcount = t.stackcount;

	active = t.active;
	cross = -1;
	highlight = -1;
	target = -1;

	mouse.Clear();
	oldp.Clear();

	sc.Set(t.sc);
	SetAlign(t.GetAlign());
}

i32 TabBar::GetNextId()
{
	return id++;
}

void TabBar::ContextMenu(Bar& bar)
{
	i32 ii = GetHighlight(); // Need copy to freeze it, [=, this] copies 'this' and thus reference to highlight
	if (GetCursor() >= 0 && ii >= 0 && !IsCancelClose(ii))
		bar.Add(tabs.GetCount() > mintabcount, t_("Закрыть"), [=, this] {
			if (!CancelClose(tabs[ii].key)) {
				WhenClose(tabs[ii].key);
				TabClosed(tabs[ii].key);
				tabs.Remove(ii);
				MakeGroups();
				Repos();
				SetCursor(-1);
			}
		});
	if (ii >= 0 && !IsCancelCloseAll(ii))
		bar.Add(t_("Закрыть другие"), [=, this] { CloseAll(ii); });
    if (ii >= 0 && ii < GetCount() - 1 && !IsCancelCloseAll(-1, ii + 1))
		bar.Add(t_("Закрыть вкладки справа"), [=, this] { CloseAll(-1, ii + 1); });
	if (mintabcount <= 0 && !IsCancelCloseAll(-1))
		bar.Add(t_("Закрыть все"), [=, this] { CloseAll(-1); });
	bar.Add(false, t_("Док"), [=] {});
	if(ii >= 1)
		bar.Sub(t_("Поместить слева от"), [=, this](Bar& bar) {
			for(i32 i = 0; i < ii; i++)
			   bar.Add(tabs[i].value.ToTxt(), [=, this] {
				tabs.Move(ii,i);
				SetCursor0(i);
				Repos();
				Refresh();
			});;
		});
	if(tabs.GetCount() - 2 >= ii && ii >= 0)
		bar.Sub(t_("Поместить справа за"),  [=, this](Bar& bar)  {
			for(i32 i = ii+1; i < tabs.GetCount(); i++)
				bar.Add(tabs[i].value.ToTxt(),[=, this] {
				tabs.Move(ii,i+1);
				SetCursor0(i);
				Repos();
				Refresh();
			});
		});
	if(grouping && ii >= 0) {
		if(group > 0)
			bar.Add(t_("Закрыть группу"), OTVET(CloseGroup));
		bar.Separator();
		i32 cnt = groups.GetCount();
		for(i32 i = 0; i < cnt; i++)
		{
			Txt name = Format("%s (%d)", groups[i].name, groups[i].count);
			Bar::Item &it = bar.Add(name, OTVET1(GoGrouping, i));
			if(i == group)
				it.Image(TabBarImg::CHK);
			if(i == 0 && cnt > 1)
				bar.Separator();
		}
	}
}

void TabBar::CloseAll(i32 exception, i32 last_closed)
{
	ValArray vv;
	for(i32 i = last_closed; i < tabs.GetCount(); i++)
		if(i != exception)
			vv.Add(tabs[i].key);

	if(exception < 0 && last_closed == 0 ? !ConfirmCloseAll() || CancelCloseAll()
	                                     : !ConfirmCloseSome(vv) || CancelCloseSome(vv))
		return;

	WhenCloseSome(vv);
	if(exception < 0 && last_closed == 0)
		WhenCloseAll();

	for(i32 i = tabs.GetCount() - 1; i >= last_closed; i--)
		if(i != exception) {
			if (!CancelClose(tabs[i].key) && ConfirmClose(tabs[i].key)) {
				WhenClose(tabs[i].key);
				TabClosed(tabs[i].key);
				tabs.Remove(i);
			}
		}

	SetCursor(last_closed ? last_closed - 1 : 0);

	MakeGroups();
	Repos();
	Refresh();
}

void TabBar::CloseGroup()
{
	if(group <= 0)
		return;
	Val v = GetData();
	DoCloseGroup(group);
	SetData(v);
}

bool TabBar::IsCancelClose(i32 id)
{
	if (CancelCloseAll())
		return true;

	if (CancelCloseSome) {
		ValArray vv;
		vv.Add(tabs[id].key);
		if (CancelCloseSome(vv))
			return true;
	}

	if (CancelClose(tabs[id].key))
		return true;
	return false;
}

bool TabBar::IsCancelCloseAll(i32 exception, i32 last_closed)
{
	ValArray vv;
	for(i32 i = last_closed; i < tabs.GetCount(); i++)
		if(i != exception)
			vv.Add(tabs[i].key);

	if(exception < 0 && last_closed == 0 ? CancelCloseAll() : CancelCloseSome(vv))
		return true;

	if (CancelClose) {
		for(i32 i = tabs.GetCount() - 1; i >= last_closed; i--)
			if(i != exception) {
				if (!CancelClose(tabs[i].key))
					return false;
			}
		return true;
	}
	return false;
}

TabBar::Tab::Tab()
{
	id = -1;
	stack = -1;
	visible = true;
	itn = 0;
	items.SetCount(5);

	pos = cross_pos = tab_pos = Point(0, 0);
	cross_size = size = tab_size = Size(0, 0);
}

void TabBar::Tab::Set(const Tab& t)
{
	id = t.id;

	img = t.img;
	col = t.col;
	key = t.key;
	value = t.value;
	group = t.group;

	stackid = t.stackid;
	stack = t.stack;

	visible = t.visible;

	pos = t.pos;
	size = t.size;

	cross_pos = t.cross_pos;
	cross_size = t.cross_size;

	tab_pos = t.tab_pos;
	tab_size = t.tab_size;

	items <<= t.items;
}

void TabBar::Tab::Serialize(Stream& s)
{
	s % id % key % value % group % stackid % stack % visible;
}

bool TabBar::Tab::HasMouse(const Point& p) const
{
	if(!visible)
		return false;

	return p.x >= tab_pos.x && p.x < tab_pos.x + tab_size.cx &&
	       p.y >= tab_pos.y && p.y < tab_pos.y + tab_size.cy;
}

bool TabBar::Tab::HasMouseCross(const Point& p) const
{
	if(!visible)
		return false;

	return p.x >= cross_pos.x && p.x < cross_pos.x + cross_size.cx &&
	       p.y >= cross_pos.y && p.y < cross_pos.y + cross_size.cy;
}

i32 TabBar::FindGroup(const Txt& g) const
{
	for(i32 i = 0; i < groups.GetCount(); i++)
		if(groups[i].name == g)
			return i;
	return -1;
}

void TabBar::DoStacking()
{
	Val v = GetData();

	// Reset stack info
	for (i32 i = 0; i < tabs.GetCount(); i++) {
		Tab &t = tabs[i];
		t.stack = -1;
		t.stackid = GetStackId(t);
	}
	// Create stacks
	Vec< Array<Tab> > tstack;
	for (i32 i = 0; i < tabs.GetCount(); i++) {
		Tab &ti = tabs[i];
		if (ti.stack < 0) {
			ti.stack = tstack.GetCount();
			Array<Tab> &ttabs = tstack.Add();
			ttabs.Add(ti);
			for (i32 j = i + 1; j < tabs.GetCount(); j++)	{
				Tab &tj = tabs[j];
				if (tj.stack < 0 && tj.stackid == ti.stackid && (!grouping || tj.group == ti.group)) {
					tj.stack = ti.stack;
					ttabs.Add(tj);
				}
			}
		}
	}
	stackcount = tstack.GetCount();
	// Recombine
	tabs.SetCount(0);
	for (i32 i = 0; i < tstack.GetCount(); i++) {
		if (stacksort)
			StableSort(tstack[i], *stacksorter);
		tabs.AppendPick(pick(tstack[i]));
	}
	highlight = -1;
	SetData(v);
	MakeGroups();
	Repos();
}

void TabBar::DoUnstacking()
{
	stackcount = 0;
	for (i32 i = 0; i < tabs.GetCount(); i++)
		tabs[i].stack = -1;
	highlight = -1;
	MakeGroups();
	Repos();
	if (HasCursor())
		SetCursor(-1);
	else
		Refresh();
}

void TabBar::SortStack(i32 stackix)
{
	if (!stacksort) return;

	i32 head = FindStackHead(stackix);
	i32 tail = head;
	while (tail < tabs.GetCount() && tabs[tail].stack == stackix)
		++tail;
	SortStack(stackix, head, tail-1);
}

void TabBar::SortStack(i32 stackix, i32 head, i32 tail)
{
	if (!stacksort) return;

	i32 headid = tabs[head].id;
	StableSort(SubRange(tabs, head, tail - head).Write(), *stacksorter);
	while (tabs[head].id != headid)
		CycleTabStack(head, stackix);
}

void TabBar::MakeGroups()
{
	for(const auto& tab : tabs)
		if(FindGroup(tab.group) < 0)
			NewGroup(tab.group);

	groups[0].count = tabs.GetCount();
	groups[0].first = 0;
	groups[0].last = tabs.GetCount() - 1;

	if (groupsort)
		StableSort(tabs, *groupsorter);

	for(i32 i = 1; i < groups.GetCount(); i++)
	{
		groups[i].count = 0;
		groups[i].first = 10000000;
		groups[i].last = 0;
	}

	for(i32 i = 0; i < tabs.GetCount(); i++)
	{
		Tab &tab = tabs[i];
		i32 n = FindGroup(tab.group);
		ASSERT(n >= 0);
		if (n > 0) {
			if(groups[n].active < 0)
				groups[n].active = tab.id;
			groups[n].count++;
			groups[n].last = i;

			if(i < groups[n].first)
				groups[n].first = i;
			if(i > groups[n].last)
				groups[n].last = i;
		}
	}

	i32 cnt = groups.GetCount() - 1;
	for(i32 i = cnt; i > 0; i--)
		if(groups[i].count == 0)
			groups.Remove(i);

	if(group > groups.GetCount() - 1 && group > 0)
		group--;
}

void TabBar::GoGrouping(i32 n)
{
	Val c = GetData();
	group = n;
	Txt g = GetGroupName();
	for(i32 i = 0; i < tabs.GetCount(); i++)
		if(tabs[i].group == g)
			c = GetKey(i);
	Repos();
	SyncScrollBar();
	SetData(c);
	Refresh();
}

void TabBar::DoGrouping(i32 n)
{
	group = n;
	Repos();
	SyncScrollBar();
}


void TabBar::DoCloseGroup(i32 n)
{
	i32 cnt = groups.GetCount();
	if(cnt <= 0)
		return;

	Txt groupName = groups[n].name;

	/*
		do WhenCloseSome()/CancelCloseSome() checking
		before WhenClose()/CancelClose() stuff
		(that code must be reviewed anyways...)
		In order to leave existing code as it is, following
		changes have effect *ONLY* if WhenCloseSome()/CancelCloseSome()
		callbacks are used, otherwise previous path is taken.
		I think we should anyways review some parts of it later
	*/

	if(WhenCloseSome || CancelCloseSome)
	{
		ValArray vv;
		i32 nTabs = 0;
		for(i32 i = 0; i < tabs.GetCount(); i++)
			if(groupName == tabs[i].group) {
				vv.Add(tabs[i].key);
				nTabs++;
			}
		// at first, we check for CancelCloseSome()
		if(vv.GetCount() && !CancelCloseSome(vv) && ConfirmCloseSome(vv)) {
			// we didn't cancel globally, now we check CancelClose()
			// for each tab -- group gets removed ONLY if ALL of
			// group tabs are closed
			vv.Clear();
			Vec<i32>vi;
			for(i32 i = tabs.GetCount() - 1; i >= 0; i--) {
				if(groupName == tabs[i].group && tabs.GetCount() > 1) {
					Val v = tabs[i].key;
					if(!CancelClose(v) && ConfirmClose(v))
					{
						nTabs--;
						WhenClose(v);
						// record keys and indexes of tabs to remove
						vv << v;
						vi << i;
					}
				}
			}
			// and now do the true removal
			WhenCloseSome(vv);
			for(i32 i = 0; i < vv.GetCount(); i++)
			{
				if (!CancelClose(vv[i]) && ConfirmClose(vv[i])) {
					WhenClose(vv[i]);
					TabClosed(vv[i]);
					tabs.Remove(vi[i]);
				}
			}
			// remove group if all of its tabs get closed
			if(!nTabs) {
				if(cnt == n)
					group--;
				if(cnt > 1)
					groups.Remove(n);
			}
			MakeGroups();
			Repos();
			SetCursor(-1);
		}
		return;
	}

	// previous code path, taken if WhenCancelSome()/WhenCloseSome()
	for(i32 i = tabs.GetCount() - 1; i >= 0; i--)
	{
		if(groupName == tabs[i].group && tabs.GetCount() > 1) {
			Val v = tabs[i].value; // should be key ??
			if (!CancelClose(v) && ConfirmClose(v)) {
				WhenClose(v);
				TabClosed(v);
				tabs.Remove(i);
			}
		}
	}

	if (cnt == n)
		group--;

	if(cnt > 1) // what if CancelClose suppressed some tab closing ?
		groups.Remove(n);
	MakeGroups();
	Repos();
	SetCursor(-1);
}

void TabBar::NewGroup(const Txt &name)
{
	Group &g = groups.Add();
	g.name = name;
	g.count = 0;
	g.first = 10000000;
	g.last = 0;
	g.active = -1;
}

Image TabBar::AlignImage(i32 align, const Image& img)
{
	switch(align) {
		case AlignedFrame::LEFT:
			return RotateAntiClockwise(img);
		case AlignedFrame::RIGHT:
			return RotateClockwise(img);
		case AlignedFrame::BOTTOM:
			return MirrorVert(img);
		default:
			return img;
	}
}

Val TabBar::AlignVal(i32 align, const Val &v, const Size &sz)
{
	Size isz = sz;
	if(align == AlignedFrame::LEFT || align == AlignedFrame::RIGHT)
		Swap(isz.cx, isz.cy);

	ImageDraw w(isz.cx, isz.cy);
	w.DrawRect(isz, SColorFace());
	ChPaint(w, isz, v);
	return AlignImage(align, (Image)w);
}

void TabBar::TabItem::Clear()
{
	text.Clear();
	ink = Null;
	img = Null;
	side = LEFT;
	clickable = false;
	cross = false;
	stacked_tab = -1;
}

TabBar::TabItem& TabBar::Tab::AddItem()
{
	if(itn < items.GetCount())
	{
		TabItem& ti = items[itn++];
		ti.Clear();
		return ti;
	}
	else
	{
		++itn;
		return items.Add();
	}
}

void TabBar::Tab::Clear()
{
	itn = 0;
}

TabBar::TabItem& TabBar::Tab::AddImage(const Image& img, i32 side)
{
	TabItem& ti = AddItem();
	ti.img = img;
	ti.size = img.GetSize();
	ti.side = side;
	return ti;
}

TabBar::TabItem& TabBar::Tab::AddVal(const Val& q, const Font& font, const Color& ink)
{
	TabItem& ti = AddItem();

	ti.font = font;
	ti.ink = ink;

	if(IsType<AttrText>(q)) {
		const AttrText& t = ValTo<AttrText>(q);
		ti.text = t.text;
		if(!IsNull(t.font))
			ti.font = t.font;

		if(!IsNull(t.ink))
			ti.ink = t.ink;
	}
	else
		ti.text = IsTxt(q) ? q : StdConvert().Format(q);

	ti.size = GetTextSize(ti.text, ti.font);
	return ti;
}

TabBar::TabItem& TabBar::Tab::AddText(const WTxt& s, const Font& font, const Color& ink)
{
	TabItem& ti = AddItem();

	ti.font = font;
	ti.ink = ink;
	ti.text = s;
	ti.size = GetTextSize(ti.text, ti.font);
	return ti;
}

TabBar::TabItem& TabBar::Tab::AddSpace(i32 space, i32 side)
{
	TabItem& ti = AddItem();

	ti.size.cx = space;
	ti.size.cy = 0;
	ti.side = side;
	return ti;
}

void TabBar::ComposeTab(Tab& tab, const Font &font, Color ink, i32 style)
{
	if(PaintIcons() && tab.HasIcon())
	{
		tab.AddImage(tab.img);
		tab.AddSpace(DPI(TB_SPACEICON));
	}
	tab.AddVal(tab.value, font, ink).Clickable();
}

void TabBar::ComposeStackedTab(Tab& tab, const Tab& stacked_tab, const Font& font, Color ink, i32 style)
{
	tab.AddImage(stacked_tab.img);
	tab.AddText("|...", font, ink);
}

i32 TabBar::GetTextAngle()
{
	return AlignedFrame::IsVert() ? (GetAlign() == LEFT ? 900 : 2700) : 0;
}

Point TabBar::GetTextPosition(i32 align, const Rect& r, i32 cy, i32 space) const
{
	Point 	p;

	if(align == LEFT)
	{
		p.y = r.bottom - space;
		p.x = r.left + (r.GetWidth() - cy) / 2;
	}
	else if(align == RIGHT)
	{
		p.y = r.top + space;
		p.x = r.right - (r.GetWidth() - cy) / 2;
	}
	else
	{
		p.x = r.left + space;
		p.y = r.top + (r.GetHeight() - cy) / 2;
	}
	return p;
}

Point TabBar::GetImagePosition(i32 align, const Rect& r, i32 cx, i32 cy, i32 space, i32 side, i32 offset) const
{
	Point p;

	if (align == LEFT)
	{
		p.x = r.left + (r.GetWidth() - cy) / 2 + offset;
		p.y = side == LEFT ? r.bottom - space - cx : r.top + space;
	}
	else if (align == RIGHT)
	{
		p.x = r.right - (r.GetWidth() + cy) / 2 - offset;
		p.y = side == LEFT ? r.top + space : r.bottom - space - cx;
	}
	else if (align == TOP)
	{
		p.x = side == LEFT ? r.left + space : r.right - cx - space;
		p.y = r.top + (r.GetHeight() - cy) / 2 + offset;
	}
	else if (align == BOTTOM)
	{
		p.x = side == LEFT ? r.left + space : r.right - cx - space;
		p.y = r.bottom - (r.GetHeight() + cy) / 2 - offset;
	}
	return p;
}

void TabBar::PaintTabItems(Tab& t, Draw &w, const Rect& rn, i32 align)
{
	i32 pos_left = TB_MARGIN;
	i32 pos_right = (IsVert() ? rn.GetHeight() : rn.GetWidth()) - TB_MARGIN;

	for(i32 i = 0; i < t.itn; i++)
	{
		const TabItem& ti = t.items[i];

		Point p(0, 0);
		i32 pos = ti.side == LEFT ? pos_left : pos_right - ti.size.cx;

		if(!IsNull(ti.img))
		{
			p = GetImagePosition(align, rn, ti.size.cx, ti.size.cy, pos, LEFT);
			w.DrawImage(p.x, p.y, IsVert() ? AlignImage(align, ti.img) : ti.img);
		}

		if(!IsNull(ti.text))
		{
			p = GetTextPosition(align, rn, ti.size.cy, pos);
			w.DrawText(p.x, p.y, GetTextAngle(), ti.text, ti.font, ti.ink);
		}

		if(ti.cross)
		{
			t.cross_size = ti.size;
			t.cross_pos = p;
		}

		if(ti.stacked_tab >= 0 && ti.clickable)
		{
			Tab& st = tabs[ti.stacked_tab];

			if(align == RIGHT)
			{
				st.tab_pos = Point(rn.left, rn.top + pos);
				st.tab_size = Size(rn.GetWidth(), ti.size.cx);
			}
			else if(align == LEFT)
			{
				st.tab_pos = Point(rn.left, rn.bottom - pos - ti.size.cx);
				st.tab_size = Size(rn.GetWidth(), ti.size.cx);
			}
			else
			{
				st.tab_pos = Point(rn.left + pos, rn.top);
				st.tab_size = Size(ti.size.cx, rn.GetHeight());
			}

			#ifdef TABBAR_DEBUG
			DrawFrame(w, Rect(st.tab_pos, st.tab_size), Red);
			#endif
		}

		if(ti.side == LEFT)
			pos_left += ti.size.cx;
		else
			pos_right -= ti.size.cx;
	}
}

void TabBar::PaintTab(Draw &w, const Size &sz, i32 n, bool enable, bool dragsample)
{
	TabBar::Tab &t = tabs[n];
	const Style& s = *style;
	i32 align = GetAlign();
	i32 cnt = dragsample ? 1 : tabs.GetCount();

	bool ac = (n == active && enable);
	bool hl = (n == highlight && enable) || (stacking && highlight >= 0 && tabs[highlight].stack == t.stack);

	i32 ndx = !enable ? CTRL_DISABLED :
		       ac ? CTRL_PRESSED :
		       hl ? CTRL_HOT : CTRL_NORMAL;

	i32 c = align == LEFT ? cnt - n : n;
	i32 lx = n > 0 ? s.extendleft : 0;
	i32 x = t.pos.x - sc.GetPos() - lx + s.margin;

	i32 dy = -s.sel.top * ac;
	i32 sel = s.sel.top;

	i32 df = 0;

	if (IsBR())
	{
		dy = -dy;
		sel = s.sel.bottom;
		df = Fixed(sz).cy;
	}

	Size  sa = Size(t.size.cx + lx + s.sel.right + s.sel.left, t.size.cy + s.sel.bottom);
	Point pa = Point(x - s.sel.left, IsBR() ? df - sa.cy : 0);

	Size  sn = Size(t.size.cx + lx, t.size.cy - s.sel.top);
	Point pn = Point(x, IsBR() ? df - sn.cy - s.sel.top : s.sel.top);

	Rect ra(Fixed(pa), Fixed(sa));
	Rect rn(Fixed(pn), Fixed(sn));

	t.tab_pos = (ac ? ra : rn).TopLeft();
	t.tab_size = (ac ? ra : rn).GetSize();

	const Val& sv = (cnt == 1 ? s.both : c == 0 ? s.first : c == cnt - 1 ? s.last : s.normal)[ndx];

	Image img = AlignVal(align, sv, t.tab_size);

	if(!IsNull(t.col))
	{
		img = Colorize(img, t.col);
	}

	if(dragsample)
	{
		w.DrawImage(Rect(Point(0, 0), t.tab_size), img);
		rn = Rect(Fixed(Point(s.sel.left * ac, sel * ac + dy)), Fixed(sn));
	}
	else
	{
		w.DrawImage(Rect(t.tab_pos, t.tab_size), img);
		rn = Rect(Fixed(Point(pn.x, pn.y + dy)), Fixed(sn));
	}

	#ifdef TABBAR_DEBUG
	DrawFrame(w, rn, Green);
	#endif

	if (display)
		display->Paint(w, rn, t.value, s.text_color[ndx], SColorDisabled(), ndx);

	t.Clear();

	if(crosses && cnt > mintabcount && !dragsample && !IsCancelClose(n)) {
		TabItem& ti = t.AddItem();
		ti.img = s.crosses[cross == n ? 2 : ac || hl ? 1 : 0];
		ti.side = crosses_side;
		ti.cross = true;
		ti.size = s.crosses[0].GetSize();
		t.AddSpace(DPI(3), crosses_side);
	}

	ComposeTab(t, s.font, s.text_color[ndx], ndx);

	if (stacking) {
		i32 ix = n + 1;

		while (ix < tabs.GetCount() && tabs[ix].stack == t.stack) {
			Tab &q = tabs[ix];
			i32 ndx = !enable ? CTRL_DISABLED :
					   highlight == ix ? CTRL_HOT : CTRL_NORMAL;

			i32 sn = t.itn;
			ComposeStackedTab(t, q, s.font, s.text_color[ndx], ndx);
			if(t.itn > sn)
				for(; sn < t.itn; sn++)
					t.items[sn].stacked_tab = ix;

			ix++;
		}
	}

	PaintTabItems(t, w, rn, align);
}

void TabBar::Paint(Draw &w)
{
	i32 align = GetAlign();
	const Style &st = *style;
	Size ctrlsz = GetSize();
	Size sz = GetBarSize(ctrlsz);

	if (align == BOTTOM || align == RIGHT)
		w.Offset(ctrlsz.cx - sz.cx, ctrlsz.cy - sz.cy);

	#ifdef TABBAR_DEBUG
	w.DrawRect(sz, Yellow);
	#else
	w.DrawRect(sz, SColorFace());
	#endif

	if(IsVert())
		w.DrawRect(align == LEFT ? sz.cx - 1 : 0, 0, 1, sz.cy, Blend(SColorDkShadow, SColorShadow));
	else
		w.DrawRect(0, align == TOP ? sz.cy - 1 : 0, sz.cx, 1, Blend(SColorDkShadow, SColorShadow));

	if (!tabs.GetCount()) {
		if (align == BOTTOM || align == RIGHT)
			w.End();
		return;
	}

	i32 limt = sc.GetPos() + (IsVert() ? sz.cy : sz.cx);
	i32 first = 0;
	i32 last = tabs.GetCount() - 1;
	// Find first visible tab
	for(i32 i = 0; i < tabs.GetCount(); i++) {
		Tab &tab = tabs[i];
		if (tab.pos.x + tab.size.cx > sc.GetPos()) {
			first = i;
			break;
		}
	}
	// Find last visible tab
	for(i32 i = first + 1; i < tabs.GetCount(); i++) {
		if (tabs[i].visible && tabs[i].pos.x > limt) {
			last = i;
			break;
		}
	}
	// Draw active group
	for (i32 i = first; i <= last; i++) {
		if(tabs[i].visible && i != active)
			PaintTab(w, sz, i, IsEnabled());
	}
	// Clear tab_size for non-visible tabs to prevent mouse handling bugs
	for (i32 i = 0; i < first; i++)
		tabs[i].tab_size = Size(0, 0);
	for (i32 i = last + 1; i < tabs.GetCount(); i++)
		tabs[i].tab_size = Size(0, 0);
	// Draw inactive groups
	if (inactivedisabled)
		for (i32 i = first; i <= last; i++) {
			if(!tabs[i].visible && i != active && (!stacking || IsStackHead(i)))
				PaintTab(w, sz, i, !IsEnabled());
		}

	// Draw selected tab
	if(active >= first && active <= last)
		PaintTab(w, sz, active, true);

	// Separators
	if (grouping && groupseps) {
		i32 cy = IsVert() ? sz.cx : sz.cy;
		for (i32 i = 0; i < separators.GetCount(); i++) {
			i32 x = separators[i];
			if (x > sc.GetPos() && x < limt) {
				// Paint separator
				ChPaint(w, Rect(Fixed(Point(x - sc.GetPos() + GetStyle().sel.left, 0)),
					Fixed(Size(TB_SPACE - GetStyle().sel.left, cy-1))),
					st.group_separators[IsVert() ? 1 : 0]);
			}
		}
	}

	// Draw drag highlights
	if(target >= 0)
	{
		// Draw target marker
		i32 drag = isctrl ? highlight : active;
		if(target != drag && target != GetNext(drag, true))
		{
			last = GetLast();
			first = GetFirst();
			i32 x = (target == last + 1 ? tabs[last].Right() : tabs[target].pos.x)
			        - sc.GetPos() - (target <= first ? 1 : 2)
			        + st.margin - (target > 0 ? st.extendleft : 0);

			if (IsHorz())
				DrawVertDrop(w, x + 1, 0, sz.cy);
			else
				DrawHorzDrop(w, 0, x + 1, sz.cx);
		}
		// Draw transparent drag image
		Point mouse = GetMousePos() - GetScreenRect().TopLeft();
		Size isz = dragtab.GetSize();
		i32 p = 0;
		i32 sep = TB_SBSEPARATOR * sc.IsVisible();

		i32 top = drag == active ? st.sel.bottom : st.sel.top;
		if (align == BOTTOM || align == RIGHT)
			p = i32(drag == active) * -top + sep;
		else
			p = i32(drag != active) * top;

		if (IsHorz())
			w.DrawImage(mouse.x - isz.cx / 2, p, isz.cx, isz.cy, dragtab);
		else
			w.DrawImage(p, mouse.y - isz.cy / 2, isz.cx, isz.cy, dragtab);
	}

	if (align == BOTTOM || align == RIGHT)
		w.End();

	// If not in a frame fill any spare area
	if (!InFrame())
		w.DrawRect(GetClientArea(), SColorFace());
}

Image TabBar::GetDragSample()
{
	i32 h = drag_highlight;
	if(stacking)
		h = FindStackHead(tabs[h].stack);
	return GetDragSample(h);
}

Image TabBar::GetDragSample(i32 n)
{
	if (n < 0) return Image();
	Tab &t = tabs[n];

	Size tsz(t.tab_size);
	ImageDraw iw(tsz);
	iw.DrawRect(tsz, SColorFace()); //this need to be fixed - if inactive tab is dragged gray edges are visible

	PaintTab(iw, tsz, n, true, true);

	Image img = iw;
	ImageBuffer ib(img);
	Unmultiply(ib);
	RGBA *s = ~ib;
	RGBA *e = s + ib.GetLength();
	while(s < e) {
		s->a = 180;
		s++;
	}
	Premultiply(ib);
		return static_cast<Image>(ib);
}

void TabBar::Scroll()
{
	Refresh();
}

i32 TabBar::GetWidth(i32 n)
{
	return GetStdSize(tabs[n]).cx + GetExtraWidth(n);
}

i32 TabBar::GetExtraWidth(i32 n)
{
	return DPI(TB_MARGIN) * 2 + (DPI(TB_SPACE) + GetStyle().crosses[0].GetSize().cx) * (crosses && !IsCancelClose(n));
}

Size TabBar::GetStdSize(const Val &q)
{
	if (display)
		return display->GetStdSize(q);
	else if (q.GetType() == TXT_V || q.GetType() == WTXT_V)
		return GetTextSize(WTxt(q), GetStyle().font);
	else
		return GetTextSize("A Tab", GetStyle().font);
}

Size TabBar::GetStackedSize(const Tab &t)
{
	if (!IsNull(t.img))
		return t.img.GetSize();
	return GetTextSize("...", GetStyle().font, 3);
}

Size TabBar::GetStdSize(const Tab &t)
{
	return (PaintIcons() && t.HasIcon()) ? (GetStdSize(t.value) + Size(TB_ICON + 2, 0)) : GetStdSize(t.value);
}

TabBar& TabBar::Add(const Val &value, Image icon, Txt group, bool make_active)
{
	return InsertKey(tabs.GetCount(), value, value, icon, group, make_active);
}

TabBar& TabBar::Insert(i32 ix, const Val &value, Image icon, Txt group, bool make_active)
{
	return InsertKey(tabs.GetCount(), value, value, icon, group, make_active);
}

TabBar& TabBar::AddKey(const Val &key, const Val &value, Image icon, Txt group, bool make_active)
{
	return InsertKey(tabs.GetCount(), key, value, icon, group, make_active);
}

TabBar& TabBar::InsertKey(i32 ix, const Val &key, const Val &value, Image icon, Txt group, bool make_active)
{
	i32 id = InsertKey0(ix, key, value, icon, group);

	SortTabs0();
	MakeGroups();
	Repos();
	active = -1;
	if (make_active || (!allownullcursor && active < 0))
		SetCursor((groupsort || stacking) ? FindId(id) : ( minmax(ix, 0, tabs.GetCount() - 1)));
	return *this;
}

i32 TabBar::InsertKey0(i32 ix, const Val &key, const Val &value, Image icon, Txt group)
{
	ASSERT(ix >= 0);
	i32 g = 0;
	if (!group.IsEmpty()) {
		g = FindGroup(group);
		if (g < 0) {
			NewGroup(group);
			g = groups.GetCount() - 1;
		}
	}

	group = groups[g].name;
	Tab t;
	t.value = value;
	t.key = key;
	t.img = icon;
	t.id = GetNextId();
	t.group = Nvl(TrimBoth(group), "Безымянная Группа");
	if (stacking) {
		t.stackid = GetStackId(t);

		// Override index
		i32 tail = -1;
		for (i32 i = 0; i < tabs.GetCount(); i++) {
			if (tabs[i].stackid == t.stackid && (!grouping || tabs[i].group == t.group)) {
				tail = FindStackTail(tabs[i].stack);
				break;
			}
		}
		if (tail >= 0) {
			ix = tail+1;
			t.stack = tabs[tail].stack;
			tail++;
		}
		else {
			ix = (ix < tabs.GetCount()) ? FindStackHead(tabs[ix].stack) : ix;
			t.stack = stackcount++;
		}
		tabs.Insert(ix, t);
		if (tail >= 0)
			SortStack(t.stack, FindStackHead(t.stack), ix);

	}
	else
		tabs.Insert(ix, t);
	return t.id;
}

i32 TabBar::GetWidth() const
{
	if (!tabs.GetCount()) return 0;
	i32 ix = GetLast();
	const Style& s = StyleDefault();
	if (IsStackHead(ix))
		return tabs[ix].Right() + s.margin * 2;
	i32 stack = tabs[ix].stack;
	ix--;
	while (ix >= 0 && tabs[ix].stack == stack)
		ix--;
	return tabs[ix + 1].Right() + s.margin * 2;

}

i32 TabBar::GetHeight(bool scrollbar) const
{
	return TabBar::GetStyleHeight() + TB_SBSEPARATOR * i32(scrollbar);
}

i32 TabBar::GetStyleHeight() const
{
	const Style& s = GetStyle();
	return s.tabheight + s.sel.top;
}

void TabBar::Repos()
{
	if(!tabs.GetCount())
		return;

	Txt g = GetGroupName();
	i32 j;
	bool first = true;
	j = 0;
	separators.Clear();
	for(i32 i = 0; i < tabs.GetCount(); i++)
		j = TabPos(g, first, i, j, false);
	if (inactivedisabled)
		for(i32 i = 0; i < tabs.GetCount(); i++)
			if (!tabs[i].visible)
				j = TabPos(g, first, i, j, true);
	SyncScrollBar();
}

Size TabBar::GetBarSize(Size ctrlsz) const
{
	return IsVert() ? Size(GetFrameSize() - scrollbar_sz * i32(sc.IsShown()), ctrlsz.cy)
			: Size(ctrlsz.cx, GetFrameSize() - scrollbar_sz * i32(sc.IsShown()));
}

Rect TabBar::GetClientArea() const
{
	Rect rect = GetSize();
	switch (GetAlign()) {
		case TOP:
			rect.top += GetFrameSize();
			break;
		case BOTTOM:
			rect.bottom -= GetFrameSize();
			break;
		case LEFT:
			rect.left += GetFrameSize();
			break;
		case RIGHT:
			rect.right -= GetFrameSize();
			break;
	};
	return rect;
}

i32 TabBar::TabPos(const Txt &g, bool &first, i32 i, i32 j, bool inactive)
{
	bool ishead = IsStackHead(i);
	bool v = IsNull(g) ? true : g == tabs[i].group;
	Tab& t = tabs[i];

	if(ishead && (v || inactive))
	{
		t.visible = v;
		t.pos.y = 0;
		t.size.cy = GetStyleHeight();

		// Normal visible or inactive but greyed out tabs
		t.pos.x = first ? 0 : tabs[j].Right();

		// Separators
		if (groupseps && grouping && !first && t.group != tabs[j].group) {
			separators.Add(t.pos.x);
			t.pos.x += TB_SPACE;
		}

		i32 cx = GetStdSize(t).cx;

		// Stacked/shortened tabs
		if (stacking) {
			for(i32 n = i + 1; n < tabs.GetCount() && tabs[n].stack == t.stack; n++)
				cx += GetStackedSize(tabs[n]).cx;
		}

		t.size.cx = cx + GetExtraWidth(i);

		if (stacking) {
			for(i32 n = i + 1; n < tabs.GetCount() && tabs[n].stack == t.stack; n++) {
				Tab &q = tabs[n];
				q.visible = false;
				q.pos = t.pos;
				q.size = t.size;
			}
		}

		j = i;
		first = false;
	}
	else if (!(v || inactive)) {
		t.visible = false;
		t.pos.x = sc.GetTotal() + GetBarSize(GetSize()).cx;
	}
	return j;
}

void TabBar::ShowScrollbarFrame(bool b)
{
	SetFrameSize((b ? sc.GetFrameSize() : TB_SBSEPARATOR) + GetHeight(b), false);
	sc.Show(b);
	RefreshParentLayout();
}

void TabBar::SyncScrollBar(bool synctotal)
{
	if (synctotal)
		sc.SetTotal(GetWidth());
	if (autoscrollhide) {
		bool v = sc.IsScrollable();
		if (sc.IsShown() != v) {
			PostCallback(OTVET1(ShowScrollbarFrame, v));
		}
	}
	else {
		SetFrameSize(sc.GetFrameSize() + GetHeight(true), false);
		sc.Show();
	}
}

i32 TabBar::FindId(i32 id) const
{
	for(i32 i = 0; i < tabs.GetCount(); i++)
		if(tabs[i].id == id)
			return i;
	return -1;
}

i32 TabBar::GetNext(i32 n, bool drag) const
{
	for(i32 i = n + 1; i < tabs.GetCount(); i++)
		if(tabs[i].visible)
			return i;
	return drag ? tabs.GetCount() : -1;
}

i32 TabBar::GetPrev(i32 n, bool drag) const
{
	for(i32 i = n - 1; i >= 0; i--)
		if(tabs[i].visible)
			return i;
	return -1;
}

void TabBar::Clear()
{
	highlight = -1;
	drag_highlight = -1;
	active = -1;
	target = -1;
	cross = -1;
	stackcount = 0;
	tabs.Clear();
	groups.Clear();
	NewGroup(t_("TabBarGroupAll\aAll"));
	group = 0;
	Refresh();
	jump_stack.Reset();
}

TabBar& TabBar::Crosses(bool b, i32 side)
{
	crosses = b;
	crosses_side = side;
	Repos();
	Refresh();
	return *this;
}

TabBar& TabBar::SortTabs(bool b)
{
	tabsort = b;
	if (b)
		DoTabSort(*tabsorter);
	return *this;
}

TabBar& TabBar::SortTabsOnce()
{
	DoTabSort(*tabsorter);
	return *this;
}

TabBar& TabBar::SortTabsOnce(TabSort &sort)
{
	DoTabSort(sort);
	return *this;
}

TabBar& TabBar::SortTabs(TabSort &sort)
{
	tabsorter = &sort;
	return SortTabs(true);
}

TabBar& TabBar::SortTabVals(ValOrder &sort)
{
	valuesorter_inst.vo = &sort;
	tabsorter = &valuesorter_inst;
	return SortTabs(true);
}

TabBar& TabBar::SortTabValsOnce(ValOrder &sort)
{
	TabValSort q;
	q.vo = &sort;
	DoTabSort(q);
	return *this;
}

TabBar& TabBar::SortTabKeys(ValOrder &sort)
{
	keysorter_inst.vo = &sort;
	tabsorter = &keysorter_inst;
	return SortTabs(true);
}

TabBar& TabBar::SortTabKeysOnce(ValOrder &sort)
{
	TabKeySort q;
	q.vo = &sort;
	DoTabSort(q);
	return *this;
}

TabBar& TabBar::SortGroups(bool b)
{
	groupsort = b;
	if (!b) return *this;;

	Val v = GetData();
	MakeGroups();
	Repos();
	if (!IsNull(v))
		SetData(v);
	Refresh();
	return *this;
}

TabBar& TabBar::SortGroupsOnce()
{
	if (!grouping) return *this;;

	Val v = GetData();
	MakeGroups();
	Repos();
	if (!IsNull(v))
		SetData(v);
	Refresh();
	return *this;
}

TabBar& TabBar::SortGroupsOnce(TabSort &sort)
{
	TabSort *current = groupsorter;
	groupsorter = &sort;
	SortGroupsOnce();
	groupsorter = current;
	return *this;
}

TabBar& TabBar::SortGroups(TabSort &sort)
{
	groupsorter = &sort;
	return SortGroups(true);
}

TabBar& TabBar::SortStacks(bool b)
{
	stacksort = b;
	if (stacking) {
		DoStacking();
		Refresh();
	}
	return *this;
}

TabBar& TabBar::SortStacksOnce()
{
	if (stacking) {
		DoStacking();
		Refresh();
	}
	return *this;
}

TabBar& TabBar::SortStacksOnce(TabSort &sort)
{
	TabSort *current = stacksorter;
	stacksorter = &sort;
	SortStacksOnce();
	stacksorter = current;
	return *this;
}

TabBar& TabBar::SortStacks(TabSort &sort)
{
	stacksorter = &sort;
	return SortStacks(true);
}

TabBar& TabBar::SortStacks(ValOrder &sort)
{
	stacksorter_inst.vo = &sort;
	stacksorter = &stacksorter_inst;
	return SortStacks(true);
}

void TabBar::DoTabSort(TabSort &sort)
{
	Val v = GetData();
	StableSort(tabs, sort);
	Repos();
	if (!IsNull(v))
		SetData(v);
	Refresh();
}

void TabBar::SortTabs0()
{
	if (tabsort)
		StableSort(tabs, *tabsorter);
}

TabBar& TabBar::Grouping(bool b)
{
	grouping = b;
	Repos();
	Refresh();
	return *this;
}

TabBar& TabBar::ContextMenu(bool b)
{
	contextmenu = b;
	return *this;
}

TabBar& TabBar::GroupSeparators(bool b)
{
	groupseps = b;
	Repos();
	Refresh();
	return *this;
}

TabBar& TabBar::AutoScrollHide(bool b)
{
	autoscrollhide = b;
	sc.Hide();
	SetFrameSize(GetHeight(false), false);
	SyncScrollBar(GetWidth());
	return *this;
}

TabBar& TabBar::InactiveDisabled(bool b)
{
	inactivedisabled = b;
	Repos();
	Refresh();
	return *this;
}

TabBar& TabBar::AllowNullCursor(bool b)
{
	allownullcursor = b;
	return *this;
}

TabBar& TabBar::Icons(bool v)
{
	icons = v;
	Repos();
	Refresh();
	return *this;
}

TabBar& TabBar::Stacking(bool b)
{
	stacking = b;
	if (b)
		DoStacking();
	else
		DoUnstacking();
	Refresh();
	return *this;
}

void TabBar::FrameSet()
{
	i32 al = GetAlign();
	Ctrl::ClearFrames();
	sc.Clear();
	sc.SetFrameSize(scrollbar_sz).SetAlign((al >= 2) ? al - 2 : al + 2);
	sc <<= OTVET(Scroll);
	sc.Hide();

	if (sc.IsChild()) sc.Remove();
	switch (al) {
		case LEFT:
			Ctrl::Add(sc.LeftPos(GetHeight(), scrollbar_sz).VSizePos());
			break;
		case RIGHT:
			Ctrl::Add(sc.RightPos(GetHeight(), scrollbar_sz).VSizePos());
			break;
		case TOP:
			Ctrl::Add(sc.TopPos(GetHeight(), scrollbar_sz).HSizePos());
			break;
		case BOTTOM:
			Ctrl::Add(sc.BottomPos(GetHeight(), scrollbar_sz).HSizePos());
			break;
	};

	SyncScrollBar(true);
}

TabBar& TabBar::SetScrollThickness(i32 sz)
{
	scrollbar_sz = max(sz + 2, 3);
	FrameSet();
	RefreshLayout();
	return *this;
}

void TabBar::Layout()
{
	if (autoscrollhide && tabs.GetCount())
		SyncScrollBar(false);
	Repos();
}

i32 TabBar::FindVal(const Val &v) const
{
	for (i32 i = 0; i < tabs.GetCount(); i++)
		if (tabs[i].value == v)
			return i;
	return -1;
}

i32 TabBar::FindKey(const Val &v) const
{
	for (i32 i = 0; i < tabs.GetCount(); i++)
		if (tabs[i].key == v)
			return i;
	return -1;
}

bool TabBar::IsStackHead(i32 n) const
{
	return tabs[n].stack < 0
		|| n == 0
		|| (n > 0 && tabs[n - 1].stack != tabs[n].stack);
}

bool TabBar::IsStackTail(i32 n) const
{
	return tabs[n].stack < 0
		|| n >= tabs.GetCount() - 1
		|| (n < tabs.GetCount() && tabs[n + 1].stack != tabs[n].stack);
}

i32 TabBar::GetStackCount(i32 stackix) const
{
	i32 tc = tabs.GetCount();
	i32 L = 0;

	for ( i32 i = 0; i < tc; ++i )
		if ( tabs[ i ].stack == stackix )
		  ++L;

	return L;
}

i32 TabBar::FindStackHead(i32 stackix) const
{
	i32 i = 0;
	while (tabs[i].stack != stackix)
		i++;
	return i;
}

i32 TabBar::FindStackTail(i32 stackix) const
{
	i32 i = tabs.GetCount() - 1;
	while (tabs[i].stack != stackix)
		i--;
	return i;
}

i32 TabBar::SetStackHead(Tab &t)
// Returns index of stack head
{
	ASSERT(stacking);
	i32 id = t.id;
	i32 stack = t.stack;
	i32 head = FindStackHead(stack);
	while (tabs[head].id != id)
		CycleTabStack(head, stack);
	return head;
}

i32 TabBar::CycleTabStack(i32 n)
// Returns index of stack head
{
	i32 head = FindStackHead(n);
	CycleTabStack(head, n);
	return head;
}

void TabBar::CycleTabStack(i32 head, i32 n)
{
	// Swap tab to end of stack
	i32 ix = head;
	while (!IsStackTail(ix)) {
		tabs.Swap(ix, ix + 1);
		++ix;
	}
}

Val TabBar::GetData() const
{
	return (HasCursor() && active < GetCount())
		? GetKey(active)
		: Val();
}

void TabBar::SetData(const Val &key)
{
	i32 n = FindKey(key);
	if (n >= 0) {
		if (stacking && tabs[n].stack >= 0)
			n = SetStackHead(tabs[n]);
		SetCursor(n);
	}
}

void TabBar::Set(i32 n, const Val &newkey, const Val &newvalue)
{
	Set(n, newkey, newvalue, tabs[n].img);
}

void TabBar::Set(i32 n, const Val &newkey, const Val &newvalue, Image icon)
{
	ASSERT(n >= 0 && n < tabs.GetCount());
	tabs[n].key = newkey;
	tabs[n].value = newvalue;
	tabs[n].img = icon;
	if (stacking) {
		Txt id = tabs[n].stackid;
		tabs[n].stackid = GetStackId(tabs[n]);
		if (tabs[n].stackid != id) {
			tabs.Remove(n);
			InsertKey0(GetCount(), newkey, newvalue, tabs[n].img, tabs[n].group);
		}
	}
	Repos();
	Refresh();
}

void TabBar::SetVal(const Val &key, const Val &newvalue)
{
	Set(FindKey(key), key, newvalue);
}

void TabBar::SetVal(i32 n, const Val &newvalue)
{
	Set(n, tabs[n].key, newvalue);
}

void TabBar::SetKey(i32 n, const Val &newkey)
{
	Set(n, newkey, tabs[n].value);
}

void TabBar::SetIcon(i32 n, Image icon)
{
	ASSERT(n >= 0 && n < tabs.GetCount());
	tabs[n].img = icon;
	Repos();
	Refresh();
}

void TabBar::LeftDown(Point p, dword keyflags)
{
	p = AdjustMouse(p);
	SetCapture();

	if(keyflags & K_SHIFT)
	{
		highlight = -1;
		Refresh();
		Fix(p);
		oldp = p;
		return;
	}

	drag_highlight = highlight;

	isctrl = keyflags & K_CTRL;
	if(isctrl)
		return;

	if(cross != -1) {
		if (cross < tabs.GetCount()) {
			i32 tempCross = cross;
			Val v = tabs[cross].key;
			ValArray vv;
			vv.Add(v);
			i32 ix = cross;
			if (!CancelClose(v) && !CancelCloseSome(vv) && ConfirmClose(v)) {
				WhenClose(v);
				WhenCloseSome(vv);
				TabClosed(v);
				Close(ix);
			}
			if (tempCross >= 0 && tempCross < tabs.GetCount())
				ProcessMouse(tempCross, p);
		}
	}
	else if(highlight >= 0) {
		if (stacking && highlight == active) {
			CycleTabStack(tabs[active].stack);
			Repos();
			CursorChanged();
			UpdateActionRefresh();
		}
		else
			SetCursor0(highlight, true);
	}
}

void TabBar::LeftUp(Point p, dword keyflags)
{
	ReleaseCapture();
}

void TabBar::LeftDouble(Point p, dword keysflags)
{
	WhenLeftDouble();
}

void TabBar::RightDown(Point p, dword keyflags)
{
	if (contextmenu)
	{
		// 2014/03/07 needed on X11 otherwise may crash
		// if focus is nowhere (probable bug somewhere else...)
		if(!GetActiveCtrl())
			GetParent()->SetFocus();
		MenuBar::Execute(OTVET(ContextMenu), GetMousePos());
	}
}

void TabBar::MiddleDown(Point p, dword keyflags)
{
    if (highlight >= 0)
    {
        Val v = tabs[highlight].key;
        ValArray vv;
        vv.Add(v);
        i32 highlightBack = highlight;
        if (!CancelClose(v) && !CancelCloseSome(vv) && ConfirmCloseSome(vv) && ConfirmClose(v)) {
            // highlight can be changed by the prompt. When reading "v", it can be invalid. I use the value from before the prompt to fix it
            Val v = tabs[highlightBack].key;
            // 2014/03/06 - FIRST the callbacks, THEN remove the tab
            // otherwise keys in WhenCloseSome() are invalid
            WhenClose(v);
            WhenCloseSome(vv);
            TabClosed(v);
            Close(highlightBack);
        }
    }
}

void TabBar::MiddleUp(Point p, dword keyflags)
{
}

i32 TabBar::GetTargetTab(Point p)
{
	p.x += sc.GetPos();

	i32 f = GetFirst();
	i32 l = GetLast();

	if(tabs[f].visible && p.x < tabs[f].pos.x + tabs[f].size.cx / 2)
		return f;

	i32 t = -1;

	for(i32 i = l; i >= f; i--)
		if(tabs[i].visible && p.x >= tabs[i].pos.x + tabs[i].size.cx / 2)
		{
			t = i;
			break;
		}

	if(stacking)
		l = FindStackHead(tabs[l].stack);

	if(t == l)
		t = tabs.GetCount();
	else
	 	t = GetNext(t);

	return t;
}

void TabBar::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	sc.AddPos(-zdelta / 4, true);
	Scroll();
	MouseMove(p, 0);
}

Point TabBar::AdjustMouse(Point const &p) const
{
	i32 align = GetAlign();
	if(align == TOP || align == LEFT)
		return p;

	Size ctrlsz = GetSize();
	Size sz = GetBarSize(ctrlsz);
	return Point(p.x - ctrlsz.cx + sz.cx, p.y - ctrlsz.cy + sz.cy);
}

bool TabBar::ProcessMouse(i32 i, const Point& p)
{
	if(i >= 0 && i < tabs.GetCount() && tabs[i].HasMouse(p))
	{
		if (stacking && ProcessStackMouse(i, p))
			return true;
		bool iscross = crosses && !IsCancelClose(i) ? tabs[i].HasMouseCross(p) : false;
		if(highlight != i || (iscross && cross != i || !iscross && cross == i))
		{
			cross = iscross ? i : -1;
			SetHighlight(i);
		}
		return true;
	}
	return false;
}

bool TabBar::ProcessStackMouse(i32 i, const Point& p)
{
	i32 j = i + 1;
	while (j < tabs.GetCount() && tabs[j].stack == tabs[i].stack) {
		if (Rect(tabs[j].tab_pos, tabs[j].tab_size).Contains(p)) {
			cross = -1;
			if (highlight != j)
				SetHighlight(j);
			return true;
		}
		j++;
	}
	return false;
}

void TabBar::SetHighlight(i32 n)
{
	highlight = n;
	WhenHighlight();
	Refresh();
}

void TabBar::SetColor(i32 n, Color c)
{
	tabs[n].col = c;
	Refresh();
}

void TabBar::MouseMove(Point p, dword keyflags)
{
	p = AdjustMouse(p);
	if(HasCapture() && (keyflags & K_SHIFT))
	{
		Fix(p);
		sc.AddPos(p.x - oldp.x, true);
		oldp = p;
		Refresh();
		return;
	}

	if(HasCapture())
		return;

	if(ProcessMouse(active, p))
		return;

	for(i32 i = 0; i < tabs.GetCount(); i++)
	{
		if(i == active)
			continue;

		if(ProcessMouse(i, p))
			return;
	}

	if(highlight >= 0 || cross >= 0)
	{
		highlight = cross = -1;
		WhenHighlight();
		Refresh();
	}
}

void TabBar::MouseLeave()
{
	if(isdrag)
		return;
	highlight = cross = -1;
	WhenHighlight();
	Refresh();
}

void TabBar::DragAndDrop(Point p, PasteClip& d)
{
	Fix(p);
	i32 c = GetTargetTab(p);
	i32 tab = isctrl ? drag_highlight : active;

	if (&GetInternal<TabBar>(d) != this || tabsort || c < 0 || !allowreorder) return;

	if (stacking) {
		tab = FindStackHead(tabs[tab].stack);
		if(c < tabs.GetCount())
			c = FindStackHead(tabs[c].stack);
	}

	bool sametab = c == tab || c == GetNext(tab, true);
	bool internal = AcceptInternal<TabBar>(d, "tabs");

	if (CancelDragAndDrop && CancelDragAndDrop(tab, c > tab ? c-1 : c))
	{
		target = -1;
		isdrag = false;
		d.Reject();
		return;
	}

	if(!sametab && internal && d.IsAccepted())
	{
		i32 id = active >= 0 ? tabs[active].id : -1;

		// Count stack
		i32 count = 1;
		if (stacking) {
			i32 ix = tab + 1;
			i32 stack = tabs[tab].stack;
			while (ix < tabs.GetCount() && tabs[ix].stack == stack)
				ix++;
			count = ix - tab;
		}
		// Copy tabs
		Array<Tab> stacktemp;
		stacktemp.SetCount(count);
		//Copy(&stacktemp[0], &tabs[tab], count);
		for(i32 i = 0; i < count; i++)
			stacktemp[i].Set(tabs[tab + i]);
		// Remove
		tabs.Remove(tab, count);
		if (tab < c)
			c -= count;
		// Re-insert
		tabs.InsertPick(c, pick(stacktemp));

		active = id >= 0 ? FindId(id) : -1;
		isdrag = false;
		target = -1;
		MakeGroups();
		Repos();
		Refresh();
		Sync();
		MouseMove(p, 0);
	}
	else if(isdrag)
	{
		if(internal)
		{
			target = -1;
			isdrag = false;
		}
		else
			target = c;
		Refresh();
	}
}

void TabBar::CancelMode()
{
	isdrag = false;
	target = -1;
	Refresh();
}

void TabBar::LeftDrag(Point p, dword keyflags)
{
	if(keyflags & K_SHIFT)
		return;
	if(highlight < 0)
		return;

	Sync();
	isdrag = true;
	dragtab = GetDragSample();
	DoDragAndDrop(InternalClip(*this, "tabs"));
}

void TabBar::DragEnter()
{
}

void TabBar::DragLeave()
{
	target = -1;
	Refresh();
}

void TabBar::DragRepeat(Point p)
{
	if(target >= 0)
	{
		Point dx = GetDragScroll(this, p, 16);
		Fix(dx);
		if(dx.x != 0)
			sc.AddPos(dx.x);
	}
}

bool TabBar::SetCursor0(i32 n, bool action)
{
	if(tabs.GetCount() == 0)
		return false;

	if(n < 0)
	{
		n = max(0, FindId(GetGroupActive()));
		active = -1;
		highlight = -1;
		drag_highlight = -1;
		if (allownullcursor)
			return true;
	}

	bool is_all = IsGroupAll();
	bool same_group = tabs[n].group == GetGroupName();

	if((same_group || is_all) && active == n)
		return false;

	bool repos = false;

	if (!IsStackHead(n))
	{
		n = SetStackHead(tabs[n]);
		repos = true;
	}

	active = n;

	if(!is_all && !same_group)
	{
		SetGroup(tabs[n].group);
		repos = true;
	}
	if (repos)
		Repos();

	SetGroupActive(tabs[n].id);

	i32 cx = tabs[n].pos.x - sc.GetPos();
	if(cx < 0)
		sc.AddPos(cx - 10);
	else
	{
		Size sz = Ctrl::GetSize();
		Fix(sz);
		cx = tabs[n].pos.x + tabs[n].size.cx - sz.cx - sc.GetPos();
		if(cx > 0)
			sc.AddPos(cx + 10);
	}

	if(action)
	{
		CursorChanged();
		UpdateAction();
	}

	Refresh();

	if(Ctrl::HasMouse())
	{
		Refresh();
		Sync();
		MouseMove(GetMouseViewPos(), 0);
	}
	return true;
}

void TabBar::SetCursor(i32 n)
{
	SetCursor0(n, true);
}

void TabBar::SetTabGroup(i32 n, const Txt &group)
{
	ASSERT(n >= 0 && n < tabs.GetCount());
	i32 g = FindGroup(group);
	if (g <= 0)
		NewGroup(group);
	else if (groups[g].active == tabs[n].id)
		SetGroupActive(tabs[n].id);
	tabs[n].group = group;
	MakeGroups();
	Repos();
}

void TabBar::CloseForce(i32 n, bool action)
{
	if(n < 0 || n >= tabs.GetCount())
		return;
	if(n == active)
	{
		i32 c = FindId(tabs[n].id);
		i32 nc = GetNext(c);
		if(nc < 0)
			nc = max(0, GetPrev(c));
		SetGroupActive(tabs[nc].id);
	}
	sc.AddTotal(-tabs[n].size.cx);
	tabs.Remove(n);
	MakeGroups();
	Repos();

	if(n == active)
		SetCursor0(-1, action);
	else {
		if (n < active)
			active--;
		Refresh();
		if (n == highlight && Ctrl::HasMouse()) {
			//СДЕЛАТЬ: That must be refactored
			highlight = -1;
			drag_highlight = -1;
			Refresh();
			Sync();
			MouseMove(GetMouseViewPos(), 0);
		}
	}
}

void TabBar::Close(i32 n, bool action)
{
	if(tabs.GetCount() <= mintabcount)
		return;

	CloseForce(n, action);
}

void TabBar::CloseKey(const Val &key)
{
	i32 tabix = FindKey(key);
	if (tabix < 0) return;
	Close(tabix);
}

TabBar::Style& TabBar::Style::DefaultCrosses()
{
	crosses[0] = TabBarImg::CR0();
	crosses[1] = TabBarImg::CR1();
	crosses[2] = TabBarImg::CR2();
	return *this;
}

TabBar::Style& TabBar::Style::Variant1Crosses()
{
	crosses[0] = TabBarImg::VARIANT1_CR0();
	crosses[1] = TabBarImg::VARIANT1_CR1();
	crosses[2] = TabBarImg::VARIANT1_CR2();
	return *this;
}

TabBar::Style& TabBar::Style::Variant2Crosses()
{
	crosses[0] = TabBarImg::VARIANT2_CR0();
	crosses[1] = TabBarImg::VARIANT2_CR1();
	crosses[2] = TabBarImg::VARIANT2_CR2();
	return *this;
}

TabBar::Style& TabBar::Style::GroupSeparators(Val horz, Val vert)
{
	group_separators[0] = horz;
	group_separators[0] = vert;
	return *this;
}

TabBar::Style& TabBar::Style::DefaultGroupSeparators()
{
	return GroupSeparators(TabBarImg::SEP(), TabBarImg::SEPV());
}

Vec<Val> TabBar::GetKeys() const
{
	Vec<Val> keys;
	keys.SetCount(tabs.GetCount());
	for (i32 i = 0; i < tabs.GetCount(); i++)
		keys[i] = tabs[i].key;
	return keys;
}

Vec<Image> TabBar::GetIcons() const
{
	Vec<Image> img;
	img.SetCount(tabs.GetCount());
	for (i32 i = 0; i < tabs.GetCount(); i++)
		img[i] = tabs[i].img;
	return img;
}

TabBar& TabBar::CopyBaseSettings(const TabBar& src)
{
	crosses = src.crosses;
	crosses_side = src.crosses_side;
	grouping = src.grouping;
	contextmenu = src.contextmenu;
	autoscrollhide = src.autoscrollhide;
	nosel = src.nosel;
	nohl = src.nohl;
	inactivedisabled = src.inactivedisabled;
	stacking = src.stacking;
	groupsort = src.groupsort;
	groupseps = src.groupseps;
	tabsort = src.tabsort;
	allownullcursor = src.allownullcursor;
	icons = src.icons;
	mintabcount = src.mintabcount;
	return *this;
}

TabBar& TabBar::CopySettings(const TabBar &src)
{

	CopyBaseSettings(src);

	if (stacking != src.stacking)
		Stacking(src.stacking);
	else {
		MakeGroups();
		Repos();
		Refresh();
	}
	return *this;
}

void TabBar::Serialize(Stream& s)
{
	i32 version = 1;
	s / version;

	s % id;
	s % crosses;
	s % crosses_side;
	s % grouping;
	s % autoscrollhide;
	s % nosel;
	s % nohl;
	s % inactivedisabled;
	s % stacking;
	s % groupsort;
	s % groupseps;
	s % tabsort;
	s % allownullcursor;
	s % icons;
	s % mintabcount;
	s % active;

	cross = -1;
	highlight = -1;
	drag_highlight = -1;
	target = -1;

	i32 n = groups.GetCount();
	s % n;
	groups.SetCount(clamp(n, 0, 10000));

	for(i32 i = 0; i < groups.GetCount(); i++)
		s % groups[i];

	n = tabs.GetCount();
	s % n;
	tabs.SetCount(clamp(n, 0, 10000));

	for(i32 i = 0; i < tabs.GetCount(); i++)
		s % tabs[i];

	i32 g = GetGroup();
	s % g;
	group = g;

	Repos();
}

TabBar& TabBar::SetStyle(const TabBar::Style& s)	{
	if(style != &s) {
		style = &s;
		RefreshLayout();
		Refresh();
	}
	return *this;
}

CH_STYLE(TabBar, Style, StyleDefault)
{
	Assign(TabCtrl::StyleDefault());
#ifdef TARGET_WIN32
	if(IsWinVista())
		Variant2Crosses();
	else
		DefaultCrosses();
#else
	DefaultCrosses();
#endif
	DefaultGroupSeparators();
}

}
