#include "CtrlCore.h"
#include <X/bmp/bmp.h>

#ifdef GUI_WIN

namespace drx {

#define LLOG(x)  // LOG(x)

VecMap<i32, ClipData>& sClipMap()
{
	static VecMap<i32, ClipData> x;
	return x;
}

extern HWND utilityHWND;

i32  GetClipboardFormatCode(tukk format_id)
{
	GuiLock ___;
	i32 x = (i32)(intptr_t)format_id;
	if(x >= 0 && x < 65535)
		return x;
	Txt fmt = format_id;
	if(fmt == "text")
		return CF_TEXT;
	if(fmt == "wtext")
		return CF_UNICODETEXT;
	if(fmt == "dib")
		return CF_DIB;
	if(fmt == "files")
		return CF_HDROP;
	static StaticMutex m;
	Mutex::Lock __(m);
	static VecMap<Txt, i32> format_map;
	i32 f = format_map.Find(format_id);
	if(f < 0) {
		f = format_map.GetCount();
		format_map.Add(format_id,
#ifdef TARGET_WINCE
			::RegisterClipboardFormat(ToSystemCharset(format_id))
#else
			::RegisterClipboardFormat(format_id)
#endif
		);
	}
	return format_map[f];
}

bool DebugClipboard()
{
	static bool b = GetIniKey("DebugClipboard") == "1";
	return b;
}

void ClipboardLog(tukk txt)
{
	if(!DebugClipboard())
		return;
	FileAppend f(GetExeDirFile("clip.log"));
	f << GetSysTime() << ": " << txt << "\n";
}

void ClipboardError(tukk txt)
{
	if(!DebugClipboard())
		return;
	Txt s = txt;
	s << "\n" << GetLastErrorMessage();
	MessageBox(::GetActiveWindow(), s, "Clipboard error", MB_ICONSTOP | MB_OK | MB_APPLMODAL);
	ClipboardLog(Txt().Cat() << s << " ERROR");
}

Txt FromWin32CF(i32 cf);

void ClipboardError(tukk txt, i32 format)
{
	if(!DebugClipboard())
		return;
	ClipboardError(Txt().Cat() << txt << ' ' << FromWin32CF(format));
}

bool ClipboardOpen()
{
	// Win32 has serious race condition problem with clipboard; system or other apps open it
	// right after we close it thus blocking us to send more formats
	// So the solution is to wait and retry... (mirek, 2011-01-09)
	for(i32 i = 0; i < 200; i++) {
		if(OpenClipboard(utilityHWND)) {
			ClipboardLog("----- ClipboardOpen OK");
			return true;
		}
		Sleep(10);
	}
	ClipboardError("ClipboardOpen has failed!");
	return false;
}

void ClearClipboard()
{
	GuiLock __;
	sClipMap().Clear();
	ClipboardLog("* ClearClipboard");
	if(ClipboardOpen()) {
		if(!EmptyClipboard())
			ClipboardError("EmptyClipboard ERROR");
		if(!CloseClipboard())
			ClipboardError("CloseClipboard ERROR");
	}
}

void SetClipboardRaw(i32 format, u8k *data, i32 length)
{
	GuiLock __;
	HANDLE handle = NULL;
	ClipboardLog("* SetClipboardRaw");
	if(data) {
		handle = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, length + 2);
		u8 *ptr;
		if(!handle) {
			ClipboardError("GlobalAlloc ERROR");
			return;
		}
		if(!(ptr = (u8 *)GlobalLock(handle))) {
			ClipboardError("GlobalLock ERROR");
			GlobalFree(handle);
			return;
		}
		memcpy(ptr, data, length);
		ptr[length] = 0;
		ptr[length + 1] = 0;
		GlobalUnlock(handle);
	}
	if(SetClipboardData(format, handle) != handle) {
		ClipboardError("SetCliboardData", format);
		LLOG("SetClipboardData error: " << GetLastErrorMessage());
		GlobalFree(handle);
	}
}

void AppendClipboard(i32 format, u8k *data, i32 length)
{
	GuiLock __;
	ClipboardLog("* AppendClipboard");
	if(ClipboardOpen()) {
		SetClipboardRaw(format, data, length);
		if(!CloseClipboard())
			ClipboardError("CloseClipboard", format);
	}
}

void AppendClipboard(tukk format, u8k *data, i32 length)
{
	GuiLock __;
	Vec<Txt> f = Split(format, ';');
	for(i32 i = 0; i < f.GetCount(); i++)
		AppendClipboard(GetClipboardFormatCode(f[i]), data, length);
}

void AppendClipboard(tukk format, const Txt& data)
{
	GuiLock __;
	AppendClipboard(format, data, data.GetLength());
}

