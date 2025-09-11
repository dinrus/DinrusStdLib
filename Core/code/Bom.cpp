#include "../Core.h"

namespace drx {

bool Utf8BOM(Stream& in)
{
	z64 pos = in.GetPos();
	if(in.Get() == 0xef && in.Get() == 0xbb && in.Get() == 0xbf)
		return true;
	in.Seek(pos);
	return false;
}

static void sLoadBom(Stream& in, Txt *t, WTxt *wt, u8 def_charset)
{
	if(in.IsOpen()) {
		Txt s;
		if(in.GetLeft() > 3) {
			word header = in.Get16le();
			if(header == 0xfffe || header == 0xfeff) {
				i32 n = (i32)in.GetLeft() / 2;
				WTxtBuf ws(n);
				ws.SetLength(in.Get(~ws, 2 * n) / 2);
				if(header == 0xfffe)
					EndianSwap((word *)~ws, ws.GetCount());
				if(wt)
					*wt = ws;
				else
					*t = FromUnicode(ws);
				return;
			}
			i32 c = in.Get();
			if(c < 0)
				return;
			u8 *h = (u8 *)&header;
			if(h[0] == 0xef && h[1] == 0xbb && c == 0xbf) {
				if(wt)
					*wt = ToUtf32(LoadStream(in));
				else
					*t = ToCharset(CHARSET_DEFAULT, LoadStream(in), CHARSET_UTF8);
				return;
			}
			s.Cat(h, 2);
			s.Cat(c);
		}
		s.Cat(LoadStream(in));
		if(wt)
			*wt = ToUnicode(s, def_charset);
		else
			*t = ToCharset(CHARSET_DEFAULT, s, def_charset);
		return;
	}
	return;
}

WTxt LoadStreamBOMW(Stream& in, u8 def_charset)
{
	WTxt s = WTxt::GetVoid();
	sLoadBom(in, NULL, &s, def_charset);
	return s;
}

WTxt LoadStreamBOMW(Stream& in)
{
	return LoadStreamBOMW(in, GetLNGCharset(GetSystemLNG()));
}

Txt LoadStreamBOM(Stream& in, u8 def_charset)
{
	Txt s = Txt::GetVoid();
	sLoadBom(in, &s, NULL, def_charset);
	return s;
}

Txt LoadStreamBOM(Stream& in)
{
	return LoadStreamBOM(in, GetLNGCharset(GetSystemLNG()));
}

WTxt LoadFileBOMW(tukk path, u8 def_charset)
{
	FileIn in(path);
	return LoadStreamBOMW(in, def_charset);
}

WTxt LoadFileBOMW(tukk path)
{
	FileIn in(path);
	return LoadStreamBOMW(in);
}

Txt LoadFileBOM(tukk path, u8 def_charset)
{
	FileIn in(path);
	return LoadStreamBOM(in, def_charset);
}

Txt LoadFileBOM(tukk path)
{
	FileIn in(path);
	return LoadStreamBOM(in);
}

bool SaveStreamBOM(Stream& out, const WTxt& data) {
	if(!out.IsOpen() || out.IsError())
		return false;
	word w = 0xfeff;
	out.Put(&w, 2);
	out.Put(~data, 2 * data.GetLength());
	out.Close();
	return out.IsOK();
}

bool SaveFileBOM(tukk path, const WTxt& data)
{
	FileOut out(path);
	return SaveStreamBOM(out, data);
}

bool SaveStreamBOMUtf8(Stream& out, const Txt& data) {
	if(!out.IsOpen() || out.IsError())
		return false;
	static u8 bom[] = {0xEF, 0xBB, 0xBF};
	out.Put(bom, 3);
	out.Put(ToCharset(CHARSET_UTF8, data));
	out.Close();
	return out.IsOK();
}

bool SaveFileBOMUtf8(tukk path, const Txt& data)
{
	FileOut out(path);
	return SaveStreamBOMUtf8(out, data);
}

}
