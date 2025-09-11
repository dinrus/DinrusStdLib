#include "RichText.h"

namespace drx {

Draw& SimplePageDraw::Page(i32)
{
	return w;
}

void RichText::ApplyZoom(Zoom z)
{
	if(z.m == z.d)
		return;
	RichStyles ostyle(style, 1);
	for(i32 i = 0; i < style.GetCount(); i++)
		style[i].format *= z;
	RichTxt::ApplyZoom(z, ostyle, style);
	RefreshAll();
}

Zoom& sRichTextStdScreenZoom()
{
	static Zoom *zz;
	ONCELOCK {
		static Zoom z(96, 600);
		zz = &z;
	}
	return *zz;
}

void SetRichTextStdScreenZoom(i32 m, i32 d)
{
	sRichTextStdScreenZoom() = Zoom(m, d);
}

Zoom GetRichTextStdScreenZoom()
{
	return sRichTextStdScreenZoom();
}

struct QTFDisplayCls : Display {
	virtual void Paint(Draw& w, const Rect& r, const Val& q,
		               Color ink, Color paper, dword style) const;
	virtual Size GetStdSize(const Val& q) const;
	virtual Size RatioSize(const Val& q, i32 cx, i32 cy) const;
	virtual bool Vcenter() const { return false; }
};

Size QTFDisplayCls::GetStdSize(const Val& q) const
{
	Size sz;
	RichText txt = ParseQTF((Txt)q);
	txt.ApplyZoom(GetRichTextStdScreenZoom());
	sz.cx = txt.GetWidth();
	sz.cy = txt.GetHeight(Zoom(1, 1), sz.cx);
	return sz;
}

Size QTFDisplayCls::RatioSize(const Val& q, i32 cx, i32 cy) const
{
	if(cy == 0 && cx > 0) {
		RichText txt = ParseQTF((Txt)q);
		txt.ApplyZoom(GetRichTextStdScreenZoom());
		return Size(cx, txt.GetHeight(Zoom(1, 1), cx));
	}
	return GetStdSize(q);
}

void QTFDisplayCls::Paint(Draw& draw, const Rect& r, const Val& v, Color ink, Color paper, dword style) const
{
	RichText rtext = ParseQTF(~v);
	rtext.ApplyZoom(GetRichTextStdScreenZoom());
	draw.DrawRect(r, paper);
	draw.Clipoff(r);
	PaintInfo pi;
	pi.highlightpara = -1;
	pi.zoom = Zoom(1, 1);
	if(style & (CURSOR|SELECT|READONLY))
		pi.textcolor = ink;
	pi.darktheme = IsDarkTheme();
	if(Vcenter()) {
		i32 cy = rtext.GetHeight(Zoom(1, 1), r.Width());
		rtext.Paint(draw, 0, max(0, (r.Height() - cy) / 2), r.Width(), pi);
	}
	else
		rtext.Paint(draw, 0, 0, r.Width(), pi);
	draw.End();
}

const Display& QTFDisplay()
{
	return Single<QTFDisplayCls>();
}

struct QTFDisplayCCls : QTFDisplayCls {
	virtual bool Vcenter() const { return true; }
};

const Display& QTFDisplayVCenter()
{
	return Single<QTFDisplayCCls>();
}

RichText AsRichText(wtukk s, const RichPara::Format& f)
{
	RichText clip;
	RichPara p;
	p.format = f;
	p.part.Add().format = f;
	WTxt& part = p.part.Top().text;
	while(*s) {
		if(*s == '\n') {
			clip.Cat(p);
			part.Clear();
		}
		if(*s >= 32 || *s == '\t')
			part.Cat(*s);
		s++;
	}
	clip.Cat(p);
	return clip;
}

struct DrawingPageDraw__ : public DrawingDraw, public PageDraw {
	virtual Draw& Page(i32 i);

	Array<Drawing>  page;
	i32             pagei;
	Size            size;

	void  Flush();

	DrawingPageDraw__() { pagei = -1; }
};

Draw& DrawingPageDraw__::Page(i32 i)
{
	ASSERT(i >= 0);
	if(i != pagei) {
		Flush();
		pagei = i;
		Create(size);
	}
	return *this;
}

void DrawingPageDraw__::Flush()
{
	if(pagei >= 0) {
		Drawing dw = GetResult();
		page.At(pagei).Append(dw);
		Create(size);
	}
}

Array<Drawing> RenderPages(const RichText& txt, Size pagesize)
{
	DrawingPageDraw__ pd;
	pd.size = pagesize;
	PaintInfo paintinfo;
	paintinfo.top = PageY(0, 0);
	paintinfo.bottom = PageY(INT_MAX, INT_MAX);
	paintinfo.indexentry = Null;
	paintinfo.hyperlink = Null;
	txt.Paint(pd, pagesize, paintinfo);
	pd.Flush();
	return pick(pd.page);
}

Txt Pdf(const RichText& txt, Size pagesize, i32 margin, bool pdfa, const PdfSignatureInfo *sign)
{
	ASSERT_(GetDrawingToPdfFn(), "Pdf зависит от пакета PdfDraw");
	Array<Drawing> pages = RenderPages(txt, pagesize);
	return GetDrawingToPdfFn() && pages.GetCount() ? (*GetDrawingToPdfFn())(pages, pagesize, margin, pdfa, sign)
	                                               : Txt();
}

}