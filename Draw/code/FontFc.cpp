#include <drx/Draw/Draw.h>

#define LLOG(x)     //  LOG(x)
#define LTIMING(x)  //  TIMING(x)

#if !defined(CUSTOM_FONTSYS) && !defined(TARGET_COCOA)

#ifdef TARGET_POSIX

#include <fontconfig/fontconfig.h>
#include <fontconfig/fcfreetype.h>

namespace drx {

static FT_Library sFTlib;

EXITBLOCK
{
	if(sFTlib)
		FT_Done_FreeType(sFTlib);
}

bool sInitFt(void)
{
	if(sFTlib)
		return true;
	return FT_Init_FreeType(&sFTlib) == 0;
}

FcPattern *CreateFcPattern(Font font)
{
	LTIMING("CreateXftFont");
	i32 hg = abs(font.GetHeight());
	if(hg == 0) hg = 10;
	Txt face = font.GetFaceName();
	FcPattern *p = FcPatternCreate();
	FcPatternAddString(p, FC_FAMILY, (FcChar8*)~face);
	FcPatternAddInteger(p, FC_SLANT, font.IsItalic() ? FC_SLANT_ITALIC : FC_SLANT_ROMAN);
	FcPatternAddInteger(p, FC_PIXEL_SIZE, hg);
	FcPatternAddInteger(p, FC_WEIGHT, font.IsBold() ? FC_WEIGHT_BOLD : FC_WEIGHT_NORMAL);
	FcPatternAddBool(p, FC_MINSPACE, 1);
	FcConfigSubstitute(0, p, FcMatchPattern);
	FcDefaultSubstitute(p);
	FcResult result;
	FcPattern *m = FcFontMatch(0, p, &result);
	FcPatternDestroy(p);
	return m;
}

FT_Face CreateFTFace(const FcPattern *pattern, Txt *rpath) {
	FT_Face	    face = NULL;

	double	    dsize;
	double	    aspect;
	FcChar8    *filename;

	if(!sInitFt())
		return NULL;

	if(FcPatternGetString(pattern, FC_FILE, 0, &filename) != FcResultMatch)
		return NULL;
	if(rpath)
		*rpath = (tukk )filename;

	if(FcPatternGetDouble(pattern, FC_PIXEL_SIZE, 0, &dsize) != FcResultMatch)
		dsize = 16;

	if (FcPatternGetDouble(pattern, FC_ASPECT, 0, &aspect) != FcResultMatch)
		aspect = 1.0;

	if(FT_New_Face(sFTlib, (tukk )filename, 0, &face))
		return NULL;

	FT_F26Dot6 ysize = (FT_F26Dot6) (dsize * 64.0);
	FT_F26Dot6 xsize = (FT_F26Dot6) (dsize * aspect * 64.0);

	FT_Set_Char_Size(face, xsize, ysize, 0, 0);
	return face;
}

#define FONTCACHE 37

struct FtFaceEntry {
	Font    font;
	FT_Face face;
	Txt  path;
};

FT_Face (*FTFaceXft)(Font fnt, Txt *rpath);

FT_Face FTFace(Font fnt, Txt *rpath = NULL)
{
	LTIMING("FTFace");
	if(FTFaceXft)
		return (*FTFaceXft)(fnt, rpath);
	static FtFaceEntry ft_cache[FONTCACHE];
	ONCELOCK {
		for(i32 i = 0; i < FONTCACHE; i++)
			ft_cache[i].font.Height(-30000);
	}
	FtFaceEntry be;
	be = ft_cache[0];
	for(i32 i = 0; i < FONTCACHE; i++) {
		FtFaceEntry e = ft_cache[i];
		if(i)
			ft_cache[i] = be;
		if(e.font == fnt) {
			if(rpath)
				*rpath = e.path;
			if(i)
				ft_cache[0] = e;
			return e.face;
		}
		be = e;
	}
	LTIMING("FTFace2");
	if(be.face) {
		LLOG("Removing " << be.font << " - " << (uk )be.face);
		FT_Done_Face(be.face);
	}
	be.font = fnt;
	FcPattern *p = CreateFcPattern(fnt);
	be.face = CreateFTFace(p, &be.path);
	FcPatternDestroy(p);
	ft_cache[0] = be;
	if(rpath)
		*rpath = be.path;
	return be.face;
}

CommonFontInfo (*GetFontInfoSysXft)(Font font);

CommonFontInfo GetFontInfoSys(Font font)
{
	sInitFt();
	if(GetFontInfoSysXft)
		return (*GetFontInfoSysXft)(font);
	CommonFontInfo fi;
	Txt path;
	FT_Face face = FTFace(font, &path);
	if(face) {
		fi.ascent = face->size->metrics.ascender >> 6;
		fi.descent = -(face->size->metrics.descender >> 6);
		fi.external = (face->size->metrics.height >> 6) - fi.ascent - fi.descent;
		fi.internal = 0;
		fi.overhang = 0;
		fi.maxwidth = face->size->metrics.max_advance >> 6;
		fi.avewidth = fi.maxwidth;
		fi.default_char = '?';
		fi.fixedpitch = font.GetFaceInfo() & Font::FIXEDPITCH;
		fi.ttf = path.EndsWith(".ttf") || path.EndsWith(".otf") || path.EndsWith(".otc") || path.EndsWith(".ttc");
		fi.fonti = face->face_index;
		if(path.GetCount() < 250)
			strcpy(fi.path, ~path);
		else
			*fi.path = 0;

		if(font.GetFaceInfo() & Font::COLORIMG) { // Experimental estimate for cairo results
			fi.colorimg_cy = fi.ascent + fi.descent;
			i32 h = 4 * font.GetHeight() / 3;
			fi.ascent = h * fi.ascent / fi.colorimg_cy;
			fi.descent = h - fi.ascent;
		}
	}
	return fi;
}

#define FLOOR(x)    ((x) & -64)
#define CEIL(x)	    (((x)+63) & -64)
#define TRUNC(x)    ((x) >> 6)
#define ROUND(x)    (((x)+32) & -64)

GlyphInfo (*GetGlyphInfoSysXft)(Font font, i32 chr);

GlyphInfo  GetGlyphInfoSys(Font font, i32 chr)
{
	LTIMING("GetGlyphInfoSys");
	GlyphInfo gi;
	FT_Face face = FTFace(font, NULL);
	gi.lspc = gi.rspc = 0;
	gi.width = 0x8000;
	LLOG("GetGlyphInfoSys " << font << " " << (char)chr << " " << FormatIntHex(chr));
	if(face) {
		LTIMING("GetGlyphInfoSys 2");
		i32 glyph_index = FT_Get_Char_Index(face, chr);
		if(glyph_index) {
			if(FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT|FT_LOAD_NO_BITMAP) == 0 ||
			   FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) == 0) {
				FT_Glyph_Metrics& m = face->glyph->metrics;
				i32 left  = FLOOR(m.horiBearingX);
				i32 width = TRUNC(CEIL(m.horiBearingX + m.width) - left);
				gi.width = TRUNC(ROUND(face->glyph->advance.x));
				if(font.GetFaceInfo() & Font::COLORIMG) {
					gi.lspc = gi.rspc = 0;
					i32 q = GetFontInfo(font).colorimg_cy;
					gi.width = font.GetCy() * gi.width / q + max(1, font.GetCy() / 10); // add a little space there...
				}
				else {
					gi.lspc = TRUNC(left);
					gi.rspc = gi.width - width - gi.lspc;
				}
				gi.glyphi = glyph_index;
			}
		}
	}
	return gi;
}

