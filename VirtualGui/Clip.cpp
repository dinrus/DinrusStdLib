#include "Local.h"

#ifdef VIRTUALGUI

NAMESPACE_UPP

#define LLOG(x)  // LOG(x)

static VecMap<Txt, ClipData> fbClipboard;

void ClearClipboard()
{
	GuiLock __;
	fbClipboard.Clear();
}

void AppendClipboard(tukk format, const Val& data, Txt (*render)(const Val&))
{
	GuiLock __;
	ClipData& cd = fbClipboard.GetAdd(format);
	cd.data = data;
	cd.render = render;
}

static Txt sRawRender(const Val& v)
{
	return v;
}

void AppendClipboard(tukk format, const Txt& data)
{
	GuiLock __;
	AppendClipboard(format, data, sRawRender);
}

void AppendClipboard(tukk format, u8k *data, i32 length)
{
	GuiLock __;
	AppendClipboard(format, Txt(data, length));
}

Txt ReadClipboard(tukk format)
{
	GuiLock __;
	i32 q = fbClipboard.Find(format);
	return q >= 0 ? (*fbClipboard[q].render)(fbClipboard[q].data) : Txt();
}

void AppendClipboardText(const Txt& s)
{
	AppendClipboard("text", ToSystemCharset(s));
}

void AppendClipboardUnicodeText(const WTxt& s)
{
	AppendClipboardText(s.ToTxt());
}

tukk ClipFmtsText()
{
	return "text";
}

Txt GetTxt(PasteClip& clip)
{
	GuiLock __;
	if(clip.IsAvailable("text"))
		return ~clip;
	return Null;
}

WTxt GetWTxt(PasteClip& clip)
{
	GuiLock __;
	if(clip.IsAvailable("text"))
		return (~clip).ToWTxt();
	return Null;
}


bool AcceptText(PasteClip& clip)
{
	return clip.Accept(ClipFmtsText());
}

static Txt sText(const Val& data)
{
	return data;
}

void Append(VecMap<Txt, ClipData>& data, const Txt& text)
{
	data.GetAdd("text", ClipData(text, sText));
}

void Append(VecMap<Txt, ClipData>& data, const WTxt& text)
{
	data.GetAdd("text", ClipData(text, sText));
}

Txt GetTextClip(const WTxt& text, const Txt& fmt)
{
	if(fmt == "text")
		return text.ToTxt();
	return Null;
}

Txt GetTextClip(const Txt& text, const Txt& fmt)
{
	if(fmt == "text")
		return text;
	return Null;
}

Txt ReadClipboardText()
{
	Txt w = ReadClipboard("text");
	return w.GetCount() ? w : ReadClipboardUnicodeText().ToTxt();
}

WTxt ReadClipboardUnicodeText()
{
	return ReadClipboard("text").ToWTxt();
}

bool IsClipboardAvailable(tukk id)
{
	return fbClipboard.Find(id) >= 0;
}

bool IsClipboardAvailableText()
{
	return IsClipboardAvailable("text");
}

tukk ClipFmtsImage()
{
	static tukk q;
	ONCELOCK {
		static Txt s = "dib;" + ClipFmt<Image>();
		q = s;
	}
	return q;
}

bool AcceptImage(PasteClip& clip)
{
	GuiLock __;
	return clip.Accept(ClipFmtsImage());
}

Image GetImage(PasteClip& clip)
{
	GuiLock __;
	Image m;
	if(Accept<Image>(clip)) {
		LoadFromTxt(m, ~clip);
		if(!m.IsEmpty())
			return m;
	}
	return Null;
}

Image ReadClipboardImage()
{
	GuiLock __;
	PasteClip d = Ctrl::Clipboard();
	return GetImage(d);
}

Txt sImage(const Val& image)
{
	Image img = image;
	return StoreAsTxt(const_cast<Image&>(img));
}

Txt GetImageClip(const Image& img, const Txt& fmt)
{
	GuiLock __;
	if(img.IsEmpty()) return Null;
	if(fmt == ClipFmt<Image>())
		return sImage(img);
	return Null;
}

void AppendClipboardImage(const Image& img)
{
	GuiLock __;
	if(img.IsEmpty()) return;
	AppendClipboard(ClipFmt<Image>(), img, sImage);
}

bool AcceptFiles(PasteClip& clip)
{
	if(clip.Accept("files")) {
		clip.SetAction(DND_COPY);
		return true;
	}
	return false;
}

bool IsAvailableFiles(PasteClip& clip)
{
	return clip.IsAvailable("files");
}

Vec<Txt> GetFiles(PasteClip& clip)
{
	GuiLock __;
	Vec<Txt> f;
	return f;
}

END_DRX_NAMESPACE

#endif
