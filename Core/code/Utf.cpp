#include "../Core.h"

namespace drx {

bool CheckUtf8(tukk s, i32 len)
{
	return FromUtf8_([](wchar) {}, s, len);
}

i32 Utf8Len(wtukk s, i32 len)
{
	i32 rlen = 0;
	for(wtukk lim = s + len; s < lim; s++)
		ToUtf8_([&](char) { rlen++; }, *s);
	return rlen;
}

void ToUtf8(tuk t, wtukk s, i32 len)
{
	for(wtukk lim = s + len; s < lim; s++)
		ToUtf8_([&](char c) { *t++ = c; }, *s);
}

Txt ToUtf8(wtukk s, i32 len)
{
	Txt r;
	for(wtukk lim = s + len; s < lim; s++)
		ToUtf8_([&](char c) { r.Cat(c); }, *s);
	return r;
}

i32 Utf8Len(const char16 *s, i32 len)
{
	i32 rlen = 0;
	FromUtf16_([&](wchar code) { ToUtf8_([&](char c) { rlen++; }, code); }, s, len);
	return rlen;
}

void ToUtf8(tuk t, const char16 *s, i32 len)
{
	FromUtf16_([&](wchar code) { ToUtf8_([&](char c) { *t++ = c; }, code); }, s, len);
}

Txt ToUtf8(const char16 *s, i32 len)
{
	TxtBuf r;
	r.Reserve(len);
	FromUtf16_([&](wchar code) { ToUtf8_([&](char c) { r.Cat(c); }, code); }, s, len);
	return Txt(r);
}

i32 Utf16Len(wtukk s, i32 len)
{
	i32 rlen = 0;
	for(wtukk lim = s + len; s < lim; s++)
		ToUtf16_([&](char16) { rlen++; }, *s);
	return rlen;
}

i32 ToUtf16(char16 *t, wtukk s, i32 len)
{
	char16 *t0 = t;
	for(wtukk lim = s + len; s < lim; s++)
		ToUtf16_([&](char16 c) { *t++ = c; }, *s);
	return i32(t - t0);
}

Vec<char16> ToUtf16(wtukk s, i32 len)
{
	Vec<char16> r;
	r.Reserve(len);
	for(wtukk lim = s + len; s < lim; s++)
		ToUtf16_([&](char16 c) { r.Add(c); }, *s);
	return r;
}

i32 Utf16Len(tukk s, i32 len)
{
	i32 rlen = 0;
	FromUtf8_([&](wchar code) {
		ToUtf16_([&](char16) { rlen++; }, code);
	}, s, len);
	return rlen;
}

i32 ToUtf16(char16 *t, tukk s, i32 len)
{
	char16 *t0 = t;
	FromUtf8_([&](wchar code) {
		ToUtf16_([&](char16 c) { *t++ = c; }, code);
	}, s, len);
	return i32(t - t0);
}

Vec<char16> ToUtf16(tukk s, i32 len)
{
	Vec<char16> r;
	FromUtf8_([&](wchar code) {
		ToUtf16_([&](char16 c) { r.Add(c); } , code);
	}, s, len);
	return r;
}

i32 Utf32Len(tukk s, i32 len)
{
	i32 rlen = 0;
	FromUtf8_([&](wchar) { rlen++; }, s, len);
	return rlen;
}

void ToUtf32(wtuk t, tukk s, i32 len)
{
	FromUtf8_([&](wchar c) { *t++ = c; }, s, len);
}

WTxt ToUtf32(tukk s, i32 len)
{
	WTxtBuf r;
	FromUtf8_([&](wchar c) { r.Cat(c); }, s, len);
	return WTxt(r);
}

i32 Utf32Len(const char16 *s, i32 len)
{
	i32 rlen = 0;
	FromUtf16_([&](wchar) { rlen++; }, s, len);
	return rlen;
}

void ToUtf32(wtuk t, const char16 *s, i32 len)
{
	FromUtf16_([&](wchar c) { *t++ = c; }, s, len);
}

WTxt ToUtf32(const char16 *s, i32 len)
{
	WTxtBuf r;
	r.Reserve(len);
	FromUtf16_([&](wchar c) { r.Cat(c); }, s, len);
	return WTxt(r);
}

Txt Utf8ToAscii(const Txt& src)
{
	TxtBuf r(src.GetLength());
	tukk s = src.begin();
	tukk lim = src.end();

	tuk t = r;
	while(s < lim)
		*t++ = (u8)*s < 128 ? *s++ : ToAscii(FetchUtf8(s, lim));
	r.SetLength(i32(t - ~r));
	return Txt(r);
}

Txt Utf8ToUpperAscii(const Txt& src)
{
	TxtBuf r(src.GetLength());
	tukk s = src.begin();
	tukk lim = src.end();

	tuk t = r;
	while(s < lim) {
		*t++ = (u8)*s <= 'Z' ? *s++ : ToUpperAscii(FetchUtf8(s, lim));
	}
	r.SetLength(i32(t - ~r));
	return Txt(r);
}

Txt Utf8ToLowerAscii(const Txt& src)
{
	TxtBuf r(src.GetLength());
	tukk s = src.begin();
	tukk lim = src.end();

	tuk t = r;
	while(s < lim)
		*t++ = ToLowerAscii(FetchUtf8(s, lim));
	r.SetLength(i32(t - ~r));
	return Txt(r);
}

///////////////////////////
wchar FetchUtf8(tukk &_s, bool nolim, tukk _lim, bool& ok)
{
	u8k *s = (u8k *)_s;
	u8k *lim = (u8k *)_lim;
	dword code = *s;
	if(code < 0x80) {
		_s++;
		return *s;
	}
	else
	if(code >= 0xC2) {
		dword c;
		if(code < 0xE0 && (nolim || s + 1 < lim) &&
		   s[1] >= 0x80 && s[1] < 0xc0 &&
		   (c = ((code - 0xC0) << 6) + s[1] - 0x80) >= 0x80 && c < 0x800) {
			_s += 2;
			return c;
		}
		else
		if(code < 0xF0 && (nolim || s + 2 < lim) &&
		   s[1] >= 0x80 && s[1] < 0xc0 && s[2] >= 0x80 && s[2] < 0xc0 &&
		   (c = ((code - 0xE0) << 12) + ((s[1] - 0x80) << 6) + s[2] - 0x80) >= 0x800 &&
		   !(c >= 0xEE00 && c <= 0xEEFF)) {
			_s += 3;
			return c;
		}
		else
		if(code < 0xF8 && (nolim || s + 3 < lim) &&
		   s[1] >= 0x80 && s[1] < 0xc0 && s[2] >= 0x80 && s[2] < 0xc0 && s[3] >= 0x80 && s[3] < 0xc0 &&
		   (c = ((code - 0xF0) << 18) + ((s[1] - 0x80) << 12) + ((s[2] - 0x80) << 6) + s[3] - 0x80) >= 0x10000 &&
		   c < 0x110000) {
			_s += 4;
			return c;
		}
	}
	_s++;
	ok = false;
	return 0xEE00 + code; // ERROR ESCAPE
}

wchar FetchUtf8(tukk &s, tukk lim, bool& ok)
{
	return FetchUtf8(s, false, lim, ok);
}

wchar FetchUtf8(tukk &s, tukk lim)
{
	bool ok;
	return FetchUtf8(s, false, lim, ok);
}

wchar FetchUtf8(tukk &s, bool& ok)
{
	return FetchUtf8(s, true, NULL, ok);
}

};