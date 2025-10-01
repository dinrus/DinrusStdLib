#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_GTK

namespace drx {

void ImageGdk::Free()
{
	if(pixbuf) {
		g_object_unref(pixbuf);
		pixbuf = NULL;
	}
}

bool ImageGdk::Set(const Image& m)
{
	Free();
	if(m.GetSerialId() != img.GetSerialId()) {
		img = Unmultiply(m);
		if(!IsNull(img)) {
			Size sz = img.GetSize();
			pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, true, 8, sz.cx, sz.cy);
			u8 *l = (u8 *)gdk_pixbuf_get_pixels(pixbuf);
			i32 stride = gdk_pixbuf_get_rowstride(pixbuf);
			for(i32 y = 0; y < sz.cy; y++) {
				const RGBA *s = img[y];
				const RGBA *e = s + sz.cx;
				u8 *t = l;
				while(s < e) {
					*t++ = s->r;
					*t++ = s->g;
					*t++ = s->b;
					*t++ = s->a;
					s++;
				}
				l += stride;
			}
		}
		return true;
	}
	return false;
}

ImageGdk::ImageGdk(const Image& m)
{
	pixbuf = NULL;
	Set(m);
}

ImageGdk::ImageGdk()
{
	pixbuf = NULL;
}

ImageGdk::~ImageGdk()
{
	Free();
}

}

#endif
