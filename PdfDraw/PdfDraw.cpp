#include "PdfDraw.h"

namespace drx {

#include "ICCColorSpace.i"

#define LDUMP(x) // DUMP(x)
#define LLOG(x)  // DLOG(x)

#define PDF_COMPRESS

dword PdfDraw::GetInfo() const
{
	return DOTS|DRAWTEXTLINES;
}

Size PdfDraw::GetPageSize() const
{
	return pgsz;
}

void PdfDraw::Init(i32 pagecx, i32 pagecy, i32 _margin, bool _pdfa)
{
	Clear();
	margin = _margin;
	pdfa = _pdfa;
	pgsz.cx = pagecx;
	pgsz.cy = pagecy;
	pgsz += margin;
	current_offset = Point(0, 0);
	StartPage();
}

void  PdfDraw::Clear()
{
	out.Clear();
	page.Clear();
	offset.Clear();
	out << "%PDF-1.7\n";
	out << "%\xf1\xf2\xf3\xf4\n\n";
	empty = true;
}

i32 PdfDraw::BeginObj()
{
	offset.Add(out.GetLength());
	out << offset.GetCount() << " 0 obj\n";
	return offset.GetCount();
}

void PdfDraw::EndObj()
{
	out << "endobj\n\n";
}

void PdfDraw::PutRect(const Rect& rc)
{
	page << Pt(rc.left) << ' ' << Pt(pgsz.cy - rc.bottom) << ' '
		 << Pt(rc.Width()) << ' ' << Pt(rc.Height()) << " re\n";
}

i32 PdfDraw::PutStream(const Txt& data, const Txt& keys, bool compress)
{
#ifdef PDF_COMPRESS
	if(compress) {
		Txt c = ZCompress(data);
		if(c.GetLength() < data.GetLength()) {
			BeginObj();
			out << "<< " << keys
			    << "/Length " << c.GetLength() << " "
			    << "/Length1 " << data.GetLength() << " "
			    << "/Filter /FlateDecode "
			    << " >>\n"
			    << "stream\r\n" << c << "\r\nendstream\n";
			EndObj();
			return offset.GetCount();
		}
	}
#endif
	BeginObj();
	out << "<< " << keys << " /Length " << data.GetLength() <<
	       " /Length1 "<< data.GetLength() << " >>\n"
	    << "stream\r\n" << data << "\r\nendstream\n";
	EndObj();
	return offset.GetCount();
}

void PdfDraw::PutrgColor(Color rg, zu64 pattern)
{
	if(IsNull(rgcolor) || rg != rgcolor || pattern != patternid) {
		if(!pattern)
			page << PdfColor(rg) << " rg\n";
		else {
			i32 f = patterns.FindAdd(pattern);
			if(!patternid)
				page << "/Cspat cs\n";
			page << PdfColor(rg) << " /Pat" << (f + 1) << " scn\n";
		}

	}
	rgcolor = rg;
	patternid = pattern;
}

void PdfDraw::PutRGColor(Color RG)
{
	if(IsNull(RGcolor) || RG != RGcolor)
		page << PdfColor(RGcolor = RG) << " RG\n";
}

void PdfDraw::PutLineWidth(i32 lw)
{
	lw = max(Nvl(lw, 0), 1);
	if(linewidth != lw)
		page << Pt(linewidth = lw) << " w\n";
}

void PdfDraw::StartPage()
{
	rgcolor = RGcolor = Null;
	fontid = -1;
	patternid = 0;
	textht = Null;
	linewidth = -1;
	if(margin)
		OffsetOp(Point(margin, margin));
}

void PdfDraw::EndPage()
{
	if(margin)
		EndOp();
	if(page.GetCount())
		empty = false;
	PutStream(page);
	page.Clear();
}

void PdfDraw::PushOffset()
{
	offset_stack.Add(current_offset);
}

void PdfDraw::PopOffset()
{
	ASSERT(offset_stack.GetCount());
	if(offset_stack.GetCount()) // be defensive
		current_offset = offset_stack.Pop();
}

void PdfDraw::BeginOp()
{
	PushOffset();
	page << "q\n";
}

void PdfDraw::EndOp()
{
	fontid = -1;
	patternid = 0;
	textht = Null;
	rgcolor = RGcolor = Null;
	linewidth = -1;
	page << "Q\n";
	PopOffset();
}

void PdfDraw::OffsetOp(Point p)
{
	page << "q ";
	if(p.x || p.y)
		page << "1 0 0 1 " << Pt(p.x) << ' ' << Pt(-p.y) << " cm\n";
	PushOffset();
	current_offset += p;
}

bool PdfDraw::ClipOp(const Rect& r)
{
	page << "q ";
	PutRect(r);
	page << "W* n\n";
	PushOffset();
	return true;
}

bool PdfDraw::ClipoffOp(const Rect& r)
{
	page << "q ";
	PutRect(r);
	page << "W* n\n";
	if(r.left || r.top)
		page << "1 0 0 1 " << Pt(r.left) << ' ' << Pt(-r.top) << " cm\n";
	PushOffset();
	current_offset += r.TopLeft();
	return true;
}

bool PdfDraw::ExcludeClipOp(const Rect& r)
{
	return true; // СДЕЛАТЬ
/*	
	if(r.left <= actual_clip.left && r.right >= actual_clip.right) {
		if(r.top <= actual_clip.top) actual_clip.top = max(actual_clip.top, r.bottom);
		if(r.bottom >= actual_clip.bottom) actual_clip.bottom = min(actual_clip.bottom, r.top);
	}
	if(r.top <= actual_clip.top && r.bottom >= actual_clip.bottom) {
		if(r.left <= actual_clip.left) actual_clip.left = max(actual_clip.left, r.right);
		if(r.right >= actual_clip.right) actual_clip.right = min(actual_clip.right, r.left);
	}

	PutRect(actual_clip);
	PutRect(r & actual_clip);
	page << "W* n\n";
	return !actual_clip.IsEmpty();
*/
}

bool PdfDraw::IntersectClipOp(const Rect& r)
{
	PutRect(r);
	page << "W* n\n";
	return true;
}

bool PdfDraw::IsPaintingOp(const Rect&) const
{
	return true;
}

PdfDraw::CharPos PdfDraw::GetCharPos(Font fnt, wchar chr)
{
	fnt.Underline(false);
	VecMap<wchar, CharPos>& fc = fontchars.GetAdd(fnt);
	i32 q = fc.Find(chr);
	if(q >= 0)
		return fc[q];
	CharPos& p = fc.Add(chr);
	q = pdffont.FindLast(fnt);
	if(q < 0 || pdffont[q].GetCount() > 240) {
		p.fi = pdffont.GetCount();
		p.ci = 1; // PDF does not seem to like 0 character in text in some versions
		Vec<wchar>& x = pdffont.Add(fnt);
		x.Add(32);
		x.Add(chr);
	}
	else {
		p.fi = q;
		p.ci = pdffont[q].GetCount();
		pdffont[q].Add(chr);
	}
	return p;
}

void  PdfDraw::FlushText(i32 dx, i32 fi, i32 height, const Txt& txt)
{
	if(fi < 0)
		return;
	if(dx)
		page << Pt(dx) << " 0 Td ";
	PutFontHeight(fi, height);
	page << "<" << txt << "> Tj\n";
}

Txt PdfDraw::PdfColor(Color c)
{
	return Format("%3nf %3nf %3nf", c.GetR() / 255.0, c.GetG() / 255.0, c.GetB() / 255.0);
}

Txt PdfDraw::PdfTxt(tukk s)
{
	TxtBuf b;
	b.Cat('(');
	while(*s) {
		if(findarg(*s, '(', ')', '\\') >= 0)
			b.Cat('\\');
		b.Cat(*s++);
	}
	b.Cat(')');
	return Txt(b);
}

void PdfDraw::PutFontHeight(i32 fi, double ht)
{
	if(fi != fontid || IsNull(textht) || ht != textht)
		page << "/F" << ((fontid = fi) + 1) << ' ' << Pt(textht = ht) << " Tf\n";
}


PdfDraw::OutlineInfo PdfDraw::GetOutlineInfo(Font fnt)
{
	fnt.Height(0);
	i32 q = outline_info.Find(fnt);
	if(q >= 0)
		return outline_info[q];
	OutlineInfo of;
	of.sitalic = of.standard_ttf = false;

	TTFReader ttf;
	if((fnt.GetFaceInfo() & Font::COLORIMG) == 0 && ttf.Open(fnt, false, true)) {
		of.standard_ttf = true;
		of.sitalic = fnt.IsItalic() && (ttf.head.macStyle & 2) == 0;
		of.sbold = fnt.IsBold() && (ttf.head.macStyle & 1) == 0;
		LLOG(fnt << ", sbold: " << of.sbold << ", sitalic: " << of.sitalic);
	}

	outline_info.Add(fnt, of);

	return of;
}

enum { FONTHEIGHT_TTF = -9999 };

Image RenderGlyph(i32 cx, i32 x, Font font, i32 chr, i32 py, i32 pcy, Color fg, Color bg);

PdfDraw::CGlyph PdfDraw::ColorGlyph(Font fnt, i32 chr)
{
	auto key = MakeTuple(fnt, chr);
	i32 q = color_glyph.Find(key);
	if(q >= 0)
		return color_glyph[q];

	CGlyph cg;
	cg.sz = { fnt[chr], fnt.GetCy() };
	cg.x = 0;
	i32 l = fnt.GetLeftSpace(chr);
	if(l < 0) {
		cg.x = -l;
		cg.sz.cx -= l;
	}
	i32 r = fnt.GetRightSpace(chr);
	if(r < 0)
		cg.sz.cx -= r;

	Image m[2];
	for(i32 i = 0; i < 2; i++)
		m[i] = RenderGlyph(cg.sz.cx, cg.x, fnt, chr, 0, cg.sz.cy, Blue(), i ? Black() : LtGreen());
	Image cm = RecreateAlpha(m[0], m[1]);
	cg.image = PdfImage(cm, cm.GetSize());
	color_glyph.Add(key, cg);
	return cg;
}

void PdfDraw::DrawTextOp(i32 x, i32 y, i32 angle, wtukk s, Font fnt,
		                 Color ink, i32 n, i32k *dx)
{
	LLOG("DrawTextOp " << x << ", " << y << " angle: " << angle << ", text: " << s << ", font " << fnt);
	if(!n) return;
	double sina = 0, cosa = 1;
	if(angle)
		Draw::SinCos(angle, sina, cosa);
	i32 posx = 0;
	bool straight = true;
	OutlineInfo of = GetOutlineInfo(fnt);
	Xform2D m;
	if(of.sitalic) {
		m = m.SheerX(0.165);
		straight = false;
	}
	if(angle) {
		straight = false;
		m = Xform2D::Rotation(-angle * M_PI / 1800.0) * m;
	}

	auto Fmt = [](double x) { return FormatF(x, 5); };

	if(fnt.GetFaceInfo() & Font::COLORIMG) {
		Pointf prev(0, 0);
		for(i32 i = 0; i < n; i++) {
			CGlyph cg = ColorGlyph(fnt, s[i]);
			page << "q ";
			if(straight)
				page << Ptf(cg.sz.cx) << " 0 0 " << Ptf(cg.sz.cy) << ' '
			         << Ptf(x + posx + cg.x) << ' ' << Ptf(pgsz.cy - y - cg.sz.cy);
			else {
				Xform2D mm = m * Xform2D::Scale(Pt(cg.sz.cx), Pt(cg.sz.cy));
				page << Fmt(mm.x.x) << ' ' << Fmt(mm.x.y) << ' ' << Fmt(mm.y.x) << ' ' << Fmt(mm.y.y)
				     << ' ' << Ptf(x + posx * cosa + cg.sz.cx * sina)
				     << ' ' << Ptf(pgsz.cy - (y - posx * sina) - cg.sz.cy * cosa);
			}
			page << " cm /Image" << cg.image + 1 << " Do Q\n";

			posx += dx ? dx[i] : fnt[s[i]];
		}
		if(url.GetCount()) { // For now, only 'zero angle' text can have links
			UrlInfo& u = page_url.At(offset.GetCount()).Add();
			u.rect = RectC(x, y, posx, fnt.GetCy()).Offseted(current_offset);
			u.url = url;
		}
	}
	else {
		i32 h = fnt.GetHeight();
		if(h == 0)
			fnt.Height(100);
		if(h < 0)
			fnt.Height(-h);
		Font ff = fnt;
		i32 fh = fnt.GetHeight();
		if(of.standard_ttf)
			fnt.Height(FONTHEIGHT_TTF);
		Txt txt;
		PutrgColor(ink);
		PutRGColor(ink);
		i32 nbld = 0;
		i32 sbld = 1;
		if(of.sbold) {
			nbld = abs(ff.GetHeight()) / 30;
			sbld = clamp(nbld, 1, 5);
		}
		for(i32 q = 0; q <= nbld; q += sbld) {
			page << "BT ";
			posx = q;
			i32 fi = -1;
			Pointf prev(0, 0);
			for(i32 i = 0; i < n; i++) {
				Pointf next(Pt(x + posx * cosa + ff.GetAscent() * sina),
				            Pt(pgsz.cy - (y - posx * sina) - ff.GetAscent() * cosa));
				CharPos fp = GetCharPos(fnt, s[i]);
				if(fi != fp.fi) {
					fi = fp.fi;
					PutFontHeight(fi, fh);
				}
				if(straight)
					page << Fmt(next.x - prev.x) << ' ' << Fmt(next.y - prev.y) << " Td";
				else
					page << Fmt(m.x.x) << ' ' << Fmt(m.x.y) << ' ' << Fmt(m.y.x) << ' ' << Fmt(m.y.y)
					     << ' ' << Fmt(next.x) << ' ' << Fmt(next.y) << " Tm";
				page << " <" << FormatIntHex(fp.ci, 2);
				page << "> Tj\n";
				posx += dx ? dx[i] : ff[s[i]];
				prev = next;
				if(q == 0 && url.GetCount()) { // For now, only 'zero angle' text can have links
					UrlInfo& u = page_url.At(offset.GetCount()).Add();
					u.rect = RectC(x, y, posx, ff.GetCy()).Offseted(current_offset);
					u.url = url;
				}
			}
			page << "ET\n";
		}
	}
}

void PdfDraw::Escape(const Txt& data)
{
	if(data.StartsWith("url:"))
		url = data.Mid(4);
	if(data.StartsWith("data:"))
		this->data = data.Mid(5);
}

PdfDraw::RGlyph PdfDraw::RasterGlyph(Font fnt, i32 chr)
{
	RGlyph rg;
	FontInfo fi = fnt.Info();
	rg.x = 0;
	rg.sz.cx = fi[chr];
	rg.sz.cy = fi.GetHeight();
	i32 l = fi.GetLeftSpace(chr);
	if(l < 0) {
		rg.x = -l;
		rg.sz.cx -= l;
	}
	i32 r = fi.GetRightSpace(chr);
	if(r < 0)
		rg.sz.cx -= r;
	if(false) {
		Image m[2];
		for(i32 i = 0; i < 2; i++)
			m[i] = RenderGlyph(rg.sz.cx, rg.x, fnt, chr, 0, rg.sz.cy, Blue(), i ? Black() : LtGreen());
		Image cm = RecreateAlpha(m[0], m[1]);
		rg.color_image = PdfImage(cm, cm.GetSize());
	}
	else {
		RasterFormat fmt;
		fmt.Set1mf();
		i32 linebytes = fmt.GetByteCount(rg.sz.cx);
		Buffer<u8> ob(linebytes);
		i32 y = 0;
		while(y < rg.sz.cy) {
			i32 ccy = min(16, rg.sz.cy - y);
			Image m = RenderGlyph(rg.sz.cx, rg.x, fnt, chr, y, ccy, Black(), LtGreen());
			for(i32 i = 0; i < m.GetHeight(); i++) {
				fmt.Write(ob, m[i], rg.sz.cx, NULL);
				rg.data.Cat((tukk )~ob, linebytes);
			}
			y += ccy;
		}
	}
	return rg;
}

void PdfDraw::DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color)
{
	if(IsNull(color) || cx <= 0 || cy <= 0) return;
	PutrgColor(color);
	PutRGColor(color);
	PutRect(RectC(x, y, cx, cy));
	page << "f\n";
}

