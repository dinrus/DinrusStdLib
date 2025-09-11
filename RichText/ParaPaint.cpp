#include "RichText.h"

namespace drx {

#define IMAGECLASS RichTextImg
#define IMAGEFILE <drx/RichText/RichText.iml>
#include <drx/Draw/iml_source.h>

Color PaintInfo::ResolveInk(Color ink) const
{
	return Nvl(textcolor, darktheme ? Nvl(DarkThemeCached(ink), SColorText()) : Nvl(ink, SColorText()));
}

Color PaintInfo::ResolvePaper(Color paper) const
{
	return IsNull(paper) ? paper : darktheme ? DarkThemeCached(paper) : paper;
}

RichPara::Lines RichPara::Begin(RichContext& rc) const
{
	Lines pl = FormatLines(rc.page.Width());
	rc.py.y += format.before + format.ruler;
	pl.Justify(format);
	return pl;
}

void RichPara::Flush(Draw& draw, const PaintInfo& pi, wtuk text,
                     const CharFormat **i0,
                     i32 *wd, i32 pos, i32 len, i32 x0, i32 x, i32 y0,
                     i32 y, i32 linecy, i32 lineascent, Zoom z,
                     bool highlight)
{
	if(*i0) {
		const CharFormat& f = **i0;
		i32 width = z * x - z * x0;
		i32 zy0 = z * y0;
		i32 zx0 = z * x0;
		if(!IsNull(f.indexentry) && !IsNull(pi.indexentry)) {
			if(pi.indexentrybg)
				draw.DrawRect(zx0, z * y, width, z * (y + linecy) - z * y,
				              pi.ResolvePaper(pi.indexentry));
			else
				draw.DrawRect(zx0, zy0, width, 2, pi.indexentry);
		}
		if(!IsNull(f.paper) && !highlight && IsNull(pi.textcolor))
			draw.DrawRect(zx0, z * y, width, z * (y + linecy) - z * y, pi.ResolvePaper(f.paper));
		Font fnt = f;
		i32 zht = z * tabs(f.GetHeight());
		i32 ssa = 0;
		i32 ssd = 0;
		if(f.sscript) {
			FontInfo fi = fnt(zht).Info();
			ssa = fi.GetAscent();
			ssd = fi.GetDescent();
			zht = 3 * zht / 5;
		}
		fnt.Height(zht ? zht : 1);
		FontInfo fi = fnt.Info();
		Color ink = pi.ResolveInk(f.ink);
		if(f.dashed) {
			i32 dx = max(fi.GetAscent() / 5, 2);
			for(i32 i = 0; dx * i < width; i++)
				draw.DrawRect(zx0 + i * dx, zy0 + fi.GetDescent() / 2,
				              dx / 2, max(fi.GetDescent() / 3, 1), ink);
		}
		if(!IsNull(f.link) && !IsNull(pi.hyperlink) && !(fnt.IsUnderline() || f.dashed)) {
			fnt.Underline();
			if(!pi.darktheme && IsNull(pi.textcolor))
				ink = pi.hyperlink;
		}
		x = zx0;
		Vec< Tuple2<i32, i32> > bak;
		if(f.link.GetCount())
			draw.Escape("url:" + f.link);
		for(i32 i = 0; i < len; i++) {
			i32 w = wd[pos + i];
			Image img;
			wchar& c = text[pos + i];
			if(c == ' ')
				img = RichTextImg::SpaceChar();
			if(c == 160)
				img = RichTextImg::HardSpaceChar();
			if(c >= 9 && c < 9 + 4) {
				if(c > 9) {
					static char fct[] = { ' ', '.', '-', '_' };
					i32 fc = fct[c - 9];
					i32 pw = fi[fc];
					i32 px = (x + pw - 1) / pw * pw;
					while(px + pw < x + w) {
						draw.DrawText(px, zy0 - fi.GetAscent(), Txt(fc, 1), fnt, Black);
						px += pw;
					}
				}
				img = RichTextImg::TabChar();
				bak.Add(MakeTuple(pos + i, c));
				c = ' ';
			}
			if(img && !IsNull(pi.showcodes)) {
				Size sz = img.GetSize();
				if(sz.cy < z * lineascent)
					draw.DrawImage(x + (w - sz.cx) / 2, zy0 - sz.cy, img, pi.showcodes);
			}
			x += w;
		}
		draw.DrawText(zx0,
		              f.sscript == 1 ? zy0 - ssa :
		              f.sscript == 2 ? zy0 + ssd - fi.GetHeight()
		                             : zy0 - fi.GetAscent(),
		              text + pos, fnt, ink, len, wd + pos);
		for(i32 i = 0; i < bak.GetCount(); i++)
			text[bak[i].a] = bak[i].b;
		if(f.link.GetCount())
			draw.Escape("url:");
	}
}

bool RichPara::BreaksPage(const RichContext& rc, const Lines& pl, i32 i) const
{
	i32 linecy = pl[i].Sum();
	if(linecy >= rc.page.Height()) return false;
	if(linecy + rc.py.y > rc.page.bottom)
		return true;
	if(format.orphan || pl.GetCount() < 2) return false;
	if((i == 0 || i == pl.GetCount() - 2) && rc.py.y + linecy + pl[i + 1].Sum() > rc.page.bottom)
		return true;
	return false;
}

struct RichObjectImageMaker : ImageMaker {
	RichObj object;
	Size       sz;
	void       *context;

