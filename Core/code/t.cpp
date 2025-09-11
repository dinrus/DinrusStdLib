#include "../Core.h"

namespace drx {

#define TFILE <drx/Core/Core.t>
#include <drx/Core/t.h>

static StaticMutex slng;

static i32 sIdLen(tukk txt)
{
	tukk s;
	for(s = txt; *s; s++)
		if(*s == '\a' && s[1] != '\a')
			break;
	return (i32)(s - txt);
}

static bool sIdEq(tukk a, tukk b)
{
	for(;;) {
		if((*a == '\0' || *a == '\a' && a[1] != '\a') &&
		   (*b == '\0' || *b == '\a' && b[1] != '\a'))
			return true;
		if(*a != *b)
			return false;
		a++;
		b++;
	}
}

struct CharS : Moveable<CharS> {
	tukk s;

	hash_t GetHashVal() const             { return memhash(s, sIdLen(s)); }
	bool operator==(const CharS& b) const   { return sIdEq(s, b.s); }
};

struct LngRec : Moveable<LngRec> {
	i32         lang;
	tukk text;
};

struct LngModule {
	Txt                            name;
	VecMap<CharS, Vec<LngRec> > map;
};

static Array<LngModule>& sMod()
{
	static Array<LngModule> m;
	return m;
}

tukk GetENUS(tukk txt)
{
	for(tukk s = txt; *s; s++) {
		if(*s == '\v') {
			if(s[1] == '\v')
				s++;
			else
				return s + 1;
		}
		if(*s == '\a') {
			if(s[1] == '\a')
				s++;
			else
				return s + 1;
		}
	}
	return txt;
}

tukk GetENUSc(tukk txt)
{
	txt = GetENUS(txt);
	Txt r;
	bool cv = false;
	for(tukk s = txt; *s; s++)
		if(s[0] == '\v' && s[1] == '\v') {
			r.Cat('\v');
			s++;
			cv = true;
		}
		else
		if(s[0] == '\a' && s[1] == '\a') {
			r.Cat('\a');
			s++;
			cv = true;
		}
		else
			r.Cat(*s);
	return cv ? PermanentCopy(r) : txt;
}

void AddModule(const LngEntry__* l, tukk name)
{
	Mutex::Lock __(slng);
	Array<LngModule>& ma = sMod();
	LngModule& m = ma.Add();
	m.name = name;
	Vec<LngRec> *lr = NULL;
	while(l->lang) {
		if(l->lang == 1) {
			CharS ids;
			ids.s = l->text;
			lr = &m.map.GetAdd(ids);
			LngRec& r = lr->Add();
			r.lang = LNG_('R','U','R','U');
			r.text = GetENUSc(l->text);
		}
		else
		if(l->text && *l->text && lr) {
			LngRec& r = lr->Add();
			r.lang = l->lang;
			r.text = l->text;
		}
		l++;
	}
}

// ----------------------------------------------------

static const LngRec *sFindLngRec(tukk id, i32 lang, dword mask)
{
	CharS ids;
	ids.s = id;
	lang &= mask;
	Array<LngModule>& ma = sMod();
	for(i32 i = 0; i < ma.GetCount(); i++) {
		LngModule& m = ma[i];
		i32 q = m.map.Find(ids);
		if(q >= 0) {
			const Vec<LngRec>& r = m.map[q];
			for(i32 i = 0; i < r.GetCount(); i++)
				if(i32(r[i].lang & mask) == lang)
					return &r[i];
		}
	}
	return NULL;
}

static const LngRec *sFindLngRec(tukk id, i32 lang)
{
	const LngRec *r;
	r = sFindLngRec(id, lang, LNGC_(0xff, 0xff, 0xff, 0xff, 0));
	if(r)
		return r;
	r = sFindLngRec(id, lang, LNGC_(0xff, 0xff, 0x00, 0x00, 0));
	return r;
}

struct ZoneAlloc {
	Vec<tuk > zsmall, zbig;
	tuk ptr, lim;

	tuk Alloc(i32 sz);
	void  Clear();

