#include "CtrlLib.h"

namespace drx {

#define LDUMP(x) // DUMP(x)

Switch& Switch::Set(i32 i, const Val& val) {
	cs.At(i).value = val;
	Refresh();
	return *this;
}

Switch& Switch::Set(i32 i, const Val& val, tukk text, i32 gap) {
	Case& v = cs.At(i);
	if(value.IsVoid())
		value = val;
	SetLabel(i, text, gap);
	v.value = val;
	Refresh();
	return *this;
}

Switch& Switch::Add(const Val& val, tukk text, i32 gap) {
	Set(cs.GetCount(), val, text);
	return *this;
}

Switch& Switch::Add(tukk text, i32 gap) {
	Set(cs.GetCount(), cs.GetCount(), text, gap);
	return *this;
}

i32 Switch::GetIndex() const {
	for(i32 i = 0; i < cs.GetCount(); i++)
		if(cs[i].value == value)
			return i;
	return -1;
}

Switch&  Switch::SetLabel(i32 i, tukk text, i32 gap) {
	if(i >= cs.GetCount())
		cs.At(i).value = i;
	Case& v = cs[i];
	v.accesskey = ExtractAccessKey(text, v.label);
	v.gap = gap;
	Refresh();
	return *this;
}

dword Switch::GetAccessKeys() const
{
	dword keys = 0;
	for(i32 i = 0; i < cs.GetCount(); i++)
		keys |= AccessKeyBit(cs[i].accesskey);
	return keys;
}

void  Switch::AssignAccessKeys(dword used)
{
	for(i32 i = 0; i < cs.GetCount(); i++) {
		Case& v = cs[i];
		if(!v.accesskey) {
			v.accesskey = ChooseAccessKey(v.label, used);
			if(v.accesskey) Refresh();
			used |= AccessKeyBit(v.accesskey);
		}
	}
	Ctrl::AssignAccessKeys(used);
}

Switch& Switch::SetLabel(tukk text) {
	tukk q = text;
	i32 i = 0;
	tukk s = text;
	i32 gap = 0;
	while(*s) {
		if(*s == '^') {
			if(s[1] == '^')
				s += 2;
			else {
				gap = atoi(Txt(q, s));
				q = ++s;
			}
		}
		else
		if(*s == '|') {
			if(s[1] == '|')
				s += 2;
			else {
				gap = atoi(Txt(q, s)) | GAP_SEPARATOR;
				q = ++s;
			}
		}
		else
		if(*s == '\r') {
			SetLabel(i++, Txt(q, s), gap);
			gap = 0;
			q = ++s;
			if(*q == '\n')
				q = ++s;
		}
		else
		if(*s == '\n') {
			SetLabel(i++, Txt(q, s), gap);
			gap = 0;
			q = ++s;
		}
		else
			s++;
	}
	SetLabel(i++, Txt(q, s), gap);
	cs.SetCount(i);
	return *this;
}

void  Switch::SetData(const Val& val) {
	if(val != value) {
		value = val;
		Update();
		Refresh();
	}
}

Val Switch::GetData() const {
	return value;
}

void Switch::EnableCase(i32 i, bool enable) {
	cs.At(i).enabled = enable;
	Refresh();
}

void Switch::EnableVal(const Val& val, bool enable) {
	for(i32 i = 0; i < cs.GetCount(); i++)
		if(cs[i].value == val)
			EnableCase(i, enable);
}

void Switch::GotFocus() {
	Refresh();
}

void Switch::LostFocus() {
	Refresh();
}

void Switch::Paint(Draw& w) {
	if(cs.GetCount() == 0) return;
	Size sz = GetSize();
	if(!IsTransparent())
		w.DrawRect(0, 0, sz.cx, sz.cy, SColorFace);
	i32 tcy = GetTextSize("W", font).cy;
	Size isz = CtrlsImg::S0().GetSize();
	linecy = max(mincy, max(isz.cy + DPI(2), tcy));
	bool horz = false;
	if(direction == 1) horz = true;
	for(i32 pass = !!direction; pass < 2; pass++) { // first pass to decide horz, second to actually draw
		i32 y = 0;
		i32 x = 0;
		
		if(horz)
			linecy = sz.cy;
	
		i32 ty = (linecy - tcy) / 2;
		bool ds = !IsShowEnabled();
		i32 i;
		light = -1;
		for(i = 0; i < cs.GetCount(); i++) {
			Case& v = cs[i];

			if(v.gap) {
				i32 gsz = (v.gap & 255) * tcy / 4;
				if(pass && (v.gap & GAP_SEPARATOR)) {
					if(horz)
						w.DrawRect(x + y + gsz / 2, y, DPI(1), linecy, SColorDisabled());
					else
						w.DrawRect(x, y + gsz / 2, sz.cx, DPI(1), SColorDisabled());
				}
				(horz ? x : y) += gsz;
			}
			
			bool dv = ds || !v.enabled;
	
			Size tsz = GetSmartTextSize(v.label, font);
			i32 iy = (linecy - isz.cy) / 2;
			i32 width = horz ? tsz.cx + isz.cx + DPI(4) : sz.cx;
			Rect hr = RectC(x, y, width, linecy);
			bool mousein = HasMouseIn(hr);
			if(mousein)
				light = i;
			if(pass) {
				Image img;
				i32 q = dv ? CTRL_DISABLED :
				        pushindex == i ? CTRL_PRESSED :
				        mousein ? CTRL_HOT :
				        CTRL_NORMAL;
				img = CtrlsImg::Get((v.value == value ? CtrlsImg::I_S1 : CtrlsImg::I_S0) + q);
				w.DrawImage(x, y + iy, img);
				DrawSmartText(w, x + isz.cx + DPI(4), y + ty, sz.cx, v.label, font,
				              dv || IsReadOnly() ? SColorDisabled : GetLabelTextColor(this),
				              VisibleAccessKeys() ? v.accesskey : 0);
				if(HasFocus() && (pushindex == i || v.value == value && pushindex < 0))
					DrawFocus(w, RectC(x + isz.cx + DPI(2), y + ty - DPI(1), tsz.cx + DPI(3), tsz.cy + DPI(2)) & sz);
			}
			v.rect = hr;
			if(horz)
				x += hr.Width() + tcy / 2;
			else
				y += linecy;
		}
		
		if(y > sz.cy)
			horz = true;
	}
	
}

i32 Switch::GetIndex(Point p) {
	for(i32 i = 0; i < cs.GetCount(); i++) {
		if(cs[i].rect.Contains(p) && cs[i].enabled)
			return i;
	}
	return -1;
}

void Switch::MouseMove(Point p, dword keyflags) {
	if(IsReadOnly()) return;
	i32 i = GetIndex(p);
	i32 a = -1;
	if(keyflags & K_MOUSELEFT)
		a = i;
	if(pushindex != a) {
		pushindex = a;
		Refresh();
	}
	if(i != light)
		Refresh();
}

void Switch::LeftDown(Point p, dword keyflags) {
	if(IsReadOnly()) return;
	if(Ctrl::ClickFocus()) SetWantFocus();
	pushindex = GetIndex();
	MouseMove(p, keyflags);
}

void Switch::LeftUp(Point p, dword keyflags) {
	if(IsReadOnly()) return;
	if(pushindex >= 0 && pushindex < cs.GetCount()) {
		const Val& v = cs[pushindex].value;
		if(v != value) {
			value = v;
			UpdateAction();
		}
		Refresh();
	}
	pushindex = -1;
}

void Switch::MouseLeave() {
	pushindex = -1;
	Refresh();
}

bool Switch::Key(dword key, i32) {
	if(IsReadOnly()) return false;
	i32 index, i;
	index = i = GetIndex();
	if(key == K_UP || key == K_LEFT) {
		for(i32 t = 0; t < cs.GetCount(); t++) {
			if(--index < 0) index = cs.GetCount() - 1;
			if(cs[index].enabled)
				break;
		}
	}
	else
	if(key == K_DOWN || key == K_RIGHT) {
		for(i32 t = 0; t < cs.GetCount(); t++) {
			if(++index >= cs.GetCount()) index = 0;
			if(cs[index].enabled)
				break;
		}
	}
	else
		return false;
	if(i != index && cs[index].enabled) {
		value = cs[index].value;
		UpdateActionRefresh();
	}
	return true;
}

bool Switch::HotKey(dword key) {
	if(IsReadOnly()) return false;
	for(i32 i = 0; i < cs.GetCount(); i++) {
		if(cs[i].enabled && CompareAccessKey(cs[i].accesskey, key)) {
			value = cs[i].value;
			UpdateActionRefresh();
			return true;
		}
	}
	return false;
}

void Switch::CancelMode() {
	pushindex = -1;
}

Switch::Switch() {
	direction = 0;
	linecy = 16;
	mincy = 0;
	Transparent();
	NoInitFocus();
	font = StdFont();
}

Switch::~Switch() {}

}
