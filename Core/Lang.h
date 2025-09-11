#define LNG_(a, b, c, d)   ( (((a - 'A' + 1) & 31) << 15) | (((b - 'A' + 1) & 31) << 10) | \
                             (((c - 'A' + 1) & 31) << 5) | (((d - 'A' + 1) & 31) << 0) )

#define LNGC_(a, b, c, d, cs) ( (((a - 'A' + 1) & 31) << 15) | (((b - 'A' + 1) & 31) << 10) | \
                                (((c - 'A' + 1) & 31) << 5) | (((d - 'A' + 1) & 31) << 0) | \
                                ((cs & 255) << 20) )

i32     LNGFromText(tukk s);
Txt     LNGAsText(i32 d);

u8    GetLNGCharset(i32 d);
i32     SetLNGCharset(i32 lng, u8 chrset);

#ifdef DEPRECATED
#define LNG_CZECH         0xF1CC7A // corresponds to CS-CZ windows-1250
#define LNG_ENGLISH       0x2BAB3  // LNG_('E', 'N', 'U', 'S')
#define LNG_RUSSIAN		  LNG_('R','U','R','U')
#endif

#define LNG_CURRENT       0

void                      SetLanguage(i32 lang);
i32                       GetCurrentLanguage();

void                      SetLanguage(tukk s);
Txt                       GetCurrentLanguageTxt();

i32                       GetSystemLNG();

VecMap<Txt, Txt> GetLanguage(i32 lang);//rename...
i32k                *GetAllLanguages();

Txt                       GetLangName(i32 language);
Txt                       GetNativeLangName(i32 language);

Txt                       txtGet(tukk id, i32 language = LNG_CURRENT);

void                      ExportLNGtoT();

#ifdef TARGET_WIN32
LCID                      GetLanguageLCID(i32 language);
Txt                       GetLocaleInfoA(LCID lcid, LCTYPE lctype);
WTxt                      GetLocaleInfoW(LCID lcid, LCTYPE lctype);
#endif

class LangInfo {
	void Set(i32 language);

	friend const LangInfo& GetLangInfo(i32 lang);

public:
	i32          language;
	Txt          english_name;
	Txt          native_name;

	Txt          thousand_separator, decimal_point;
	Txt          date_format, time_format; // (?)

	Txt          month[12], smonth[12], day[7], sday[7];

	i32     (*compare)(wtukk a, i32 alen, wtukk b, i32 blen, i32 lang);
	WTxt    (*getindexletter)(wtukk text, i32 lang);
  
	Txt             FormatInt(i32 value) const;
	Txt             FormatDouble(double value, i32 digits, i32 FD_flags = 0, i32 fill_exp = 0) const;
	Txt             FormatDate(Date date) const;
	Txt             FormatTime(Time time) const;

	WTxt            GetIndexLetter(wtukk text) const                            { return (*getindexletter)(text, language); }
	i32             Compare(wtukk a, i32 alen, wtukk b, i32 blen) const  { return (*compare)(a, alen, b, blen, language); }

	i32             Compare(wtukk a, wtukk b) const    { return Compare(a, strlen__(a), b, strlen__(b)); }
	i32             Compare(WTxt a, WTxt b) const              { return Compare(a, a.GetLength(), b, b.GetLength()); }
	i32             Compare(tukk a, tukk b) const      { return Compare(ToUtf32(a), ToUtf32(b)); }
	i32             Compare(Txt a, Txt b) const                { return Compare(a.ToWTxt(), b.ToWTxt()); }

	bool            operator()(wtukk a, wtukk b) const { return Compare(a, b) < 0; }
	bool            operator()(WTxt a, WTxt b) const           { return Compare(a, b) < 0; }
	bool            operator()(tukk a, tukk b) const   { return Compare(a, b) < 0; }
	bool            operator()(Txt a, Txt b) const             { return Compare(a, b) < 0; }

	Txt             ToTxt() const;

	LangInfo();

//BWC
	WTxt         month_names[12], short_month_names[12];
	WTxt         day_names[7], short_day_names[7];
	i32             Compare(wtukk a, wtukk b, i32 alen, i32 blen) const { return Compare(a, alen, b, blen); }
};

const LangInfo& GetLangInfo(i32 lang);
const LangInfo& GetLangInfo();

void  SetLangInfo(i32 lang, const LangInfo& lf);

// ------ Language internals ----------------

#include "Lang_s.h"

struct LangModuleRecord {
	tukk id;
	tukk *ptr;
};

struct LangTextRecord {
	tukk *ptr;
	tukk text;
};

void AddLangModule(tukk file, tukk modulename, i32 masterlang, const LangModuleRecord *module);
void AddLanguage(tukk modulename, i32 lang, const LangTextRecord *langtext);

void SyncLngInfo__();