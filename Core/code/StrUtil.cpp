#include "../Core.h"

namespace drx {

i32 strlen16(const char16 *s)
{
	if(!s) return 0;
	const char16 *s0 = s;
	while(*s) s++;
	return i32(s - s0);
}

i32 strlen32(wtukk s)
{
	if(!s) return 0;
	wtukk s0 = s;
	while(*s) s++;
	return i32(s - s0);
}

u32 ctoi(wchar c)
{
	if(c >= '0' && c <= '9')
		return c - '0';
	if(c >= 'A' && c <= 'Z')
		return c - 'A' + 10;
	if(c >= 'a' && c <= 'z')
		return c - 'a' + 10;
	return (u32)-1;
}

wchar CharFilterAscii(wchar c)
{
	return c >= 32 && c < 256 ? c : 0;
}

wchar CharFilterAscii128(wchar c)
{
	return c >= 32 && c < 128 ? c : 0;
}

wchar CharFilterUnicode(wchar c)
{
	return c >= 32 && c < 0x10FFFF ? c : 0;
}

wchar CharFilterDigit(wchar c)
{
	return IsDigit(c) ? c : 0;
}

wchar CharFilterInt(wchar c)
{
	if(c == '+' || c == '-') return c;
	return CharFilterDigit(c);
}

wchar CharFilterDouble(wchar c)
{
	if(c == ',' || c == '.') return '.';
	if(c == 'e' || c == 'E') return 'E';
	return CharFilterInt(c);
}

wchar CharFilterWhitespace(wchar c)
{
	return IsSpace(c) ? c : 0;
}

wchar CharFilterNotWhitespace(wchar c)
{
	return IsSpace(c) ? 0 : c;
}

wchar CharFilterAlpha(wchar c)
{
	return IsAlpha(c) ? c : 0;
}

wchar CharFilterToUpper(wchar c)
{
	return ToUpper(c);
}

wchar CharFilterToLower(wchar c)
{
	return ToLower(c);
}

wchar CharFilterToUpperAscii(wchar c)
{
	return ToUpperAscii(c);
}

wchar CharFilterAlphaToUpper(wchar c)
{
	return IsAlpha(c) ? IsUpper(c) ? c : ToUpper(c) : 0;
}

wchar CharFilterAlphaToLower(wchar c)
{
	return IsAlpha(c) ? IsLower(c) ? c : ToLower(c) : 0;
}

wchar CharFilterDefaultToUpperAscii(wchar c)
{
	return ToUpper(ToAscii(c, CHARSET_DEFAULT));
}

wchar CharFilterCrLf(wchar c)
{
	return c == '\r' || c == '\n' ? c : 0;
}

wchar CharFilterNoCrLf(wchar c)
{
	return c != '\r' && c != '\n' ? c : 0;
}

Txt Filter(tukk s, wchar (*filter)(wchar))
{
	Txt result;
	while(*s) {
		char c = (*filter)((u8)*s++);
		if(c) result.Cat(c);
	}
	return result;
}

Txt FilterWhile(tukk s, wchar (*filter)(wchar))
{
	Txt result;
	while(*s) {
		char c = (*filter)((u8)*s++);
		if(!c) break;
		result.Cat(c);
	}
	return result;
}

WTxt Filter(wtukk s, wchar (*filter)(wchar))
{
	WTxt result;
	while(*s) {
		wchar c = (*filter)(*s++);
		if(c) result.Cat(c);
	}
	return result;
}

WTxt FilterWhile(wtukk s, wchar (*filter)(wchar))
{
	WTxt result;
	while(*s) {
		wchar c = (*filter)(*s++);
		if(!c) break;
		result.Cat(c);
	}
	return result;
}

}