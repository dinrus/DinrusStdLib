#include "Esc.h"

namespace drx {

#define LTIMING(x) // RTIMING(x)

Vec<EscVal>& EscVal::CloneArray()
{
	LTIMING("CloneArray");
	ASSERT(IsArray());
	if(array->refcount != 1) {
		EscArray *c = new EscArray;
		c->array <<= array->array;
		array->Release();
		array = c;
	}
	hash = 0;
	return array->array;
}

const Vec<EscVal>& EscVal::GetArray() const
{
	ASSERT(IsArray());
	return array->array;
}

EscVal EscVal::ArrayGet(i32 i) const
{
	return GetArray()[i];
}

EscVal EscVal::ArrayGet(i32 i, i32 n) const
{
	LTIMING("ArrayGet");
	const Vec<EscVal>& sa = GetArray();
	ASSERT(i >= 0 && i + n <= sa.GetCount());
	EscVal t;
	t.SetEmptyArray();
	t.array->array.Append(sa, i, n);
	return t;
}

void EscVal::SetEmptyArray()
{
	Free();
	type = ESC_ARRAY;
	hash = 0;
	array = new EscArray;
}

bool EscVal::ArraySet(i32 i, EscVal val)
{
	LTIMING("ArraySet");
	Vec<EscVal>& ta = CloneArray();
	if(i > max_total || i - ta.GetCount() + total > max_total)
		return false;
	CloneArray().At(i) = val;
	return true;
}

void EscVal::ArrayAdd(EscVal val)
{
	LTIMING("ArrayAdd");
	if(IsVoid())
		SetEmptyArray();
	CloneArray().Add(val);
}

bool EscVal::Replace(i32 i, i32 n, EscVal a)
{
	LTIMING("Array Replace");
	ASSERT(i >= 0);
	Vec<EscVal>& ta = CloneArray();
	if(i > max_total || i + n > max_total || i + n - ta.GetCount() + total > max_total)
		return false;
	if(i > 0)
		ta.At(i - 1);
	const Vec<EscVal>& sa = a.GetArray();
	i32 q = sa.GetCount() - n;
	if(q > 0)
		ta.InsertN(i, q);
	else
		ta.Remove(i, -q);
	for(q = 0; q < sa.GetCount(); q++)
		ta[q + i] = sa[q];
	return true;
}

bool EscVal::Append(EscVal a)
{
	if(IsVoid())
		SetEmptyArray();
	return Replace(GetCount(), 0, a);
}

EscVal::operator WTxt() const
{
	LTIMING("operator Txt");
	WTxtBuf s;
	if(IsArray()) {
		const Vec<EscVal>& a = GetArray();
		for(i32 i = 0; i < a.GetCount(); i++)
			if(a[i].IsInt()) {
				i32 c = a[i].GetInt();
				if(c >= 0 && c < 65536)
					s.Cat(c);
			}
	}
	return WTxt(s);
}

void EscVal::InitTxt(const WTxt& s)
{
	type = ESC_ARRAY;
	array = new EscArray();
	hash = 0;
	Vec<EscVal>& a = array->array;
	a.SetCount(s.GetCount());
	for(i32 i = 0; i < s.GetCount(); i++)
		a[i] = (z64)s[i];
	total++;
}

i32   EscVal::GetTotalCount()
{
	return total;
}

void  EscVal::SetMaxTotalCount(i32 n)
{
	max_total = n;
}

i32   EscVal::GetMaxTotalCount()
{
	return max_total;
}

EscVal::EscVal()
{
	type = ESC_VOID; hash = 0; total++;
}

EscVal::EscVal(double n)
{
	number = n;
	type = ESC_DOUBLE;
	hash = 0;
	total++;
}

EscVal::EscVal(z64 n)
{
	i64_ = n;
	type = ESC_INT64;
	hash = 0;
	total++;
}

EscVal::EscVal(i32 n)
{
	i64_ = n;
	type = ESC_INT64;
	hash = 0;
	total++;
}

}
