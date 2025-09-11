#include "Local.h"

#ifdef VIRTUALGUI

NAMESPACE_UPP

#define LLOG(x)    // LOG(x)
#define LDUMP(x)  //DDUMP(x)
#define LDUMPC(x) //DDUMPC(x)
#define LTIMING(x)  // RTIMING(x)

struct NilGui : VirtualGui {
	virtual Size        GetSize() { return Size(0, 0); }
	virtual dword       GetMouseButtons() { return 0; }
	virtual dword       GetModKeys() { return 0; }
	virtual bool        IsMouseIn() { return false; }
	virtual bool        ProcessEvent(bool *quit) { return false; }
	virtual void        WaitEvent(i32 ms) {}
	virtual void        WakeUpGuiThread() {}
	virtual void        Quit() {}
	virtual bool        IsWaitingEvent() { return false; }
	virtual SystemDraw& BeginDraw() { static SystemDraw h; static NilDraw n; h.SetTarget(&n); return h; }
	virtual void        CommitDraw() {}
};

NilGui NilGuiInstance;

VirtualGui *VirtualGuiPtr = &NilGuiInstance;

Size           desktop_size;
Ptr<Ctrl>      Ctrl::desktop;
Vec<Ctrl *> Ctrl::topctrl;

bool           Ctrl::invalid;

Point          Ctrl::fbCursorPos = Null;
Image          Ctrl::fbCursorImage;

Rect           Ctrl::fbCaretRect;
i32            Ctrl::fbCaretTm;
bool           Ctrl::fbEndSession;
i32            Ctrl::PaintLock;

i32 Ctrl::FindTopCtrl() const
{
	for(i32 i = 0; i < topctrl.GetCount(); i++)
		if(this == topctrl[i])
			return i;
	return -1;
}

bool Ctrl::IsAlphaSupported()
{
	return false;
}

bool Ctrl::IsCompositedGui()
{
	return false;
}

Vec<Ctrl *> Ctrl::GetTopCtrls()
{
	Vec<Ctrl *> ctrl;
	if(desktop)
		ctrl.Add(desktop);
	for(i32 i = 0; i < topctrl.GetCount(); i++)
		if(!dynamic_cast<TopWindowFrame *>(topctrl[i]))
			ctrl.Add(topctrl[i]);
	return ctrl;
}

Ctrl *Ctrl::GetOwner()
{
	GuiLock __;
	i32 q = FindTopCtrl();
	if(q >= 0) {
		Top *top = topctrl[q]->GetTop();
		if(q > 0 && top) {
			Ctrl *x = top->owner_window;
			return dynamic_cast<TopWindowFrame *>(x) ? x->GetOwner() : x;
		}
	}
	return NULL;
}

Ctrl *Ctrl::GetActiveCtrl()
{
	GuiLock __;
	return focusCtrl ? focusCtrl->GetTopCtrl() : NULL;
}

i32 Ctrl::RegisterSystemHotKey(dword key, Function<void ()> cb)
{
	return -1;
}

void Ctrl::UnregisterSystemHotKey(i32 id)
{
}

bool Ctrl::IsWaitingEvent()
{
	return VirtualGuiPtr->IsWaitingEvent();
}

void Ctrl::SyncTopWindows()
{
	for(i32 i = 0; i < topctrl.GetCount(); i++) {
		TopWindow *w = dynamic_cast<TopWindow *>(topctrl[i]);
		if(w)
			w->SyncRect();
	}
}

ViewDraw::ViewDraw(Ctrl *ctrl)
{
}

ViewDraw::~ViewDraw()
{
}

Rect Ctrl::GetClipBound(const Vec<Rect>& inv, const Rect& r)
{
	Rect ri = Null;
	for(i32 j = 0; j < inv.GetCount(); j++) {
		Rect rr = inv[j] & r;
		if(!rr.IsEmpty())
			ri = IsNull(ri) ? rr : rr | ri;
	}
	return ri;
}

void Ctrl::PaintScene(SystemDraw& draw)
{
	if(!desktop)
		return;
	LLOG("@ DoPaint");
	LTIMING("DoPaint paint");
	draw.Begin();
	Vec<Rect> invalid;
	invalid.Add(VirtualGuiPtr->GetSize());
	for(i32 i = topctrl.GetCount() - 1; i >= 0; i--) {
		Rect r = topctrl[i]->GetRect();
		Rect ri = GetClipBound(invalid, r);
		if(!IsNull(ri)) {
			draw.Clipoff(r);
			topctrl[i]->UpdateArea(draw, ri - r.TopLeft());
			draw.End();
			Subtract(invalid, r);
			draw.ExcludeClip(r);
		}
	}
	Rect ri = GetClipBound(invalid, desktop->GetRect().GetSize());
	if(!IsNull(ri))
		desktop->UpdateArea(draw, ri);
	draw.End();
}

