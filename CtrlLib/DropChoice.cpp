#include "CtrlLib.h"

namespace drx {

DropChoice::DropChoice() {
	always_drop = hide_drop = false;
	AddButton().Main() <<= OTVET(Drop);
	NoDisplay();
	list.WhenSelect = [=, this] { Select(); };
	dropfocus = true;
	EnableDrop(false);
	dropwidth = 0;
	appending = false;
	updownkeys = true;
	rodrop = false;
}

void DropChoice::AddTo(Ctrl& _owner)
{
	MultiButtonFrame::AddTo(_owner);
	owner = &_owner;

	if(auto *ef = dynamic_cast<EditField *>(owner))
		ef->WhenPaper = [=, this](Color c) { MultiButtonFrame::SetPaper(c); };
}

void DropChoice::EnableDrop(bool b)
{
	MainButton().Enable(b);
	if(hide_drop)
		MainButton().Show(b);
}

void DropChoice::PseudoPush()
{
	MultiButton::PseudoPush(0);
}

void DropChoice::Drop() {
	if(!owner || owner->IsReadOnly() && !rodrop || list.GetCount() == 0 && !WhenDrop) return;
	WhenDrop();
	if(dropfocus)
		owner->SetWantFocus();
	i32 i = list.Find(owner->GetData());
	if(i >= 0)
		list.SetCursor(i);
	else
	if(list.GetCount() > 0)
		list.SetCursor(0);
	list.PopUp(owner,dropwidth);
}

void DropChoice::Select() {
	if(!owner || owner->IsReadOnly() && !rodrop) return;
	WhenSelect();
}

Val DropChoice::Get() const {
	if(!owner || owner->IsReadOnly() && !rodrop) return Val();
	i32 c = list.GetCursor();
	if(c < 0) return Val();
	return list.Get(c);
}

i32 DropChoice::GetIndex() const
{
	if(!owner || owner->IsReadOnly() && !rodrop) return -1;
	return list.GetCursor();
}

bool DropChoice::DataSelect(Ctrl& owner, DropChoice& drop, const Txt& appends) {
	Val g = drop.Get();
	if(g.IsVoid()) return false;
	Val s = owner.GetData();
	if(!appends.IsVoid()) {
		Txt txt = s;
		if(!txt.IsEmpty()) txt.Cat(appends);
		txt.Cat((Txt)g);
		s = txt;
	}
	else
		s = g;
	owner.SetData(s);
	owner.WhenAction();
	return true;
}

void DropChoice::DoWheel(i32 zdelta)
{
	if(!appending)
		DoKey(zdelta < 0 ? K_UP : K_DOWN);
}

bool DropChoice::DoKey(dword key) {
	if(owner && !owner->IsReadOnly() && list.GetCount()) {
		i32 q = list.GetCursor();
		switch(key) {
		case K_ALT_DOWN:
			PseudoPush();
			return true;
		case K_DOWN:
			if(!updownkeys)
				return false;
			if(appending)
				PseudoPush();
			else {
				list.SetCursor(q <= 0 ? list.GetCount() - 1 : q - 1);
				Select();
			}
			return true;
		case K_UP:
			if(!updownkeys)
				return false;
			if(appending)
				PseudoPush();
			else {
				list.SetCursor(q < 0 || q >= list.GetCount() - 1 ? 0 : q + 1);
				Select();
			}
			return true;
		}
	}
	return false;
}

void DropChoice::Add(const Val& s)
{
	list.Add(s);
	EnableDrop(true);
}

void DropChoice::FindAdd(const Val& s)
{
	if (list.Find(s) < 0)
		list.Add(s);
	EnableDrop(true);
}

void DropChoice::Remove(i32 i)
{
	list.Remove(i);
	EnableDrop(list.GetCount());
}

void DropChoice::Clear() {
	list.Clear();
	EnableDrop(always_drop);
}

void DropChoice::Serialize(Stream& s) {
	// This is unfortunately a fix for bug and its consequences in existing .cfg files...
	i32 version = 0x01;
	s / version;
	if(version < 0x01) {
		i32 n = list.GetCount();
		s / n;
		Val v;
		if(s.IsLoading())
			for(i32 i = 0; i < n; i++)
				s % v;
	}
}

void DropChoice::SerializeList(Stream& s) {
	i32 version = 0x00;
	i32 n = list.GetCount();
	s / version / n;
	Val v;
	if(s.IsLoading()) {
		Clear();
		for(i32 i = 0; i < n; i++) {
			s % v;
			Add(v);
		}
	}
	else
		for(i32 i = 0; i < n; i++) {
			v = list.Get(i);
			s % v;
		}
	EnableDrop(list.GetCount() || always_drop);
}

void DropChoice::AddHistory(const Val& v, i32 max) {
	if(IsNull(v)) return;
	for(i32 i = 0; i < list.GetCount(); i++)
		if(list.Get(i) == v) {
			list.Remove(i);
			break;
		}
	list.Insert(0, v);
	if(list.GetCount() > max)
		list.SetCount(max);
	EnableDrop(list.GetCount() || always_drop);
	list.SetCursor(-1);
}

DropChoice& DropChoice::AlwaysDrop(bool e)
{
	always_drop = e;
	EnableDrop(list.GetCount() || always_drop);
	return *this;
}

}
