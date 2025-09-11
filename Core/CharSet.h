#define CHRSET_(id, s) CHARSET_##id,

enum {
	CHARSET_DEFAULT,
#include "CharSet.i"
};

#undef CHRSET_

#define  CHARSET_TOASCII      253
#define  CHARSET_UTF32        254 // auxilary
#define  CHARSET_UTF8         255
#define  CHARSET_UNICODE      255 // deprecated

enum {
	DEFAULTCHAR = 0x1f
};

#define CHRSET_(id, s) extern word CHRTAB_##id[128];

#include "CharSet.i"

#undef CHRSET_

#include "Utf.hpp"

#define case_id_rus \
	case L'а':case L'б':case L'в':case L'г':case L'д':case L'е':case L'ё':case L'ж':case L'з': \
	case L'и':case L'й':case L'к':case L'л':case L'м':case L'н':case L'о':case L'п':case L'р': \
	case L'с':case L'т':case L'у':case L'ф':case L'х':case L'ц':case L'ч':case L'ш':case L'щ': \
	case L'ъ':case L'ы':case L'ь':case L'э':case L'ю':case L'я': \
	case L'А':case L'Б':case L'В':case L'Г':case L'Д':case L'Е':case L'Ё':case L'Ж':case L'З': \
	case L'И':case L'Й':case L'К':case L'Л':case L'М':case L'Н':case L'О':case L'П':case L'Р': \
	case L'С':case L'Т':case L'У':case L'Ф':case L'Х':case L'Ц':case L'Ч':case L'Ш':case L'Щ': \
	case L'Ъ':case L'Ы':case L'Ь':case L'Э':case L'Ю':case L'Я'

 bool IsRus(wchar c);
 bool IsUtf8Lead(wchar c);

wchar ReadSurrogatePair(const char16 *s, const char16 *lim);

wchar FetchUtf8(tukk &s, tukk lim, bool& ok);
wchar FetchUtf8(tukk &s, tukk lim);
wchar FetchUtf8(tukk &s, bool& ok);
wchar FetchUtf8(tukk &s);

bool   CheckUtf8(tukk s, i32 len);
bool   CheckUtf8(tukk s);

bool   CheckUtf8(tukk s, i32 len);
bool   CheckUtf8(const Txt& s);

i32    Utf8Len(wtukk s, i32 len);
i32    Utf8Len(wtukk s);
i32    Utf8Len(wchar code);
i32    Utf8Len(const WTxt& s);

i32    Utf8Len(const char16 *s, i32 len);
i32    Utf8Len(const char16 *s) ;
i32    Utf8Len(const Vec<char16>& s) ;

void   ToUtf8(tuk t, wtukk s, i32 len);
Txt    ToUtf8(wtukk s, i32 len);
Txt    ToUtf8(wtukk s);
Txt    ToUtf8(wchar code);
Txt    ToUtf8(const WTxt& s) ;

void   ToUtf8(tuk t, const char16 *s, i32 len);
Txt    ToUtf8(const char16 *s, i32 len);
Txt    ToUtf8(const char16 *s) ;
Txt    ToUtf8(const Vec<char16>& s);

i32 Utf16Len(wtukk s, i32 len);
i32 Utf16Len(wtukk s);
i32 Utf16Len(const WTxt& s);
i32 Utf16Len(wchar code) ;

i32 Utf16Len(tukk s, i32 len);
i32 Utf16Len(tukk s);
i32 Utf16Len(const Txt& s);

i32 ToUtf16(char16 *t, wtukk s, i32 len);
Vec<char16> ToUtf16(wtukk s, i32 len);
Vec<char16> ToUtf16(wtukk s);
Vec<char16> ToUtf16(const WTxt& s);
Vec<char16> ToUtf16(wchar code);

i32 ToUtf16(char16 *t, tukk s, i32 len);
Vec<char16> ToUtf16(tukk s, i32 len);
Vec<char16> ToUtf16(tukk s);
Vec<char16> ToUtf16(const Txt& s);

