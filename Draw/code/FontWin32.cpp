#include <drx/Draw/Draw.h>

namespace drx {

#ifndef CUSTOM_FONTSYS

#ifdef TARGET_WIN32

#include <usp10.h>

#define LLOG(x)     //  LOG(x)
#define LTIMING(x)  //  TIMING(x)

#define FONTCACHE 96

struct HFontEntry {
	Font    font;
	HFONT   hfont;
	i32     angle;
};

HFONT GetWin32Font(Font fnt, i32 angle)
{
	LTIMING("GetWin32Font");
	static HFontEntry cache[FONTCACHE];
	ONCELOCK {
		for(i32 i = 0; i < FONTCACHE; i++)
			cache[i].font.Height(-30000);
	}
	HFontEntry be;
	be = cache[0];
	for(i32 i = 0; i < FONTCACHE; i++) {
		HFontEntry e = cache[i];
		if(i)
			cache[i] = be;
		if(e.font == fnt && e.angle == angle) {
			if(i)
				cache[0] = e;
			return e.hfont;
		}
		be = e;
	}
	LTIMING("GetWin32Font2");
	if(be.hfont)
		DeleteObject(be.hfont);

	be.font = fnt;
	be.angle = angle;
	be.hfont = CreateFontW(
		fnt.GetHeight() ? -abs(fnt.GetHeight()) : -12,
		fnt.GetWidth(), angle, angle, fnt.IsBold() ? FW_BOLD : FW_NORMAL,
		fnt.IsItalic(), fnt.IsUnderline(), fnt.IsStrikeout(),
		fnt.GetFace() == Font::SYMBOL ? SYMBOL_CHARSET : DEFAULT_CHARSET,
		fnt.IsTrueTypeOnly() ? OUT_TT_ONLY_PRECIS : OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS,
		fnt.IsNonAntiAliased() ? NONANTIALIASED_QUALITY : DEFAULT_QUALITY,
		DEFAULT_PITCH|FF_DONTCARE,
		ToSystemCharsetW(fnt.GetFaceName())
	);

	cache[0] = be;
	return be.hfont;
}

static void Win32_GetGlyphIndices(HDC hdc, LPCWSTR s, i32 n, LPWORD r, DWORD flag)
{
	typedef DWORD (WINAPI *GGIW)(HDC, LPCWSTR, i32, LPWORD, DWORD);
	static GGIW fn;
	ONCELOCK
		if(HMODULE hDLL = LoadLibrary("gdi32"))
			fn = (GGIW) GetProcAddress(hDLL, "GetGlyphIndicesW");
	if(fn)
		fn(hdc, s, n, r, flag);
	else
		memset8(r, 0, n * sizeof(WORD));
}

CommonFontInfo GetFontInfoSys(Font font)
{
	CommonFontInfo fi;
	memset8(&fi, 0, sizeof(fi));
	HFONT hfont = GetWin32Font(font, 0);
	if(hfont) {
		HDC hdc = ::CreateIC("DISPLAY", NULL, NULL, NULL);
		HFONT ohfont = (HFONT) ::SelectObject(hdc, hfont);
		TEXTMETRIC tm;
		::GetTextMetrics(hdc, &tm);
		fi.ascent = tm.tmAscent;
		fi.descent = tm.tmDescent;
		fi.external = tm.tmExternalLeading;
		fi.internal = tm.tmInternalLeading;
		fi.overhang = tm.tmOverhang;
		fi.avewidth = tm.tmAveCharWidth;
		fi.maxwidth = tm.tmMaxCharWidth;
		fi.firstchar = tm.tmFirstChar;
		fi.charcount = tm.tmLastChar - tm.tmFirstChar + 1;
		fi.default_char = tm.tmDefaultChar;
		fi.fixedpitch = (tm.tmPitchAndFamily & TMPF_FIXED_PITCH) == 0;
		fi.ttf = fi.scaleable = tm.tmPitchAndFamily & TMPF_TRUETYPE;
		if(fi.scaleable) {
			ABC abc;
			GetCharABCWidths(hdc, 'o', 'o', &abc);
			fi.spacebefore = abc.abcA;
			fi.spaceafter = abc.abcC;
		}
		else
			fi.spacebefore = fi.spaceafter = 0;
		::SelectObject(hdc, ohfont);
		::DeleteDC(hdc);
	}
	return fi;
}

static VecMap<Txt, FaceInfo> *sList;

static i32 CALLBACK Win32_AddFace(const LOGFONTW *logfont, const TEXTMETRICW *, dword type, LPARAM param)
{
	tukk facename = (tukk )param;

	Txt name = FromSystemCharsetW(logfont->lfFaceName);

	if(facename && stricmp(name, facename))
		return 1;

	if(logfont->lfFaceName[0] == '@')
		return 1;


	if(FindIndex(Split("Courier New CE;Courier New CYR;Courier New Greek;"
	                   "Courier New TUR;Courier New Baltic;Arial CE;Arial CYR;"
	                   "Arial Greek;Arial TUR;Arial Baltic;Arial CE;Times New Roman CE;"
	                   "Times New Roman CYR;Times New Roman Greek;Times New Roman TUR;"
	                   "Times New Roman Baltic;Times New Roman CE", ';'), name) >= 0)
		return 1;

	i32 q = sList->Find(name);
	FaceInfo& f = q < 0 ? sList->Add(name) : (*sList)[q];
	f.name = name;

	if(q < 0)
		f.info = Font::SCALEABLE;
	if((logfont->lfPitchAndFamily & 3) == FIXED_PITCH)
		f.info |= Font::FIXEDPITCH;
	if(logfont->lfPitchAndFamily & FF_ROMAN)
		f.info |= Font::SERIFSTYLE;
	if(logfont->lfPitchAndFamily & FF_SCRIPT)
		f.info |= Font::SCRIPTSTYLE;
	if(!(type & TRUETYPE_FONTTYPE))
		f.info &= ~Font::SCALEABLE;
	if(logfont->lfCharSet == SYMBOL_CHARSET || logfont->lfCharSet == OEM_CHARSET)
		f.info |= Font::SPECIAL;
	return facename ? 0 : 1;
}

static i32 Win32_EnumFace(HDC hdc, tukk face)
{
	return EnumFontFamiliesW(hdc, face ? ToSystemCharsetW(face).begin() : NULL, Win32_AddFace, (LPARAM)face);
}

static void Win32_ForceFace(HDC hdc, tukk face, tukk aface)
{
	if(!aface)
		aface = "Arial";
	if(Win32_EnumFace(hdc, face) && Win32_EnumFace(hdc, aface))
		Panic("Отсутствует шрифт " + Txt(face));
}

Vec<FaceInfo> GetAllFacesSys()
{
	VecMap<Txt, FaceInfo> list;
	sList = &list;
	list.Add("STDFONT").name = "STDFONT";
	list.Top().info = 0;
	HDC hdc = ::CreateIC("DISPLAY", NULL, NULL, NULL);
	Win32_ForceFace(hdc, "Times New Roman", "Arial");
	Win32_ForceFace(hdc, "Arial", "Arial");
	Win32_ForceFace(hdc, "Courier New", "Arial");
	Win32_ForceFace(hdc, "Symbol", "Arial");
	Win32_ForceFace(hdc, "WingDings", "Arial");
	Win32_ForceFace(hdc, "Tahoma", "Arial");
	Win32_EnumFace(hdc, NULL);
	DeleteDC(hdc);
	return list.PickVals();
}

#define GLYPHINFOCACHE 31

GlyphInfo GetUnicodeGlyphInfo(Font fnt, wchar ch)
{
	struct FontRec {
		Font         font = Null;
		SCRIPT_CACHE sc = NULL;
		HDC          hdc = NULL;
		dword        default_glyph = 0;
	};

	static FontRec cache[4];
	static i32 cachei;

	i32 ii = -1;
	for(i32 i = 0; i < 4; i++)
		if(cache[0].font == fnt) {
			ii = i;
			break;
		}

	if(ii < 0) {
		ii = cachei = (cachei + 1) & 3;
		FontRec& f = cache[ii];
		f.font = fnt;
		if(!f.hdc)
			f.hdc = ::CreateIC("DISPLAY", NULL, NULL, NULL);
		if(f.sc) {
			ScriptFreeCache(&f.sc);
			f.sc = 0;
		}
		HFONT hfont = GetWin32Font(fnt, 0);
		if(hfont)
			::SelectObject(f.hdc, hfont);

		SCRIPT_FONTPROPERTIES props;
		props.cBytes = sizeof(props);
		ScriptGetFontProperties(f.hdc, &f.sc, &props);
		f.default_glyph = props.wgDefault;
	}

	GlyphInfo gi;

	memset(&gi, 0, sizeof(gi));
	gi.width = (i16)0x8000;

	FontRec& f = cache[ii];

	char16 buf[2];
	i32 len = ToUtf16(buf, &ch, 1);

	SCRIPT_ITEM items[2];
	i32 nitems;
	if(FAILED(ScriptItemize(buf, len, 2, 0, 0, items, &nitems)))
		return gi;

	WORD glyphs[10];
	WORD cluster[10];
	i32 nglyphs;
	SCRIPT_VISATTR attr[10];
	if(FAILED(ScriptShape(f.hdc, &f.sc, buf, len, 10, &items[0].a, glyphs, cluster, attr, &nglyphs)))
		return gi;

	if(nglyphs == 0 || *glyphs == f.default_glyph)
		return gi;

	ABC abc;
	if(FAILED(ScriptGetGlyphABCWidth(f.hdc, &f.sc, glyphs[0], &abc)))
		return gi;

	gi.width = abc.abcA + abc.abcB + abc.abcC;
	gi.lspc = abc.abcA;
	gi.rspc = abc.abcC;
	gi.glyphi = *glyphs;

	return gi;
}

GlyphInfo  GetGlyphInfoSys(Font font, i32 chr)
{
	if(chr >= 0x10000)
		return GetUnicodeGlyphInfo(font, chr);

	static Font      fnt[GLYPHINFOCACHE];
	static i32       pg[GLYPHINFOCACHE];
	static GlyphInfo li[GLYPHINFOCACHE][256];

	ONCELOCK {
		for(i32 i = 0; i < GLYPHINFOCACHE; i++)
			pg[i] = -1;
	}

	i32 page = chr >> 8;
	i32 q = CombineHash(font, page) % GLYPHINFOCACHE;

	if(fnt[q] != font || pg[q] != page) {
		fnt[q] = font;
		pg[q] = page;
		HFONT hfont = GetWin32Font(font, 0);
		if(!hfont) {
			GlyphInfo n;
			memset(&n, 0, sizeof(GlyphInfo));
			n.width = (i16)0x8000;
			return n;
		}
		HDC hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
		HFONT ohfont = (HFONT) ::SelectObject(hdc, hfont);
		i32 from = page << 8;
		GlyphInfo *t = li[q];

		bool abca = false, abcw = false;
		Buffer<ABC> abc(256);
		abcw = ::GetCharABCWidthsW(hdc, from, from + 256 - 1, abc);
		if(!abcw)
			abca = ::GetCharABCWidthsA(hdc, from, from + 256 - 1, abc);
		if(abcw) {
			for(ABC *s = abc, *lim = abc + 256; s < lim; s++, t++) {
				t->width = s->abcA + s->abcB + s->abcC;
				t->lspc = s->abcA;
				t->rspc = s->abcC;
			}
		}
		else {
			Buffer<i32> wb(256);
			::GetCharWidthW(hdc, from, from + 256 - 1, wb);
			for(i32 *s = wb, *lim = wb + 256; s < lim; s++, t++) {
				t->width = *s - GetFontInfoSys(font).overhang;
				if(abca) {
					ABC aa = abc[(u8)ToAscii(from++)];
					t->lspc = aa.abcA;
					t->rspc = aa.abcC;
				}
				else
					t->lspc = t->rspc = 0;
			}
		}

		WORD pos[256];
		WCHAR wch[256];
		for(i32 i = 0; i < 256; i++)
			wch[i] = from + i;
		Win32_GetGlyphIndices(hdc, wch, 256, pos, 1);
		for(i32 i = 0; i < 256; i++)
			if(pos[i] == 0xffff) {
				li[q][i].width = (i16)0x8000;
				li[q][i].lspc = li[q][i].rspc = 0;
			}

		::SelectObject(hdc, ohfont);
		::DeleteDC(hdc);
	}
	return li[q][chr & 255];
}

Txt GetFontDataSys(Font font, tukk table, i32 offset, i32 size)
{
	Txt r;
	HFONT hfont = GetWin32Font(font, 0);
	if(hfont) {
		HDC hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
		HFONT ohfont = (HFONT) ::SelectObject(hdc, hfont);
		i32 tbl = table ? ((u8)table[0] << 0) | ((u8)table[1] << 8) | ((u8)table[2] << 16) | ((u8)table[3] << 24) : 0;
		DWORD sz = GetFontData(hdc, tbl, offset, NULL, 0);
		if(sz == GDI_ERROR) {
			LLOG("PdfDraw::Finish: GDI_ERROR on font " << pdffont.GetKey(i));
		}
		else {
			size = min(size, (i32)sz);
			TxtBuf b(size);
			GetFontData(hdc, tbl, offset, ~b, size);
			r = b;
		}
		::SelectObject(hdc, ohfont);
		::DeleteDC(hdc);
	}
	return r;
}

double fx_to_dbl(const FIXED& p) {
	return double(p.value) + double(p.fract) * (1.0 / 65536.0);
}

Pointf fx_to_dbl(const Pointf& pp, const POINTFX& p) {
	return Pointf(pp.x + fx_to_dbl(p.x), pp.y - fx_to_dbl(p.y));
}

void RenderCharPath(tukk gbuf, unsigned total_size, FontGlyphConsumer& sw, double xx, double yy)
{
	tukk cur_glyph = gbuf;
	tukk end_glyph = gbuf + total_size;
	Pointf pp(xx, yy);
	while(cur_glyph < end_glyph) {
		const TTPOLYGONHEADER* th = (TTPOLYGONHEADER*)cur_glyph;
		tukk end_poly = cur_glyph + th->cb;
		tukk cur_poly = cur_glyph + sizeof(TTPOLYGONHEADER);
		sw.Move(fx_to_dbl(pp, th->pfxStart));
		while(cur_poly < end_poly) {
			const TTPOLYCURVE* pc = (const TTPOLYCURVE*)cur_poly;
			if (pc->wType == TT_PRIM_LINE)
				for(i32 i = 0; i < pc->cpfx; i++)
					sw.Line(fx_to_dbl(pp, pc->apfx[i]));
			if (pc->wType == TT_PRIM_QSPLINE)
				for(i32 u = 0; u < pc->cpfx - 1; u++) {
					Pointf b = fx_to_dbl(pp, pc->apfx[u]);
					Pointf c = fx_to_dbl(pp, pc->apfx[u + 1]);
					if (u < pc->cpfx - 2)
						c = Mid(b, c);
					sw.Quadratic(b, c);
				}
			cur_poly += sizeof(WORD) * 2 + sizeof(POINTFX) * pc->cpfx;
		}
		sw.Close();
		cur_glyph += th->cb;
    }
}

void RenderCharacterSys(FontGlyphConsumer& sw, double x, double y, i32 ch, Font fnt)
{
	HFONT hfont = GetWin32Font(fnt, 0);
	VERIFY(hfont);
	if(hfont) {
		HDC hdc = CreateIC("DISPLAY", NULL, NULL, NULL);
		HFONT ohfont = (HFONT) ::SelectObject(hdc, hfont);
		GLYPHMETRICS gm;
		MAT2 m_matrix;
		memset8(&m_matrix, 0, sizeof(m_matrix));
		m_matrix.eM11.value = 1;
		m_matrix.eM22.value = 1;
		dword flags = GGO_NATIVE|GGO_UNHINTED;
		if(ch >= 0x10000) {
			GlyphInfo f = GetGlyphInfo(fnt, ch);
			if(f.IsNormal()) {
				flags |= GGO_GLYPH_INDEX;
				ch = f.glyphi;
			}
			else
				ch = 0x25a1;
		}
		i32 gsz = GetGlyphOutlineW(hdc, ch, flags, &gm, 0, NULL, &m_matrix);
		if(gsz >= 0) {
			TxtBuf gb(gsz);
			gsz = GetGlyphOutlineW(hdc, ch, flags, &gm, gsz, ~gb, &m_matrix);
			if(gsz >= 0)
				RenderCharPath(~gb, gsz, sw, x, y + fnt.GetAscent());
		}
		::SelectObject(hdc, ohfont);
		::DeleteDC(hdc);
	}
}

#endif

#endif

}