	ZoneAlloc()  { ptr = lim = NULL; }
	~ZoneAlloc() { Clear(); }
};

void ZoneAlloc::Clear()
{
	for(i32 i = 0; i < zsmall.GetCount(); i++)
		delete[] zsmall[i];
	for(i32 i = 0; i < zbig.GetCount(); i++)
		delete[] zbig[i];
	zsmall.Clear();
	zbig.Clear();
	ptr = lim = NULL;
}

tuk ZoneAlloc::Alloc(i32 sz)
{
	if(sz >= 1024)
		return zbig.Add() = new char[sz];
	if(ptr + sz >= lim) {
		zsmall.Add() = ptr = new char[4096];
		lim = ptr + 4096;
	}
	tuk s = ptr;
	ptr += sz;
	return s;
}

template<>
inline hash_t GetHashVal(tukk  const &s)
{
	return GetPtrHashVal(s);
}

i32 main_current_lang;

static Index<i32>& sLangIndex()
{
	static Index<i32> m;
	return m;
}

static Array< VecMap<tukk , tukk > >& sLangMap()
{
	static Array< VecMap<tukk , tukk > > m;
	return m;
}

VecMap<tukk , tukk > *sMainCurrentLangMapPtr;
thread_local VecMap<tukk , tukk > *sCurrentLangMapPtr;

VecMap<tukk , tukk >& sCurrentLangMap()
{
	if(sCurrentLangMapPtr)
		return *sCurrentLangMapPtr;
	if(sMainCurrentLangMapPtr)
		return *sMainCurrentLangMapPtr;
	return sLangMap().At(0);
}

static Array< VecMap<Txt, Txt> >& sSLangMap()
{
	static Array< VecMap<Txt, Txt> > m;
	return m;
}

VecMap<Txt, Txt>          *sMainCurrentSLangMapPtr;
thread_local VecMap<Txt, Txt> *sCurrentSLangMapPtr;

VecMap<Txt, Txt>& sCurrentSLangMap()
{
	if(sCurrentLangMapPtr)
		return *sCurrentSLangMapPtr;
	if(sMainCurrentSLangMapPtr)
		return *sMainCurrentSLangMapPtr;
	return sSLangMap().At(0);
}

thread_local i32 thread_current_lang;

i32 GetCurrentLanguage()
{
	return thread_current_lang ? thread_current_lang : main_current_lang;
}

void SetCurrentLanguage(i32 lang)
{
	{
		Mutex::Lock __(slng);
		thread_current_lang = lang;
		i32 ii = sLangIndex().FindAdd(lang);
		sCurrentLangMapPtr = &sLangMap().At(ii);
		sCurrentSLangMapPtr = &sSLangMap().At(ii);
		if(Thread::IsMain())
		{
			main_current_lang = lang;
			sMainCurrentSLangMapPtr = sCurrentSLangMapPtr;
			sMainCurrentLangMapPtr = sCurrentLangMapPtr;
		}
		static i32 n = 1;
		if(ii > n) { // protected against too many language/charset switches
			n = 2 * n;
			Single<ZoneAlloc>().Clear();
			for(i32 i = 0; i < sLangIndex().GetCount(); i++) {
				sSLangMap()[i].Clear();
				sLangMap()[i].Clear();
			}
		}
		SyncLngInfo__();
	}
	SetDateFormat(t_("date-format\a%2:02d/%3:02d/%1:4d"));
	SetDateScan(t_("date-scan\amdy"));
	SetDateFilter(t_("date-filter\aA/\a .-"));
}

tukk t_GetLngTxt_(tukk id)
{
	VecMap<tukk , tukk >& map = sCurrentLangMap();
	i32 q = map.Find(id);
	if(q >= 0)
		return map[q];
	const LngRec *r = sFindLngRec(id, GetCurrentLanguage());
	if(r) {
		i32 dch = GetLNGCharset(GetCurrentLanguage());
		if(dch == CHARSET_UTF8) {
			map.Add(id, r->text);
			return r->text;
		}
		Txt text = ToCharset(dch, r->text, CHARSET_UTF8);
		tuk q = Single<ZoneAlloc>().Alloc(text.GetLength() + 1);
		strcpy(q, ~text);
		map.Add(id, q);
		return q;
	}

	tukk txt = GetENUSc(id);
	map.Add(id, txt);
	return txt;
}

tukk t_GetLngTxt(tukk id)
{
	Mutex::Lock __(slng);
	return t_GetLngTxt_(id);
}

Txt GetLngTxt_(i32 lang, tukk id)
{
	if(!lang)
		lang = GetCurrentLanguage();
	const LngRec *r = sFindLngRec(id, lang);
	if(r) {
		i32 dch = GetLNGCharset(lang);
		if(dch == CHARSET_UTF8)
			return r->text;
		else
			return ToCharset(dch, r->text, CHARSET_UTF8);
	}
	return GetENUSc(id);
}

Txt GetLngTxt(i32 lang, tukk id)
{
	Mutex::Lock __(slng);
	return GetLngTxt_(lang, id);
}

Txt GetLngTxt(tukk id)
{
	Mutex::Lock __(slng);
	VecMap<Txt, Txt>& map = sCurrentSLangMap();
	i32 q = map.Find(id);
	if(q >= 0)
		return map[q];
	Txt s = GetLngTxt_(GetCurrentLanguage(), id);
	map.Add(id, s);
	return s;
}

Index<i32> GetLngSet()
{
	Mutex::Lock __(slng);
	Index<i32> ndx;
	Array<LngModule>& ma = sMod();
	for(i32 i = 0; i < ma.GetCount(); i++) {
		LngModule& m = ma[i];
		for(i32 j = 0; j < m.map.GetCount(); j++) {
			Vec<LngRec>& lr = m.map[j];
			for(i32 k = 0; k < lr.GetCount(); k++)
				ndx.FindAdd(lr[k].lang);
		}
	}
	return ndx;
}

Index<i32> GetLngSet(const Txt& module)
{
	Mutex::Lock __(slng);
	Index<i32> ndx;
	Array<LngModule>& ma = sMod();
	for(i32 i = 0; i < ma.GetCount(); i++) {
		LngModule& m = ma[i];
		if (m.name != module)
			continue;

		for (i32 j = 0; j < m.map.GetCount(); ++j) {
			Vec<LngRec>& lr = m.map[j];
			for(i32 k = 0; k < lr.GetCount(); k++)
				ndx.FindAdd(lr[k].lang);
		}
	}

	return ndx;
}

void    SaveLngFile(FileOut& out, i32 lang, i32 lang2)
{
	Mutex::Lock __(slng);
	out << "LANGUAGE " << AsCTxt(LNGAsText(lang)) << ";\r\n";
	Array<LngModule>& ma = sMod();
	for(i32 i = 0; i < ma.GetCount(); i++) {
		LngModule& m = ma[i];
		out << "// " << m.name << "\r\n";
		for(i32 j = 0; j < m.map.GetCount(); j++) {
			Txt id = m.map.GetKey(j).s;
			if (lang2)
				out << "// " << AsCTxt(GetLngTxt_(lang2, id), 60, "\t", ASCTXT_SMART) << "\r\n";
			out << AsCTxt(id, 70) << ",\r\n"
			    << "\t" << AsCTxt(GetLngTxt_(lang, id), 60, "\t", ASCTXT_SMART)
			    << ";\r\n\r\n";
		}
	}
}

void LngSetAdd(tukk id, i32 lang, tukk txt, bool addid)
{
	Mutex::Lock __(slng);
	CharS ids;
	ids.s = PermanentCopy(id);
	Txt text = ToCharset(CHARSET_UTF8, txt, GetLNGCharset(lang));
	lang = SetLNGCharset(lang, CHARSET_UTF8);
	Array<LngModule>& ma = sMod();
	for(i32 i = 0; i < ma.GetCount(); i++) {
		LngModule& m = ma[i];
		i32 q = m.map.Find(ids);
		if(q >= 0) {
			tuk t = PermanentCopy(text);
			Vec<LngRec>& r = m.map[q];
			for(i32 i = 0; i < r.GetCount(); i++)
				if(r[i].lang == lang) {
					r[i].text = t;
					return;
				}
			LngRec& rec = r.Add();
			rec.lang = lang;
			rec.text = t;
			return;
		}
	}
	if(addid) {
		if(ma.GetCount() == 0)
			ma.Add();
		LngRec& r = ma.Top().map.Add(ids).Add();
		tuk t = PermanentCopy(text);
		strcpy(t, text);
		r.lang = lang;
		r.text = t;
	}
}

bool   LoadLngFile(tukk file)
{
	Txt data = LoadFile(file);
	CParser p(data);
	try {
		while(!p.IsEof()) {
			p.PassId("LANGUAGE");
			i32 lang = LNGFromText(p.ReadTxt());
			if(!lang)
				return false;
			p.PassChar(';');
			while(p.IsTxt()) {
				Txt id = p.ReadTxt();
				p.PassChar(',');
				LngSetAdd(id, lang, p.ReadTxt(), false);
				p.PassChar(';');
			}
		}
	}
	catch(CParser::Error) {
		return false;
	}
	return true;
}

}