void Ctrl::PaintCaretCursor(SystemDraw& draw)
{
	if(!IsNull(fbCaretRect))
		draw.DrawRect(fbCaretRect, InvertColor);
	if(VirtualGuiPtr->IsMouseIn() && !(VirtualGuiPtr->GetOptions() & GUI_SETMOUSECURSOR))
		draw.DrawImage(fbCursorPos.x, fbCursorPos.y, fbCursorImage);
}

void Ctrl::DoPaint()
{
	if(!PaintLock) {
		if(invalid && desktop) {
			invalid = false;
			SystemDraw& draw = VirtualGuiPtr->BeginDraw();
			PaintScene(draw);
			PaintCaretCursor(draw);
			VirtualGuiPtr->CommitDraw();
		}
	}
}

void Ctrl::WndUpdate(const Rect&)
{
	GuiLock __;
	Invalidate();
	DoPaint();
}

Rect Ctrl::GetWndScreenRect() const
{
	GuiLock __;
	return GetRect();
}

void Ctrl::WndShow(bool b)
{
	GuiLock __;
}

void Ctrl::WndUpdate()
{
	GuiLock __;
}

bool Ctrl::IsWndOpen() const {
	GuiLock __;
	return FindTopCtrl() >= 0 || this == desktop;
}

void Ctrl::SetAlpha(u8 alpha)
{
	GuiLock __;
}

Rect Ctrl::GetWorkArea() const
{
	GuiLock __;
	return GetVirtualScreenArea();
}

void Ctrl::GetWorkArea(Array<Rect>& rc)
{
	GuiLock __;
	Array<Rect> r;
	r.Add(GetVirtualScreenArea());
}

Rect Ctrl::GetVirtualWorkArea()
{
	return GetVirtualScreenArea();
}
/*
Rect Ctrl::GetWorkArea(Point pt)
{
	return GetVirtualScreenArea();
}
*/
Rect Ctrl::GetVirtualScreenArea()
{
	GuiLock __;
	return desktop_size;
//	if(desktop)
//		DLOG("GET " << desktop->GetRect());
//	return desktop ? desktop->GetRect() : Rect(0, 0, 0, 0);
}

Rect Ctrl::GetPrimaryWorkArea()
{
	return GetVirtualScreenArea();
}

Rect Ctrl::GetPrimaryScreenArea()
{
	return GetVirtualScreenArea();
}

i32 Ctrl::GetKbdDelay()
{
	GuiLock __;
	return 500;
}

i32 Ctrl::GetKbdSpeed()
{
	GuiLock __;
	return 1000 / 32;
}

void Ctrl::DestroyWnd()
{
	for(i32 i = 0; i < topctrl.GetCount(); i++) {
		Top *top = topctrl[i]->GetTop();
		if(top && top->owner_window == this)
			topctrl[i]->WndDestroy();
	}
	i32 q = FindTopCtrl();
	if(q >= 0) {
		Invalidate();
		topctrl.Remove(q);
	}
	if(top)
		DeleteTop();
	isopen = false;
	TopWindow *win = dynamic_cast<TopWindow *>(this);
	if(win)
		win->DestroyFrame();
}

void Ctrl::WndDestroy()
{
	DestroyWnd();
	if(topctrl.GetCount())
		topctrl.Top()->ActivateWnd();
}

void Ctrl::PutForeground()
{
	i32 q = FindTopCtrl();
	if(q >= 0) {
		Invalidate();
		topctrl.Remove(q);
		topctrl.Add(this);
	}
	Vec< Ptr<Ctrl> > fw;
	for(i32 i = 0; i < topctrl.GetCount(); i++)
		if(topctrl[i]) {
			Top *top = topctrl[i]->GetTop();
			if(top && top->owner_window == this && topctrl[i] != this)
				fw.Add(topctrl[i]);
		}
	for(i32 i = 0; i < fw.GetCount(); i++)
		if(fw[i])
			fw[i]->PutForeground();
}

void Ctrl::SetWndForeground()
{
	GuiLock __;
	ASSERT(IsOpen());
	if(IsWndForeground())
		return;
	Ctrl *to = this;
	while(to->GetTop() && to->GetTop()->owner_window)
		to = to->GetTop()->owner_window;
	to->PutForeground();
	if(this != focusCtrl)
		ActivateWnd();
}

bool Ctrl::IsWndForeground() const
{
	GuiLock __;
	bool b = false;
	for(i32 i = 0; i < topctrl.GetCount(); i++) {
		const TopWindow *tw = dynamic_cast<const TopWindow *>(topctrl[i]);
		if(tw)
			b = tw == this;
	}
	return b;
}

void Ctrl::WndEnable(bool)
{
	GuiLock __;
}

