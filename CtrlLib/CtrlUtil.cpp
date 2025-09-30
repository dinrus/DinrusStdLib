#include "CtrlLib.h"

namespace drx {

#define LLOG(x) // RLOG(x)

// #define SLOWANIMATION

#define IMAGECLASS CtrlImg
#define IMAGEFILE  <drx/CtrlLib/Ctrl.iml>
#include <drx/Draw/iml_source.h>

void Animate(Ctrl& c, const Rect& target, i32 type)
{
	if(type < 0)
		type = GUI_PopUpEffect();
	Rect r0 = c.GetRect();
	dword time0 = msecs();
	i32 anitime = 150;
#ifdef SLOWANIMATION
	anitime = 1500;
#endif
	if(type)
		for(;;) {
			i32 t = i32(msecs() - time0);
			if(t > anitime)
				break;
			if(type == GUIEFFECT_SLIDE) {
				Rect r = r0;
				if(r.left > target.left)
				   r.left -= ((r.left - target.left)* t) / anitime;
				if(r.top > target.top)
				   r.top -= ((r.top - target.top) * t) / anitime;
				if(r.right < target.right)
				   r.right += ((target.right - r.right) * t) / anitime;
				if(r.bottom < target.bottom)
				   r.bottom += ((target.bottom - r.bottom) * t) / anitime;
				if(r.GetWidth() > target.GetWidth())
				   r.right = r.left + target.GetWidth();
				if(r.GetHeight() > target.GetHeight())
				   r.bottom = r.top + target.GetHeight();
				c.SetRect(r);
				if(r == target)
					break;
			}
			else
			if(type == GUIEFFECT_FADE)
				c.SetAlpha((u8)(255 * t / anitime));
			else
				break;
			c.Sync();
			c.ProcessEvents();
			Sleep(0);
#ifdef SLOWANIMATION
			Sleep(100);
#endif
		}
	c.SetRect(target);
	c.SetAlpha(255);
}

void Animate(Ctrl& c, i32 x, i32 y, i32 cx, i32 cy, i32 type)
{
	Animate(c, RectC(x, y, cx, cy), type);
}

bool CtrlLibDisplayError(const Val& e) {
	if(!e.IsError())
		return false;
	Txt s = GetErrorText(e);
	if(s.IsEmpty())
		s = t_("Неверные данные.");
	Exclamation(s);
	return true;
}

INITBLOCK
{
	DisplayErrorFn() = &CtrlLibDisplayError;
}
/*
Txt SaveCtrlLayout(Ctrl::LogPos p, const Txt& classname, const Txt& variable,
					  const Txt& label, const Txt& help) {
	Txt out;
	if(classname.IsEmpty())
		out << "\tUNTYPED(";
	else
		out << "\tITEM(" << classname << ", ";
	out << variable << ", ";
	switch(p.x.GetAlign()) {
	case Ctrl::LEFT:   out << Format("LeftPos(%d, %d).", p.x.GetA(), p.x.GetB()); break;
	case Ctrl::RIGHT:  out << Format("RightPos(%d, %d).", p.x.GetA(), p.x.GetB()); break;
	case Ctrl::SIZE:   out << Format("HSizePos(%d, %d).", p.x.GetA(), p.x.GetB()); break;
	case Ctrl::CENTER: out << Format("HCenterPos(%d, %d).", p.x.GetB(), p.x.GetA()); break;
	}
	switch(p.y.GetAlign()) {
	case Ctrl::TOP:    out << Format("TopPos(%d, %d)", p.y.GetA(), p.y.GetB()); break;
	case Ctrl::BOTTOM: out << Format("BottomPos(%d, %d)", p.y.GetA(), p.y.GetB()); break;
	case Ctrl::SIZE:   out << Format("VSizePos(%d, %d)", p.y.GetA(), p.y.GetB()); break;
	case Ctrl::CENTER: out << Format("VCenterPos(%d, %d)", p.y.GetB(), p.y.GetA()); break;
	}
	if(!label.IsEmpty()) {
		out << ".SetLabel(\"";
		for(tukk s = label; *s; s++)
			if(*s == '\n')
				out.Cat("\\n");
			else
				out.Cat(*s);
		out << "\")";
	}
	if(!help.IsEmpty()) {
		out << ".HelpC(\"";
		for(tukk s = help; *s; s++)
			if(*s == '\n')
				out.Cat("\\n");
			else
				out.Cat(*s);
		out << "\")";
	}
	out << ")\n";
	return out;
}
*/
void Show2(Ctrl& ctrl1, Ctrl& ctrl2, bool show) {
	ctrl1.Show(show);
	ctrl2.Show(show);
}

void Hide2(Ctrl& ctrl1, Ctrl& ctrl2) {
	Show2(ctrl1, ctrl2, false);
}

void DelayCallback::Invoke() {
	KillTimeCallback(this);
	SetTimeCallback(delay, target, this);
}

void sSyncLabel(Label *lbl, tukk label, EditTxt *text)
{
	lbl->SetLabel(Txt().Cat() << label << " (" << text->GetLength() << "/" << text->GetMaxLength() << ")");
}

bool EditText(Txt& s, tukk title, tukk label, wchar (*f)(wchar), i32 maxlen, bool notnull)
{
	WithEditTxtLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, title);
	dlg.lbl = label;
	dlg.text <<= s.ToWTxt();
	dlg.text.NotNull(notnull);
	dlg.text.SetFilter(f);
	if(maxlen) {
		dlg.text.MaxLen(maxlen);
		dlg.text <<= callback3(sSyncLabel, &dlg.lbl, label, &dlg.text);
		dlg.text.WhenAction();
	}
	if(dlg.Execute() == IDOK) {
		s = ~dlg.text;
		return true;
	}
	return false;
}

