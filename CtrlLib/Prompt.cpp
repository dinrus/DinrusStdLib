#include "CtrlLib.h"

namespace drx {

struct PromptDlgWnd__ : TopWindow {
	bool    esc;
	Button *b;

	virtual bool HotKey(dword key) {
		if(TopWindow::HotKey(key))
			return true;
		if(IsAlpha(key))
			return TopWindow::HotKey(K_ALT_A + ToUpper((i32)key) - 'A');
		if(key == K_ESCAPE && esc)
			b->PseudoPush();
		return false;
	}
};

static void sAdd(Ctrl& dlg, i32 fcy, i32 bcy, i32& bx, i32 bcx, i32 gap, Button& b, tukk button, const Image& img)
{
	if(button) {
		dlg << b.BottomPos(fcy, bcy).LeftPos(bx, bcx);
		b.SetLabel(button);
		if(!IsNull(img))
			b.SetImage(img);
		bx += gap + bcx;
	}
}

void sExecutePrompt(PromptDlgWnd__ *dlg, i32 *result)
{
	dlg->Open();
	Vec<Ctrl *> wins = Ctrl::GetTopWindows();
	for(i32 i = 0; i < wins.GetCount(); i++) {
		TopWindow *w = dynamic_cast<TopWindow *>(wins[i]);
		if(w && w->IsTopMost()) {
			dlg->TopMost();
			break;
		}
	}
	*result = dlg->RunAppModal();
	dlg->Close();
}
                        
RedirectPromptFn RedirectPrompt;

void RedirectPrompts(RedirectPromptFn r)
{
	RedirectPrompt = r;
}

VecMap<Txt, i32> dsa_history;

void ClearPromptOptHistory()
{
	GuiLock __;
	dsa_history.Clear();
}

void ClearPromptOptHistory(Gate<Txt> filter)
{
	GuiLock __;
	for(i32 i = 0; i < dsa_history.GetCount(); i++)
		if(filter(dsa_history.GetKey(i)))
			dsa_history.Unlink(i);
	dsa_history.Sweep();
}

void SerializePromptOptHistory(Stream& s)
{
	i32 version = 0;
	s / version;
	s % dsa_history;
}

i32 Prompt(i32 dontshowagain, tukk dsa_id_, i32 beep,
           Event<const Txt&> WhenLink,
           tukk title, const Image& iconbmp, tukk qtf, bool okcancel,
           tukk button1, tukk button2, tukk button3,
		   i32 cx,
		   Image im1, Image im2, Image im3)
{
	if(RedirectPrompt)
		return (*RedirectPrompt)(WhenLink, title, iconbmp, qtf, okcancel,
                                 button1, button2, button3,
                                 cx, im1, im2, im3);
	Txt dsa_id;
	if(dontshowagain) {
		dsa_id = dsa_id_;
		if(IsNull(dsa_id)) {
			Txt body = title;
			body << "\1" << qtf;
			if(button1) body << "\1" << button1;
			if(button2) body << "\1" << button2;
			if(button3) body << "\1" << button3;
			dsa_id = SHA256Txt(body);
		}
		GuiLock __;
		i32 q = dsa_history.Find(dsa_id);
		if(q >= 0)
			return dsa_history[q];
	}
	switch(beep) {
	case BEEP_INFORMATION:
		BeepInformation();
		break;
	case BEEP_EXCLAMATION:
		BeepExclamation();
		break;
	case BEEP_QUESTION:
		BeepQuestion();
		break;
	case BEEP_ERROR:
		BeepError();
		break;
	}
	i32 fcy = Draw::GetStdFontCy();
	EnterGuiMutex(); // Ctrl derived classes can only be initialized with GuiLock
	PromptDlgWnd__ dlg;
	RichTextCtrl qtfctrl;
	Icon         icon;
	Option       dsa;
	dsa.SetLabel(t_("Больше не показывать"));
	qtfctrl.WhenLink = WhenLink;
	icon.SetImage(iconbmp);
	Button b1, b2, b3;
	qtfctrl.CopyWithTabs();
	qtfctrl.SetQTF(Txt("[g ") + qtf, GetRichTextStdScreenZoom());
	i32 bcy = Ctrl::VertLayoutZoom(24);
	i32 bcx = Ctrl::HorzLayoutZoom(72);
	if(button1)
		bcx = max(2 * fcy + GetTextSize(button1, Draw::GetStdFont()).cx, bcx);
	if(button2)
		bcx = max(2 * fcy + GetTextSize(button2, Draw::GetStdFont()).cx, bcx);
	if(button3)
		bcx = max(2 * fcy + GetTextSize(button3, Draw::GetStdFont()).cx, bcx);
	Size bsz = icon.GetStdSize();
	if(cx == 0) {
		cx = qtfctrl.GetWidth();
		if(!cx)
			cx = 350;
		if(dontshowagain)
			cx = max(cx, dsa.GetMinSize().cx);
		cx += 2 * fcy;
		if(bsz.cx)
			cx += bsz.cx + fcy;
	}
	i32 nbtn = !!button1 + !!button2 + !!button3;
	dlg.esc = okcancel && nbtn == 1;
	cx = min(Zx(520), max(nbtn * bcx + (1 + nbtn) * fcy, cx));
	i32 qcx = cx - 2 * fcy;
	if(bsz.cx)
		qcx -= bsz.cx + fcy;
	i32 ccy = qtfctrl.GetHeight(qcx);
	i32 qcy = min(Zy(400), ccy);
	if(qcy <= ccy) {
		qcx += ScrollBarSize() + fcy;
		cx += ScrollBarSize() + fcy;
	}
	i32 mcy = max(qcy, bsz.cy);
	i32 cy = mcy + 48 * fcy / 10;
	if(dontshowagain)
		cy += fcy;
	dlg.SetRect(Size(cx, cy));
	dlg << icon.TopPos(fcy, bsz.cy).LeftPos(fcy, bsz.cx);
	dlg << qtfctrl.TopPos(fcy + (mcy - qcy) / 2, qcy).RightPos(fcy, qcx);
	if(okcancel) {
		b1.Ok();
		if(nbtn == 2)
			b2.Cancel();
		if(nbtn == 3)
			b3.Cancel();
	}
	b1.WhenAction = dlg.Breaker(1);
	b2.WhenAction = dlg.Breaker(0);
	b3.WhenAction = dlg.Breaker(-1);
	dlg.b = &b1;
	i32 bx = bcx;
	i32 gap = fcy / 2;
	fcy = 8 * fcy / 10;
	if(button2)
		bx += gap + bcx;
	if(button3)
		bx += gap + bcx;
	bx = (cx - bx) / 2;
	if(dontshowagain) {
		dlg << dsa.BottomPos(bcy + 2 * fcy).RightPos(fcy, qcx);
	}
	if(SwapOKCancel()) {
		sAdd(dlg, fcy, bcy, bx, bcx, gap, b2, button2, im2);
		sAdd(dlg, fcy, bcy, bx, bcx, gap, b3, button3, im3);
		sAdd(dlg, fcy, bcy, bx, bcx, gap, b1, button1, im1);
	}
	else {
		sAdd(dlg, fcy, bcy, bx, bcx, gap, b1, button1, im1);
		sAdd(dlg, fcy, bcy, bx, bcx, gap, b2, button2, im2);
		sAdd(dlg, fcy, bcy, bx, bcx, gap, b3, button3, im3);
	}
	dlg.WhenClose = dlg.Breaker(button3 ? -1 : 0);
	dlg.Title(title);
	LeaveGuiMutex();
	i32 result;
	Ctrl::Call(callback2(sExecutePrompt, &dlg, &result));
	if(dontshowagain && dsa && (dontshowagain > 0 || result > 0))
		dsa_history.Add(dsa_id, result);
	return result;
}

i32 Prompt(Event<const Txt&> WhenLink, i32 beep,
           tukk title, const Image& icon, tukk qtf, bool okcancel,
           tukk button1, tukk button2, tukk button3,
		   i32 cx, Image im1, Image im2, Image im3)
{
	return Prompt(false, NULL, beep, WhenLink, title, icon, qtf, okcancel, button1, button2, button3, cx, im1, im2, im3);
}

i32 Prompt(i32 beep, tukk title, const Image& icon, tukk qtf, bool okcancel,
           tukk button1, tukk button2, tukk button3,
		   i32 cx)
{
	return Prompt(callback(LaunchWebBrowser), beep, title,
	              icon, qtf, okcancel, button1, button2, button3, cx, Null, Null, Null);
}

i32 Prompt(Event<const Txt&> WhenLink, i32 beep,
            tukk title, const Image& icon, tukk qtf, bool okcancel,
            tukk button1, tukk button2, tukk button3, i32 cx)
{
	return Prompt(false, NULL, beep, WhenLink, title, icon, qtf, okcancel,
	              button1, button2, button3, cx, Null, Null, Null);
}

i32 Prompt(i32 beep, tukk title, const Image& icon, tukk qtf,
           tukk button1, tukk button2, tukk button3,
		   i32 cx)
{
	return Prompt(beep, title, icon, qtf, true, button1, button2, button3, cx);
}

/// 

i32 Prompt(Event<const Txt&> WhenLink,
           tukk title, const Image& icon, tukk qtf, bool okcancel,
           tukk button1, tukk button2, tukk button3,
		   i32 cx, Image im1, Image im2, Image im3)
{
	return Prompt(false, NULL, BEEP_NONE, WhenLink, title, icon, qtf, okcancel, button1, button2, button3, cx, im1, im2, im3);
}

i32 Prompt(tukk title, const Image& icon, tukk qtf, bool okcancel,
           tukk button1, tukk button2, tukk button3,
		   i32 cx)
{
	return Prompt(callback(LaunchWebBrowser), title,
	              icon, qtf, okcancel, button1, button2, button3, cx, Null, Null, Null);
}

i32 Prompt(Event<const Txt&> WhenLink,
            tukk title, const Image& icon, tukk qtf, bool okcancel,
            tukk button1, tukk button2, tukk button3, i32 cx)
{
	return Prompt(false, NULL, BEEP_NONE, WhenLink, title, icon, qtf, okcancel,
	              button1, button2, button3, cx, Null, Null, Null);
}

i32 Prompt(tukk title, const Image& icon, tukk qtf,
           tukk button1, tukk button2, tukk button3,
		   i32 cx)
{
	return Prompt(title, icon, qtf, true, button1, button2, button3, cx);
}

void PromptOK(tukk qtf) {
	Prompt(BEEP_INFORMATION, Ctrl::GetAppName(), CtrlImg::information(), qtf, t_("OK"));
}

void Exclamation(tukk qtf) {
	Prompt(BEEP_EXCLAMATION, Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, t_("OK"));
}

void ShowExc(const Exc& exc) {
	Prompt(BEEP_EXCLAMATION, Ctrl::GetAppName(), CtrlImg::exclamation(), DeQtf(exc), t_("OK"));
}

void ErrorOK(tukk qtf) {
	Prompt(BEEP_ERROR, Ctrl::GetAppName(), CtrlImg::error(), qtf, t_("OK"));
}

i32 PromptOKCancel(tukk qtf) {
	return Prompt(BEEP_QUESTION, Ctrl::GetAppName(), CtrlImg::question(), qtf, t_("OK"), t_("Отмена"));
}

i32 PromptOKCancelAll(tukk qtf) {
	return Prompt(BEEP_QUESTION, Ctrl::GetAppName(), CtrlImg::question(), qtf, t_("OK"), t_("Отмена"), t_("Все"));
}

i32 ErrorOKCancel(tukk qtf) {
	return Prompt(BEEP_QUESTION, Ctrl::GetAppName(), CtrlImg::error(), qtf, t_("OK"), t_("Отмена"));
}

CH_IMAGE(YesButtonImage, Null);
CH_IMAGE(NoButtonImage, Null);
CH_IMAGE(AbortButtonImage, Null);
CH_IMAGE(RetryButtonImage, Null);

i32 PromptYesNo(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_QUESTION,
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, false,
	              t_("&Да"), t_("&Нет"), NULL, 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptYesNoAll(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_QUESTION,
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, false,
	              t_("&Да"), t_("&Нет"), t_("Все"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNo(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_ERROR,
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Да"), t_("&Нет"), NULL, 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptYesNoCancel(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_QUESTION,
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("Отмена"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNoCancel(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_ERROR,
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("Отмена"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNoAll(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_ERROR,
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("Все"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptAbortRetry(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_EXCLAMATION,
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), NULL, 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 ErrorAbortRetry(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_ERROR,
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), NULL, 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 PromptRetryCancel(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_EXCLAMATION,
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, true,
	              t_("&Повторить"), t_("Отмена"), NULL, 0,
	              RetryButtonImage(), Null, Null);
}

i32 ErrorRetryCancel(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_ERROR,
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Повторить"), t_("Отмена"), NULL, 0,
	              RetryButtonImage(), Null, Null);
}

i32 PromptAbortRetryIgnore(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_EXCLAMATION,
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), t_("&Игнорировать"), 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 ErrorAbortRetryIgnore(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_ERROR,
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), t_("&Игнорировать"), 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 PromptSaveDontSaveCancel(tukk qtf) {
	return Prompt(callback(LaunchWebBrowser), BEEP_QUESTION,
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, true,
	              t_("&Сохранить"), t_("&Не сохранять"), t_("Отммена"), 0,
	              CtrlImg::save(), NoButtonImage(), Null);
}

i32 PromptOpt(tukk opt_id, i32 beep, Event<const Txt&> WhenLink,
              tukk title, const Image& icon, tukk qtf, bool okcancel,
              tukk button1, tukk button2, tukk button3,
		      i32 cx, Image im1, Image im2, Image im3)
{
	return Prompt(true, opt_id, beep, WhenLink, title, icon, qtf, okcancel, button1, button2, button3, cx, im1, im2, im3);
}

i32 PromptOpt(tukk opt_id, i32 beep,
              tukk title, const Image& icon, tukk qtf, bool okcancel,
              tukk button1, tukk button2, tukk button3,
		      i32 cx)
{
	return PromptOpt(opt_id, beep, callback(LaunchWebBrowser), title,
	              icon, qtf, okcancel, button1, button2, button3, cx, Null, Null, Null);
}

i32 PromptOpt(tukk opt_id, i32 beep,
              tukk title, const Image& icon, tukk qtf,
              tukk button1, tukk button2, tukk button3,
		      i32 cx)
{
	return PromptOpt(opt_id, beep, title, icon, qtf, true, button1, button2, button3, cx);
}

void PromptOKOpt(tukk qtf, tukk opt_id) {
	PromptOpt(opt_id, BEEP_INFORMATION, Ctrl::GetAppName(), CtrlImg::information(), qtf, t_("OK"));
}

void ExclamationOpt(tukk qtf, tukk opt_id) {
	PromptOpt(opt_id, BEEP_EXCLAMATION,  Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, t_("OK"));
}

void ShowExcOpt(const Exc& exc, tukk opt_id) {
	PromptOpt(opt_id, BEEP_EXCLAMATION, Ctrl::GetAppName(), CtrlImg::exclamation(), DeQtf(exc), t_("OK"));
}

void ErrorOKOpt(tukk qtf, tukk opt_id) {
	PromptOpt(opt_id, BEEP_ERROR, Ctrl::GetAppName(), CtrlImg::error(), qtf, t_("OK"));
}

i32 PromptOKCancelOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_QUESTION, Ctrl::GetAppName(), CtrlImg::question(), qtf, t_("OK"), t_("Отмена"));
}

i32 PromptOKCancelAllOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_QUESTION, Ctrl::GetAppName(), CtrlImg::question(), qtf, t_("OK"), t_("Отмена"), t_("Все"));
}

i32 ErrorOKCancelOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_ERROR, Ctrl::GetAppName(), CtrlImg::error(), qtf, t_("OK"), t_("Отмена"));
}

i32 PromptYesNoOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_QUESTION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, false,
	              t_("&Да"), t_("&Нет"), NULL, 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptYesNoAllOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_QUESTION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, false,
	              t_("&Да"), t_("&Нет"), t_("Все"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNoOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Да"), t_("&Нет"), NULL, 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptYesNoCancelOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_QUESTION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("Отмена"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNoCancelOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("ОТмена"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNoAllOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("Все"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptAbortRetryOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_EXCLAMATION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), NULL, 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 ErrorAbortRetryOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), NULL, 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 PromptRetryCancelOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_EXCLAMATION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, true,
	              t_("&Повторить"), t_("Отмена"), NULL, 0,
	              RetryButtonImage(), Null, Null);
}

i32 ErrorRetryCancelOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Повторить"), t_("Отмена"), NULL, 0,
	              RetryButtonImage(), Null, Null);
}

i32 PromptAbortRetryIgnoreOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_EXCLAMATION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), t_("&Игнорировать"), 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 ErrorAbortRetryIgnoreOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), t_("&Игнорировать"), 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 PromptSaveDontSaveCancelOpt(tukk qtf, tukk opt_id) {
	return PromptOpt(opt_id, BEEP_QUESTION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, true,
	              t_("&Сохранить"), t_("&Не сохранять"), t_("Отмена"), 0,
	              CtrlImg::save(), NoButtonImage(), Null);
}

i32 PromptOpt1(tukk opt_id, i32 beep, Event<const Txt&> WhenLink,
              tukk title, const Image& icon, tukk qtf, bool okcancel,
              tukk button1, tukk button2, tukk button3,
		      i32 cx, Image im1, Image im2, Image im3)
{
	return Prompt(-1, opt_id, beep, WhenLink, title, icon, qtf, okcancel, button1, button2, button3, cx, im1, im2, im3);
}

i32 PromptOpt1(tukk opt_id, i32 beep,
              tukk title, const Image& icon, tukk qtf, bool okcancel,
              tukk button1, tukk button2, tukk button3,
		      i32 cx)
{
	return PromptOpt1(opt_id, beep, callback(LaunchWebBrowser), title,
	              icon, qtf, okcancel, button1, button2, button3, cx, Null, Null, Null);
}

