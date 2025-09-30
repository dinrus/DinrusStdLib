#include "CtrlLib.h"


namespace drx {

Size  LNGCtrl::GetMinSize() const
{
	i32 c = EditField::GetStdHeight();
	i32 d = 9 * c / 4;
	return Size(2 * d + 2, hi.GetMinSize().cy);
}

Val LNGCtrl::GetData() const
{
	return ~lo;
}

void  LNGCtrl::SetData(const Val& v)
{
	i32 l = v;
	hi <<= l & LNG_(0xff, 0xff, 0, 0);
	LoadLo();
	lo <<= l;
	Update();
}

void LNGCtrl::LoadLo() {
	lo.Clear();
	Index<i32> h;
	i32 hv = ~hi;
	i32 set = 0;
	if(hv == 0) {
		lo.Add(0, "--");
		lo <<= 0;
		return;
	}
	lo.Add(hv, "");
	for(i32k *l = GetAllLanguages(); *l; l++)
		if((*l & LNG_(0xff, 0xff, 0, 0)) == hv) {
			h.FindAdd(*l);
			if(!set)
				set = *l;
		}
	Vec<i32> q = h.PickKeys();
	Sort(q);
	for(i32 i = 0; i < q.GetCount(); i++)
		lo.Add(q[i], LNGAsText(q[i]).Mid(3));
	lo <<= set;
}

void LNGCtrl::Hi()
{
	LoadLo();
	UpdateAction();
}

void LNGCtrl::Lo()
{
	UpdateAction();
}

LNGCtrl::LNGCtrl() {
	i32 c = EditField::GetStdHeight();
	i32 d = 9 * c / 4;
	Add(hi.LeftPos(0, d).VSizePos());
	Add(lo.LeftPos(d + 2, d).VSizePos());

	Index<i32> h;
	for(i32k *l = GetAllLanguages(); *l; l++)
		h.FindAdd(*l & LNG_(0xff, 0xff, 0, 0));
	Vec<i32> q = h.PickKeys();
	Sort(q);
	hi.Add(0, "--");
	for(i32 i = 0; i < q.GetCount(); i++)
		hi.Add(q[i], LNGAsText(q[i]));
	hi <<= OTVET(Hi);
	lo <<= OTVET(Lo);
}

LNGCtrl::~LNGCtrl()
{
}

}