bool EditText(Txt& s, tukk title, tukk label, wchar (*filter)(wchar), i32 maxlen)
{
	return EditText(s, title, label, filter, maxlen, false);
}

bool EditText(Txt& s, tukk title, tukk label, i32 maxlen)
{
	return EditText(s, title, label, CharFilterUnicode, maxlen, false);
}

bool EditTextNotNull(Txt& s, tukk title, tukk label, wchar (*filter)(wchar), i32 maxlen)
{
	return EditText(s, title, label, filter, maxlen, true);
}


bool EditText(WTxt& s, tukk title, tukk label, wchar (*f)(wchar), i32 maxlen, bool notnull)
{
	Txt ss = s.ToTxt();
	if(EditText(ss, title, label, f, maxlen, notnull)) {
		s = ss.ToWTxt();
		return true;
	}
	return false;
}

bool EditTextNotNull(Txt& s, tukk title, tukk label, i32 maxlen)
{
	return EditText(s, title, label, CharFilterUnicode, maxlen, true);
}

bool EditText(WTxt& s, tukk title, tukk label, wchar (*filter)(wchar), i32 maxlen)
{
	return EditText(s, title, label, filter, maxlen, false);
}

bool EditText(WTxt& s, tukk title, tukk label, i32 maxlen)
{
	return EditText(s, title, label, CharFilterUnicode, maxlen, false);
}

bool EditTextNotNull(WTxt& s, tukk title, tukk label, wchar (*filter)(wchar), i32 maxlen)
{
	return EditText(s, title, label, filter, maxlen, true);
}

bool EditTextNotNull(WTxt& s, tukk title, tukk label, i32 maxlen)
{
	return EditText(s, title, label, CharFilterUnicode, maxlen, true);
}

bool EditNumber(i32& n, tukk title, tukk label, i32 min, i32 max, bool notnull)
{
	WithEditIntLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, title);
	dlg.lbl = label;
	dlg.number <<= n;
	dlg.number.MinMax(min, max);
	dlg.number.NotNull(notnull);
	if(dlg.Execute() == IDOK) {
		n = ~dlg.number;
		return true;
	}
	return false;
}

bool EditNumber(double& n, tukk title, tukk label, double min, double max, bool notnull)
{
	WithEditDoubleLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, title);
	dlg.lbl = label;
	dlg.number <<= n;
	dlg.number.MinMax(min, max);
	dlg.number.NotNull(notnull);
	if(dlg.Execute() == IDOK) {
		n = ~dlg.number;
		return true;
	}
	return false;
}

bool EditDateDlg(Date& d, tukk title, tukk label, Date min, Date max, bool notnull)
{
	WithEditDateLayout<TopWindow> dlg;
	CtrlLayoutOKCancel(dlg, title);
	dlg.lbl = label;
	dlg.date <<= d;
	dlg.date.MinMax(min, max);
	dlg.date.NotNull(notnull);
	if(dlg.Execute() == IDOK) {
		d = ~dlg.date;
		return true;
	}
	return false;
}

Event<> CtrlRetriever::operator^=(Event<> cb)
{
	for(i32 i = 0; i < item.GetCount(); i++) {
		CtrlItem0 *m = dynamic_cast<CtrlItem0 *>(&item[i]);
		if(m)
			m->ctrl->WhenAction = cb;
	}
	return cb;
}

Event<> CtrlRetriever::operator<<(Event<> cb)
{
	for(i32 i = 0; i < item.GetCount(); i++) {
		CtrlItem0 *m = dynamic_cast<CtrlItem0 *>(&item[i]);
		if(m)
			m->ctrl->WhenAction << cb;
	}
	return cb;
}

void CtrlRetriever::Set()
{
	for(i32 i = 0; i < item.GetCount(); i++)
		item[i].Set();
}

void CtrlRetriever::Retrieve()
{
	for(i32 i = 0; i < item.GetCount(); i++)
		item[i].Retrieve();
}

void IdCtrls::Add(Id id, Ctrl& ctrl)
{
	Item& m = item.Add();
	m.id = id;
	m.ctrl = &ctrl;
}

ValMap IdCtrls::Get() const
{
	ValMap m;
	for(i32 i = 0; i < item.GetCount(); i++)
		m.Add(item[i].id, item[i].ctrl->GetData());
	return m;
}

void IdCtrls::Set(const ValMap& m)
{
	for(i32 i = 0; i < item.GetCount(); i++)
		item[i].ctrl->SetData(m[item[i].id]);
}

