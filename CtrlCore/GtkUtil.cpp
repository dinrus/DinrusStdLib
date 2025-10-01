#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_GTK

namespace drx {

GdkRect::GdkRect(const Rect& r)
{
	x = r.left;
	y = r.top;
	width = r.GetWidth();
	height = r.GetHeight();
}

Txt ImageClipFromPixbufUnref(GdkPixbuf *pixbuf)
{
	Image img;
	if(pixbuf) {
		i32 chn = gdk_pixbuf_get_n_channels(pixbuf);
		if((chn == 3 && !gdk_pixbuf_get_has_alpha(pixbuf) ||
		    chn == 4 && gdk_pixbuf_get_has_alpha(pixbuf)) &&
		   gdk_pixbuf_get_colorspace(pixbuf) == GDK_COLORSPACE_RGB &&
		   gdk_pixbuf_get_bits_per_sample(pixbuf) == 8) {
			Size sz(gdk_pixbuf_get_width (pixbuf), gdk_pixbuf_get_height(pixbuf));
			ImageBuffer m(sz);
			i32 stride = gdk_pixbuf_get_rowstride(pixbuf);
			u8 *l = (u8 *)gdk_pixbuf_get_pixels(pixbuf);
			for(i32 y = 0; y < sz.cy; y++) {
				RGBA *s = m[y];
				const RGBA *e = s + sz.cx;
				u8k *t = l;
				if(chn == 4)
					while(s < e) {
						s->r = *t++;
						s->g = *t++;
						s->b = *t++;
						s->a = *t++;
						s++;
					}
				else
					while(s < e) {
						s->r = *t++;
						s->g = *t++;
						s->b = *t++;
						s->a = 255;
						s++;
					}
				l += stride;
			}
			img = m;
		}
		g_object_unref(pixbuf);
	}
	return StoreAsTxt(img);
}

Txt FilesClipFromUrisFree(gchar **uris)
{
	if(uris) {
		Txt h;
		for(i32 i = 0; uris[i]; i++)
			h << uris[i] << '\n';
		g_strfreev (uris);
		return h;
	}
	return Null;
}

GdkAtom GAtom(const Txt& id)
{
	GuiLock __;
	static VecMap<Txt, GdkAtom> m;
	i32 q = m.Find(id);
	return q >= 0 ? m[q] : (m.Add(id) = gdk_atom_intern(~id, FALSE));
}

i32 rmsecs()
{
	static i32 msecs0 = msecs();
	return msecs(msecs0);
}
 
}

#endif