Vec<FaceInfo> GetAllFacesSys()
{
	static tukk basic_fonts[] = {
		"sans-serif",
		"serif",
		"sans-serif",
		"monospace",
	};

	VecMap<Txt, FaceInfo> list;
	for(i32 i = 0; i < __countof(basic_fonts); i++) {
		Txt name = (tukk )basic_fonts[i];
		FaceInfo& fi = list.Add(name);
		fi.name = name;
		fi.info = Font::SCALEABLE;
		if(i == Font::SERIF)
			fi.info |= Font::SERIFSTYLE;
		if(i == Font::MONOSPACE)
			fi.info |= Font::FIXEDPITCH;
	}
	FcPattern *p = FcPatternCreate();
	FcObjectSet *os = FcObjectSetBuild(FC_FAMILY, FC_SPACING, FC_SCALABLE,
	                        #ifndef ASTRA
	                                         FC_SYMBOL, FC_COLOR,
	                        #endif
	                                         (uk )0);
	FcFontSet *fs = FcFontList(NULL, p, os);
	FcPatternDestroy(p);
	FcObjectSetDestroy(os);
	for(i32 i = 0; i < fs->nfont; i++) {
		FcChar8 *family = NULL;
		FcPattern *pt = fs->fonts[i];
		if(FcPatternGetString(pt, FC_FAMILY, 0, &family) == 0 && family) {
			Txt name = (tukk )family;
			FaceInfo& fi = list.GetAdd(name);
			fi.name = name;
			i32 iv;
			FcBool bv;
			if(FcPatternGetInteger(pt, FC_SPACING, 0, &iv) == 0 && iv == FC_MONO)
				fi.info |= Font::FIXEDPITCH;
			#ifndef ASTRA
			if(FcPatternGetBool(pt, FC_SYMBOL, 0, &bv) == 0 && bv)
				fi.info |= Font::SPECIAL;
			if(FcPatternGetBool(pt, FC_COLOR, 0, &bv) == 0 && bv)
				fi.info |= Font::COLORIMG;
			#endif
			if(FcPatternGetBool(pt, FC_SCALABLE, 0, &bv) == 0 && bv)
				fi.info |= Font::SCALEABLE;
			Txt h = ToLower(fi.name);
			if((h.Find("serif") >= 0 || h.Find("roman") >= 0) && h.Find("sans") < 0)
				fi.info |= Font::SERIFSTYLE;
			if(h.Find("script") >= 0)
				fi.info |= Font::SCRIPTSTYLE;
		}
	}
	FcFontSetDestroy(fs);
	return list.PickVals();
}

