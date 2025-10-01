#include "CtrlCore.h"

#ifdef GUI_GTK

#ifdef GDK_WINDOWING_X11

namespace drx {

#define Time        XTime
#define Font        XFont
#define Display     XDisplay
#define Picture     XPicture

#ifndef TARGET_OPENBSD // avoid warning
#define CurrentTime XCurrentTime
#endif

#include <gdk/gdkx.h>

#undef Picture
#undef Time
#undef Font
#undef Display

#ifndef TARGET_OPENBSD // avoid warning
#undef CurrentTime
#endif

XDisplay *Xdisplay()
{
	return GDK_DISPLAY_XDISPLAY(gdk_display_get_default());
}

Window Xroot()
{
	return GDK_WINDOW_XID(gdk_screen_get_root_window(gdk_screen_get_default()));
}

Txt GetProperty(Window w, Atom property, Atom rtype)
{
	GuiLock __;
	Txt result;
	i32 format;
	u64 nitems, after = 1;
	long offset = 0;
	Atom type = None;
	u8* data;
	long rsize = minmax((long)(XMaxRequestSize(Xdisplay()) - 100), (long)256, (long)65536);
	while(after > 0) {
		if(XGetWindowProperty(Xdisplay(), w, property, offset, rsize, 0,
		                      rtype, &type, &format, &nitems, &after, &data) != Success)
			break;
		if(type == None)
		    break;
		if(data) {
			i32 len = format == 32 ? sizeof(u64) * nitems : nitems * (format >> 3);
			result.Cat(data, len);
			XFree((tuk )data);
			offset += nitems / (32 / format);
		}
		else
			break;
	}
	result.Shrink();
	XFlush(Xdisplay());
	return result;
}

Atom XAtomRaw(tukk name)
{
	return XInternAtom(Xdisplay(), name, 0);
}

Atom XAtom(tukk name)
{
	GuiLock __;
	Atom x;
	INTERLOCKED {
		static VecMap<Txt, i32> atoms;
		i32 q = atoms.Get(name, Null);
		if(IsNull(q)) {
			q = XAtomRaw(name);
			atoms.Add(name, q);
		}
		x = q;
	}
	return x;
}

Vec<i32> GetPropertyInts(GdkWindow *w, tukk property)
{
	GuiLock __;
	Vec<i32> result;
	Txt p = GetProperty(GDK_WINDOW_XID(w), XAtom(property), AnyPropertyType);
	i64k *ptr = (i64k *)~p;
	i64k *lim = ptr + p.GetLength() / sizeof(i64);
	result.Reserve(p.GetLength() / sizeof(i64));
	while(ptr < lim)
		result.Add(*ptr++);
	return result;
}

dword X11mods(dword key)
{
	dword mod = 0;
	if(key & K_ALT)
		mod |= Mod1Mask;
	if(key & K_SHIFT)
		mod |= ShiftMask;
	if(key & K_CTRL)
		mod |= ControlMask;
	return mod;
}

i32 Ctrl::RegisterSystemHotKey(dword key, Function<void ()> cb)
{
	GuiLock __;
	ASSERT(key >= K_DELTA);
	gdk_x11_display_error_trap_push(gdk_display_get_default());
	KeyCode k = XKeysymToKeycode(Xdisplay(), key & 0xffff);
	dword mod = X11mods(key);
	bool r = false;
	for(dword nlock = 0; nlock < 2; nlock++)
		for(dword clock = 0; clock < 2; clock++)
			for(dword slock = 0; slock < 2; slock++)
				r = XGrabKey(Xdisplay(), k,
				             mod | (nlock * Mod2Mask) | (clock * LockMask) | (slock * Mod3Mask),
				             Xroot(), true, GrabModeAsync, GrabModeAsync) || r;
	gdk_x11_display_error_trap_pop_ignored(gdk_display_get_default());
	if(!r) return -1;
	i32 q = hotkey.GetCount();
	for(i32 i = 0; i < hotkey.GetCount(); i++)
		if(!hotkey[i]) {
			q = i;
			break;
		}
	hotkey.At(q) = Event<> () << cb;
	keyhot.At(q) = k;
	modhot.At(q) = mod;
	return q;
}

void Ctrl::UnregisterSystemHotKey(i32 id)
{
	GuiLock __;
	if(id >= 0 && id < hotkey.GetCount() && hotkey[id]) {
		gdk_x11_display_error_trap_push(gdk_display_get_default());
		for(dword nlock = 0; nlock < 2; nlock++)
			for(dword clock = 0; clock < 2; clock++)
				for(dword slock = 0; slock < 2; slock++)
					XUngrabKey(Xdisplay(), keyhot[id],
					           modhot[id] | (nlock * Mod2Mask) | (clock * LockMask) | (slock * Mod3Mask),
					           Xroot());
		gdk_x11_display_error_trap_pop_ignored(gdk_display_get_default());
		hotkey[id].Clear();
	}
}

GdkFilterReturn Ctrl::RootKeyFilter(GdkXEvent *xevent, GdkEvent *Xevent, gpointer data)
{
	XEvent *event = (XEvent *)xevent;
	if(event->type == KeyPress)
		for(i32 i = 0; i < hotkey.GetCount(); i++)
			if(hotkey[i] && keyhot[i] == event->xkey.keycode &&
			   modhot[i] == (event->xkey.state & (Mod1Mask|ShiftMask|ControlMask))) {
				hotkey[i]();
				return GDK_FILTER_REMOVE;
			}
	return GDK_FILTER_CONTINUE;
}

}

#endif

#endif
