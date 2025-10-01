#include <drx/CtrlCore/CtrlCore.h>

#ifdef GUI_GTK

#include <cairo/cairo-ft.h>

// #include <freetype2/freetype/ftoutln.h>

namespace drx {

#define LLOG(x)

FcPattern *CreateFcPattern(Font font);
FT_Face    FTFace(Font fnt, Txt *rpath = NULL);

struct FontSysData {
	cairo_scaled_font_t *scaled_font;
	bool                 colorimg = false;
	Sizef                colorimg_size;
	
	void Init(Font font, i32 angle);
	~FontSysData() { cairo_scaled_font_destroy(scaled_font); }
};

void FontSysData::Init(Font font, i32 angle)
{
	LLOG("FontSysData::Init " << font << ", " << angle);
	Std(font);

	FcPattern *p = CreateFcPattern(font);
	cairo_font_face_t *font_face = cairo_ft_font_face_create_for_pattern(p);
	FcPatternDestroy(p);
	
	cairo_matrix_t font_matrix[1], ctm[1];
	cairo_matrix_init_identity(ctm);
	
	i32 fh = font.GetHeight();

	cairo_matrix_init_scale(font_matrix, fh, fh);

	if(angle)
		cairo_matrix_rotate(font_matrix, -angle * M_PI / 1800);

	if(font.IsItalic() && !(FTFace(font)->style_flags & FT_STYLE_FLAG_ITALIC)) { // Synthetic italic
		cairo_matrix_t shear[1];
		cairo_matrix_init_identity(shear);
		shear->xy = -0.165;
		cairo_matrix_multiply(font_matrix, shear, font_matrix);
	}

	cairo_font_options_t *opt = cairo_font_options_create();
	scaled_font = cairo_scaled_font_create(font_face, font_matrix, ctm, opt);

	cairo_font_options_destroy(opt);
	cairo_font_face_destroy(font_face);
}

struct FontDataSysMaker : LRUCache<FontSysData, Tuple2<Font, i32> >::Maker {
	Font font;
	i32  angle;

	virtual Tuple2<Font, i32>  Key() const        { return MakeTuple(font, angle); }
	virtual i32 Make(FontSysData& object) const   { object.Init(font, angle); return 1; }
};

i32    gtk_antialias = -1; // These are not really used with GTK backed.....
i32    gtk_hinting = -1;
Txt    gtk_hintstyle;
Txt    gtk_rgba;

void SystemDraw::FlushText()
{
	if(textcache.GetCount() == 0)
		return;
	static LRUCache<FontSysData, Tuple2<Font, i32> > cache;
	FontDataSysMaker m;
	m.font = textfont;
	m.angle = textangle;
	FontSysData& sf = cache.Get(m);

	cairo_set_scaled_font(cr, sf.scaled_font);
	SetColor(textink);

	Buffer<cairo_glyph_t> gs(textcache.GetCount());
	for(i32 i = 0; i < textcache.GetCount(); i++) {
		cairo_glyph_t& g = gs[i];
		g.index = textcache[i].index;
		g.x = textcache[i].x;
		g.y = textcache[i].y;
	}

	cairo_show_glyphs(cr, gs, textcache.GetCount());
	
	cache.Shrink(INT_MAX, 128);

	textcache.Clear();
}

void SystemDraw::DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font, Color ink, i32 n, i32k *dx)
{
	GuiLock __;
	
	if(textcache.GetCount() && (font != textfont || ink != textink || angle != textangle))
		FlushText();
	
	textfont = font;
	textink = ink;
	textangle = angle;
	
	i32 ascent = font.GetAscent();
	double sina = 0;
	double cosa = 1;
	if(angle)
		Draw::SinCos(angle, sina, cosa);
	i32 xpos = 0;
	for(i32 i = 0; i < n; i++) {
		TextGlyph& g = textcache.Add();
		g.index = GetGlyphInfo(font, text[i]).glyphi;
		g.x = i32(x + cosa * xpos + sina * ascent);
		g.y = i32(y + cosa * ascent - sina * xpos);
		xpos += dx ? dx[i] : font[text[i]];
	}
}

}

#endif