i32     Utf32Len(const char16 *s, i32 len);
i32     Utf32Len(const char16 *s);
i32     Utf32Len(const Vec<char16>& s);

i32     Utf32Len(tukk s, i32 len);
i32     Utf32Len(tukk s);
i32     Utf32Len(const Txt& s);

void        ToUtf32(wtuk t, const char16 *s, i32 len);
WTxt        ToUtf32(const char16 *s, i32 len);
WTxt        ToUtf32(const char16 *s);
WTxt        ToUtf32(const Vec<char16>& s);

void        ToUtf32(wtuk t, tukk s, i32 len);
WTxt        ToUtf32(tukk s, i32 len);
WTxt        ToUtf32(tukk s);
WTxt        ToUtf32(const Txt& s);

enum { MAX_DECOMPOSED = 18 };

i32           UnicodeDecompose(wchar codepoint, wchar t[MAX_DECOMPOSED], bool only_canonical = false);
WTxt          UnicodeDecompose(wchar codepoint, bool only_canonical = false);
wchar         UnicodeCompose(const WTxt& t);

void  SetDefaultCharset(u8 charset);
u8    GetDefaultCharset();

u8  ResolveCharset(u8 charset);

u8  AddCharSet(tukk name, const word *table);
u8  AddCharSetE(tukk name, word *table);

tukk CharsetName(u8 charset);
i32  CharsetCount();
i32  CharsetByName(tukk name);

void ConvertCharset(tuk t, u8 tcharset, tukk s, u8 scharset, i32 n);

Txt  ToCharset(u8 charset, const Txt& s, u8 scharset = CHARSET_DEFAULT, i32 defchar = DEFAULTCHAR);

extern word unicode_fast_upper__[2048];
extern word unicode_fast_lower__[2048];
extern u8 unicode_fast_ascii__[2048];
extern u8 unicode_fast_info__[2048];
extern u8 unicode_fast_upper_ascii__[];
extern u8 unicode_fast_lower_ascii__[];

wchar ToUpperRest_(wchar c);
wchar ToLowerRest_(wchar c);
char  ToAsciiRest_(wchar c);
bool  IsRTL_(wchar c);
bool  IsMark_(wchar c);
bool  IsLetter_(wchar c);
bool  IsUpper_(wchar c);
bool  IsLower_(wchar c);

wchar ToUpper(wchar c);
wchar ToLower(wchar c);
char  ToAscii(wchar c);
char  ToUpperAscii(wchar c);
char  ToLowerAscii(wchar c);
bool  IsLower(wchar c);
bool  IsUpper(wchar c);
bool  IsLetter(wchar c);

bool IsRTL(wchar c);
bool IsMark(wchar c);

bool  IsLetter(char c);
bool  IsUpper(char c);
bool  IsLower(char c);
wchar ToUpper(char c) ;
wchar ToLower(char c) ;
char  ToAscii(char c)  ;
char  ToUpperAscii(char c) ;
char  ToLowerAscii(char c)  ;

bool  IsLetter(i8 c);
bool  IsUpper(i8 c);
bool  IsLower(i8 c);
wchar ToUpper(i8 c);
wchar ToLower(i8 c) ;
char  ToAscii(i8 c);
char  ToUpperAscii(i8 c);
char  ToLowerAscii(i8 c) ;

bool  IsLetter(char16 c);
bool  IsUpper(char16 c) ;
bool  IsLower(char16 c) ;
wchar ToUpper(char16 c);
wchar ToLower(char16 c);
char  ToAscii(char16 c) ;
char  ToUpperAscii(char16 c) ;
char  ToLowerAscii(char16 c) ;

bool IsDigit(wchar c);
bool IsAlpha(wchar c);
bool IsAlNum(wchar c);
bool IsLeNum(wchar c) ;
bool IsPunct(wchar c) ;
bool IsSpace(wchar c);
bool IsXDigit(wchar c);
bool IsDoubleWidth(wchar c);