void PdfDraw::DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color)
{
	if(!IsNull(width)) {
		PutRGColor(color);
		PutLineWidth(width);
		page << " 1 J "
		     << Pt(x1) << ' ' << Pt(pgsz.cy - y1) << " m "
		     << Pt(x2) << ' ' << Pt(pgsz.cy - y2) << " l S\n";
	}
}

static Image sJPEGDummy()
{
	static Image h = CreateImage(Size(1, 1), Cyan);
	return h;
}

void DrawJPEG(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Txt& jpeg_data)
{
	w.Escape("data:" + jpeg_data);
	w.DrawImage(x, y, cx, cy, sJPEGDummy());
}

i32 PdfDraw::PdfImage(const Image& img, const Rect& src)
{
	Tuple2<z64, Rect> key = MakeTuple(img.GetSerialId(), src);
	i32 q = images.Find(key);
	if(q < 0) {
		q = images.GetCount();
		images.Add(key, img);
	}
	return q;
}

void PdfDraw::DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& _img, const Rect& src, Color c)
{
	Image img = _img;
	if(!IsNull(c))
		img = CachedSetColorKeepAlpha(img, c);
	
	i32 q = PdfImage(img, src);
	
	if(img.GetSerialId() == sJPEGDummy().GetSerialId())
		jpeg.Add(data);
	
	page << "q "
	     << Pt(cx) << " 0 0 " << Pt(cy) << ' '
	     << Pt(x) << ' ' << Pt(pgsz.cy - y - cy)
	     << " cm /Image" << q + 1 << " Do Q\n";
}

