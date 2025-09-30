#include "../Core.h"

#ifdef TARGET_WIN32
#include <wingdi.h>
#include <winnls.h>
#endif
#ifdef TARGET_POSIX
#include <locale.h>
	#ifndef TARGET_ANDROID
	#include <langinfo.h>
	#endif
#endif

namespace drx {

#define LLOG(x)  LOG(x)

Txt LNGAsText(i32 d)
{
	Txt result;
	i32 c = (d >> 15) & 31;
	if(c) {
		result.Cat(c + 'A' - 1);
		c = (d >> 10) & 31;
		if(c) {
			result.Cat(c + 'A' - 1);
			c = (d >> 5) & 31;
			if(c) {
				result.Cat('-');
				result.Cat(c + 'A' - 1);
				c = d & 31;
				if(c) result.Cat(c + 'A' - 1);
			}
		}
	}
	c = (d >> 20) & 255;
	if(c)
		result << ' ' << CharsetName(c);
	return result;
}

u8 GetLNGCharset(i32 d)
{
	u8 cs = u8(d >> 20);
	return cs ? cs : CHARSET_UTF8;
}

i32  SetLNGCharset(i32 lng, u8 chrset)
{
	return (lng & ~(0xffffffff << 20)) | (chrset << 20);
}

i32 LNGFromText(tukk s)
{
	i32 l = 0;
	if(IsAlpha(*s)) {
		l = (ToUpper(*s++) - 'A' + 1) << 15;
		if(IsAlpha(*s)) {
			l |= (ToUpper(*s++) - 'A' + 1) << 10;
			if(*s && !IsAlpha(*s))
				s++;
			if(IsAlpha(*s)) {
				l |= (ToUpper(*s++) - 'A' + 1) << 5;
				if(IsAlpha(*s)) {
					l |= (ToUpper(*s++) - 'A' + 1);
					while(*s && *s != ' ')
						s++;
					if(*s == ' ') {
						s++;
						i32 cs = CharsetByName(s);
						if(cs > 0)
							l |= (cs << 20);
						else
							return 0;
					}
					return l;
				}
			}
		}
	}
	return 0;
}

#ifdef TARGET_WIN32

Txt GetUserLocale(dword type)
{
#ifdef TARGET_WINCE
	wchar h[256];
	i32 n = ::GetLocaleInfo(GetUserDefaultLCID(), type, h, 256);
	return n ? WTxt(h, n - 1).ToTxt() : Txt();
#else
	char h[256];
	i32 n =:: GetLocaleInfo(GetUserDefaultLCID(), type, h, 256);
	return n ? Txt(h, n - 1) : Txt();
#endif
}

i32 GetSystemLNG()
{
	static i32 lang;
	ONCELOCK {
		lang = LNGFromText(GetUserLocale(LOCALE_SISO639LANGNAME) + GetUserLocale(LOCALE_SISO3166CTRYNAME));
		if(!lang)
			lang = LNG_('E', 'N', 'U', 'S');
		i32 cs = atoi(GetUserLocale(LOCALE_IDEFAULTANSICODEPAGE));
		if(cs >= 1250 && cs <= 1258)
			lang = SetLNGCharset(lang, CHARSET_WIN1250 + cs - 1250);
	}
	return lang;
}
#endif

#ifdef TARGET_POSIX
i32 GetSystemLNG() {
	static i32 lang;
	ONCELOCK {
		Txt s = Environment().Get("LANG", Null);
		lang = LNGFromText(s);
		if(!lang)
			lang = LNG_ENGLISH;
		tukk q = strchr(s, '.');
		if(q)
			lang = SetLNGCharset(lang, CharsetByName(q + 1));
	};
	return lang;
};

#endif

class LangConvertClass : public Convert {
	virtual Val  Format(const Val& q) const {
		return LNGAsText((i32)q);
	}

	virtual Val  Scan(const Val& text) const {
		if(IsNull(text)) return 0;
		i32 q = LNGFromText((Txt)text);
		if(!q) return ErrorVal(t_("Неверная спецификация языка."));
		return (i32) q;
	}

	virtual wchar Filter(wchar chr) const {
		return chr == ' ' || chr == '-' || IsDigit(chr) ? chr : IsAlpha(chr) ? ToUpper(chr) : 0;
	}
};

Convert& LNGConvert()
{
	return Single<LangConvertClass>();
}

void SetLanguage(i32 lang) {
	if(lang != LNG_CURRENT)
		SetDefaultCharset(GetLNGCharset(lang));
	SetCurrentLanguage(lang);
}

INITBLOCK {
	SetLanguage(LNG_('R', 'U', 'R', 'U'));
}

void SetLanguage(tukk s)
{
	SetLanguage(LNGFromText(s));
}

Txt GetCurrentLanguageTxt()
{
	return LNGAsText(GetCurrentLanguage());
}

Txt GetLangName(i32 language)
{
	return GetLangInfo(language).english_name;
}

Txt GetNativeLangName(i32 language) {
	return GetLangInfo(language).native_name.ToTxt();
}

}