Txt Utf8ToAscii(const Txt& src);
Txt Utf8ToUpperAscii(const Txt& src);
Txt Utf8ToLowerAscii(const Txt& src);

void ToUpper(char16 *t, const char16 *s, i32 len);
void ToLower(char16 *t, const char16 *s, i32 len);
void ToAscii(char16 *t, const char16 *s, i32 len);

void ToUpper(char16 *s, i32 len);
void ToLower(char16 *s, i32 len);
void ToAscii(char16 *s, i32 len);

bool IsLetter(wchar c, u8 charset);
bool IsUpper(wchar c, u8 charset);
bool IsLower(wchar c, u8 charset);
i32  ToUpper(wchar c, u8 charset);
i32  ToLower(wchar c, u8 charset);
i32  ToAscii(wchar c, u8 charset);

void ToUpper(tuk t, tukk s, i32 len, u8 charset = CHARSET_DEFAULT);
void ToLower(tuk t, tukk s, i32 len, u8 charset = CHARSET_DEFAULT);
void ToAscii(tuk t, tukk s, i32 len, u8 charset = CHARSET_DEFAULT);

void ToUpper(tuk s, i32 len, u8 charset = CHARSET_DEFAULT);
void ToLower(tuk s, i32 len, u8 charset = CHARSET_DEFAULT);
void ToAscii(tuk s, i32 len, u8 charset = CHARSET_DEFAULT);

WTxt InitCaps(wtukk s);
WTxt InitCaps(const WTxt& s);
WTxt ToUpper(const WTxt& w);
WTxt ToLower(const WTxt& w);
WTxt ToAscii(const WTxt& w);

Txt  InitCaps(tukk s, u8 charset = CHARSET_DEFAULT);
Txt  ToUpper(const Txt& s, u8 charset = CHARSET_DEFAULT);
Txt  ToLower(const Txt& s, u8 charset = CHARSET_DEFAULT);
Txt  ToAscii(const Txt& s, u8 charset = CHARSET_DEFAULT);
Txt  ToUpperAscii(const Txt& s, u8 charset = CHARSET_DEFAULT);
Txt  ToLowerAscii(const Txt& s, u8 charset = CHARSET_DEFAULT);

Txt  ToUpper(tukk s, u8 charset = CHARSET_DEFAULT);
Txt  ToLower(tukk s, u8 charset = CHARSET_DEFAULT);
Txt  ToAscii(tukk s, u8 charset = CHARSET_DEFAULT);

WTxt LoadStreamBOMW(Stream& in, u8 def_charset);
WTxt LoadStreamBOMW(Stream& in);
Txt  LoadStreamBOM(Stream& in, u8 def_charset);
Txt  LoadStreamBOM(Stream& in);

WTxt LoadFileBOMW(tukk path, u8 def_charset);
WTxt LoadFileBOMW(tukk path);
Txt  LoadFileBOM(tukk path, u8 def_charset);
Txt  LoadFileBOM(tukk path);

bool    SaveStreamBOM(Stream& out, const WTxt& data);
bool    SaveFileBOM(tukk path, const WTxt& data);
bool    SaveStreamBOMUtf8(Stream& out, const Txt& data);
bool    SaveFileBOMUtf8(tukk path, const Txt& data);

bool    Utf8BOM(Stream& in);

WTxt ToUnicode(const Txt& src, u8 charset);
WTxt ToUnicode(tukk src, i32 n, u8 charset);
Txt  FromUnicodeBuffer(wtukk src, i32 len, u8 charset = CHARSET_DEFAULT, i32 defchar = DEFAULTCHAR);
Txt  FromUnicodeBuffer(wtukk src);
Txt  FromUnicode(const WTxt& src, u8 charset = CHARSET_DEFAULT, i32 defchar = DEFAULTCHAR);

wchar  ToUnicode(wchar chr, u8 charset);
wchar  FromUnicode(wchar wchr, u8 charset, i32 defchar = DEFAULTCHAR);