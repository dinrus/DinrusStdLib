#include <drx/Core/Core.h>
#include <drx/Net/Rpc/Rpc.h>

namespace drx {

Txt FormatIso8601(Time t)
{
	return Format("%04.4d%02.2d%02.2d`T%02.2d`:%02.2d`:%02.2d",
	              t.year, t.month, t.day, t.hour, t.minute, t.second);
}

Time ScanIso8601(const Txt& p)
{
	Txt s = TrimBoth(p);
	// 19980717T14:08:55
	// 01234567890123456
	if(s.GetCount() != 17 || s[8] != 'T' || s[11] != ':' || s[14] != ':')
		return Null;
	Time tm;
	tm.year = atoi(s.Mid(0, 4));
	tm.month = atoi(s.Mid(4, 2));
	tm.day = atoi(s.Mid(6, 2));
	tm.hour = atoi(s.Mid(9, 2));
	tm.minute = atoi(s.Mid(12, 2));
	tm.second = atoi(s.Mid(15, 2));
	if(!tm.IsValid())
		return Null;
	return tm;
}

void ValCheck(bool b)
{
	if(!b)
		throw ValTypeMismatch();
}

void ValGet(i32& n, const Val& v)
{
	ValCheck(IsNull(v) || IsNumber(v));
	n = v;
}

void ValGet(z64& n, const Val& v)
{
	ValCheck(IsNull(v) || IsNumber(v));
	n = v;
}

void ValGet(Txt& s, const Val& v)
{
	ValCheck(IsNull(v) || IsTxt(v));
	s = v;
}

void ValGet(double& x, const Val& v)
{
	ValCheck(IsNull(v) || IsNumber(v));
	x = v;
}

void ValGet(bool& x, const Val& v)
{
	ValCheck(IsNull(v) || IsNumber(v));
	x = v;
}

Time IsoTime(const Val& v)
{
	Time tm = ScanIso8601((Txt)v);
	if(IsNull(tm))
		throw ValTypeMismatch();
	return tm;
}

void ValGet(Date& x, const Val& v)
{
	if(IsTxt(v)) {
		x = IsoTime(v);
		return;
	}
	ValCheck(IsNull(v) || IsDateTime(v));
	x = v;
}

void ValGet(Time& x, const Val& v)
{
	if(IsTxt(v)) {
		x = IsoTime(v);
		return;
	}
	ValCheck(IsNull(v) || IsDateTime(v));
	x = v;
}

void ValGet(Val& t, const Val& v)
{
	t = v;
}
	
void ValGet(ValArray& va, const Val& v)
{
	ValCheck(IsNull(v) || IsValArray(v));
	va = v;
}

void ValGet(ValMap& vm, const Val& v)
{
	ValCheck(IsNull(v) || IsValMap(v));
	vm = v;
}

void ValPut(Val& v, i32 n)
{
	v = n;
}

void ValPut(Val& v, z64 n)
{
	v = n;
}

void ValPut(Val& v, const Txt& s)
{
	v = s;
}

void ValPut(Val& v, tukk s)
{
	ValPut(v, Txt(s));
}

void ValPut(Val& v, double x)
{
	v = x;
}

void ValPut(Val& v, bool x)
{
	v = x;
}

void ValPut(Val& v, const Date& x)
{
	v = x;
}

void ValPut(Val& v, const Time& x)
{
	v = x;
}

void ValPut(Val& v, const Val& t)
{
	v = t;
}

void ValPut(Val& v, const ValArray& va)
{
	v = va;
}

void ValPut(Val& v, const ValMap& vm)
{
	v = vm;
}

void ValPut(Val& v, const Json& json)
{
	RawJsonText x;
	x.json = json;
	v = RawToVal(x);
}

}