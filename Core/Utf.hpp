#ifndef __drxCore_Utf_H__
#define __drxCore_Utf_H__

template <class Target>
force_inline bool ToUtf8_(Target t, wchar codepoint)
{
	if(codepoint < 0x80)
		t((char)codepoint);
	else
	if(codepoint < 0x800) {
		t(0xc0 | u8(codepoint >> 6));
		t(0x80 | u8(codepoint & 0x3f));
	}
	else
	if((codepoint & 0xFFFFFF00) == 0xEE00) // ERROR ESCAPE
		t((char) codepoint);
	else
	if(codepoint < 0x10000) {
		t(0xe0 | u8(codepoint >> 12));
		t(0x80 | u8((codepoint >> 6) & 0x3f));
		t(0x80 | u8(codepoint & 0x3f));
	}
	else
	if(codepoint < 0x110000) {
		t(0xf0 | u8(codepoint >> 18));
		t(0x80 | u8((codepoint >> 12) & 0x3f));
		t(0x80 | u8((codepoint >> 6) & 0x3f));
		t(0x80 | u8(codepoint & 0x3f));
	}
	else
		return false;
	return true;
}

wchar FetchUtf8(tukk &_s, bool nolim, tukk _lim, bool& ok);
wchar FetchUtf8(tukk &s, tukk lim, bool& ok);
wchar FetchUtf8(tukk &s, bool& ok);

template <class Target>
force_inline bool FromUtf8_(Target t, tukk s, size_t len)
{
	bool ok = true;
	tukk lim = s + len;
	while(s < lim)
		t(FetchUtf8(s, lim, ok));
	return ok;
}

template <class Target>
force_inline bool ToUtf16_(Target t, size_t codepoint)
{
	if(codepoint < 0x10000)
		t((char16)codepoint);
	else
	if(codepoint < 0x110000) {
		codepoint -= 0x10000;
		t(char16(0xD800 + (0x3ff & (codepoint >> 10))));
		t(char16(0xDC00 + (0x3ff & codepoint)));
	}
	else
		return false;
	return true;
}

force_inline wchar ReadSurrogatePair(const char16 *s, const char16 *lim)
{
	return (*s & 0XFC00) == 0xD800 && s + 1 < lim && (s[1] & 0xFC00) == 0xDC00 ?
		   ((wchar(s[0] & 0x3ff) << 10) | (s[1] & 0x3ff)) + 0x10000 : 0;
}

template <class Target>
force_inline void FromUtf16_(Target t, const char16 *s, size_t len)
{
	const char16 *lim = s + len;
	while(s < lim) {
		wchar c = ReadSurrogatePair(s, lim);
		if(c) {
			t(c);
			s += 2;
		}
		else {
			t(*s);
			s++;
		}
	}
}

#endif