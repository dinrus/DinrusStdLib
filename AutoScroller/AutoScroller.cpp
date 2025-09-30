#ifndef _AutoScroller_AutoScroller_hpp_
#define _AutoScroller_AutoScroller_hpp_

#include "AutoScroller.h"

using namespace drx;

constexpr i32 LINE_WIDTH  = 12;
constexpr i32 LINE_HEIGHT = 12;

AutoScroller::AutoScroller()
{
	AddFrame(scroll);
	scroll.AutoHide();
	scroll.WhenScroll = [=] { OnScroll(); };
	scroll.SetLine(DPI(LINE_WIDTH, LINE_HEIGHT));
}

void AutoScroller::MouseWheel(Point, i32 zdelta, dword)
{
	scroll.WheelY(zdelta);
}

void AutoScroller::EnableScroll(bool b)
{
	scroll.x.Enable(b);
	scroll.y.Enable(b);
}

void AutoScroller::AddPane(Ctrl& c, Size sz)
{
	ClearPane();
	pane = &c;
	Add(c);
	scroll.SetTotal(sz);
}

void AutoScroller::AddPane(Ctrl& c)
{
	AddPane(c, c.GetSize());
}

void AutoScroller::ClearPane()
{
	if (!HasPane()) {
		return;
	}
	
	pane->Remove();
	pane = nullptr;
}

void AutoScroller::Scroll(const Point& p)
{
	if (!HasPane()) {
		return;
	}
	
	Rect _r = pane->GetRect();
	Rect r(-p, _r.GetSize());
	pane->SetRect(r);
	
	WhenScrolled();
}

void AutoScroller::OnScroll()
{
	Scroll(scroll.Get());
}

void AutoScroller::Layout()
{
	Size psz = scroll.GetReducedViewSize();
	scroll.SetPage(psz);
}

#endif
