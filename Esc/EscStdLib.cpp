#include <drx/Esc/Esc.h>


namespace drx {

#define LTIMING(x)  // RTIMING(x)

void ESC_count(EscEscape& e)
{
	e = e[0].GetCount();
}

void ESC_is_void(EscEscape& e)
{
	e = e[0].IsVoid();
}

void ESC_is_number(EscEscape& e)
{
	e = e[0].IsNumber();
}

void ESC_is_array(EscEscape& e)
{
	e = e[0].IsArray();
}

void ESC_is_map(EscEscape& e)
{
	e = e[0].IsMap();
}

void ESC_int(EscEscape& e)
{
	if(e[0].IsNumber())
		e = (i32)e[0].GetNumber();
}

void ESC_to_string(EscEscape& e)
{
	e = e[0].ToTxt();
}

void ESC_to_number(EscEscape& e)
{
	if(e[0].IsArray()) {
		double d = ScanDouble((Txt)e[0]);
		if(!IsNull(d)) {
			if(FitsInInt64(d))
				e = ScanInt64((Txt)e[0]);
			else
				e = d;
		}
	}
}

void ESC_rand(EscEscape& e)
{
	e = rand();
}

void ESC_keys(EscEscape& e)
{
	e.CheckMap(0);
	EscVal v;
	const VecMap<EscVal, EscVal>& m = e[0].GetMap();
	for(i32 i = 0; i < m.GetCount(); i++)
		if(!m.IsUnlinked(i))
			v.ArrayAdd(m.GetKey(i));
	e = v;
}

void ESC_values(EscEscape& e)
{
	e.CheckMap(0);
	EscVal v;
	const VecMap<EscVal, EscVal>& m = e[0].GetMap();
	for(i32 i = 0; i < m.GetCount(); i++)
		if(!m.IsUnlinked(i))
			v.ArrayAdd(m[i]);
	e = v;
}

void ESC_reverse(EscEscape& e)
{
	e.CheckArray(0);
	const Vec<EscVal>& a = e[0].GetArray();
	EscVal r;
	for(i32 i = a.GetCount() - 1; i >= 0; i--)
		r.ArrayAdd(a[i]);
	e = r;
}

struct EscCmp {
	Esc *esc;
	bool operator()(const EscVal& a, const EscVal& b) const {
		return esc->DoCompare(a, b, "< (sort)") < 0;
	}
};

Vec<i32> EscGetSortOrder(EscEscape& e)
{
	e.CheckArray(0);
	const Vec<EscVal>& va = e[0].GetArray();
	EscCmp cmp;
	cmp.esc = &e.esc;
	return GetSortOrder(va, cmp);
}

void ESC_sort(EscEscape& e)
{
	Vec<i32> so = EscGetSortOrder(e);
	EscVal r;
	for(i32 i = 0; i < so.GetCount(); i++)
		r.ArrayAdd(e[0].ArrayGet(so[i]));
	e = r;
}

void ESC_order(EscEscape& e)
{
	Vec<i32> so = EscGetSortOrder(e);
	EscVal r;
	for(i32 i = 0; i < so.GetCount(); i++)
		r.ArrayAdd(so[i]);
	e = r;
}

// ---------------------------

void ESC_mid(EscEscape& e)
{
	e.CheckArray(0);
	i32 pos = e.Int(1);
	i32 count = e.Int(2);
	if(pos < 0 || pos + count > e[0].GetCount())
		e.ThrowError("вне границ при вызове 'mid'");
	e.ret_val = e[0].ArrayGet(pos, count);
}

void ESC_exists(EscEscape& e)
{
	e.CheckMap(0);
	e = !e[0].MapGet(e[1]).IsVoid();
}

struct ESC_FileOut : public EscHandle {
	FileStream file;
	void Put(EscEscape& e)         { if(file) file.Put(e.Int(0)); }
	void PutLine(EscEscape& e)     { if(file) file.PutLine(Txt(e[0])); }
	void Close(EscEscape& e)       { if(file) file.Close(); }

	typedef ESC_FileOut CLASSNAME;

	ESC_FileOut(EscEscape& e, EscVal& v, i32 style) {
		file.Open(Txt(e[0]), style);
		v.Escape("Put(a)", this, OTVET(Put));
		v.Escape("PutLine(a)", this, OTVET(PutLine));
		v.Escape("Close()", this, OTVET(Close));
	}
};

void ESC_OpenFileOut(EscEscape& e)
{
	EscVal v;
	ESC_FileOut *f = new ESC_FileOut(e, v, FileStream::CREATE);
	if(f->file)
		e = v;
}

void ESC_OpenFileAppend(EscEscape& e)
{
	EscVal v;
	ESC_FileOut *f = new ESC_FileOut(e, v, FileStream::APPEND);
	if(f->file)
		e = v;
}

struct ESC_FileIn : public EscHandle {
	FileIn file;
	void IsEof(EscEscape& e)       { e = file.IsEof(); }
	void Get(EscEscape& e)         { e = file.Get(); }
	void GetLine(EscEscape& e)     { e = file.GetLine(); }
	void Close(EscEscape& e)       { if(file) file.Close(); }

