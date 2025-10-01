#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_GTK

#define LLOG(x) // DLOG(x)

namespace drx {

void Ctrl::GuiPlatformConstruct()
{
}

void Ctrl::GuiPlatformDestruct()
{
}

void Ctrl::GuiPlatformGetTopRect(Rect& r) const
{
}

bool Ctrl::GuiPlatformRefreshFrameSpecial(const Rect& r)
{
	return false;
}

bool Ctrl::GuiPlatformSetFullRefreshSpecial()
{
	return false;
}

void GuiPlatformAdjustDragImage(ImageBuffer&)
{
}

bool GuiPlatformHasSizeGrip()
{
	return true;
}

void GuiPlatformGripResize(TopWindow *q)
{
	if(q->IsOpen()) {
		Point p = GetMousePos();
		gtk_window_begin_resize_drag(q->gtk(), GDK_WINDOW_EDGE_SOUTH_EAST,
		                             1, Ctrl::LSC(p.x), Ctrl::LSC(p.y), Ctrl::CurrentTime);
	}
}

Color GuiPlatformGetScreenPixel(i32 x, i32 y)
{
	return Black;
}

void GuiPlatformAfterMenuPopUp()
{
}

Txt Ctrl::Name() const
{
	return Name0();
}

void Ctrl::InstallPanicBox()
{
}

GdkWindow *Ctrl::gdk() const
{
	const Top *top = GetTop();
	return top ? gtk_widget_get_window(top->window) : NULL;
}

GtkWindow *Ctrl::gtk() const
{
	const Top *top = GetTop();
	return top ? (GtkWindow *)top->window : NULL;
}



}

#endif
