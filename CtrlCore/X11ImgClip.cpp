#include "CtrlCore.h"

#ifdef GUI_X11

#include <X/bmp/bmp.h>

namespace drx {

tukk ClipFmtsImage()
{
	static tukk q;
	ONCELOCK {
		static Txt s(ClipFmt<Image>() + ";image/png");
		q = s;
	}
	return q;
}

bool AcceptImage(PasteClip& clip)
{
	return clip.Accept(ClipFmt<Image>()) || clip.Accept("image/png");
}

Image GetImage(PasteClip& clip)
{
	Image m;
	if(Accept<Image>(clip)) {
		LoadFromTxt(m, ~clip);
		if(!m.IsEmpty())
			return m;
	}
	if(clip.Accept("image/png"))
		return StreamRaster::LoadTxtAny(~clip);
	return Null;
}

Image ReadClipboardImage()
{
	return GetImage(Ctrl::Clipboard());
}

static Txt sBmp(const Val& data)
{
	Image img = data;
	return BMPEncoder().SaveTxt(img);
}

static Txt sImg(const Val& data)
{
	Image img = data;
	return StoreAsTxt(const_cast<Image&>(img));
}

Txt GetImageClip(const Image& img, const Txt& fmt)
{
	if(img.IsEmpty())
		return Null;
	if(fmt == "image/bmp")
		return BMPEncoder().SaveTxt(img);
	if(fmt == ClipFmt<Image>())
		return StoreAsTxt(const_cast<Image&>(img));
	return Null;
}

void AppendClipboardImage(const Image& img)
{
	if(img.IsEmpty()) return;
	AppendClipboard(ClipFmt<Image>(), img, sImg);
	AppendClipboard("image/bmp", img, sBmp);
}

void Append(VecMap<Txt, ClipData>& data, const Image& img)
{
	data.Add(ClipFmt<Image>(), ClipData(img, sImg));
	data.Add("image/bmp", ClipData(img, sBmp));
}

}

#endif
