#include "CtrlLib.h"

namespace drx {

i32 DropList::FindKey(const Val& k) const
{
	return key.Find(k);
}

void DropList::Sync() {
	Val v;
	if(displayall)
		v = value;
	i32 i = key.Find(value);
	const Display& d = valuedisplay ? *valuedisplay : i >= 0 ? list.GetDisplay(i)
	                                                         : list.GetDisplay();
	if(i >= 0) v = list.Get(i);
	MultiButton::SetDisplay(d);
	MultiButton::SetValCy(list.GetLineCy());
	v = valueconvert->Format(v);
	Set(v, false);
}

void DropList::Change(i32 q) {
	if(key.GetCount() == 0) return;
	i32 i = key.Find(value);
	for(i32 l = 0; l < list.GetCount(); l++) {
		i += q;
		if(i >= key.GetCount()) i = key.GetCount() - 1;
		if(i < 0) i = 0;
		if(list.IsLineEnabled(i) && list.GetLineCy(i) > 0) {
			if(value != key[i]) {
				value = key[i];
				Sync();
				UpdateAction();
			}
			return;
		}
	}
}

bool DropList::Key(dword k, i32) {
	if(IsReadOnly()) return false;
	switch(k) {
	case K_ALT_DOWN:
		Drop();
		break;
	case K_DOWN:
	case K_RIGHT:
		Change(1);
		break;
	case K_UP:
	case K_LEFT:
		Change(-1);
		break;
	default:
		if(k >= 32 && k < K_CHAR_LIM) {
			bool b = list.Key(k);
			if(list.GetCursor() >= 0 && list.GetCursor() < key.GetCount() && key[list.GetCursor()] != value)
				Select();
			return b;
		}
		return false;
	}
	return true;
}

void DropList::MouseWheel(Point, i32 zdelta, dword)
{
	if(usewheel)
		Change(zdelta < 0 ? 1 : -1);
}

void DropList::Drop() {
	if(IsReadOnly()) return;
	if(dropfocus)
		SetFocus();
	WhenDrop();
	list.SetCursor(key.Find(value));
	list.PopUp(this, dropwidth);
}

void DropList::DropPush()
{
	Drop();
}

void DropList::Select() {
	i32 c = list.GetCursor();
	if(c >= 0)
		value = key[c];
	if(dropfocus)
		SetFocus();
	Sync();
	UpdateAction();
}

void DropList::Cancel() {
	if(dropfocus)
		SetFocus();
	Sync();
}

void DropList::Remove(i32 i)
{
	key.Remove(i);
	list.Remove(i);
	Adjust();
}

void DropList::ClearList() {
	key.Clear();
	list.Clear();
	Sync();
	EnableDrop(false);
}

void DropList::Clear() {
	ClearList();
	value = Val();
	Sync();
	Update();
}

DropList& DropList::Add(const Val& _key, const Val& text, bool enable)
{
	key.Add(_key);
	list.Add(text);
	if(!enable)
		list.SetLineCy(list.GetCount() - 1, 0);
	EnableDrop();
	Sync();
	return *this;
}

DropList& DropList::Add(std::initializer_list<std::pair<Val, Val>> init)
{
	for(const auto& i : init)
		Add(i.first, i.second);
	return *this;
}

struct DummyVal__ {};

DropList& DropList::AddSeparator()
{
	key.Add(RawToVal(DummyVal__()));
	list.AddSeparator();
	EnableDrop();
	Sync();
	return *this;
}

void DropList::Trim(i32 n) {
	if(GetIndex() >= n)
		SetIndex(0);
	key.Trim(n);
	list.SetCount(n);
	Sync();
	EnableDrop(n);
}

void DropList::SetData(const Val& data) {
	if(value != data) {
		value = data;
		Update();
		Sync();
	}
}

Val DropList::GetData() const {
	return notnull && IsNull(value) ? NotNullError() : value;
}

Val DropList::GetVal() const {
	i32 i = FindKey(value);
	return i < 0 ? Null : GetVal(i);
}

void  DropList::SetVal(i32 i, const Val& v) {
	list.Set(i, v);
	EnableDrop();
	Sync();
}

void  DropList::SetVal(const Val& v) {
	i32 i = FindKey(value);
	if(i >= 0)
		SetVal(i, v);
	Sync();
}

DropList& DropList::SetValConvert(const Convert& cv)
{
	valueconvert = &cv;
	Sync();
	return *this;
}

DropList& DropList::SetConvert(const Convert& cv)
{
	list.SetConvert(cv);
	return SetValConvert(cv);
}

DropList& DropList::SetDisplay(i32 i, const Display& d)
{
	list.SetDisplay(i, d);
	Sync();
	return *this;
}

DropList& DropList::SetDisplay(const Display& d)
{
	list.SetDisplay(d);
	Sync();
	return *this;
}

DropList& DropList::SetDisplay(const Display& d, i32 lcy)
{
	SetDisplay(d);
	SetLineCy(lcy);
	Sync();
	return *this;
}

DropList& DropList::ValDisplay(const Display& d)
{
	valuedisplay = &d;
	Sync();
	return *this;
}

void  DropList::Adjust()
{
	i32 i = FindKey(value);
	if(i < 0) {
		if(GetCount())
			SetIndex(0);
		else
			SetData(Null);
	}
}

void DropList::Adjust(const Val& k)
{
	i32 i = FindKey(value);
	if(i < 0 && GetCount())
		SetData(k);
}

Val DropList::Format(const Val& q) const {
	i32 i = FindKey(q);
	return valueconvert->Format(i < 0 ? displayall ? q : Val() : GetVal(i));
}

DropList& DropList::AlwaysDrop(bool e)
{
	alwaysdrop = e;
	if(e)
		EnableDrop();
	return *this;
}

DropList::DropList()
{
	displayall = false;
	valueconvert = &NoConvert();
	valuedisplay = NULL;
	dropfocus = false;
	notnull = false;
	alwaysdrop = false;
	SetupDropPush();
	NoInitFocus();
	EnableDrop(false);
	list.WhenSelect = OTVET(Select);
	list.WhenCancel = OTVET(Cancel);
	dropwidth = 0;
	usewheel = true;
}

DropList::~DropList() {}

void Append(DropList& list, const VecMap<Val, Val>& values)
{
	for(i32 i = 0; i < values.GetCount(); i++)
		list.Add(values.GetKey(i), values[i]);
}

void Append(MapConvert& convert, const VecMap<Val, Val>& values)
{
	for(i32 i = 0; i < values.GetCount(); i++)
		convert.Add(values.GetKey(i), values[i]);
}

void Append(DropList& list, const VecMap<i32, Txt>& values)
{
	for(i32 i = 0; i < values.GetCount(); i++)
		list.Add(values.GetKey(i), values[i]);
}

void Append(MapConvert& convert, const VecMap<i32, Txt>& values)
{
	for(i32 i = 0; i < values.GetCount(); i++)
		convert.Add(values.GetKey(i), values[i]);
}

void Append(DropList& list, const MapConvert& convert)
{
	for(i32 i = 0; i < convert.GetCount(); i++)
		list.Add(convert.GetKey(i), convert.GetVal(i));
}

void operator*=(DropList& list, const VecMap<Val, Val>& values)
{
	list.ClearList();
	Append(list, values);
}

void operator*=(MapConvert& convert, const VecMap<Val, Val>& values)
{
	convert.Clear();
	Append(convert, values);
}

void operator*=(DropList& list, const VecMap<i32, Txt>& values)
{
	list.ClearList();
	Append(list, values);
}

void operator*=(MapConvert& convert, const VecMap<i32, Txt>& values)
{
	convert.Clear();
	Append(convert, values);
}

void operator*=(DropList& list, const MapConvert& convert)
{
	list.ClearList();
	Append(list, convert);
}

}
