#include "RichEdit.h"

namespace drx {

struct RichEditHdrFtr : public RichEdit {
	ToolBar  toolbar;

	void RefreshBar();
	void TheBar(Bar& bar);

	void  PageNumber();
	void  PageCount();

	void  Init(i32 pageno, i32 pagecount);

	typedef RichEditHdrFtr CLASSNAME;

	RichEditHdrFtr();
};

void RichEditHdrFtr::TheBar(Bar& bar)
{
	EditTools(bar);
	bar.Gap();
	FontTools(bar);
	bar.Gap();
	InkTool(bar);
	PaperTool(bar);
	bar.Gap();
	LanguageTool(bar);
	SpellCheckTool(bar);
	bar.Break();
	StyleTool(bar);
	bar.Gap();
	ParaTools(bar);
	bar.Gap();
	TableTools(bar);
	bar.Gap();
	bar.Add(!IsReadOnly(), t_("Вставить номер страницы"), RichEditImg::PageNumber(), OTVET(PageNumber));
	bar.Add(!IsReadOnly(), t_("Вставить счёт страниц"), RichEditImg::PageCount(), OTVET(PageCount));
}

void RichEditHdrFtr::PageNumber()
{
	PasteText(ParseQTF("{:VALUE:PAGENUMBER:}"));
	EvaluateFields();
}

void RichEditHdrFtr::PageCount()
{
	PasteText(ParseQTF("{:VALUE:PAGECOUNT:}"));
	EvaluateFields();
}

void RichEditHdrFtr::RefreshBar()
{
	toolbar.Set(OTVET(TheBar));
}

RichEditHdrFtr::RichEditHdrFtr()
{
	InsertFrame(0, toolbar);
	WhenRefreshBar = callback(this, &RichEditHdrFtr::RefreshBar);
	SetVar("PAGECOUNT", "###");
	SetVar("PAGENUMBER", "#");
	SetVar("__DISPLAY_VALUE_FIELDS", 1);
}

struct HeaderFooterDlg : WithHeaderFooterLayout<TopWindow> {
	typedef HeaderFooterDlg CLASSNAME;

	RichEditHdrFtr header_editor, footer_editor;

	void Sync();
	void Set(const Txt& header_qtf, const Txt& footer_qtf);
	void Get(Txt& header_qtf, Txt& footer_qtf);
	void Load(const RichText& text);
	void Save(RichText& text);

	HeaderFooterDlg();
};

void HeaderFooterDlg::Sync()
{
	header_editor.Enable(use_header);
	footer_editor.Enable(use_footer);
}

HeaderFooterDlg::HeaderFooterDlg()
{
	CtrlLayoutOKCancel(*this, "Заглавие / Подвал");
	use_header <<= use_footer <<= OTVET(Sync);
	Sync();
}

void HeaderFooterDlg::Set(const Txt& header_qtf, const Txt& footer_qtf)
{
	use_header = !IsNull(header_qtf);
	if(use_header)
		header_editor.SetQTF(header_qtf);
	header_editor.EvaluateFields();
	use_footer = !IsNull(footer_qtf);
	if(use_footer)
		footer_editor.SetQTF(footer_qtf);
	footer_editor.EvaluateFields();
	Sync();
}

void HeaderFooterDlg::Get(Txt& header_qtf, Txt& footer_qtf)
{
	header_qtf = use_header ? header_editor.GetQTF() : Txt();
	footer_qtf = use_footer ? footer_editor.GetQTF() : Txt();
}

void HeaderFooterDlg::Load(const RichText& text)
{
	Set(text.GetHeaderQtf(), text.GetFooterQtf());
}

void HeaderFooterDlg::Save(RichText& text)
{
	if(use_header)
		text.SetHeaderQtf(header_editor.GetQTF());
	else
		text.ClearHeader();
	if(use_footer)
		text.SetFooterQtf(footer_editor.GetQTF());
	else
		text.ClearFooter();
}

void RichEdit::HeaderFooter()
{
	HeaderFooterDlg dlg;
	dlg.Load(text);
	dlg.SetRect(0, 0, GetSize().cx, dlg.GetLayoutSize().cy);
	if(dlg.Execute() == IDOK) {
		dlg.Save(text);
		Refresh();
		Finish();
	}
}

bool EditRichHeaderFooter(Txt& header_qtf, Txt& footer_qtf)
{
	HeaderFooterDlg dlg;
	dlg.Set(header_qtf, footer_qtf);
	if(dlg.Execute() == IDOK) {
		dlg.Get(header_qtf, footer_qtf);
		return true;
	}
	return false;
}

}