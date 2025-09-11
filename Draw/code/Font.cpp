#include <drx/Draw/Draw.h>

#define LLOG(x)  // DLOG(x)

namespace drx {

StaticMutex sFontLock;

bool Replace(Font fnt, i32 chr, Font& rfnt);

void Std(Font& font)
{
	font.RealizeStd();
}

Size Font::StdFontSize;
Font Font::AStdFont;

INITBLOCK {
	Val::Register<Font>("Font");
}

static bool sListValid;

void InvalidateFontList()
{
	sListValid = false;
}

Vec<FaceInfo>& Font::FaceList()
{
	static Vec<FaceInfo> list;
	ONCELOCK {
		list = GetAllFacesSys();
	}
	return list;
}

void sInitFonts()
{
	Mutex::Lock __(sFontLock);
	Font::FaceList();
	GetStdFont();
}

INITBLOCK {
	sInitFonts();
}

i32 Font::GetFaceCount()
{
	Mutex::Lock __(sFontLock);
	return FaceList().GetCount();
}

Txt Font::GetFaceName(i32 index)
{
	Mutex::Lock __(sFontLock);
	if(index == 0)
		return "STDFONT";
	const Vec<FaceInfo>& l = FaceList();
	if(index >= 0 && index < l.GetCount())
		return l[index].name;
	return Null;
}

dword Font::GetFaceInfo(i32 index)
{
	Mutex::Lock __(sFontLock);
	const Vec<FaceInfo>& l = FaceList();
	if(index >= 0 && index < l.GetCount())
		return l[index].info;
	return 0;
}

void Font::SetFace(i32 index, const Txt& name, dword info)
{
	Mutex::Lock __(sFontLock);
	FaceInfo& f = FaceList().At(index);
	f.name = name;
	f.info = info;
}

void Font::SetFace(i32 index, const Txt& name)
{
	i32 q = FindFaceNameIndex(name);
	q = q >= 0 ? GetFaceInfo(q) : 0;
	SetFace(index, name, q);
}

wchar FontFilter(wchar c)
{
	return (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') ? c : (c >= 'A' && c <= 'Z') ? ToLower(c) : 0;
}

i32  Font::FindFaceNameIndex(const Txt& name) {
	if(name == "STDFONT")
		return 0;
	for(i32 i = 1; i < GetFaceCount(); i++)
		if(GetFaceName(i) == name)
			return i;
	Txt n = Filter(name, FontFilter);
	for(i32 i = 1; i < GetFaceCount(); i++)
		if(Filter(GetFaceName(i), FontFilter) == n)
			return i;
	if(n == "serif")
		return SERIF;
	if(n == "sansserif")
		return SANSSERIF;
	if(n == "monospace")
		return MONOSPACE;
	if(n == "stdfont")
		return STDFONT;
	return 0;
}

void Font::SyncStdFont()
{
	Mutex::Lock __(sFontLock);
	StdFontSize = Size(AStdFont.GetAveWidth(), AStdFont().GetCy());
	LLOG("SyncStdFont " << StdFontSize);
	SyncUHDMode();
}

void (*whenSetStdFont)();

void Font::SetStdFont0(Font font)
{
	LLOG("SetStdFont " << font);
	Mutex::Lock __(sFontLock);
	static i32 x = 0;
	if(x) return;
	x++;
	InitStdFont();
	AStdFont = font;
	LLOG("AStdFont1: " << AStdFont);
	SyncStdFont();
	LLOG("AStdFont2: " << AStdFont);
	if(whenSetStdFont)
		(*whenSetStdFont)();
	LLOG("AStdFont3: " << AStdFont);
	x--;
	static i32 w = 0;
	if(w) return;
	w++;
	if(whenSetStdFont)
		(*whenSetStdFont)();
	LLOG("AStdFont4: " << AStdFont);
	w--;
}

bool Font::std_font_override;

void Font::SetDefaultFont(Font font)
{
	LLOG("SetDefaultFont " << font);
	if(!std_font_override)
		SetStdFont0(font);
}

void Font::SetStdFont(Font font)
{
	std_font_override = true;
	SetStdFont0(font);
}

void Font::InitStdFont()
{
	ONCELOCK { // TODO: This is now sort of obsolete function....
	//	Mutex::Lock __(sFontLock);
	//	FaceList();
		AStdFont = Arial(12);
	//	SyncStdFont();
	}
}

Font Font::GetStdFont()
{
	InitStdFont();
	return AStdFont;
}

Size Font::GetStdFontSize()
{
	InitStdFont();
	return StdFontSize;
}

Font StdFont()
{
	return Font(0, -32000);
}

void Font::RealizeStd()
{
	if(IsNullInstance())
		*this = GetStdFont();
	if(v.face == STDFONT)
		Face(GetStdFont().GetFace());
	if(v.height == -32000)
		Height(GetStdFont().GetHeight());
}

i32 Font::GetHeight() const
{
	return v.height == -32000 ? GetStdFont().GetHeight() : v.height;
}

Txt Font::GetFaceName() const {
	if(IsNullInstance()) return Txt();
	if(GetFace() == 0)
		return "STDFONT";
	return GetFaceName(GetFace());
}

dword Font::GetFaceInfo() const {
	if(IsNullInstance()) return 0;
	return GetFaceInfo(GetFace());
}

Font& Font::FaceName(const Txt& name) {
	i32 n = FindFaceNameIndex(name);
	Face(n < 0 ? 0xffff : n);
	return *this;
}

void Font::Serialize(Stream& s) {
	i32 version = 1;
	s / version;
	if(version >= 1) {
		enum {
			OLD_STDFONT, OLD_SCREEN_SERIF, OLD_SCREEN_SANS, OLD_SCREEN_FIXED,
			OLD_ROMAN,
			OLD_ARIAL,
			OLD_COURIER,
		};
		i32 f = GetFace();
		if(f > COURIER)
			f = -1;
		s / f;
		Txt name;
		if(f == OLD_ROMAN)
			f = ROMAN;
		if(f == OLD_ARIAL)
			f = ARIAL;
		if(f == OLD_COURIER)
			f = COURIER;
		if(f < 0) {
			name = GetFaceName();
			s % name;
		}
		if(s.IsLoading()) {
			if(f >= 0)
				Face(f);
			else {
				FaceName(name);
				if(IsNull(name))
					SetNull();
			}
		}
	}
	else {
		Txt name = GetFaceName();
		s % name;
		if(s.IsLoading()) {
			FaceName(name);
			if(IsNullInstance())
				Face(COURIER);
		}
	}
	s % v.flags % v.height % v.width;
}

Txt Font::GetTextFlags() const
{
	Txt txt;
	if(IsBold())
		txt << "bold ";
	if(IsItalic())
		txt << "italic ";
	if(IsUnderline())
		txt << "underline ";
	if(IsStrikeout())
		txt << "strikeout ";
	if(IsNonAntiAliased())
		txt << "noaa ";
	if(IsTrueTypeOnly())
		txt << "ttonly ";
	if(txt.GetCount())
		txt.Trim(txt.GetCount() - 1);
	return txt;
}

void Font::ParseTextFlags(tukk s)
{
	CParser p(s);
	v.flags = 0;
	while(!p.IsEof()) {
		if(p.Id("bold"))
			Bold();
		else
		if(p.Id("italic"))
			Italic();
		else
		if(p.Id("underline"))
			Underline();
		else
		if(p.Id("strikeout"))
			Strikeout();
		else
		if(p.Id("noaa"))
			NonAntiAliased();
		else
		if(p.Id("ttonly"))
			TrueTypeOnly();
		else
			p.SkipTerm();
	}
}

Txt Font::GetFaceNameStd() const
{
	switch(GetFace()) {
	case STDFONT:   return "STDFONT";
	case SERIF:     return "serif";
	case SANSSERIF: return "sansserif";
	case MONOSPACE: return "monospace";
	}
	return GetFaceName();
}

void Font::Jsonize(JsonIO& jio)
{
	Txt n, tf;
	if(jio.IsStoring()) {
		n = GetFaceNameStd();
		tf = GetTextFlags();
		if(IsNullInstance())
			n.Clear();
	}
	jio("face", n)("height", v.height)("width", v.width)("flags", tf);
	if(IsNull(n))
		SetNull();
	else {
		FaceName(n);
		ParseTextFlags(tf);
	}
}

void Font::Xmlize(XmlIO& xio)
{
	Txt n, tf;
	if(xio.IsStoring()) {
		n = GetFaceNameStd();
		tf = GetTextFlags();
		if(IsNullInstance())
			n.Clear();
	}
	xio.Attr("face", n)
	   .Attr("height", v.height)
	   .Attr("width", v.width)
	   .Attr("flags", tf);
	if(IsNull(n))
		SetNull();
	else {
		FaceName(n);
		ParseTextFlags(tf);
	}
}

template<>
Txt AsTxt(const Font& f) {
	if(IsNull(f)) return "<null>";
	Txt s = "<" + f.GetFaceName() + Format(":%d", f.GetHeight());
	if(f.IsBold())
		s += " Полужирный";
	if(f.IsItalic())
		s += " Курсив";
	if(f.IsUnderline())
		s += " Подчёркнутый";
	if(f.IsStrikeout())
		s += " Зачёркнутый";
	return s + '>';
}

struct CharEntry {
	z64     font;
	GlyphInfo info;
	wchar     chr;
};

CharEntry fc_cache_global[16384];

inline hash_t GlyphHash(Font font, i32 chr)
{
	return FoldHash(CombineHash(font.GetHashVal(), chr));
}

bool IsNormal_nc(Font font, i32 chr)
{ // do not change cache - to be used in Replace
	Mutex::Lock __(sFontLock);
	font.RealizeStd();
	CharEntry& e = fc_cache_global[GlyphHash(font, chr) & 16383];
	if(e.font == font.AsInt64() && e.chr == chr)
		return e.info.IsNormal();
	return GetGlyphInfoSys(font, chr).IsNormal();
}

struct GlyphInfoMaker : ValMaker {
	Font font;
	i32  chr;

	virtual Txt Key() const {
		TxtBuf s;
		z64 h = font.AsInt64();
		RawCat(s, h);
		RawCat(s, chr);
		return Txt(s);
	}
	virtual i32    Make(Val& object) const {
		CharEntry& e = CreateRawVal<CharEntry>(object);
		e.font = font.AsInt64();
		e.chr = chr;
		Font rfnt;
		if(PreferColorEmoji(chr) && !(font.GetFaceInfo() & Font::COLORIMG)
		   && Replace(font, chr, rfnt) && rfnt != font) {
			e.info.width = (i16)0x8000;
			e.info.lspc = rfnt.GetFace();
			e.info.rspc = rfnt.GetHeight();
		}
		else {
			e.info = GetGlyphInfoSys(font, chr);
			if(!e.info.IsNormal()) {
				ComposedGlyph cg;
				if(Compose(font, chr, cg)) {
					e.info.lspc = -1;
					e.info.rspc = (i16)cg.basic_char;
				}
				else
				if(Replace(font, chr, rfnt)) {
					e.info.lspc = rfnt.GetFace();
					e.info.rspc = rfnt.GetHeight();
				}
				else
					e.info.lspc = -2;
			}
		}
		return sizeof(e);
	}
};

CharEntry GetGlyphEntry(Font font, i32 chr, hash_t hash)
{
	Mutex::Lock __(sFontLock);
	GlyphInfoMaker m;
	m.font = font;
	m.chr = chr;
	return MakeVal(m).To<CharEntry>();
}

thread_local CharEntry fc_cache[512];

GlyphInfo GetGlyphInfo(Font font, i32 chr)
{
	font.RealizeStd();
	hash_t hash = GlyphHash(font, chr);
	CharEntry& e = fc_cache[hash & 511];
	if(e.font != font.AsInt64() || e.chr != chr)
		e = GetGlyphEntry(font, chr, hash);
	return e.info;
}

struct FontEntry {
	CommonFontInfo info;
	z64          font;
};

thread_local FontEntry fi_cache[63];

const CommonFontInfo& GetFontInfo(Font font)
{
	font.RealizeStd();
	dword hash = FoldHash(font.GetHashVal()) % 63;
	FontEntry& e = fi_cache[hash];
	if(e.font != font.AsInt64()) {
		Mutex::Lock __(sFontLock);
		e.font = font.AsInt64();
		e.info = GetFontInfoSys(font);
	}
	return e.info;
}

thread_local z64 lastFiFont = INT_MIN;
thread_local CommonFontInfo lastFontInfo;
thread_local z64 lastStdFont = INT_MIN;

const CommonFontInfo& Font::Fi() const
{
	if(lastStdFont != AStdFont.AsInt64()) {
		lastFiFont = INT_MIN;
		lastStdFont = AStdFont.AsInt64();
	}
	if(AsInt64() == lastFiFont)
		return lastFontInfo;
	lastFontInfo = GetFontInfo(*this);
	lastFiFont = AsInt64();
	return lastFontInfo;
}

bool Font::IsNormal(i32 ch) const
{
	return GetGlyphInfo(*this, ch).IsNormal();
}

bool Font::IsComposed(i32 ch) const
{
	return GetGlyphInfo(*this, ch).IsComposed();
}

bool Font::IsReplaced(i32 ch) const
{
	return GetGlyphInfo(*this, ch).IsReplaced();
}

bool Font::IsMissing(i32 ch) const
{
	return GetGlyphInfo(*this, ch).IsMissing();
}

i32 Font::HasChar(i32 ch) const
{
	return !GetGlyphInfo(*this, ch).IsMissing();
}

void GlyphMetrics(GlyphInfo& f, Font font, i32 chr)
{
	if(f.IsReplaced())
		f = GetGlyphInfo(font().Face(f.lspc).Height(f.rspc), chr);
	if(f.IsComposed()) {
		f = GetGlyphInfo(font, f.rspc);
		if(f.IsComposedLM())
			f.rspc += f.width / 2;
	}
}

GlyphInfo GetGlyphMetrics(Font font, i32 chr)
{
	font.RealizeStd();
	GlyphInfo f = GetGlyphInfo(font, chr);
	if(f.IsMissing()) {
		Font fnt = Arial(font.GetHeight());
		wchar chr = 0x25a1;
		f = GetGlyphInfo(fnt, chr);
		if(!f.IsNormal()) {
			chr = ' ';
			f = GetGlyphInfo(fnt, chr);
		}
	}
	else
		GlyphMetrics(f, font, chr);
	return f;
}

i32 Font::GetWidth(i32 c) const {
	return GetGlyphMetrics(*this, c).width;
}

i32 Font::GetLeftSpace(i32 c) const {
	return GetGlyphMetrics(*this, c).lspc;
}

i32 Font::GetRightSpace(i32 c) const {
	return GetGlyphMetrics(*this, c).rspc;
}

Txt Font::GetData(tukk table, i32 offset, i32 size) const
{
	Mutex::Lock __(sFontLock);
	ASSERT(!table || strlen(table) == 4);
	return GetFontDataSys(*this, table, offset, size);
}

void Font::Render(FontGlyphConsumer& sw, double x, double y, i32 ch) const
{
	Mutex::Lock __(sFontLock);
	RenderCharacterSys(sw, x, y, ch, *this);
}

FontInfo Font::Info() const
{
	FontInfo h;
	h.font = *this;
	return h;
}

}