#include "CtrlLib.h"

namespace drx {

#define LLOG(x) // LOG(x)

Zoom  RichTextView::GetZoom() const
{
	i32 szcx = GetSize().cx;
	if(!sb.IsShown() && sb.IsAutoHide())
		szcx -= ScrollBarSize();
	return IsNull(zoom) ? Zoom(szcx - margin.left - margin.right, cx) : zoom;
}

i32   RichTextView::GetPageCx(bool reduced) const
{
	i32 szcx = GetSize().cx;
	if(reduced && !sb.IsShown() && sb.IsAutoHide())
		szcx -= ScrollBarSize();
	return IsNull(zoom) ? cx : (szcx - margin.left - margin.right) / zoom;
}

Rect  RichTextView::GetPage() const
{
	return Rect(0, 0, GetPageCx(), INT_MAX);
}

i32 RichTextView::GetCy() const
{
	return GetZoom() * text.GetHeight(GetPage()).y;
}

i32 RichTextView::TopY() const
{
	if(vcenter && sb.GetTotal() < sb.GetPage())
		return (sb.GetPage() - sb.GetTotal()) / 2;
	return 0;
}

void  RichTextView::Paint(Draw& w)
{
	Size sz = GetSize();
	w.DrawRect(sz, background);
	sz.cx -= margin.left + margin.right;
	sz.cy -= margin.top + margin.bottom;
	w.Clipoff(margin.left, margin.top, sz.cx, sz.cy);
	PaintInfo pi;
	if(!hldec)
		pi.hyperlink = Null;
	if(sell < selh) {
		pi.sell = sell;
		pi.selh = selh;
	}
	pi.indexentry = Null;
	pi.highlightpara = highlight;
	pi.WhenHighlight = WhenHighlight;
	pi.highlight = highlight_color;
	pi.zoom = GetZoom();
	pi.textcolor = textcolor;
	i32 q = sb * pi.zoom;
	w.Offset(0, -q);
	SimplePageDraw pw(w);
	pi.top = PageY(0, sb);
	pi.bottom = PageY(0, sb + sz.cy / pi.zoom);
	pi.usecache = true;
	pi.sizetracking = sizetracking;
	pi.shrink_oversized_objects = shrink_oversized_objects;
	pi.darktheme = IsDarkTheme();
	Rect pg = GetPage();
	pg.top = TopY();
	text.Paint(pw, pg, pi);
	w.End();
	w.End();
}

void  RichTextView::SetSb()
{
	sb.SetTotal(text.GetHeight(GetPage()).y);
	sb.SetPage((GetSize().cy - margin.top - margin.bottom) / GetZoom());
}

bool  RichTextView::Key(dword key, i32 count)
{
	if(key == K_CTRL_C || key == K_SHIFT_INSERT) {
		Copy();
		return true;
	}
	return sb.VertKey(key);
}

void  RichTextView::MouseWheel(Point p, i32 zdelta, dword keyflags)
{
	if(!WhenMouseWheel(zdelta, keyflags))
		sb.Wheel(zdelta);
}

Image RichTextView::CursorImage(Point p, dword keyflags)
{
	i32 pos = GetPointPos(p);
	if(WhenLink && pos >= 0 && !IsNull(GetLink(pos, p)))
		return Image::Hand();
	if(HasCapture() && icursor)
		return Image::IBeam();
	return Image::Arrow();
}

WTxt RichTextView::GetSelText() const
{
	if(anchor == cursor)
		return text.GetPlainText(true, copy_with_tabs);
	else {
		WTxt h = text.GetPlainText(false, copy_with_tabs).Mid(sell, selh - sell);
		WTxt r;
		for(wtukk s = ~h; s < h.End(); s++) {
			if(*s == '\n')
				r.Cat('\r');
			r.Cat(*s);
		}
		return r;
	}
}

void RichTextView::Copy()
{
	RefreshSel();
	WriteClipboardUnicodeText(GetSelText());
}

Txt RichTextView::GetSelectionData(const Txt& fmt) const
{
	return GetTextClip(GetSelText(), fmt);
}

void RichTextView::RightDown(Point p, dword keyflags)
{
	MenuBar b;
	b.Add(cursor != anchor, t_("Копировать"), CtrlImg::copy(), OTVET(Copy)).Key(K_CTRL_C);
	b.Execute();
}

Point RichTextView::GetTextPoint(Point p) const
{
	p -= margin.TopLeft();
	Zoom zoom = GetZoom();
	p.y += sb * zoom;
	return Point(p.x / zoom, p.y / zoom - TopY());
}

i32  RichTextView::GetPointPos(Point p) const
{
	Size sz = GetSize();
	sz.cx -= margin.left + margin.right;
	sz.cy -= margin.top + margin.bottom;
	p = GetTextPoint(p);
	return text.GetPos(p.x, PageY(0, p.y), GetPage());
}

Txt RichTextView::GetLink(i32 pos, Point p) const
{
	Txt link;
	RichObj object = text.GetRichPos(pos).object;
	if(object) {
		Rect rc = text.GetCaret(pos, GetPage());
		//TODO: Perhaps use GetTextPoint here?
		link = object.GetLink(p - rc.TopLeft(), rc.Size());
	}

	if(IsNull(link)) {
		RichPos richpos = text.GetRichPos(pos);
		Rect rc = text.GetCaret(pos, GetPage());
		if(richpos.chr != '\n' && rc.Contains(GetTextPoint(p)))
			link = Nvl(richpos.fieldformat.link, richpos.format.link);
	}
	return link;
}

void RichTextView::RefreshRange(i32 a, i32 b)
{
	i32 l = max(min(a, b) - 1, 0); // Extend the range to cover 'weird' cases (line break)
	i32 h = min(max(a, b) + 1, GetLength());
	if(l == h)
		return;
	Rect r1 = text.GetCaret(l, GetPage()) + margin.TopLeft();
	Rect r2 = text.GetCaret(h, GetPage()) + margin.TopLeft();
	Zoom zoom = GetZoom();
	Refresh(Rect(0, zoom * (r1.top - sb + TopY()), GetSize().cx, zoom * (r2.bottom - sb + zoom.d - 1) + TopY()));
}

void  RichTextView::RefreshSel()
{
	i32 l = minmax(min(cursor, anchor), 0, text.GetLength());
	i32 h = minmax(max(cursor, anchor), 0, text.GetLength());
	if(sell == l && selh == h || sell == selh && l == h)
		return;
	RichPos pl = text.GetRichPos(l);
	RichPos ph = text.GetRichPos(h);
	RichPos psell = text.GetRichPos(sell);
	RichPos pselh = text.GetRichPos(selh);
	if(psell.parai != pl.parai || pselh.parai != ph.parai ||
	   psell.table != pl.table || pselh.table != ph.table ||
	   psell.cell != pl.cell || pselh.cell != ph.cell)
		Refresh();
	else {
		RefreshRange(l, sell);
		RefreshRange(h, selh);
	}
	sell = l;
	selh = h;
	if(IsSelection())
		SetSelectionSource(ClipFmtsText());
}

void RichTextView::ClearSelection()
{
	if(IsSelection()) {
		anchor = cursor;
		RefreshSel();
	}
}

void  RichTextView::LeftDown(Point p, dword keyflags)
{
	i32 pos = GetPointPos(p);
	if(pos < 0) {
		cursor = anchor = 0;
		return;
	}
	Txt link = GetLink(pos, p);
	if(!IsNull(link))
		WhenLink(link);
	else {
		cursor = pos;
		if(!(keyflags & K_SHIFT))
			anchor = pos;
		RefreshSel();
		SetFocus();
		SetCapture();
		WhenLeftClick();
	}
}

void RichTextView::LeftDouble(Point p, dword keyflags)
{
	i32 pos = GetPointPos(p);
	if(IsLeNum(text[pos])) {
		anchor = pos;
		while(anchor > 0 && IsLeNum(text[anchor - 1]))
			anchor--;
		cursor = pos;
		while(cursor < text.GetLength() && IsLeNum(text[cursor]))
			cursor++;
		while(cursor < text.GetLength() && text[cursor] == ' ')
			cursor++;
		RefreshSel();
		SetFocus();
	}
}

void RichTextView::LeftTriple(Point p, dword keyflags)
{
    i32 pos = GetPointPos(p);
	RichPos rp = text.GetRichPos(pos);
	anchor = pos - rp.posinpara;
	cursor = anchor + rp.paralen + 1;
    RefreshSel();
    SetFocus();
}

void RichTextView::MouseMove(Point p, dword keyflags)
{
	i32 pos = GetPointPos(p);
	WhenMouseMove(pos);
	if(HasCapture()) {
		if(pos < 0)
			return;
		cursor = pos;
		Rect r1 = text.GetCaret(cursor, GetPage());
		sb.ScrollInto(r1.top, r1.Height());
		RefreshSel();
	}
}

void RichTextView::LeftRepeat(Point p, dword keyflags)
{
	MouseMove(p, keyflags);
}

void  RichTextView::EndSizeTracking()
{
	sizetracking = false;
	Refresh();
}

void  RichTextView::Layout()
{
	sizetracking = false;
	if(IsOpen() && lazy) {
		sizetracking = true;
		KillTimeCallback(TIMEID_ENDSIZETRACKING);
		SetTimeCallback(250, OTVET(EndSizeTracking), TIMEID_ENDSIZETRACKING);
	}
	SetSb();
	Refresh();
}

Val RichTextView::GetData() const
{
	if(text.IsEmpty()) return Val();
	return GetQTF();
}

void  RichTextView::SetData(const Val& v)
{
	SetQTF(Txt(v));
}

void  RichTextView::Scroll()
{
	Refresh();
}

bool RichTextView::GotoLabel(Gate<const WTxt&> match, bool dohighlight, bool find_last)
{
	Vec<RichValPos> f = text.GetValPos(GetPage(), RichText::LABELS);
	highlight = Null;
	bool ret = false;
	for(i32 i = 0; i < f.GetCount(); i++)
		if(match(f[i].data)) {
			sb = f[i].py.y;
			if(dohighlight)
				highlight = f[i].pos;
			Refresh();
			if(!find_last)
				return true;
			ret = true;
		}
	return ret;
}

bool RichTextView::GotoLabel(const Txt& lbl, bool dohighlight, bool find_last)
{
	WTxt lw = lbl.ToWTxt();
	return GotoLabel([&](const WTxt& data) { return data == lw; }, dohighlight, find_last);
}

void  RichTextView::Clear()
{
	sb = 0;
	text.Clear();
	SetSb();
	Refresh();
	anchor = cursor = sell = selh = 0;
}

void  RichTextView::Pick(RichText&& rt)
{
	sb = 0;
	anchor = cursor = sell = selh = 0;
	text = pick(rt);
	SetSb();
	UpdateRefresh();
	highlight = -1;
}

void  RichTextView::Pick(RichText&& txt, Zoom z) {
	if(z.m != z.d)
		const_cast<RichText&>(txt).ApplyZoom(z);
	Pick(pick(txt));
	sb.SetLine(z * 100);
}

void  RichTextView::SetQTF(tukk qtf, Zoom z)
{
	Pick(ParseQTF(qtf), z);
}

RichTextView& RichTextView::PageWidth(i32 _cx)
{
	cx = _cx;
	sb = 0;
	SetSb();
	Refresh();
	return *this;
}

RichTextView& RichTextView::SetZoom(Zoom z)
{
	zoom = z;
	sb = 0;
	SetSb();
	Refresh();
	return *this;
}

RichTextView& RichTextView::Background(Color c)
{
	background = c;
	Transparent(IsNull(c));
	Refresh();
	return *this;
}

RichTextView& RichTextView::TextColor(Color _color)
{
	if(_color == White)
	{
		textcolor =  Color(198, 212, 0);
	}
	else
	 textcolor = _color;
	Refresh();
	return *this;
}

RichTextView& RichTextView::Highlight(Color _color)
{
	highlight_color = _color;
	Refresh();
	return *this;
}

RichTextView& RichTextView::VCenter(bool b)
{
	vcenter = b;
	return *this;
}

RichTextView& RichTextView::Margins(const Rect& m)
{
	margin = m;
	Refresh();
	return *this;
}

RichTextView& RichTextView::HMargins(i32 a)
{
	margin.left = margin.right = a;
	Refresh();
	return *this;
}

RichTextView& RichTextView::VMargins(i32 a)
{
	margin.top = margin.bottom = a;
	Refresh();
	return *this;
}

RichTextView& RichTextView::Margins(i32 a)
{
	margin.Set(a, a, a, a);
	Refresh();
	return *this;
}

void LinkInRichTextClipboard__();

RichTextView::RichTextView()
{
	cx = 3968;
	sizetracking = false;
	sb.SetLine(100);
	sb.WhenScroll = OTVET(Scroll);
	zoom = Null;
	background = SColorPaper;
	textcolor = Null;
	vcenter = false;
	margin = Rect(0, 0, 0, 0);
	highlight = -1;
	hldec = true;
	WhenLink = callback(LaunchWebBrowser);
	anchor = cursor = sell = selh = 0;
	SetFrame(ViewFrame());
	AddFrame(sb);
	NoWantFocus();
	lazy = true;
	shrink_oversized_objects = true;
}

RichTextView::~RichTextView() {}

void RichTextCtrl::SetData(const Val& v)
{
	SetQTF(Txt(v));
}

RichTextCtrl::RichTextCtrl()
{
	SetZoom(Zoom(1, 1));
	Transparent();
	Background(Null);
	SetFrame(NullFrame());
	AutoHideSb();
}

#ifndef TARGET_PDA

void Print(Draw& w, const RichText& text, const Rect& page, const Vec<i32>& pg)
{
	LLOG("Print");
	i32 lpage = text.GetHeight(page).page;
	PrintPageDraw pw(w);
	Size sz = w.GetPageMMs();
	Size pgsz = page.Size();
	i32 x = (6000 * sz.cx / 254 - pgsz.cx) / 2;
	i32 y = (6000 * sz.cy / 254 - pgsz.cy) / 2;
	for(i32 pi = 0; pi < pg.GetCount(); pi++) {
		i32 i = pg[pi];
		w.StartPage();
		w.Offset(x, y);
		pw.SetPage(i);
		PaintInfo paintinfo;
		paintinfo.top = PageY(i, 0);
		paintinfo.bottom = PageY(i + 1, 0);
		paintinfo.indexentry = Null;
		if(text.IsPrintNoLinks())
			paintinfo.hyperlink = Null;
		text.Paint(pw, page, paintinfo);
		w.End();
		Txt footer = text.GetFooter();
		if(!IsNull(footer) && lpage) {
			Txt n = Format(footer, i + 1, lpage + 1);
			Size nsz = GetTextSize(n, Arial(90).Italic());
			pw.Page(i).DrawText(
				x + pgsz.cx - nsz.cx, y + pgsz.cy + 100,
				n, Arial(90).Italic());
		}
		w.EndPage();
	}
}

void Print(Draw& w, const RichText& text, const Rect& page)
{
	i32 n = text.GetHeight(page).page;
	Vec<i32> pg;
	for(i32 i = 0; i <= n; i++)
		pg.Add(i);
	Print(w, text, page, pg);
}

bool Print(const RichText& text, const Rect& page, i32 currentpage, tukk name)
{
	PrinterJob pj(name);
	pj.CurrentPage(currentpage);
	pj.PageCount(text.GetHeight(page).page + 1);
	pj.Landscape(page.GetWidth() > page.GetHeight());
	if(pj.Execute()) {
		Print(pj, text, page, pj.GetPages());
		return true;
	}
	return false;
}

#endif

}