void AppendClipboard(tukk format, const Val& data, Txt (*render)(const Val&))
{
	GuiLock __;
	Vec<Txt> f = Split(format, ';');
	for(i32 i = 0; i < f.GetCount(); i++) {
		i32 c = GetClipboardFormatCode(f[i]);
		sClipMap().GetAdd(c) = ClipData(data, render);
		AppendClipboard(c, NULL, 0);
	}
}

void Ctrl::RenderFormat(i32 format)
{
	GuiLock __;
	i32 q = sClipMap().Find(format);
	if(q >= 0) {
		Txt s = sClipMap()[q].Render();
		SetClipboardRaw(format, s, s.GetLength());
	}
}

void Ctrl::RenderAllFormats()
{
	GuiLock __;
	if(sClipMap().GetCount() && OpenClipboard(utilityHWND)) {
		for(i32 i = 0; i < sClipMap().GetCount(); i++)
			RenderFormat(sClipMap().GetKey(i));
		CloseClipboard();
	}
}

void Ctrl::DestroyClipboard()
{
	GuiLock __;
	sClipMap().Clear();
}

Txt ReadClipboard(tukk format)
{
	GuiLock __;
	if(!ClipboardOpen())
		return Null;
	HGLOBAL hmem = GetClipboardData(GetClipboardFormatCode(format));
	if(hmem == 0) {
		ClipboardError("GetClipboardData failed");
		CloseClipboard();
		return Null;
	}
	u8k *src = (u8k *)GlobalLock(hmem);
	ASSERT(src);
	i32 length = (i32)GlobalSize(hmem);
	if(length < 0) {
		ClipboardError("ReadCliboard length < 0");
		CloseClipboard();
		return Null;
	}
	Txt out(src, length);
	GlobalUnlock(hmem);
	CloseClipboard();
	return out;
}

void AppendClipboardText(const Txt& s)
{
	AppendClipboardUnicodeText(s.ToWTxt());
}

void AppendClipboardUnicodeText(const WTxt& s)
{
	Vec<char16> ws = ToUtf16(s);
	AppendClipboard("wtext", (u8k *)ws.begin(), 2 * ws.GetCount());
}

tukk ClipFmtsText()
{
	return "wtext;text";
}

Txt GetTxt(PasteClip& clip)
{
	GuiLock __;
	if(clip.Accept("wtext")) {
		Txt s = ~clip;
		return ToUtf8((const char16 *)~s, strlen16((const char16 *)~s));
	}
	if(clip.Accept("text"))
		return ~clip;
	return Null;
}