void PdfDraw::DrawPolyPolylineOp(const Point *vertices, i32 vertex_count,
	                    i32k *counts, i32 count_count,
	                    i32 width, Color color, Color doxor)
{
	if(IsNull(color) || IsNull(width))
		return;
	PutRGColor(color);
	PutLineWidth(width);
	while(--count_count >= 0) {
		i32 part = *counts++;
		page << Pt(vertices->x) << ' ' << Pt(pgsz.cy - vertices->y) << " m\n";
		vertices++;
		for(; --part > 0; vertices++)
			page << Pt(vertices->x) << ' ' << Pt(pgsz.cy - vertices->y) << " l\n";
		page << "S\n";
	}
}

void PdfDraw::DrawEllipseOp(const Rect& r, Color color, i32 pen, Color outline)
{
	bool fill = !IsNull(color), stroke = !IsNull(outline) && !IsNull(pen);
	if(fill) PutrgColor(color);
	if(stroke) { PutRGColor(outline); PutLineWidth(pen); }
	if(!fill && !stroke)
		return;
	char closeop = (stroke && fill ? 'B' : fill ? 'f' : 'S');
	i32 sizelim = r.Width() | r.Height();
	i32 bits = 1;
	while(bits < 20 && sizelim > (1 << bits))
		bits++;
	i32 parts = 3 + max(((bits * (bits - 6)) >> 3) | 1, 1);
	double k = 2 * M_PI / 3 / parts;
	Pointf center(Pt((r.left + r.right) / 2.0), Pt(pgsz.cy - ((r.top + r.bottom) / 2.0)));
	Pointf size(Pt(r.Width() / 2.0), Pt(r.Height() / 2.0));
	for(i32 partid = 0; partid < parts; partid++) {
		double phi1 = (partid + 0) * (2 * M_PI / parts);
		double phi2 = (partid + 1) * (2 * M_PI / parts);
		double s1 = sin(phi1), c1 = cos(phi1), s2 = sin(phi2), c2 = cos(phi2);
		Pointf A = center + Pointf(c1, s1) * size;
		Pointf D = center + Pointf(c2, s2) * size;
		Pointf dA = Pointf(-s1, c1) * size;
		Pointf dD = Pointf(-s2, c2) * size;
		Pointf B = A + k * dA;
		Pointf C = D - k * dD;
		if(!partid)
			page << FormatF(A.x, 2) << ' ' << FormatF(A.y, 2) << " m\n";
		page << FormatF(B.x, 2) << ' ' << FormatF(B.y, 2) << ' '
			<< FormatF(C.x, 2) << ' ' << FormatF(C.y, 2) << ' '
			<< FormatF(D.x, 2) << ' ' << FormatF(D.y, 2) << " c\n";
	}
	page << closeop << '\n';
}

