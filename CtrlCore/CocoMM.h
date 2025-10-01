#ifndef _CtrlCore_CocoMM_h_
#define _CtrlCore_CocoMM_h_

#include <drx/Core/config.h>

#if defined(TARGET_COCOA) && !defined(VIRTUALGUI)

#define Point NS_Point
#define Rect  NS_Rect
#define Size  NS_Size
#include <AppKit/AppKit.h>
#undef  Point
#undef  Rect
#undef  Size

#endif

#include "CtrlCore.h"

#ifdef GUI_COCOA

namespace drx {

template <class T>
struct CFRef {
	T ptr;
	T operator~()   { return ptr; }
	operator T()    { return ptr; }
	T  operator->() { return ptr; }
	T  Detach()     { T h = ptr; ptr = NULL; return h; }
	CFRef(T p)      { ptr = p; }
	~CFRef()        { if(ptr) CFRelease(ptr); }
};

struct AutoreleasePool {
	NSAutoreleasePool *pool;

	AutoreleasePool() {
		pool = [[NSAutoreleasePool alloc] init];
	}
	~AutoreleasePool() {
	    [pool release];
	}
};

CGImageRef createCGImage(const Image& img);

NSImage *GetNSImage(const Image& img);

// From Draw/FontCoco.cpp
WTxt ToWTxt(CFTxtRef s);
Txt  ToTxt(CFTxtRef s);

inline WTxt ToWTxt(NSTxt *s) { return ToWTxt((CFTxtRef)s); }
inline Txt  ToTxt(NSTxt *s)  { return ToTxt((CFTxtRef)s); }

inline CGContextRef GetCG(SystemDraw& w) { return (CGContextRef)w.GetCGHandle(); }

#define      cgHandle   (CGContextRef)handle

struct PointCG {
	i32 x, y;

	operator CGPoint() const { return CGPointMake(x, y); }
	
	PointCG(i32 x, i32 y) : x(x), y(y) {}
	PointCG();
};

struct RectCG {
	i32 x, y, cx, cy;

	operator CGRect() const { return CGRectMake(x, y, cx, cy); }
	
	RectCG(i32 x, i32 y, i32 cx, i32 cy) : x(x), y(y), cx(cx), cy(cy) {}
	RectCG();
};

NSRect DesktopRect(const drx::Rect& r);

}

@interface CocoView : NSView <NSWindowDelegate, NSTextInputClient>
{
	@public
	drx::Ptr<drx::Ctrl> ctrl;
}
@end

@interface CocoWindow : NSWindow
{
	@public
	drx::Ptr<drx::Ctrl> ctrl;
	bool active;
}
@end

struct drx::MMCtrl {
	static void SyncRect(CocoView *view);
};

struct drx::CocoTop {
	CocoWindow *window = NULL;
	CocoView *view = NULL;
	Ptr<Ctrl> owner;
};

void SyncRect(CocoView *view);

inline drx::Rect MakeRect(const CGRect& r, i32 dpi) {
	return drx::RectC(dpi * r.origin.x, dpi * r.origin.y, dpi * r.size.width, dpi * r.size.height);
}

inline CGRect CGRectDPI(const drx::Rect& r) {
	if(drx::IsUHDMode())
		return CGRectMake(0.5 * r.left, 0.5 * r.top, 0.5 * r.GetWidth(), 0.5 * r.GetHeight());
	else
		return CGRectMake(r.left, r.top, r.GetWidth(), r.GetHeight());
}

#endif

#endif