bool IdCtrls::Accept()
{
	for(i32 i = 0; i < item.GetCount(); i++)
		if(!item[i].ctrl->Accept()) return false;
	return true;
}

void IdCtrls::ClearModify() {
	for(i32 i = 0; i < item.GetCount(); i++)
		item[i].ctrl->ClearModify();
}

bool IdCtrls::IsModified() {
	for(i32 i = 0; i < item.GetCount(); i++)
		if(item[i].ctrl->IsModified()) return true;
	return false;
}

void IdCtrls::Enable(bool b)
{
	for(i32 i = 0; i < item.GetCount(); i++)
		item[i].ctrl->Enable(b);
}

void IdCtrls::SetNull()
{
	for(i32 i = 0; i < item.GetCount(); i++)
		item[i].ctrl->SetData(Null);
}

void Set(ArrayCtrl& array, i32 ii, IdCtrls& m)
{
	for(i32 i = 0; i < m.GetCount(); i++)
		array.Set(ii, m(i), ~m[i]);
}

void Get(ArrayCtrl& array, i32 ii, IdCtrls& m)
{
	for(i32 i = 0; i < m.GetCount(); i++)
		m[i] <<= array.Get(ii, m(i));
}

Event<> IdCtrls::operator<<(Event<> action)
{
	for(i32 i = 0; i < item.GetCount(); i++)
		item[i].ctrl->WhenAction << action;
	return action;
}

Event<> IdCtrls::operator^=(Event<> action)
{
	for(i32 i = 0; i < item.GetCount(); i++)
		item[i].ctrl->WhenAction = action;
	return action;
}

Txt sProfile(const MemoryProfile& mem)
{
	return AsTxt(mem);
}

void MemoryProfileInfo() {
	MemoryProfile mem;
	Txt text = sProfile(mem);
	const MemoryProfile *peak = PeakMemoryProfile();
	if(peak)
		text << "\r\n=== Подобрать профиль памяти\r\n" << sProfile(*peak);
	PromptOK("[C " + DeQtfLf(text));
};

FileSelButton::FileSelButton(MODE mode, tukk title)
: title(title), mode(mode)
{
	button.NoWantFocus();
	button.SetImage(mode == MODE_DIR ? CtrlImg::DirSmall() : CtrlImg::FileSmall());
	button << [=, this] { OnAction(); };
}

void FileSelButton::Attach(Ctrl& parent)
{
	button.Width(DPI(20));
	bool not_mb = true;
	for(i32 i = 0; i < parent.GetFrameCount(); i++)
		if(dynamic_cast<MultiButtonFrame *>(&parent.GetFrame(i))) {
			not_mb = false;
			break;
		}
	if(not_mb && dynamic_cast<EditField *>(&parent)) {
		parent.InsertFrame(0, button);
		parent.InsertFrame(1, RightGapFrame());
	}
	else
		parent.AddFrame(button);
}

void FileSelButton::OnAction()
{
	Ctrl *owner = button.GetParent();
	ASSERT(owner);
	Txt old = ~*owner;
	if(mode == MODE_DIR) {
		for(i32 i = 0; i < 4; i++) {
			if(DirExists(old))
				break;
			old = GetFileFolder(old);
		}
		ActiveDir(old);
	}
	else
		Set(old);
	if(mode == MODE_OPEN ? ExecuteOpen(title) : mode == MODE_SAVE ? ExecuteSaveAs(title) : ExecuteSelectDir(title))
	{
		*owner <<= Get();
		owner->Action();
		WhenSelected();
	}
}

void FileSelButton::Detach()
{
	Ctrl *p = button.GetParent();
	if(p) p->RemoveFrame(button);
}

struct ZoomIconMaker : ImageMaker {
	double zoom;

	Txt Key() const override {
		return Txt((u8 *)&zoom, sizeof(zoom));
	}
	Image Make() const override {
		Size sz(DPI(16), DPI(16));
		ImagePainter w(sz);
		w.Clear(RGBAZero());
		w.Move(DPI(11), DPI(11)).Line(DPI(16), DPI(16)).Stroke(DPI(2), SBlack());
		w.Circle(DPI(7), DPI(7), IsUHDMode() ? 12 : 6.5).Stroke(IsUHDMode() ? 3 : 1, SBlack());
		Txt txt = AsTxt(i32(zoom * 100));
		Image numbers = IsUHDMode() ? CtrlImg::Numbers2() : CtrlImg::Numbers1();
		i32 gcx = IsUHDMode() ? 6 : 4;
		Size tsz(txt.GetCount() * gcx, numbers.GetHeight());
		i32 y = DPI(7) - tsz.cy / 2;
		i32 x = DPI(7) - tsz.cx / 2;
		for(i32 c : txt) {
			w.DrawImage(x, y, gcx, tsz.cy, numbers, RectC((c - '0') * gcx, 0, gcx, tsz.cy), SLtBlue());
			x += gcx;
		}
		return w;
	}
};

Image MakeZoomIcon(double scale)
{
	ZoomIconMaker m;
	m.zoom = scale;
	return MakeImage(m);
}

}