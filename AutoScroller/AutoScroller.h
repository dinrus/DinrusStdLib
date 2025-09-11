#ifndef _AutoScroller_AutoScroller_h_
#define _AutoScroller_AutoScroller_h_

#include <drx/CtrlLib/CtrlLib.h>

namespace drx
{
	class AutoScroller : public ParentCtrl
	{
	public:
		AutoScroller();
		
	public:
		void Layout() override;
		void MouseWheel(Point, i32 zdelta, dword) override;
		
	public:
		void EnableScroll(bool b = true);
		void DisableScroll()         { EnableScroll(false); }
		bool IsScrollEnabled() const { return scroll.x.IsEnabled() || scroll.y.IsEnabled(); }
	
		void AddPane(Ctrl& c, Size sz);
		void AddPane(Ctrl& c);
		
		Ctrl* GetPane() const        { return pane; }
		bool HasPane() const         { return pane; }
		void ClearPane();
	
		void Scroll(const Point& p);
		void OnScroll();
	
	public:
		Event<> WhenScrolled;
		ScrollBars scroll;
	
	protected:
		Ptr<Ctrl> pane;
	};
}

#endif
