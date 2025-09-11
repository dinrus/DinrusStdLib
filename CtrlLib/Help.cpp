#include "CtrlLib.h"
#include <drx/Synth/Synth.h>

namespace drx {


Topic HelpWindow::AcquireTopic(const Txt& topic)
{
	return GetTopic(topic);
}

void HelpWindow::BarEx(Bar& bar)
{}

void HelpWindow::FinishText(RichText& text)
{}

bool HelpWindow::GoTo0(const Txt& link)
{
	if(IsNull(link) || doing_goto)
		return true;
	bool lnk = false;
	if(current_link != link) {
		Topic t = AcquireTopic(link);
		SetBar();
		if(IsNull(t.text))
			return false;
		label = t.label;
		topic = t.link;
		doing_goto++; // suppress recursive GoTo
		if(~tree != topic)
			tree.FindSetCursor(topic);
		doing_goto--;
		Title(ToUtf32(t.title));
		RichText txt = ParseQTF(t.text);
		FinishText(txt);
		view.Pick(pick(txt), zoom);
		current_link = link;
		lnk = true;
	}
	if(WhenMatchLabel) {
		WTxt lw = label.ToWTxt();
		return view.GotoLabel([=, this](const WTxt& data) { return WhenMatchLabel(data, lw); }, true, true) || lnk;
	}
	return view.GotoLabel(label, true, true) || lnk;
}

HelpWindow::Pos HelpWindow::GetPos()
{
	Pos p;
	p.link = topic;
	p.scy = view.GetSb();
	return p;
}

bool HelpWindow::GoTo(const Txt& link)
{
	if(IsNull(link) || doing_goto)
		return false;
	Pos p = GetPos();
	if(GoTo0(link)) {
		if(!IsNull(p.link) && p.link != link) {
			back.Add(p);
			forward.Clear();
		}
		SetBar();
		return true;
	}
	if(link.StartsWith("www.") || link.StartsWith("http") || link.StartsWith("mailto:")) {
		LaunchWebBrowser(link);
		return true;
	}
	return false;
}

void HelpWindow::Back()
{
	Pos p = GetPos();
	if(back.GetCount()) {
		GoTo0(back.Top().link);
		if(back.GetCount()) {
			view.SetSb(back.Top().scy);
			back.Drop();
		}
		if(!IsNull(p.link))
			forward.Add(p);
		SetBar();
	}
}

void HelpWindow::Forward()
{
	Pos p = GetPos();
	if(forward.GetCount()) {
		GoTo0(forward.Top().link);
		if(forward.GetCount()) {
			view.SetSb(forward.Top().scy);
			forward.Drop();
		}
		if(!IsNull(p.link))
			back.Add(p);
		SetBar();
	}
}

void HelpWindow::SetZoom()
{
	zoom.d = 1000000 / Zy(800);
	current_link = Null;
	GoTo0(topic);
	Refresh();
	SetBar();
}

void HelpWindow::FontSize()
{
	i32 q = zoom.m / 20;
	if(q < 6 || q > 10)
		q = 6;
	else
		q++;
	zoom.m = 20 * q;
	SetZoom();
}

void HelpWindow::Print()
{
#ifndef TARGET_PDA
	Topic t = AcquireTopic(topic);
	DRX::Print(ParseQTF(t.text), Size(3968, 6074), 0);
#endif
}

void HelpWindow::Tools(Bar& bar)
{
	bar.Add(back.GetCount(), t_("Назад"), CtrlImg::go_back(), OTVET(Back))
	   .Key(K_ALT_LEFT);
	bar.Add(forward.GetCount(), t_("Вперёд"), CtrlImg::go_forward(), OTVET(Forward))
	   .Key(K_ALT_RIGHT);
	bar.Gap();
	bar.Add(t_("Font size"), MakeZoomIcon(zoom.m / 160.0), OTVET(FontSize));
	bar.Gap();
#ifndef TARGET_PDA
	bar.Add(t_("Печать"), CtrlImg::print(), OTVET(Print));
#endif
	BarEx(bar);
}

void HelpWindow::SetBar()
{
	toolbar.Set(OTVET(Tools));
}

bool HelpWindow::Key(dword key, i32 count)
{
	switch(key) {
	case K_ESCAPE:
		Close();
		return true;
	case K_MOUSE_BACKWARD:
		Back();
		return true;
	case K_MOUSE_FORWARD:
		Forward();
		return true;
	}
	return view.Key(key, count);
}

void HelpWindow::ClearTree()
{
	tree.Clear();
}

i32 HelpWindow::AddTree(i32 parent, const Image& img, const Txt& topic, const Txt& title)
{
	tree_view.NoZoom();
	return tree.Add(parent, img, topic, title, false);
}

void HelpWindow::SortTree(i32 id)
{
	tree.SortDeep(id);
}

void HelpWindow::SortTree(i32 id, i32 (*cmp)(const Val& v1, const Val& v2))
{
	tree.SortDeep(id, cmp);
}

void HelpWindow::SortTree(i32 id, i32 (*cmp)(const Val& k1, const Val& v1,
                                             const Val& k2, const Val& v2))
{
	tree.SortDeep(id, cmp);
}

void HelpWindow::FinishTree()
{
	if(!tree.FindSetCursor(topic))
		CurrentOrHome();
}

void HelpWindow::OpenDeep(i32 id)
{
	tree.OpenDeep(id);
}

void HelpWindow::Ids(i32 pid, Vec<i32>& r)
{
	i32 n = tree.GetChildCount(pid);
	for(i32 i = 0; i < n; i++) {
		i32 id = tree.GetChild(pid, i);
		if(!IsNull(tree.GetVal(id))) {
			r.Add(id);
			Ids(id, r);
		}
	}
}

Vec<i32> HelpWindow::Ids()
{
	Vec<i32> r;
	Ids(0, r);
	return r;
}

bool HelpWindow::PrevNext(i32 d, bool allowempty)
{
	Vec<i32> r = Ids();
	i32 id = tree.GetCursor();
	if(id < 0)
		return false;
	i32 ii = FindIndex(r, id);
	if(ii < 0)
		return false;
	for(;;) {
		ii += d;
		if(ii >= r.GetCount() || ii < 0)
			return false;
		if(!IsNull(tree.Get(r[ii])) || allowempty) {
			tree.SetCursor(r[ii]);
			return true;
		}
	}
}

bool HelpWindow::Next(bool allowempty)
{
	return PrevNext(1, allowempty);
}

bool HelpWindow::Prev(bool allowempty)
{
	return PrevNext(-1, allowempty);
}

void HelpWindow::Serialize(Stream& s)
{
	s % zoom.m;
	s % tree_view;
	SerializePlacement(s);
	SetZoom();
}

void HelpWindow::TreeCursor()
{
	if(!IsNull(tree))
		GoTo(~tree);
}

void HelpWindow::CurrentOrHome()
{
	if(~tree != current_link || IsNull(current_link)) {
		if(!IsNull(current_link) && tree.FindSetCursor(current_link))
			return;
		for(i32 i = 0; i < tree.GetLineCount(); i++) {
			Val k = tree.Get(tree.GetItemAtLine(i));
			if(!IsNull(k) && tree.FindSetCursor(k))
				break;
		}
	}
}

Vec<i32> HelpWindow::ScPositions(const Vec<i32>& p)
{
	Vec<i32> r;
	for(i32 i = 0; i < p.GetCount(); i++) {
		i32 y = max(0, view.GetZoom() * view.Get().GetCaret(p[i], view.GetPage()).top - GetSize().cy / 2);
		i32 ii = FindLowerBound(r, y);
		if(ii == r.GetCount() || r[ii] != y)
			r.Insert(ii, y);
	}
	return r;
}

bool HelpWindow::Up(const Vec<i32>& poslist)
{
	i32 q = view.GetSb();
	Vec<i32> p = ScPositions(poslist);
	for(i32 i = p.GetCount() - 1; i >= 0; i--)
		if(p[i] < q) {
			view.SetSb(p[i]);
			return view.GetSb() != q;
		}
	return false;
}

bool HelpWindow::Down(const Vec<i32>& poslist)
{
	i32 q = view.GetSb();
	Vec<i32> p = ScPositions(poslist);
	for(i32 i = 0; i < p.GetCount(); i++)
		if(p[i] > q) {
			view.SetSb(p[i]);
			return view.GetSb() != q;
		}
	return false;
}

HelpWindow::HelpWindow()
{
	tree_view.Horz(tree, view);
	tree_view.SetPos(3000);
	Add(tree_view.SizePos());
	tree_view.Zoom(1);
	view.Background(Red);
	Sizeable().Zoomable();
	Title(t_("Справка"));
	BackPaint();
	view.WhenLink = [=, this](const Txt& h) { GoTo(h); };
	AddFrame(toolbar);
	view.SetZoom(Zoom(1, 1));
	zoom.m = 160;
	SetZoom();
	view.Margins(Rect(12, 0, 12, 0));
	SetRect(Ctrl::GetWorkArea().Deflated(80));
	tree.WhenSel = OTVET(TreeCursor);
	tree.NoRoot();
	Icon(CtrlImg::help());
	SetBar();
	tree.BackPaint();
	view.BackPaintHint();
	view.WhenMouseWheel = [=, this] (i32 zdelta, dword keyflags) {
		if(keyflags & K_CTRL) {
			zoom.m = clamp((zoom.m / 5 + sgn(zdelta)) * 5, 60, 600);
			SetZoom();
			return true;
		}
		return false;
	};
}

}