void PdfDraw::DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color)
{
	//СДЕЛАТЬ!!
	NEVER();
}

void PdfDraw::DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count,
	i32k *subpolygon_counts, i32 subpolygon_count_count,
	i32k *disjunct_polygon_counts, i32 disjunct_polygon_count_count,
	Color color, i32 width, Color outline, zu64 pattern, Color doxor)
{
	bool fill = !IsNull(color) && ~pattern, stroke = !IsNull(outline) && !IsNull(width);
	if(fill)   PutrgColor(color, pattern);
	if(stroke) { PutRGColor(outline); PutLineWidth(width); }
	if(!fill && !stroke) return;
	tukk closeop = fill && stroke ? "B*" : fill ? "f*" : "S";
	while(--disjunct_polygon_count_count >= 0) {
		i32 disj = *disjunct_polygon_counts++;
		while(disj > 0) {
			i32 sub = *subpolygon_counts++;
			disj -= sub;
			page << Pt(vertices->x) << ' ' << Pt(pgsz.cy - vertices->y) << " m\n";
			vertices++;
			for(; --sub > 0; vertices++)
				page << Pt(vertices->x) << ' ' << Pt(pgsz.cy - vertices->y) << " l\n";
			page << "h\n";
		}
		page << closeop << '\n';
	}
}

Txt GetMonoPdfImage(const Image& m, const Rect& sr)
{
	Txt data;
	for(i32 y = sr.top; y < sr.bottom; y++) {
		const RGBA *p = m[y] + sr.left;
		const RGBA *e = m[y] + sr.right;
		while(p < e) {
			i32 bit = 0x80;
			u8 b = 0;
			while(bit && p < e) {
				if(!((p->r | p->g | p->b) == 0 || (p->r & p->g & p->b) == 255))
					return Null;
				b |= bit & p->r;
				bit >>= 1;
				p++;
			}
			data.Cat(b);
		}
	}
	return data;
}

Txt GetGrayPdfImage(const Image& m, const Rect& sr)
{
	Txt data;
	for(i32 y = sr.top; y < sr.bottom; y++) {
		const RGBA *p = m[y] + sr.left;
		const RGBA *e = m[y] + sr.right;
		while(p < e)
			if(p->r == p->g && p->g == p->b)
				data.Cat((p++)->r);
			else
				return Null;
	}
	return data;
}