WTxt GetWTxt(PasteClip& clip)
{
	GuiLock __;
	if(clip.Accept("wtext")) {
		Txt s = ~clip;
		return ToUtf32((const char16 *)~s, strlen16((const char16 *)~s));
	}
	if(clip.Accept("text"))
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

static Txt sWText(const Val& data)
{
	Vec<char16> h = ToUtf16(WTxt(data));
	return Txt((tuk )h.begin(), sizeof(char16) * h.GetCount());
}

void Append(VecMap<Txt, ClipData>& data, const Txt& text)
{
	data.GetAdd("text", ClipData(text, sText));
	data.GetAdd("wtext", ClipData(text, sWText));
}

void Append(VecMap<Txt, ClipData>& data, const WTxt& text)
{
	data.GetAdd("text", ClipData(text, sText));
	data.GetAdd("wtext", ClipData(text, sWText));
}

Txt GetTextClip(const WTxt& text, const Txt& fmt)
{
	if(fmt == "text")
		return text.ToTxt();
	if(fmt == "wtext")
		return sWText(text);
	return Null;
}

Txt GetTextClip(const Txt& text, const Txt& fmt)
{
	if(fmt == "text")
		return text;
	if(fmt == "wtext")
		return sWText(text.ToWTxt());
	return Null;
}

Txt ReadClipboardText()
{
#ifdef TARGET_WINCE
	return ReadClipboardUnicodeText().ToTxt();
#else
	Txt s = ReadClipboardUnicodeText().ToTxt();
	if(s.GetCount())
		return s;
	s = ReadClipboard((tukk )CF_TEXT);
	return Txt(s, (i32)strlen(~s));
#endif
}

WTxt ReadClipboardUnicodeText()
{
	Txt s = ReadClipboard((tukk )CF_UNICODETEXT);
	return ToUtf32((const char16 *)~s, strlen16((const char16 *)~s));
}

bool IsClipboardAvailable(tukk id)
{
	return ::IsClipboardFormatAvailable(GetClipboardFormatCode(id));
}

bool IsClipboardAvailableText()
{
	return IsClipboardAvailable((tukk )CF_TEXT);
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
	if(clip.Accept("dib")) {
		Txt data = ~clip;
		if((unsigned)data.GetCount() < sizeof(BITMAPINFO)) return Null;
		BITMAPINFO *lpBI = (BITMAPINFO *)~data;
		BITMAPINFOHEADER& hdr = lpBI->bmiHeader;
		u8 *bits = (u8 *)lpBI + hdr.biSize;
		if(hdr.biBitCount <= 8)
			bits += (hdr.biClrUsed ? hdr.biClrUsed : 1 << hdr.biBitCount) * sizeof(RGBQUAD);
		if(hdr.biBitCount >= 16 || hdr.biBitCount == 32) {
			if(hdr.biCompression == 3)
				bits += 12;
			if(hdr.biClrUsed != 0)
				bits += hdr.biClrUsed * sizeof(RGBQUAD);
		}
		i32 h = abs((i32)hdr.biHeight);
		ImageDraw   iw(hdr.biWidth, h);
		::StretchDIBits(iw.GetHandle(),
			0, 0, hdr.biWidth, h,
			0, 0, hdr.biWidth, h,
			bits, lpBI, DIB_RGB_COLORS, SRCCOPY);
		return iw;
	}
	return Null;
}

Image ReadClipboardImage()
{
	GuiLock __;
	PasteClip d = Ctrl::Clipboard();
	return GetImage(d);
}

Txt sDib(const Val& image)
{
	Image img = image;
	BITMAPINFOHEADER header;
	Zero(header);
	header.biSize = sizeof(header);
	header.biWidth = img.GetWidth();
	header.biHeight = -img.GetHeight();
	header.biBitCount = 32;
	header.biPlanes = 1;
	header.biCompression = BI_RGB;
	TxtBuf b(i32(sizeof(header) + 4 * img.GetLength()));
	u8 *p = (u8 *)~b;
	memcpy(p, &header, sizeof(header));
	memcpy(p + sizeof(header), ~img, 4 * img.GetLength());
	return Txt(b);
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
	if(fmt == "dib")
		return sDib(img);
	if(fmt == ClipFmt<Image>())
		return sImage(img);
	return Null;
}

void AppendClipboardImage(const Image& img)
{
	GuiLock __;
	if(img.IsEmpty()) return;
	AppendClipboard(ClipFmt<Image>(), img, sImage);
	AppendClipboard("dib", img, sDib);
}

void Append(VecMap<Txt, ClipData>& data, const Image& img)
{
	data.Add(ClipFmt<Image>(), ClipData(img, sImage));
	data.Add("dib", ClipData(img, sDib));
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

struct sDROPFILES {
    DWORD offset;
    POINT pt;
    BOOL  nc;
    BOOL  unicode;
};

Vec<Txt> GetClipFiles(const Txt& data)
{
	GuiLock __;
	Vec<Txt> f;
	if((unsigned)data.GetCount() < sizeof(sDROPFILES) + 2)
		return f;
	const sDROPFILES *df = (const sDROPFILES *)~data;
	tukk s = ((tukk )df + df->offset);
	if(df->unicode) {
		const char16 *ws = (char16 *)s;
		while(*ws) {
			WTxt fn;
			while(*ws)
				fn.Cat(*ws++);
			f.Add(fn.ToTxt());
			ws++;
		}
	}
	else
		while(*s) {
			Txt fn;
			while(*s)
				fn.Cat(*s++);
			f.Add(fn.ToTxt());
			s++;
		}
	return f;
}

Vec<Txt> GetFiles(PasteClip& clip)
{
	GuiLock __;
	Vec<Txt> f;
	return GetClipFiles(clip.Get("files"));
}

void AppendFiles(VecMap<Txt, ClipData>& clip, const Vec<Txt>& files)
{
	WTxt wfiles32;
	for(i32 i = 0; i < files.GetCount(); i++)
		wfiles32 << files[i].ToWTxt() << (wchar)0;
	Vec<char16> wfiles = ToUtf16(wfiles32);
	wfiles.Add(0);
	sDROPFILES h;
	h.unicode = true;
	h.offset = sizeof(h);
    GetCursorPos(&h.pt);
    h.nc = TRUE;
    Txt data;
	data.Cat((u8 *)&h, sizeof(h));
	data.Cat((u8 *)wfiles.begin(), sizeof(char16) * (wfiles.GetCount() + 1));
	clip.GetAdd("files") = ClipData(data);
}

bool   Has(UDropTarget *dt, tukk fmt);
Txt Get(UDropTarget *dt, tukk fmt);

bool PasteClip::IsAvailable(tukk fmt) const
{
	if(this == &Ctrl::Selection())
		return false;
	return dt ? DRX::Has(dt, fmt) : IsClipboardAvailable(fmt);
}

Txt PasteClip::Get(tukk fmt) const
{
	if(this == &Ctrl::Selection())
		return Null;
	return dt ? DRX::Get(dt, fmt) : ReadClipboard(fmt);
}

void PasteClip::GuiPlatformConstruct()
{
	dt = NULL;
}

}

#endif