i32 PromptOpt1(tukk opt_id, i32 beep,
              tukk title, const Image& icon, tukk qtf,
              tukk button1, tukk button2, tukk button3,
		      i32 cx)
{
	return PromptOpt1(opt_id, beep, title, icon, qtf, true, button1, button2, button3, cx);
}

void PromptOKOpt1(tukk qtf, tukk opt_id) {
	PromptOpt1(opt_id, BEEP_INFORMATION, Ctrl::GetAppName(), CtrlImg::information(), qtf, t_("OK"));
}

void ExclamationOpt1(tukk qtf, tukk opt_id) {
	PromptOpt1(opt_id, BEEP_EXCLAMATION,  Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, t_("OK"));
}

void ShowExcOpt1(const Exc& exc, tukk opt_id) {
	PromptOpt1(opt_id, BEEP_EXCLAMATION, Ctrl::GetAppName(), CtrlImg::exclamation(), DeQtf(exc), t_("OK"));
}

void ErrorOKOpt1(tukk qtf, tukk opt_id) {
	PromptOpt1(opt_id, BEEP_ERROR, Ctrl::GetAppName(), CtrlImg::error(), qtf, t_("OK"));
}

i32 PromptOKCancelOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_QUESTION, Ctrl::GetAppName(), CtrlImg::question(), qtf, t_("OK"), t_("Отмена"));
}