	typedef ESC_FileIn CLASSNAME;

	ESC_FileIn(EscEscape& e, EscVal& v) {
		file.Open(Txt(e[0]));
		v.Escape("IsEof()", this, OTVET(IsEof));
		v.Escape("Get()", this, OTVET(Get));
		v.Escape("GetLine()", this, OTVET(GetLine));
		v.Escape("Close()", this, OTVET(Close));
	}
};

void ESC_OpenFileIn(EscEscape& e)
{
	EscVal v;
	ESC_FileIn *f = new ESC_FileIn(e, v);
	if(f->file)
		e = v;
}

// ---------------------------

bool IsDate(const EscVal& v)
{
	return v.HasNumberField("year") && v.HasNumberField("month") && v.HasNumberField("day");
}

bool IsTime(const EscVal& v)
{
	return IsDate(v) && v.HasNumberField("hour") && v.HasNumberField("minute") && v.HasNumberField("second");
}

void SIC_IsDate(EscEscape& e)
{
	e = IsDate(e[0]);
}

void SIC_IsTime(EscEscape& e)
{
	e = IsTime(e[0]);
}

void SIC_GetSysTime(EscEscape& e)
{
	Time tm = GetSysTime();
	EscVal v;
	v.MapSet("year", (i32)tm.year);
	v.MapSet("month", (i32)tm.month);
	v.MapSet("day", (i32)tm.day);
	v.MapSet("hour", (i32)tm.hour);
	v.MapSet("minute", (i32)tm.minute);
	v.MapSet("second", (i32)tm.second);
	e = v;
}

void ESC_ToLower(EscEscape& e)
{
	Txt s = e[0];
	e = ToLower(s);
}

void ESC_ToUpper(EscEscape& e)
{
	Txt s = e[0];
	e = ToUpper(s);
}

// ---------------------------

void ESC_replace(EscEscape& e)
{
	Txt str = e[0];
	str.Replace(e[1], e[2]);
	e = str;
}

// ---------------------------

void StdLib(ArrayMap<Txt, EscVal>& global)
{
	Escape(global, "is_number(value)", ESC_is_number);
	Escape(global, "is_array(value)", ESC_is_array);
	Escape(global, "is_map(value)", ESC_is_map);
	Escape(global, "is_void(value)", ESC_is_void);
	Escape(global, "i32(value)", ESC_int);
	Escape(global, "to_string(value)", ESC_to_string);
	Escape(global, "to_number(value)", ESC_to_number);
	Escape(global, "count(value)", ESC_count);
	Escape(global, "keys(map)", ESC_keys);
	Escape(global, "values(map)", ESC_values);
	Escape(global, "rand()", ESC_rand);
	Escape(global, "reverse(array)", ESC_reverse);
	Escape(global, "sort(array)", ESC_sort);
	Escape(global, "order(array)", ESC_order);
	
	Escape(global, "replace(str, find, replace)", ESC_replace);
	
	Escape(global, "ToUpper(value)", ESC_ToUpper);
	Escape(global, "ToLower(value)", ESC_ToLower);
	
	Escape(global, "len(x)", ESC_count);
	Escape(global, "mid(array, pos, count)", ESC_mid);
	Escape(global, "exists(map, key)", ESC_exists);

	Escape(global, "OpenFileOut(x)", ESC_OpenFileOut);
	Escape(global, "OpenFileAppend(x)", ESC_OpenFileOut);
	Escape(global, "OpenFileIn(x)", ESC_OpenFileIn);

	Escape(global, "GetSysTime()", SIC_GetSysTime);
	Escape(global, "IsDate(x)", SIC_IsDate);
	Escape(global, "IsTime(x)", SIC_IsTime);

	#define FN(fn) Escfn(global, #fn "(value)", [](EscEscape& e) { e = fn(e[0].GetNumber()); });
	
	FN(sin)
	FN(cos)
	FN(tan)
	FN(asin)
	FN(acos)
	FN(atan)

	FN(sinh)
	FN(cosh)
	FN(tanh)
	FN(asinh)
	FN(acosh)
	FN(atanh)
	
	FN(exp)
	FN(log)
	FN(log10)
	FN(exp2)
	FN(log2)
	FN(sqrt)
	FN(cbrt)
	
	#undef FN
	
	Escfn(global, "pow(base, exponent)", [](EscEscape& e) { e = pow(e[0].GetNumber(), e[1].GetNumber()); });
	Escfn(global, "atan2(a, b)", [](EscEscape& e) { e = atan2(e[0].GetNumber(), e[1].GetNumber()); });
}

}
