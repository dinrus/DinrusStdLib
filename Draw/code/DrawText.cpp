#include <drx/Draw/Draw.h>

namespace drx {

#define LLOG(x)    // LOG(x)
#define LTIMING(x) // TIMING(x)

WTxt TextUnicode(tukk s, i32 n, u8 cs, Font font)
{
	if(n < 0)
		n = (i32)strlen(s);
#ifdef TARGET_WIN32
	if(font.GetFace() == Font::SYMBOL) {
		WTxtBuf b(n);
		wtuk t = b;
		while(n > 0) {
			*t++ = *s++;
			n--;
		}
		return WTxt(b);
	}
#endif
	if(cs == CHARSET_UTF8)
		return ToUtf32(s, n);
	return ToUnicode(s, n, cs);
}

void Draw::DrawText(i32 x, i32 y, i32 angle, wtukk text, Font font,
		            Color ink, i32 n, i32k *dx)
{
	if(IsNull(ink)) return;
	ink = ResolveInk(ink);
	if(n < 0)
		n = strlen__(text);
	Std(font);
	double sina = 0;
	double cosa = 1;
	i32    posx = 0;
	if(angle)
		Draw::SinCos(angle, sina, cosa);
	Font font0 = font;
	if(GetInfo() & DRAWTEXTLINES)
		font.Underline(false).Strikeout(false);
	for(i32 i = 0; i < n; i++) {
		wchar chr = text[i];
		GlyphInfo gi = GetGlyphInfo(font, chr);
		if(gi.IsNormal())
			if(angle || chr >= 0x10000)
				DrawTextOp(i32(x + cosa * posx), i32(y - sina * posx), angle, &chr, font, ink, 1, NULL);
			else {
				i32 c = 1;
				i32 dd = 0;
				while(i + c < n && c < 1000) {
					GlyphInfo gi2 = GetGlyphInfo(font, text[i + c]);
					if(!gi2.IsNormal())
						break;
					dd += dx ? dx[c - 1] : gi.width;
					c++;
					gi = gi2;
				}
				DrawTextOp(x + posx, y, 0, text + i, font, ink, c, dx);
				posx += dd;
				i += c - 1;
				if(dx)
					dx += c - 1;
			}
		else
		if(gi.IsReplaced()) {
			Font fnt = font;
			fnt.Face(gi.lspc);
			fnt.Height(gi.rspc);
			if(angle)
				DrawTextOp(i32(x + cosa * posx), i32(y - sina * (font.GetAscent() - fnt.GetAscent() + posx)),
				             angle, &chr, fnt, ink, 1, NULL);
			else
				DrawTextOp(x + posx, y + font.GetAscent() - fnt.GetAscent(), 0, &chr, fnt, ink, 1, NULL);
			GlyphMetrics(gi, font, chr);
		}
		else
		if(gi.IsComposed()) {
			ComposedGlyph cg;
			Compose(font, chr, cg);
			if(angle) {
				DrawTextOp(i32(x + cosa * posx), i32(y - sina * posx), angle, &cg.basic_char, font, ink, 1, NULL);
				DrawTextOp(i32(x + cosa * (posx + cg.mark_pos.x)), i32(y - sina * (cg.mark_pos.y + posx)), angle, &cg.mark_char, cg.mark_font, ink, 1, NULL);
			}
			else {
				DrawTextOp(x + posx, y, 0, &cg.basic_char, font, ink, 1, NULL);
				DrawTextOp(x + cg.mark_pos.x + posx, y + cg.mark_pos.y, 0, &cg.mark_char, cg.mark_font, ink, 1, NULL);
			}
			GlyphMetrics(gi, font, chr);
		}
		else {
			Font fnt = Arial(font.GetHeight());
			wchar chr = 0x25a1;
			gi = GetGlyphInfo(fnt, chr);
			if(!gi.IsNormal()) {
				chr = ' ';
				gi = GetGlyphInfo(fnt, chr);
			}
			if(angle)
				DrawTextOp(i32(x + cosa * posx), i32(y - sina * posx), angle, &chr, fnt, ink, 1, NULL);
			else
				DrawTextOp(x + posx, y, 0, &chr, fnt, ink, 1, NULL);
		}
		posx += dx ? *dx++ : gi.width;
	}

	if((GetInfo() & DRAWTEXTLINES) && (font0.IsUnderline() || font0.IsStrikeout())) {
		i32 hg = abs(font0.GetCy());
		if(hg == 0) hg = 10;
		i32 thickness = max(font.IsBold() ? hg / 10 : hg / 20, 1);

		i32 ascent = font0.GetAscent();
		Size offset = Point(0, ascent);
		if(angle) {
			offset.cx = fround(ascent * sina);
			offset.cy = fround(ascent * cosa);
		}

		x += offset.cx;
		y += offset.cy;
		i32 p;
		auto DoLine = [&] {
			if(angle == 0)
				DrawRect(x, y + p, posx, thickness, ink);
			else
				DrawLine(
					i32(x + p * sina),
					i32(y + p * cosa),
					i32(x + posx * cosa + p * sina),
					i32(y - posx * sina + p * cosa),
					thickness,
					ink
				);
		};
		if(font0.IsUnderline()) {
			i32 descent = font0.GetDescent();
			p = min(descent - thickness, max(descent > 0 ? descent / 2 : hg / 15, i32(descent > 0)));
			DoLine();
		}
		if(font0.IsStrikeout()) {
			p = -ascent / 3;
			DoLine();
		}
	}
}

// ----------------------------

void Draw::DrawText(i32 x, i32 y, wtukk text, Font font,
                    Color ink, i32 n, i32k *dx)
{
	DrawText(x, y, 0, text, font, ink, n, dx);
}

// ---------------------------

void Draw::DrawText(i32 x, i32 y, i32 angle, const WTxt& text, Font font,
                    Color ink, i32k *dx)
{
	DrawText(x, y, angle, ~text, font, ink, text.GetLength(), dx);
}

void Draw::DrawText(i32 x, i32 y, const WTxt& text, Font font, Color ink, i32k *dx)
{
	DrawText(x, y, 0, text, font, ink, dx);
}

// ---------------------------

void Draw::DrawText(i32 x, i32 y, i32 angle, tukk text, u8 charset, Font font,
                    Color ink, i32 n, i32k *dx)
{
	DrawText(x, y, angle, TextUnicode(text, n, charset, font), font, ink, dx);
}

void Draw::DrawText(i32 x, i32 y, tukk text, u8 charset, Font font,
                    Color ink, i32 n, i32k *dx)
{
	DrawText(x, y, 0, text, charset, font, ink, n, dx);
}

// ---------------------------

void Draw::DrawText(i32 x, i32 y, i32 angle, tukk text,
                    Font font, Color ink, i32 n, i32k *dx)
{
	DrawText(x, y, angle, text, CHARSET_DEFAULT, font, ink, n, dx);
}

void Draw::DrawText(i32 x, i32 y, tukk text, Font font,
                    Color ink, i32 n, i32k *dx)
{
	DrawText(x, y, text, CHARSET_DEFAULT, font, ink, n, dx);
}

// ---------------------------

void Draw::DrawText(i32 x, i32 y, i32 angle, const Txt& text, Font font,
                    Color ink, i32k *dx)
{
	DrawText(x, y, angle, text, font, ink, text.GetLength(), dx);
}

void Draw::DrawText(i32 x, i32 y, const Txt& text, Font font, Color ink, i32k *dx)
{
	WTxt h = TextUnicode(text, text.GetLength(), CHARSET_DEFAULT, font);
	DrawText(x, y, h, font, ink, h.GetLength(), dx);
}

// --------------------------

Size GetTextSize(wtukk text, Font font, i32 n)
{
	FontInfo fi = font.Info();
	if(n < 0)
		n = strlen__(text);
	Size sz;
	sz.cx = 0;
	wtukk wtext = (wtukk )text;
	while(n > 0) {
		if(*wtext >= ' ')
			sz.cx += fi[*wtext];
		n--;
		wtext++;
	}
	sz.cy = fi.GetHeight();
	return sz;
}

Size GetTextSize(const WTxt& text, Font font)
{
	return GetTextSize(text, font, text.GetLength());
}

Size GetTextSize(tukk text, u8 charset, Font font, i32 n)
{
	return GetTextSize(TextUnicode(text, n, charset, font), font);
}

Size GetTextSize(tukk text, Font font, i32 n)
{
	return GetTextSize(text, CHARSET_DEFAULT, font, n);
}

Size GetTextSize(const Txt& text, Font font)
{
	return GetTextSize(text, font, text.GetLength());
}

}