i32 PromptOKCancelAllOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_QUESTION, Ctrl::GetAppName(), CtrlImg::question(), qtf, t_("OK"), t_("Отмена"), t_("Все"));
}

i32 ErrorOKCancelOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_ERROR, Ctrl::GetAppName(), CtrlImg::error(), qtf, t_("OK"), t_("Отмена"));
}

i32 PromptYesNoOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_QUESTION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, false,
	              t_("&Да"), t_("&Нет"), NULL, 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptYesNoAllOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_QUESTION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, false,
	              t_("&Да"), t_("&Нет"), t_("Все"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNoOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Да"), t_("&Нет"), NULL, 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptYesNoCancelOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_QUESTION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("ОТмена"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNoCancelOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("Отмена"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 ErrorYesNoAllOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Да"), t_("&Нет"), t_("Все"), 0,
	              YesButtonImage(), NoButtonImage(), Null);
}

i32 PromptAbortRetryOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_EXCLAMATION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), NULL, 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 ErrorAbortRetryOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), NULL, 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 PromptRetryCancelOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_EXCLAMATION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, true,
	              t_("&Повторить"), t_("Отмена"), NULL, 0,
	              RetryButtonImage(), Null, Null);
}

i32 ErrorRetryCancelOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, true,
	              t_("&Повторить"), t_("Отмена"), NULL, 0,
	              RetryButtonImage(), Null, Null);
}

i32 PromptAbortRetryIgnoreOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_EXCLAMATION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::exclamation(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), t_("&Игнорировать"), 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 ErrorAbortRetryIgnoreOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_ERROR, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::error(), qtf, false,
	              t_("&Прервать"), t_("&Повторить"), t_("&Игнорировать"), 0,
	              AbortButtonImage(), RetryButtonImage(), Null);
}

i32 PromptSaveDontSaveCancelOpt1(tukk qtf, tukk opt_id) {
	return PromptOpt1(opt_id, BEEP_QUESTION, callback(LaunchWebBrowser),
	              Ctrl::GetAppName(), CtrlImg::question(), qtf, true,
	              t_("&Сохранить"), t_("&Не сохранять"), t_("Отмена"), 0,
	              CtrlImg::save(), NoButtonImage(), Null);
}

}