#ifndef _Report_Report_h
#define _Report_Report_h

#include <drx/CtrlLib/CtrlLib.h>
#include <drx/RichText/RichText.h>

namespace drx {

#define  LAYOUTFILE <drx/Report/Report.lay>
#include <drx/CtrlCore/lay.h>

class Report : public DrawingDraw, public PageDraw {
public:
	virtual Draw& Page(i32 i);
	virtual Size  GetPageSize() const    { return DrawingDraw::GetPageSize(); } // avoid CLANG warning
	virtual void  StartPage()            { DrawingDraw::StartPage(); } // avoid CLANG warning

private:
	Array<Drawing>      page;
	i32                 pagei;
	i32                 y;
	Txt                 header, footer;
	i32                 headercy, headerspc, footercy, footerspc;
	Point               mg;
	One<PrinterJob>     printerjob;

	void    Flush();
	Txt     FormatHF(tukk s, i32 pageno);
	i32     GetHeightHF(tukk s);
	void    PaintHF(Draw& w, i32 y, tukk qtf, i32 i);
	void    StartPage(i32 i);
	void    RestartPage();

	Callback              WhenPage;
	
public:

	i32                   GetCount()                  { Flush(); return page.GetCount(); }
	Drawing               GetPage(i32 i)              { Flush(); return page[i]; }
	Drawing               operator[](i32 i)           { return GetPage(i); }
	const Array<Drawing>& GetPages()                  { Flush(); return page; }

	void                  Clear();

	Rect                  GetPageRect();
	Size                  GetPageSize()               { return GetPageRect().Size(); }

	void                  SetY(i32 _y)                { y = _y; }
	i32                   GetY() const                { return y; }

	void                  NewPage()                   { Page(pagei + 1); }
	void                  RemoveLastPage()            { if(page.GetCount()) page.Drop(); pagei = -1; }

	void                  Put(const RichText& txt, uk context = NULL);
	void                  Put(tukk qtf);
	Report&               operator<<(tukk qtf) { Put(qtf); return *this; }

	void                  SetRichTextLayoutTracer(RichTextLayoutTracer& l) { tracer = &l; }

	Point                 GetMargins() const          { return mg; }

	bool                  ChoosePrinter(tukk jobname = t_("Отчёт"));
	bool                  ChooseDefaultPrinter(tukk jobname = t_("Отчёт"));
	PrinterJob           *GetPrinterJob()             { return ~printerjob; }

	Report&               SetPageSize(Size sz);
	Report&               SetPageSize(i32 cx, i32 cy) { return SetPageSize(Size(cx, cy)); }
	Report&               Landscape();
	Report&               Margins(i32 top, i32 left)  { mg.y = top; mg.x = left; return *this; }
	Report&               Margins(i32 m)              { return Margins(m, m); }
	Report&               Header(tukk qtf, i32 spc = 150);
	Report&               Footer(tukk qtf, i32 spc = 150);
	Report&               OnPage(Callback whenpage);
	Report&               NoHeader()                  { return Header(NULL, 0); }
	Report&               NoFooter()                  { return Footer(NULL, 0); }

	Report();
	Report(i32 cx, i32 cy);
	Report(const Size &sz);
};

class ReportView : public Ctrl {
public:
	virtual void Paint(Draw& w);
	virtual void Layout();
	virtual bool Key(dword key, i32);
	virtual void LeftDown(Point p, dword);
	virtual void MouseWheel(Point, i32 zdelta, dword);

protected:
	ScrollBar sb;
	Report   *report;
	Image     page[64];
	i32       pagei[64];
	Size      pagesize;
	i32       vsize;

	i32       pm;
	i32       pvn;
	bool      numbers;
	i32       pages;

	Image     GetPage(i32 i);

	void      Init();
	void      Sb();
	void      Numbers()               { Refresh(); }
	Size      GetReportSize();

public:
	Callback  WhenGoPage;

	enum Pages {
		PG1, PG2, PG4, PG16
	};
	ReportView& Pages(i32 pags)       { pages = pags; Layout(); return *this; }
	ReportView& Numbers(bool nums)    { numbers = nums; Refresh(); return *this; }

	void      Set(Report& report);
	Report   *Get()                   { return report; }
	i32       GetFirst() const        { return sb / pagesize.cy * pvn; }

	void      ScrollInto(i32 toppage, i32 top, i32 bottompage, i32 bottom);

	typedef ReportView CLASSNAME;

	ReportView();
};

class ReportWindow : public WithReportWindowLayout<TopWindow> {
	void Pages()   { pg.Pages(~sw); }
	void Numbers() { pg.Numbers(numbers); }
	void GoPage()  { sw <<= ReportView::PG1; Pages(); }
	void Pdf();
	void ShowPage();

	Array<Button>         button;
	Report               *report;

public:
	ReportView pg;

	typedef ReportWindow CLASSNAME;

	static void SetPdfRoutine(Txt (*pdf)(const Report& report, i32 margin));

	void SetButton(i32 i, tukk label, i32 id);

	i32  Perform(Report& report, i32 zoom = 100, tukk caption = t_("Отчёт"));

	ReportWindow();
};

Txt    Pdf(Report& report, bool pdfa = false, const PdfSignatureInfo *sign = NULL);
void   Print(Report& r, PrinterJob& pd);
bool   DefaultPrint(Report& r, i32 i, tukk _name = t_("Отчёт"));
bool   Print(Report& r, i32 i, tukk name = t_("Отчёт"));
bool   Perform(Report& r, tukk name = t_("Отчёт"));
bool   QtfReport(const Txt& qtf, tukk name = "", bool pagenumbers = false);
bool   QtfReport(Size pagesize, const Txt& qtf, tukk name = "", bool pagenumbers = false);

}

#endif