extern Txt GetFontDataSysSys(Stream& in, i32 fonti, tukk table, i32 offset, i32 size);

Txt GetFontDataSys(Font font, tukk table, i32 offset, i32 size)
{
	if(!table)
		return LoadFile(font.Fi().path);
	FileIn in(font.Fi().path);
	return GetFontDataSysSys(in, font.Fi().fonti, table, offset, size);
}

static inline double ft_dbl(i32 p)
{
    return double(p) / 64.0;
}

struct ConvertOutlinePoint {
	bool sheer;
	double xx;
	double yy;

	Pointf operator()(i32 x, i32 y) {
		double fy = ft_dbl(y);
		return Pointf(ft_dbl(x) + xx + (sheer ? 0.2 * fy : 0), yy - fy);
	}
};

bool RenderOutline(const FT_Outline& outline, FontGlyphConsumer& path, double xx, double yy, bool sheer)
{
	ConvertOutlinePoint cp;
	cp.xx = xx;
	cp.yy = yy;
	cp.sheer = sheer;

	FT_Vector   v_last;
	FT_Vector   v_control;
	FT_Vector   v_start;
	FT_Vector*  point;
	FT_Vector*  limit;
	char*       tags;
	i32   n;         // индекс контура в outline
	char  tag;       // состояние текущей точки
	i32   first = 0; // индекс первой точки в контуре
	for(n = 0; n < outline.n_contours; n++) {
		i32  last = outline.contours[n];
		limit = outline.points + last;
		v_start = outline.points[first];
		v_last  = outline.points[last];
		v_control = v_start;
		point = outline.points + first;
		tags  = (tuk) (outline.tags  + first);
		tag   = FT_CURVE_TAG(tags[0]);
		if(tag == FT_CURVE_TAG_CUBIC) return false;
		if(tag == FT_CURVE_TAG_CONIC) {
			if(FT_CURVE_TAG(outline.tags[last]) == FT_CURVE_TAG_ON) {
				// start at last point if it is on the curve
				v_start = v_last;
				limit--;
			}
			else {
				// if both first and last points are conic,
				// start at their middle and record its position
				// for closure
				v_start.x = (v_start.x + v_last.x) / 2;
				v_start.y = (v_start.y + v_last.y) / 2;
				v_last = v_start;
			}
			point--;
			tags--;
		}
		path.Move(cp(v_start.x, v_start.y));
		while(point < limit) {
			point++;
			tags++;

			tag = FT_CURVE_TAG(tags[0]);
			switch(tag) {
			case FT_CURVE_TAG_ON:
				path.Line(cp(point->x, point->y));
				continue;
			case FT_CURVE_TAG_CONIC:
				v_control.x = point->x;
				v_control.y = point->y;
			Do_Conic:
				if(point < limit) {
					FT_Vector vec;
					FT_Vector v_middle;
					point++;
					tags++;
					tag = FT_CURVE_TAG(tags[0]);
					vec.x = point->x;
					vec.y = point->y;
					if(tag == FT_CURVE_TAG_ON) {
						path.Quadratic(cp(v_control.x, v_control.y), cp(vec.x, vec.y));
						continue;
					}
					if(tag != FT_CURVE_TAG_CONIC) return false;
					v_middle.x = (v_control.x + vec.x) / 2;
					v_middle.y = (v_control.y + vec.y) / 2;
					path.Quadratic(cp(v_control.x, v_control.y), cp(v_middle.x, v_middle.y));
					v_control = vec;
					goto Do_Conic;
				}
				path.Quadratic(cp(v_control.x, v_control.y), cp(v_start.x, v_start.y));
				goto Close;

			default:
				FT_Vector vec1, vec2;
				if(point + 1 > limit || FT_CURVE_TAG(tags[1]) != FT_CURVE_TAG_CUBIC)
				    return false;
				vec1.x = point[0].x;
				vec1.y = point[0].y;
				vec2.x = point[1].x;
				vec2.y = point[1].y;
				point += 2;
				tags  += 2;
				if(point <= limit) {
					FT_Vector vec;
					vec.x = point->x;
					vec.y = point->y;
					path.Cubic(cp(vec1.x, vec1.y), cp(vec2.x, vec2.y), cp(vec.x, vec.y));
					continue;
				}
				path.Cubic(cp(vec1.x, vec1.y), cp(vec2.x, vec2.y), cp(v_start.x, v_start.y));
				goto Close;
			}
		}
	Close:
		path.Close();
		first = last + 1;
    }
	return true;
}

void RenderCharacterSys(FontGlyphConsumer& sw, double x, double y, i32 ch, Font fnt)
{
	FT_Face face = FTFace(fnt, NULL);
	i32 glyph_index = FT_Get_Char_Index(face, ch);
	if(glyph_index && FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT) == 0)
		RenderOutline(face->glyph->outline, sw, x, y + fnt.GetAscent(),
		              fnt.IsItalic() && !(face->style_flags & FT_STYLE_FLAG_ITALIC));
}

}

#endif

#endif