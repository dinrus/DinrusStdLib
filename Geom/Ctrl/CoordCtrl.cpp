#include "GeomCtrl.h"

namespace drx {

EditDegree::EditDegree()
{
	AddFrame(spin);
	spin.inc.WhenRepeat = spin.inc.WhenPush = OTVET(OnInc);
	spin.dec.WhenRepeat = spin.dec.WhenPush = OTVET(OnDec);
}

void EditDegree::OnInc()
{
	double a = GetData();
	a = floor(Nvl(a) + 1.5);
	if(a >= Nvl(GetMax(), +360.0))
		a = Nvl(GetMin(), -360.0);
	SetData(a);
	Action();
}

void EditDegree::OnDec()
{
	double a = GetData();
	a = ceil(Nvl(a) - 1.5);
	if(a <= Nvl(GetMin(), -360.0))
		a = Nvl(GetMax(), +360.0);
	SetData(a);
	Action();
}

bool EditDegree::Key(dword key, i32 repcnt)
{
	if(key == K_ALT_UP)
	{
		OnInc();
		return true;
	}
	if(key == K_ALT_DOWN)
	{
		OnDec();
		return true;
	}
	return EditVal<double, ConvertDegree>::Key(key, repcnt);
}

}