Txt PdfDraw::Finish(const PdfSignatureInfo *sign)
{
	if(page.GetLength()) {
		PutStream(page);
		empty = false;
	}

	i32 pagecount = offset.GetCount();

	// we need to generate raster glyph char procs before images because we need alpha image
	// for color emojis
	Vec<i32> charprocs;
	for(i32 i = 0; i < pdffont.GetCount(); i++) {
		Font fnt = pdffont.GetKey(i);
		if(fnt.GetHeight() != FONTHEIGHT_TTF) {
			i32 t3ch = offset.GetCount() + 1;
			i32 fa = fnt.GetCy() - fnt.GetInternal();
			Txt res;
			const Vec<wchar>& cs = pdffont[i];
			for(i32 c = 0; c < cs.GetCount(); c++) {
				RGlyph rg = RasterGlyph(fnt, pdffont[i][c]);
				Txt proc;
				if(rg.color_image >= 0)
					proc << 1000 * fnt[cs[c]] / fa << " 0 d0\n";
				else
					proc
						<< 1000 * fnt[cs[c]] / fa << " 0 0 "
						<< -1000 * fnt.GetDescent() / fa << ' '
						<< 1000 * (rg.sz.cx + rg.x) / fa << ' '
						<< 1000 * rg.sz.cy / fa
						<< " d1\n";
				proc
					<< "q "
					<< 1000 * rg.sz.cx / fa
					<< " 0 0 "
					<< 1000 * rg.sz.cy / fa
					<< " "
					<< -1000 * rg.x / fa
					<< " "
					<< -1000 * fnt.GetDescent() / fa
					<< " cm"
				;
				if(rg.color_image >= 0)
				    proc << " /Image" << rg.color_image + 1 << " Do Q";
				else
					proc << " BI /W " << rg.sz.cx << " /H " << rg.sz.cy
					     << " /BPC 1 /IM true /D [0 1]"
					     << " ID\n" << rg.data
					     << "\nEI Q";
				PutStream(proc);
			}
			charprocs.At(i) = BeginObj();
			out << "<<";
			for(i32 c = 0; c < cs.GetCount(); c++)
				out << " /Rgch" << c << ' ' << t3ch + c<< " 0 R";
			out << " >>\n";
			EndObj();
		}
	}

	Vec<i32> imageobj;
	i32 jpegi = 0;
	for(i32 i = 0; i < images.GetCount(); i++) {
		Size sz = images[i].GetSize();
		Rect sr = images.GetKey(i).b & sz;
		Txt data;
		Txt wh;
		wh << " /Width " << sr.Width() << " /Height " << sr.Height();
		const Image& m = images[i];
		if(m.GetSerialId() == sJPEGDummy().GetSerialId()) {
			Txt jpg = jpeg[jpegi++];
			TxtStream ss(jpg);
			One<StreamRaster> r = StreamRaster::OpenAny(ss);
			Size isz(1, 1);
			if(r)
				isz = r->GetSize();
			BeginObj();
			out << "<< " << " /Width " << isz.cx << " /Height " << isz.cy
			    << " /Length " << jpg.GetLength()
				<< "/Type/XObject "
				   "/ColorSpace/DeviceRGB "
				   "/Subtype/Image "
				   "/BitsPerComponent 8 "
			       "/Filter/DCTDecode >>\r\n"
			    << "stream\r\n" << jpg << "\r\nendstream\n";
			EndObj();
			imageobj << offset.GetCount();
		}
		else {
			i32 mask = -1;
			i32 smask = -1;
			if(m.GetKind() == IMAGE_MASK) {
				for(i32 y = sr.top; y < sr.bottom; y++) {
					const RGBA *p = m[y] + sr.left;
					const RGBA *e = m[y] + sr.right;
					while(p < e) {
						i32 bit = 0x80;
						u8 b = 0;
						while(bit && p < e) {
							if(p->a != 255)
								b |= bit;
							bit >>= 1;
							p++;
						}
						data.Cat(b);
					}
				}
				mask = PutStream(data, Txt().Cat()
				                    << "/Type /XObject /Subtype /Image" << wh
					                << " /BitsPerComponent 1 /ImageMask true /Decode [0 1] ");
			}
			if(m.GetKind() == IMAGE_ALPHA) {
				for(i32 y = sr.top; y < sr.bottom; y++) {
					const RGBA *p = m[y] + sr.left;
					const RGBA *e = m[y] + sr.right;
					while(p < e)
						data.Cat((p++)->a);
				}
				smask = PutStream(data, Txt().Cat()
				                    << "/Type /XObject /Subtype /Image" << wh
					                << " /BitsPerComponent 8 /ColorSpace /DeviceGray /Decode [0 1] ");
			}
			Txt imgobj;
			data = GetMonoPdfImage(m, sr);
			if(data.GetCount())
				imgobj << "/Type /XObject /Subtype /Image" << wh
				       << " /BitsPerComponent 1 /Decode [0 1] /ColorSpace /DeviceGray ";
			else {
				data = GetGrayPdfImage(m, sr);
				if(data.GetCount())
					imgobj << "/Type /XObject /Subtype /Image" << wh
					       << " /BitsPerComponent 8 /ColorSpace /DeviceGray /Decode [0 1] ";
				else {
					data.Clear();
					for(i32 y = sr.top; y < sr.bottom; y++) {
						const RGBA *p = m[y] + sr.left;
						const RGBA *e = m[y] + sr.right;
						while(p < e) {
							data.Cat(p->r);
							data.Cat(p->g);
							data.Cat(p->b);
							p++;
						}
					}
					imgobj << "/Type /XObject /Subtype /Image" << wh
					       << " /BitsPerComponent 8 /ColorSpace /DeviceRGB /Intent /Perceptual";
				}
			}
			if(mask >= 0)
				imgobj << " /Mask " << mask << " 0 R";
			if(smask >= 0)
				imgobj << " /SMask " << smask << " 0 R";
			imageobj << PutStream(data, imgobj);
		}
	}

	i32 patcsobj = -1;
	i32 patresobj = -1;
	if(!patterns.IsEmpty()) {
		patcsobj = BeginObj();
		out << "[/Pattern /DeviceRGB]\n";
		EndObj();
		patresobj = BeginObj();
		out << "<< >>\n";
		EndObj();
	}

	
	Vec<i32> fontobj;
	for(i32 i = 0; i < pdffont.GetCount(); i++) {
		Font fnt = pdffont.GetKey(i);
		const Vec<wchar>& cs = pdffont[i];
		Txt cmap;
		cmap <<
			"/CIDInit /ProcSet findresource begin\n"
			"12 dict begin\n"
			"begincmap\n"
			"/CIDSystemInfo\n"
			"<< /Registry (Adobe)\n"
			"/Ordering (UCS)\n"
			"/Supplement 0\n"
			">> def\n"
			"/CMapName /UCS" << i << " def\n"
			"/CMapType 2 def\n"
			"1 begincodespacerange\n"
			"<00> <" << FormatIntHex(cs.GetCount() - 1, 2) << ">\n"
			"endcodespacerange\n"
			"1 beginbfrange\n"
			"<00> <" << FormatIntHex(cs.GetCount() - 1, 2) << ">\n"
			"[\n";
		for(i32 c = 0; c < cs.GetCount(); c++)
			cmap << '<' << FormatIntHex(cs[c], 4) << ">\n";
		cmap <<
			"]\n"
			"endbfrange\n"
			"endcmap\n"
			"CMapName currentdict /CMap defineresource pop\n"
			"end\n"
			"end\n";
		i32 cmapi = PutStream(cmap);
		if(fnt.GetHeight() != FONTHEIGHT_TTF) {
			i32 encoding = BeginObj();
			out << "<< /Type /Encoding /Differences [0";
			for(i32 c = 0; c < cs.GetCount(); c++)
				out << " /Rgch" << c;
			out << "] >>\n";
			EndObj();

			i32 resources = BeginObj();
			out << "<< /ProcSet [ /PDF /Text /ImageB /ImageC ]\n";
			if(imageobj.GetCount()) {
				out << "/XObject << ";
				for(i32 i = 0; i < imageobj.GetCount(); i++)
					out << "/Image" << (i + 1) << ' ' << imageobj[i] << " 0 R ";
				out << ">>\n";
			}
			out << ">>\n";
			EndObj();

			fontobj.Add() = BeginObj();
			out <<
				"<< /Name /F" << i + 1 <<
				" /Type /Font\n"
				"/Subtype /Type3\n"
				"/FontBBox [0 0 0 0]\n"
				"/FontMatrix [0.001 0 0 0.001 0 0]\n"
				"/CharProcs " << charprocs[i] << " 0 R\n"
				"/Encoding " << encoding << " 0 R\n"
				"/FirstChar 0\n"
				"/LastChar " << cs.GetCount() - 1 << "\n"
				"/Widths [";
			i32 fa = fnt.GetCy() - fnt.GetInternal();
			for(i32 i = 0; i < cs.GetCount(); i++)
				out << ' ' << 1000 * fnt[cs[i]] / fa;
			out << "]\n";
						    
			out << "/Resources " << resources << " 0 R\n"
			    << "/FirstChar 0 /LastChar " << cs.GetCount() - 1 <<" /ToUnicode "
				<< cmapi
				<< " 0 R\n>>\n";
			EndObj();
		}
		else {
			TTFReader ttf;
			if(!ttf.Open(fnt))
				return Null;

			Txt name = FormatIntAlpha(i + 1, true);
			name.Cat('A', 6 - name.GetLength());
			name << '+' << ttf.ps_name;

			i32 fonti = PutStream(ttf.Subset(cs));

			BeginObj();
			i32 ascent = ttf.hhea.ascent * 1000 / ttf.head.unitsPerEm;
			i32 descent = ttf.hhea.descent * 1000 / ttf.head.unitsPerEm;
			out <<
				"<< /Type /FontDescriptor\n"
				"/FontName /" << name << "\n"
				"/Flags 4\n"
				"/FontBBox [ -1000 " << descent << " 3000 " << ascent << " ]\n" //?????
				"/ItalicAngle " << ttf.post.italicAngle / 65536.0 << "\n"
				"/Ascent " << ascent << "\n"
				"/Descent " << -descent << "\n"
				"/CapHeight " << ttf.hhea.ascent * 1000 / ttf.head.unitsPerEm << "\n"
				"/StemV 80\n"
				"/FontFile2 " << fonti << " 0 R\n" <<
				">>\n";
			EndObj();

			fontobj.Add() = BeginObj();
			out <<
				"<< /Type /Font\n"
				"/Subtype /TrueType\n"
				"/BaseFont /" << name << "\n"
				"/FirstChar 0\n"
				"/LastChar " << cs.GetCount() - 1 << "\n"
				"/Widths [ ";
			for(i32 i = 0; i < cs.GetCount(); i++)
				out << ttf.GetAdvanceWidth(cs[i]) * 1000 / ttf.head.unitsPerEm << ' ';
			out <<
				"]\n"
				"/FontDescriptor " << fonti + 1 << " 0 R\n"
				"/ToUnicode " << cmapi << " 0 R\n" <<
				">>\n";
			EndObj();
		}
	}

	i32 fonts = BeginObj();
	out << "<<\n";
	for(i32 i = 0; i < pdffont.GetCount(); i++)
		out << "/F" << i + 1 << ' ' << fontobj[i] << " 0 R \n";
	out << ">>\n";
	EndObj();

	Vec<i32> patternobj;
	patternobj.SetCount(patterns.GetCount(), -1);
	for(i32 i = 0; i < patterns.GetCount(); i++) {
		zu64 pat = patterns[i];
		TxtBuf ptk;
		ptk <<
		"/Type /Pattern\n"
		"/PatternType 1\n"
		"/PaintType 2\n"
		"/TilingType 3\n"
		"/BBox [-1 -1 9 9]\n"
		"/XStep 8\n"
		"/YStep 8\n"
		"/Resources " << patresobj << " 0 R\n"
		"/Matrix [0.75 0.0 0.0 0.75 0.0 0.0]\n" // pattern pixels -> dots
		;
		TxtBuf ptd;
		for(i32 y = 0; y < 8; y++) {
			for(i32 x = 0; x < 8; x++) {
				i32 b = 8 * y + x, e, lim;
				if(!((pat >> b) & 1)) {
					e = 0;
					lim = 8 - x;
					while(++e < lim && !((pat >> (b + e)) & 1))
						pat |= (zu64(1) << (b + e));
					if(e > 1)
						ptd << x << ' ' << (7 - y) << ' ' << e << " 1 re f\n";
					else {
						e = 0;
						lim = 8 - y;
						while(++e < lim && !((pat >> (b + 8 * e)) & 1))
							pat |= (zu64(1) << (b + 8 * e));
						if(e - y > 1)
							ptd << x << ' ' << (7 - y - e) << " 1 " << e << " re f\n";
						else {
							e = 0;
							lim = 8 - max(x, y);
							while(++e < lim && !((pat >> (b + 9 * e)) & 1))
								pat |= (zu64(1) << (b + 9 * e));
							if(e > 1) {
								ptd
								<< FormatDouble(x - 0.25, 2) << " "
								<< FormatDouble(7.75 - y, 2) << " m\n"
								<< FormatDouble(x + 0.25, 2) << " "
								<< FormatDouble(8.25 - y, 2) << " l\n"
								<< FormatDouble(x + e + 0.25, 2) << " "
								<< FormatDouble(8.25 - y - e, 2) << " l\n"
								<< FormatDouble(x + e - 0.25, 2) << " "
								<< FormatDouble(7.75 - y - e, 2) << " l\n"
								<< "f\n";
							}
							else {
								e = 0;
								lim = 8 - max(7 - x, y);
								while(++e < lim && !((pat >> (b + 7 * e)) & 1))
									pat |= (zu64(1) << (b + 7 * e));
								if(e > 1) {
									ptd
									<< FormatDouble(x + 1.25, 2) << " "
									<< FormatDouble(7.75 - y, 2) << " m\n"
									<< FormatDouble(x + 0.75, 2) << " "
									<< FormatDouble(8.25 - y, 2) << " l\n"
									<< FormatDouble(x - e + 0.75, 2) << " "
									<< FormatDouble(8.25 - y - e, 2) << " l\n"
									<< FormatDouble(x - e + 1.25, 2) << " "
									<< FormatDouble(7.75 - y - e, 2) << " l\n"
									<< "f\n";
								}
								else {
									ptd << x << ' ' << (7 - y) << " 1 1 re f\n";
								}
							}
						}
					}
				}
			}
		}
		patternobj[i] = PutStream(ptd, ptk);
		
	}

/*
	Vec<i32>  rgobj;
	Vec<Size> rgsz;
	Vec<i32>  rgx;
	for(i32 i = 0; i < pdffont.GetCount(); i++) {
		Font fnt = pdffont.GetKey(i);
		if(fnt.GetHeight()) {
			for(i32 c = 0; c < pdffont[i].GetCount(); c++) {
				RGlyph rg = RasterGlyph(fnt, pdffont[i][c]);
				i32 ii = rgobj.GetCount();
				rgobj << PutStream(rg.data, Txt().Cat()
				                   << "/Type /XObject /Subtype /Image "
				                   << " /Width " << rg.sz.cx << " /Height " << rg.sz.cy
					               << " /BitsPerComponent 1 /ImageMask true /Decode [0 1]");
				rgsz.Add(rg.sz);
				rgx.Add(rg.x);
			}
		}
	}
*/
	
	i32 dflt_rgb_def = -1;
	if(pdfa) {
		i32 icc_info = BeginObj();
		static i32k data_len = __countof(ICC_ColorSpaceInfo) - 1;
		out << "<</Length " << data_len << "/Filter/FlateDecode/N 3>>\n"
		"stream\n";
		out.Cat(ICC_ColorSpaceInfo, data_len);
		out << "\nendstream\n";
		EndObj();
		
		dflt_rgb_def = BeginObj();
		out << "[/ICCBased " << icc_info << " 0 R]\n";
		EndObj();
	}
	
	i32 resources = BeginObj();
	out << "<< /Font " << fonts << " 0 R\n"
	"/ProcSet [ /PDF /Text /ImageB /ImageC ]\n";
	if(imageobj.GetCount()) {
		out << "/XObject << ";
		for(i32 i = 0; i < imageobj.GetCount(); i++)
			out << "/Image" << (i + 1) << ' ' << imageobj[i] << " 0 R ";
		out << ">>\n";
	}
	if(!patternobj.IsEmpty()) {
		out << "/ColorSpace << /Cspat " << patcsobj << " 0 R >>\n"
		       "/Pattern << ";
		for(i32 i = 0; i < patterns.GetCount(); i++)
			out << "/Pat" << (i + 1) << ' ' << patternobj[i] << " 0 R ";
		out << ">>\n";
	}
	if(pdfa) {
		out << "/ColorSpace <</DefaultRGB " << dflt_rgb_def << " 0 R>>\n";
	}
	out << ">>\n";
	EndObj();
	
	i32 p7s_len = 0;
	if(sign)
		p7s_len = HexTxt(GetP7Signature(Txt(), sign->cert, sign->pkey)).GetCount();

	i32 len0 = out.GetCount();
	i32 offset0 = offset.GetCount();
	
	for(;;) { // in case that signature_len grows...
		i32 signature = -1;
		i32 signature_widget = -1;
		i32 p7s_start, p7s_end, pdf_length_pos = 0;
	
		i32  sign_page = Null;
		Rect sign_rect(0, 0, 0, 0);

		if(sign) {
			signature = BeginObj();
			out << "<< /Type /Sig\n";
			out << "/Contents ";
			p7s_start = out.GetCount();
			out << "<" + Txt('0', p7s_len) + ">";
			p7s_end = out.GetCount();
			out << "\n";
	
			out << "/ByteRange [0 " << p7s_start << ' ' << p7s_end << ' ';
			pdf_length_pos = out.GetCount();
			      //1234567890 -  %10d
			out << "**********]\n";
			out << "/Filter /Adobe.PPKLite\n";
			out << "/SubFilter /adbe.pkcs7.detached\n";
			Time tm = Nvl(sign->time, GetSysTime());
			
			i32 tz = GetTimeZone();
			i32 az = abs(tz) % (24 * 60); // (24 * 60) - sanity clamp to single day...
			out << Format("/M (%02d%02d%02d%02d%02d%02d%c%02d'%02d')", tm.year, tm.month, tm.day,
			              tm.hour, tm.minute, tm.second, (tz < 0 ? '-' : '+'), az / 60, az % 60);
			if(sign->reason.GetCount())
				out << "/Reason " << PdfTxt(sign->reason) << "\n";
			if(sign->name.GetCount())
				out << "/Name " << PdfTxt(sign->name) << "\n";
			if(sign->location.GetCount())
				out << "/Location " << PdfTxt(sign->location) << "\n";
			if(sign->contact_info.GetCount())
				out << "/ContactInfo " << PdfTxt(sign->contact_info) << "\n";
			out << ">>\n";
	
			EndObj();
		}
	
		Vec<Txt> url_ann;
		for(i32 pi = 0; pi < min(page_url.GetCount(), pagecount); pi++) {
			const Array<UrlInfo>& url = page_url[pi];
			for(i32 i = 0; i < url.GetCount(); i++) {
				const UrlInfo& u = url[i];
				Txt r;
				r << "/Border[0 0 0]/Rect["
				  << Pt(u.rect.left) << ' ' << Pt(pgsz.cy - u.rect.bottom) << ' '
				  << Pt(u.rect.right) << ' ' << Pt(pgsz.cy - u.rect.top) << "]\n";
				if(u.url == "<<signature>>") {
					if(sign_page == pi)
						sign_rect.Union(u.rect);
					else {
						sign_page = pi;
						sign_rect = u.rect;
					}
				}
				else {
					url_ann.At(pi) << ' ' << BeginObj() << " 0 R";
					out << "<</Type/Annot"
					    << r
						<< "/A<</Type/Action/S/URI/URI" << PdfTxt(u.url) << ">>\n"
						<< "/Subtype/Link";
					out << ">>\n";
					EndObj();
				}
			}
		}

		if(sign) {
			signature_widget = BeginObj();
			out << "<< /Type /Annot\n"
				   "/Subtype /Widget\n"
			       "/FT /Sig\n"
				   "/Ff 0\n" // not sure what is this...
				   "/T(Signature)\n"
				   "/V " << signature << " 0 R\n"
			;
			if(IsNull(sign_page)) { // invisible signature
				out << "/F 132 /Rect[0 0 0 0]\n";
				sign_page = 0;
			}
			else
				out << "/F 4 /Border[0 0 0]/Rect["
				    << Pt(sign_rect.left) << ' ' << Pt(pgsz.cy - sign_rect.bottom) << ' '
				    << Pt(sign_rect.right) << ' ' << Pt(pgsz.cy - sign_rect.top) << "]\n";

			out << "/P " << signature_widget + 2 + sign_page << " 0 R\n" // next entry is Pages and then Page
			    << ">>\n";
			EndObj();
		}

		i32 pages = BeginObj();
		out << "<< /Type /Pages\n"
		    << "/Kids [";
		for(i32 i = 0; i < pagecount; i++)
			out << i + pages + 1 << " 0 R ";
		out << "]\n"
		    << "/Count " << pagecount << "\n";
		out << ">>\n";
		EndObj();
		for(i32 i = 0; i < pagecount; i++) {
			BeginObj();
			out << "<< /Type /Page\n"
			    << "/Parent " << pages << " 0 R\n"
			    << "/MediaBox [0 0 " << Pt(pgsz.cx) << ' ' << Pt(pgsz.cy) << "]\n"
			    << "/Contents " << i + 1 << " 0 R\n"
			    << "/Resources " << resources << " 0 R\n";
			bool sgned = sign && sign_page == i;
			bool urls = i < url_ann.GetCount() && url_ann[i].GetCount();
			if(sgned || urls) {
				out << "/Annots [";
				if(urls)
					out << url_ann[i];
				if(urls && sgned)
					out << ' ';
				if(sgned)
					out << signature_widget << " 0 R";
				out << "]\n";
			}
			out << ">>\n";
			EndObj();
		}
		i32 outlines = BeginObj();
		out << "<< /Type /Outlines\n"
		       "/Count 0\n"
		       ">>\n";
		EndObj();
		i32 pdfa_metadata = -1;
		if(pdfa) {
			TxtBuf metadata;
			metadata <<
			"<?xpacket id=\"" << Uuid::Create() << "\"?>\n"
			"<x:xmpmeta xmlns:x=\"adobe:ns:meta/\" x:xmptk=\"PDFNet\">\n"
			"<rdf:RDF xmlns:rdf=\"http://www.w3.org/1999/02/22-rdf-syntax-ns#\">\n"
			"<rdf:Description rdf:about=\"\"\n"
			"xmlns:xmp=\"http://ns.adobe.com/xap/1.0/\">\n"
			"<xmp:CreateDate>0000-01-01</xmp:CreateDate>\n"
			"<xmp:ModifyDate>0000-01-01</xmp:ModifyDate>\n"
			"<xmp:CreatorTool/>\n"
			"</rdf:Description>\n"
			"<rdf:Description rdf:about=\"\"\n"
			"xmlns:dc=\"http://purl.org/dc/elements/1.1/\">\n"
			"<dc:title>\n"
			"<rdf:Alt>\n"
			"<rdf:li xml:lang=\"x-default\"/>\n"
			"</rdf:Alt>\n"
			"</dc:title>\n"
			"<dc:creator>\n"
			"<rdf:Seq>\n"
			"<rdf:li/>\n"
			"</rdf:Seq>\n"
			"</dc:creator>\n"
			"<dc:description>\n"
			"<rdf:Alt>\n"
			"<rdf:li xml:lang=\"x-default\"/>\n"
			"</rdf:Alt>\n"
			"</dc:description>\n"
			"</rdf:Description>\n"
			"<rdf:Description rdf:about=\"\"\n"
			"xmlns:pdf=\"http://ns.adobe.com/pdf/1.3/\">\n"
			"<pdf:Keywords/>\n"
			"<pdf:Producer/>\n"
			"</rdf:Description>\n"
			"<rdf:Description rdf:about=\"\"\n"
			"xmlns:pdfaid=\"http://www.aiim.org/pdfa/ns/id/\">\n"
			"<pdfaid:part>1</pdfaid:part>\n"
			"<pdfaid:conformance>B</pdfaid:conformance>\n"
			"</rdf:Description>\n"
			"</rdf:RDF>\n"
			"</x:xmpmeta>\n";
			
			TxtBuf meta_head;
			meta_head << "/Type/Metadata/Subtype/XML";
			
			pdfa_metadata = PutStream(metadata, meta_head, false);
		}
		
		i32 catalog = BeginObj();
		out << "<< /Type /Catalog\n"
		    << "/Outlines " << outlines << " 0 R\n"
		    << "/Pages " << pages << " 0 R\n";
		
		if(pdfa_metadata >= 0)
			out << "/Metadata " << pdfa_metadata << " 0 R\n";
		
		if(sign)
			out << " /AcroForm << /Fields [" << signature_widget << " 0 R] /SigFlags 3 "
			       " /Perms << /DocMDP " << signature << " 0 R >>>>";
	
		out << ">>\n";
		EndObj();
		i32 startxref = out.GetCount();
		out << "xref\n"
		    << "0 " << offset.GetCount() + 1 << "\n";
		out << "0000000000 65535 f\r\n";
		for(i32 i = 0; i < offset.GetCount(); i++)
			out << Sprintf("%010d 00000 n\r\n", offset[i]);
		out << "\n"
		    << "trailer\n"
		    << "<< /Size " << offset.GetCount() + 1 << "\n"
		    << "/Root " << catalog << " 0 R\n"
			<< "/ID [ <" << Uuid::Create() << "> <" << Uuid::Create() << "> ]\n"
		    << ">>\n"
		    << "startxref\r\n"
		    << startxref << "\r\n"
		    << "%%EOF\r\n";
	
		if(sign) {
			memcpy(~out + pdf_length_pos, Format("%10d", out.GetLength() - p7s_end), 10);
			Txt data(~out, p7s_start);
			data.Cat(~out + p7s_end, out.End());
			Txt p7s = HexTxt(GetP7Signature(data, sign->cert, sign->pkey));
			if(p7s.GetCount() <= p7s_len) {
				memcpy(~out + p7s_start + 1, p7s, p7s.GetCount());
				break;
			}
			p7s_len = p7s.GetCount();
		}
		else
			break;

		out.SetLength(len0); // p7s signature grew, scratch pdf ending and try again
		offset.SetCount(offset0);
	}
	   
	return out;
}

}