bool Ctrl::SetWndFocus()
{
	GuiLock __;
	return true;
}

bool Ctrl::HasWndFocus() const
{
	GuiLock __;
	return focusCtrl && focusCtrl->GetTopCtrl() == this;
}

bool Ctrl::SetWndCapture()
{
	GuiLock __;
	ASSERT(IsMainThread());
	return true;
}

bool Ctrl::ReleaseWndCapture()
{
	GuiLock __;
	ASSERT(IsMainThread());
	return true;
}

bool Ctrl::HasWndCapture() const
{
	GuiLock __;
	return captureCtrl && captureCtrl->GetTopCtrl() == this;
}

void Ctrl::WndInvalidateRect(const Rect&)
{
	GuiLock __;
	Invalidate();
}

void Ctrl::WndSetPos(const Rect& rect)
{
	GuiLock __;
	TopWindow *w = dynamic_cast<TopWindow *>(this);
	if(w)
		w->SyncFrameRect(rect);
	Invalidate();
	SetWndRect(rect);
}

void  Ctrl::WndScrollView(const Rect& r, i32 dx, i32 dy)
{
	GuiLock __;
	LLOG("ScrollView " << rect);
	WndInvalidateRect(r);
}

void Ctrl::PopUp(Ctrl *owner, bool savebits, bool activate, bool dropshadow, bool topmost)
{
	ASSERT(!IsChild() && !IsOpen() && FindTopCtrl() < 0);
	NewTop();
	if(owner) {
		Ctrl *owner_window = owner->GetTopWindow();
		if(!owner_window)
			owner_window = owner->GetTopCtrl();
		ASSERT(owner_window->IsOpen());
		if(owner_window != desktop) {
			owner_window->SetForeground();
			if(GetTop())
				GetTop()->owner_window = owner_window;
		}
	}
	topctrl.Add(this);
	popup = isopen = true;
	RefreshLayoutDeep();
	if(activate) SetFocusWnd();
	Invalidate();
}

Rect Ctrl::GetDefaultWindowRect() {
	GuiLock __;
	static i32 ii = 0;
	Rect rect = GetVirtualScreenArea();
	Size sz = rect.GetSize();
	rect.Deflate(sz / 10);
	rect.Offset(Size(GetStdFontCy(), 2 * GetStdFontCy()) * (++ii % 8));
	return rect;
}

Vec<WTxt> SplitCmdLine__(tukk cmd)
{
	Vec<WTxt> out;
	while(*cmd)
		if((u8)*cmd <= ' ')
			cmd++;
		else if(*cmd == '\"') {
			WTxt quoted;
			while(*++cmd && (*cmd != '\"' || *++cmd == '\"'))
				quoted.Cat(FromSystemCharset(Txt(cmd, 1)).ToWTxt());
			out.Add(quoted);
		}
		else {
			tukk begin = cmd;
			while((u8)*cmd > ' ')
				cmd++;
			out.Add(Txt(begin, cmd).ToWTxt());
		}
	return out;
}

void Ctrl::InstallPanicBox()
{
}

void Ctrl::SysEndLoop()
{
}

void Ctrl::DeleteDesktopTop()
{
	if(desktop && desktop->GetTop())
		desktop->DeleteTop();
}

void Ctrl::SetDesktop(Ctrl& q)
{
	DeleteDesktopTop();
	desktop = &q;
	desktop->SetOpen(true);
	desktop->NewTop();
	invalid = true;
	if(VirtualGuiPtr)
		SetDesktopSize(VirtualGuiPtr->GetSize());
}

void Ctrl::SetDesktopSize(Size sz)
{
	desktop_size = sz;
	if(desktop)
		desktop->SetRect(sz);
	invalid = true;
	SyncTopWindows();
}

void Ctrl::InitFB()
{
	Ctrl::GlobalBackBuffer();
	Ctrl::InitTimer();

	SetStdFont(ScreenSans(12));
	ChStdSkin();

	static StaticRect x;
	x.Color(Cyan());
	SetDesktop(x);
}

void Ctrl::EndSession()
{
	GuiLock __;
	LLOG("Ctrl::EndSession");
	fbEndSession = true;
	EndSessionLoopNo = EventLoopNo;
}

void Ctrl::ExitFB()
{
	TopWindow::ShutdownWindows();
	Ctrl::CloseTopCtrls();
	DeleteDesktopTop();
	if(fbEndSession)
		VirtualGuiPtr->Quit();
	VirtualGuiPtr = NULL;
}

void RunVirtualGui(VirtualGui& gui, Event<> app_main)
{
	VirtualGuiPtr = &gui;
	Ctrl::InitFB();
	Ctrl::SetDesktopSize(gui.GetSize());
	app_main();
	Ctrl::ExitFB();
}

END_DRX_NAMESPACE

#endif