	virtual Txt Key() const;
	virtual Image  Make() const;
};

Txt RichObjectImageMaker::Key() const
{
	TxtBuf b;
	RawCat(b, object.GetSerialId());
	RawCat(b, sz);
	RawCat(b, context);
	return Txt(b);
}

Image RichObjectImageMaker::Make() const
{
	return object.ToImage(sz, context);
}

void RichPara::DrawRuler(Draw& w, i32 x, i32 y, i32 cx, i32 cy, Color ink, i32 style)
{
	i32 segment = cy;
	i32 r = x + cx;
	switch(style) {
	case RULER_DASH:
		segment = 2 * cy;
	case RULER_DOT:
		while(x < r) {
			w.DrawRect(x, y, min(r - x, segment), cy, ink);
			x += 2 * segment;
		}
		break;
	default:
		w.DrawRect(x, y, cx, cy, ink);
		break;
	}
}

void RichPara::Paint(PageDraw& pw, RichContext rc, const PaintInfo& pi,
                     const Number& n, const Bits& spellerror, bool baselevel) const
{
	Zoom z = pi.zoom;
	PageY opy = rc.py;
	Lines pl = Begin(rc);
	if(pw.tracer) {
		PageY h = rc.py;
		h.y -= format.before + format.ruler;
		pw.tracer->Paragraph(rc.page, h, *this);
	}

	bool highlight = pi.highlightpara >= 0 && pi.highlightpara < pl.len;
	i32 hy = rc.py.y - format.before - format.ruler;
	i32 phy = rc.py.page;
	if(format.ruler && hy >= 0 && hy + format.ruler < rc.page.bottom)
		DrawRuler(pw.Page(phy), z * rc.page.left + z * format.lm, z * hy,
		                        z * rc.page.right - z * rc.page.left - z * format.rm - z * format.lm,
			                    max(1, z * format.ruler), pi.ResolveInk(format.rulerink), format.rulerstyle);
	if(pi.sell < 0 && pi.selh > 0)
		for(i32 p = opy.page; p <= rc.py.page; p++) {
			i32 top = z * (p == opy.page ? opy.y : rc.page.top);
			i32 bottom = z * (p == rc.py.page ? rc.py.y : rc.page.bottom);
			pi.DrawSelection(pw.Page(p), z * rc.page.left, top, z * rc.page.right - z * rc.page.left, bottom - top);
		}
	opy = rc.py;
	i32 x = 0;
	i32 y0 = 0;
	i32 lineascent = 0;
	PageY mpy;
	i32   mla = Null;
	for(i32 lni = 0; lni < pl.GetCount(); lni++) {
		const Line& li = pl[lni];
		i32 linecy = li.Sum();
		lineascent = li.ascent;
		if(BreaksPage(rc, pl, lni)) {
			if(li.ppos > pi.sell && li.ppos < pi.selh) {
				i32 y = z * rc.py.y;
				pi.DrawSelection(pw.Page(rc.py.page), z * rc.page.left, y, z * rc.page.right - z * rc.page.left,
				                                      z * rc.page.bottom - y);
			}
			rc.Page();
		}
		if(rc.py > pi.bottom)
			break;

		if(lni == 0) {
			mpy = rc.py;
			mla = lineascent;
		}
		const CharFormat **cf = pl.format + li.pos;
		const CharFormat **i = cf;
		const CharFormat **ilim = i + li.len;
		const HeightInfo *hg = pl.height + li.pos;
		if(rc.py + linecy >= pi.top) {
			Draw& draw = pw.Page(rc.py.page);
#ifdef _DEBUG
			i32 cloff = draw.GetCloffLevel();
#endif
			Buffer<i32> wd(li.len);
			i32 x0 = li.xpos + rc.page.left;
			x = x0;
			i32 *w = pl.width + li.pos;
			i32 *wl = w + li.len;
			i32 *t = wd;
			while(w < wl) {
				i32 x1 = x + *w++;
				*t++ = z * x1 - z * x;
				x = x1;
			}

			if(highlight)
				draw.DrawRect(z * rc.page.left, z * rc.py.y, z * rc.page.Width(),
					          z * (rc.py.y + linecy) - z * rc.py.y,
					          pi.highlight);

			const CharFormat **i0 = i;
			wtuk text = pl.text + li.pos;
			x = x0;
			w = pl.width + li.pos;
			y0 = rc.py.y + li.ascent;
			i32 pp = li.pos;
			i32 l = rc.page.right;
			i32 h = -1;

			while(i < ilim) {
				if(pl.pos[pp] == pi.sell && x < l)
					l = x;
				if(pl.pos[pp] == pi.selh && h < 0)
					h = x;
				pp++;
				if(*i0 != *i || hg->object) {
					Flush(draw, pi, text, i0, wd, (i32)(i0 - cf), (i32)(i - i0), x0, x, y0, rc.py.y, linecy,
					      lineascent, z, highlight);
					i0 = i;
					x0 = x;
				}
				if(hg->object) {
					const RichObj& o = *hg->object;
					if(o) {
						Size sz = z * o.GetSize();
						i32 ix = z * x;
						if(pi.shrink_oversized_objects && sz.cx + ix > rc.page.right)
							sz.cx = rc.page.right - ix;
						draw.DrawRect(ix, z * rc.py.y, sz.cx, z * linecy, (*i)->paper);
						draw.Clipoff(ix, z * (y0 - hg->ascent), sz.cx, sz.cy);
						if(pi.sizetracking)
							draw.DrawRect(sz, SColorFace);
						else
							if(pi.usecache) {
								RichObjectImageMaker im;
								im.object = o;
								im.sz = sz;
								im.context = pi.context;
								draw.DrawImage(0, 0, MakeImagePaintOnly(im));
							}
							else
								o.Paint(draw, sz, pi.context);
						draw.End();
					}
					i++;
					hg++;
					x += *w++;
					i0 = i;
					x0 = x;
				}
				else {
					i++;
					hg++;
					x += *w++;
				}
			}
			if(i > i0)
				Flush(draw, pi, text, i0, wd, (i32)(i0 - cf), (i32)(i - i0), x0, x, y0, rc.py.y, linecy,
				      lineascent, z, highlight);
			if(lni == 0) {
				Rect r;
				r.left = rc.page.left + format.lm;
				i32 q = li.ascent / 2;
				r.top = rc.py.y + 4 * (li.ascent - q) / 5;
				r.right = r.left + q;
				r.bottom = r.top + q;
				q = z * (r.Width() / 4);
				r.left = z * r.left;
				r.top = z * r.top;
				r.right = z * r.right;
				r.bottom = z * r.bottom;
				Rect r1 = r;
				r1.Deflate(max(1, q));
				Color bullet_ink = format.ink;
				if(format.bullet) {
					if(part.GetCount())
						bullet_ink = part[0].format.ink;
				}
				switch(format.bullet) {
				case BULLET_BOX:
					draw.DrawRect(r, pi.ResolveInk(bullet_ink));
					break;
				case BULLET_BOXWHITE:
					draw.DrawRect(r, pi.ResolveInk(bullet_ink));
					draw.DrawRect(r1, pi.ResolvePaper(White()));
					break;
				case BULLET_ROUNDWHITE:
					draw.DrawEllipse(r, pi.ResolveInk(bullet_ink));
					draw.DrawEllipse(r1, pi.ResolvePaper(White()));
					break;
				case BULLET_ROUND:
					draw.DrawEllipse(r, pi.ResolveInk(bullet_ink));
					break;
				default:
					Txt s = n.AsText(format);
					if(!IsNull(s)) {
						CharFormat cf = li.len && *pl.format ? **pl.format : format;
						cf.Height(z * cf.GetHeight());
						draw.DrawText(r.left,
						              z * y0 - cf.Info().GetAscent(),
						              s, cf, pi.ResolveInk(cf.ink));
					}
				}
			}
			i32 zlcy = z * linecy;
			if(pi.spellingchecker && zlcy > 3) {
				i32 x = z * (li.xpos + rc.page.left);
				w = wd;
				wl = w + li.len;
				i32 i = li.pos;
				i32 q = z * (rc.py.y + linecy);
				while(w < wl) {
					if(spellerror[pl.pos[i++]]) {
						if(zlcy > 16)
							draw.DrawRect(x, q - 3, *w, 1, Red);
						draw.DrawRect(x, q - 2, *w, 1, LtRed);
					}
					x += *w++;
				}
			}

			if(pi.selh == li.ppos + li.plen)
				h = x;
			if(pi.sell == li.ppos + li.plen)
				l = x;
			if(pi.sell < li.ppos)
				l = rc.page.left;
			if(pi.selh > li.ppos + li.plen)
				h = rc.page.right;
			if(pi.sell < pi.selh && pi.selh > li.ppos)
				pi.DrawSelection(draw, z * l, z * rc.py.y, z * h - z * l, z * (rc.py.y + linecy) - z * rc.py.y);
			ASSERT(draw.GetCloffLevel() == cloff);
		}
		else
			while(i < ilim) {
				i++;
				hg++;
			}
		rc.py.y += linecy;
	}
	Size sz = RichTextImg::EndParaChar().GetSize();
	if(sz.cy < z * lineascent && !IsNull(pi.showcodes))
		pw.Page(rc.py.page).DrawImage(z * x, z * y0 - sz.cy,
		                              RichTextImg::EndParaChar(),
		                              format.indexentry.GetCount() ? pi.indexentry : pi.showcodes);
	if(format.newpage && !IsNull(pi.showcodes)) {
		Draw& w = pw.Page(opy.page);
		i32 wd = z * rc.page.right - z * rc.page.left;
		i32 step = w.Pixels() ? 8 : 50;
		i32 y = z * opy.y;
		for(i32 x = 0; x < wd; x += step)
			w.DrawRect(z * rc.page.left + x, y, step >> 1, step >> 3, pi.showcodes);
	}
	if(pl.len >= pi.sell && pl.len < pi.selh && rc.py < pi.bottom) {
		i32 top = z * rc.py.y;
		pi.DrawSelection(pw.Page(rc.py.page), z * rc.page.left, top, z * rc.page.right - z * rc.page.left,
		                                      z * min(rc.py.y + format.after, rc.page.bottom) - top);
	}
	if(!IsNull(mla) && !IsNull(pi.showcodes) && pi.showlabels) {
		bool b = format.label.GetCount();
		Image img = RichTextImg::Label();
		for(i32 pass = 0;;pass++) {
			Size isz = img.GetSize();
			if(b)
				pw.Page(mpy.page).DrawImage(-7 - isz.cx, z * mpy.y + (z * mla - isz.cy) / 2,
				                            img, pi.showcodes);
			if(pass)
				break;
			b = format.newhdrftr && baselevel;
			img = RichTextImg::HdrFtr();
		}
	}
	if(pw.tracer)
		pw.tracer->EndParagraph(rc.py);
}

void RichPara::GetRichPos(RichPos& rp, i32 pos) const
{
	rp.format = format;
	i32 i = FindPart(pos);
	if(i < GetCount()) {
		const Part& p = part[i];
		(CharFormat&)rp.format = p.format;
		if(p.object) {
			rp.chr = -1;
			rp.object = p.object;
		}
		else
		if(p.IsText())
			rp.chr = p.text[pos];
		else {
			rp.chr = -2;
			rp.field = p.field;
			rp.fieldparam = p.fieldparam;
			if(p.fieldpart.GetCount() > 0)
				rp.fieldformat = p.fieldpart[0].format;
		}
	}
	else
		rp.chr = '\n';
}

RichCaret RichPara::GetCaret(i32 pos, RichContext rc) const
{
	Lines pl = Begin(rc);
	RichCaret pr;
	FontInfo fi = format.Info();
	pr.caretascent = fi.GetAscent();
	pr.caretdescent = fi.GetDescent();
	for(i32 lni = 0; lni < pl.GetCount(); lni++) {
		Line& li = pl[lni];
		i32 linecy = li.Sum();
		if(BreaksPage(rc, pl, lni))
			rc.Page();
		pr.page = rc.py.page;
		pr.top = rc.py.y;
		pr.bottom = rc.py.y + linecy;
		pr.lineascent = li.ascent;
		pr.line = lni;
		if(pos < li.ppos + li.plen) {
			i32 *w = pl.width + li.pos;
			i32 *p = pl.pos + li.pos;
			const CharFormat **i = pl.format + li.pos;
			const HeightInfo *h = pl.height + li.pos;
			i32 x = li.xpos + rc.page.left;
			if(li.len && *i) {
				pr.caretascent = h->ascent;
				pr.caretdescent = h->descent;
			}
			while(pos > *p) {
				x += *w++;
				if(*i) {
					pr.caretascent = h->ascent;
					pr.caretdescent = h->descent;
				}
				h++;
				i++;
				p++;
			}
			pr.objectcy = h->ascent + h->ydelta;
			pr.objectyd = h->ydelta;
			pr.left = x;
			pr.right = x + *w;
			pr.caretdescent = min(pr.caretdescent, pr.Height() - pr.lineascent);
			return pr;
		}
		rc.py.y += linecy;
	}
	const Line& li = pl.line.Top();
	pr.left = li.cx + li.xpos + rc.page.left;
	pr.right = rc.page.right;
	pr.caretdescent = min(pr.caretdescent, pr.Height() - pr.lineascent);
	return pr;
}

i32 RichPara::PosInLine(i32 x, const Rect& page, const Lines& pl, i32 lni) const
{
	const Line& li = pl[lni];
	i32k *w = pl.width + li.pos;
	i32k *wl = w + li.len;
	if(lni < pl.GetCount() - 1 && li.len > 0 && pl.text[li.pos + li.len - 1] == ' ')
		wl--;
	i32 xp = li.xpos + page.left;
	while(w < wl && xp + *w <= x)
		xp += *w++;
	i32 pos = (i32)(w - pl.width);
	return pos < pl.clen ? pl.pos[pos] : pl.len;
}

i32 RichPara::GetPos(i32 x, PageY y, RichContext rc) const
{
	Lines pl = Begin(rc);
	if(pl.len)
		for(i32 lni = 0; lni < pl.GetCount(); lni++) {
			const Line& li = pl[lni];
			i32 linecy = li.Sum();
			if(BreaksPage(rc, pl, lni))
				rc.Page();
			rc.py.y += linecy;
			if(y < rc.py || lni == pl.GetCount() - 1)
				return PosInLine(x, rc.page, pl, lni);
		}
	return pl.len;
}

i32 RichPara::GetVertMove(i32 pos, i32 gx, const Rect& page, i32 dir) const
{
	Lines pl = FormatLines(page.Width());
	i32 lni;
	if(pos >= 0) {
		for(lni = 0; lni < pl.GetCount() - 1; lni++) {
			const Line& li = pl[lni];
			if(pos < li.ppos + li.plen)
				break;
		}
		lni += sgn(dir);
		if(lni < 0 || lni >= pl.GetCount())
			return -1;
	}
	else
		lni = dir < 0 ? pl.GetCount() - 1 : 0;
	return PosInLine(gx, page, pl, lni);
}

void  RichPara::GatherLabels(Vec<RichValPos>& info, RichContext rc, i32 pos) const
{
	Lines pl = Begin(rc);
	WTxt ie;
	if(!pl.GetCount())
		return;
	if(BreaksPage(rc, pl, 0))
		rc.Page();
	if(format.label.IsEmpty())
		return;
	RichValPos& f = info.Add();
	f.py = rc.py;
	f.pos = pos;
	f.data = format.label.ToWTxt();
}

void  RichPara::GatherIndexes(Vec<RichValPos>& info, RichContext rc, i32 pos) const
{
	Lines pl = Begin(rc);
	WTxt ie;
	for(i32 lni = 0; lni < pl.GetCount(); lni++) {
		Line& li = pl[lni];
		i32 linecy = li.Sum();
		if(BreaksPage(rc, pl, lni))
			rc.Page();
		const CharFormat **i0 = pl.format + li.pos;
		const CharFormat **i = i0;
		const CharFormat **ilim = i + li.len;
		while(i < ilim) {
			if(*i && (*i)->indexentry != ie) {
				ie = (*i)->indexentry;
				if(!ie.IsEmpty()) {
					RichValPos& f = info.Add();
					f.py = rc.py;
					f.pos = (i32)(i - i0) + pos;
					f.data = ie;
				}
			}
			i++;
		}
		rc.py.y += linecy;
	}
}

void FontHeightRound(Font& fnt, Zoom z)
{
	fnt.Height(max((fnt.GetHeight() * z.m + (z.d >> 1)) / z.d, 1)); // do not allow to fall to zero (that is default font height)
}

void operator*=(RichPara::Format& format, Zoom z)
{
	FontHeightRound(format, z);
	format.before *= z;
	if(format.ruler) {
		format.ruler *= z;
		if(format.ruler == 0)
			format.ruler = 1;
	}
	i32 ll = format.lm + format.indent;
	format.lm *= z;
	format.indent = z * ll - format.lm;
	format.rm *= z;
	format.after *= z;
	format.tabsize *= z;
	for(i32 i = 0; i < format.tab.GetCount(); i++) {
		i32& pos = format.tab[i].pos;
		pos = ((pos & ~RichPara::TAB_RIGHTPOS) * z) | (pos & RichPara::TAB_RIGHTPOS);
	}
}

void  RichPara::ApplyZoom(Zoom z)
{
	format *= z;
	for(i32 i = 0; i < part.GetCount(); i++)
		if(part[i].IsText())
			FontHeightRound(part[i].format, z);
		else
		if(part[i].object)
			part[i].object.SetSize(z * part[i].object.GetSize());
}

}