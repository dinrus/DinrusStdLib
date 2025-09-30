#include "../Core.h"

namespace drx {

Val ParseJSON(CParser& p)
{
	p.UnicodeEscape();
	if(p.IsDouble())
		return p.ReadDouble();
	if(p.IsTxt()) {
		bool dt = p.IsChar2('\"', '\\');
		Txt s = p.ReadTxt();
		if(dt) {
			CParser p(s);
			if(p.Char('/') && p.Id("Date") && p.Char('(') && p.IsInt()) {
				z64 n = p.ReadInt64();
				if(!IsNull(n))
					return Time(1970, 1, 1) + n / 1000;
			}
		}
		return s;
	}
	if(p.Id("null"))
		return Null;
	if(p.Id("true"))
		return true;
	if(p.Id("false"))
		return false;
	if(p.Char('{')) {
		ValMap m;
		while(!p.Char('}')) {
			Txt key = p.ReadTxt();
			p.PassChar(':');
			m.Add(key, ParseJSON(p));
			if(p.Char('}')) // Stray ',' at the end of list is allowed...
				break;
			p.PassChar(',');
		}
		return m;
	}
	if(p.Char('[')) {
		ValArray va;
		while(!p.Char(']')) {
			va.Add(ParseJSON(p));
			if(p.Char(']')) // Stray ',' at the end of list is allowed...
				break;
			p.PassChar(',');
		}
		return va;
	}
	p.ThrowError("Нераспознанный элемент JSON");
	return Null;
}

Val ParseJSON(tukk s)
{
	try {
		CParser p(s);
		return ParseJSON(p);
	}
	catch(CParser::Error e) {
		return ErrorVal(e);
	}
}

Txt AsJSON(Time tm)
{
	return IsNull(tm) ? "null" : "\"\\/Date(" + AsTxt(1000 * (tm - Time(1970, 1, 1))) + ")\\/\"";
}

Txt AsJSON(Date dt)
{
	return AsJSON(ToTime(dt));
}

Json& Json::CatRaw(tukk key, const Txt& val)
{
	if(text.GetCount())
		text << ',';
	text << AsJSON(key) << ":" << val;
	return *this;
}

JsonArray& JsonArray::CatRaw(const Txt& val)
{
	if(text.GetCount())
		text << ',';
	text << val;
	return *this;
}

Txt AsJSON(const Val& v, const Txt& sep, bool pretty)
{
	Txt r;
	if(v.GetType() == VALUEMAP_V) {
		r << "{";
		Txt sep1;
		if(pretty) {
			r << "\r\n";
			sep1 = sep + '\t';
		}
		ValMap m = v;
		ValArray va = m.GetVals();
		for(i32 i = 0; i < m.GetCount(); i++) {
			if(i) {
				r << ",";
				if(pretty)
					r << "\r\n";
			}
			if(pretty)
				r << sep1;
			r << AsJSON((Txt)m.GetKey(i)) << (pretty ? ": " : ":")
			  << AsJSON(va[i], sep1, pretty);
		}
		if(pretty)
			r << "\r\n" << sep;
		r << "}";
		return r;
	}
	if(v.GetType() == VALUEARRAY_V) {
		r << "[";
		Txt sep1;
		if(pretty) {
			r << "\r\n";
			sep1 = sep + '\t';
		}
		ValArray va = v;
		for(i32 i = 0; i < va.GetCount(); i++) {
			if(i) {
				r << ",";
				if(pretty)
					r << "\r\n";
			}
			if(pretty)
				r << sep1;
			r << AsJSON(va[i], sep1, pretty);
		}
		if(pretty)
			r << "\r\n" << sep;
		r << "]";
		return r;
	}
	if(IsNumber(v) && (IsNull(v) || IsNaN((double)v)))
		return "null";
	if(v.GetType() == INT_V)
		return Format("%d", (i32)v);
	if(v.GetType() == BOOL_V)
		return (bool)v ? "true" : "false";
	if(IsNumber(v))
		return FormatG((double)v, 15);
	if(IsTxt(v))
		return AsCTxt((Txt)v, INT_MAX, NULL, ASCTXT_JSON);
	if(IsDateTime(v))
		return AsJSON((Time)v);
	if(IsNull(v))
		return "null";
	NEVER_("Non-JSON value in AsJSON: " + v.GetTypeName());
	return "null";
}

void JsonIO::Set(tukk key, const Val& v)
{
	ASSERT(IsStoring());
	if(!map)
		map.Create();
	map->Add(key, v);
}

Txt AsJSON(const Val& v, bool pretty)
{
	return AsJSON(v, Txt(), pretty);
}

template<> void Jsonize(JsonIO& io, double& var)
{
	if(io.IsLoading()) {
		const Val& v = io.Get();
		if(IsNull(v)) {
			var = Null;
			return;
		}
		if(IsNumber(v)) {
			var = io.Get();
			return;
		}
		if(IsTxt(v)) {
			double h = ScanDouble((Txt)v);
			if(!IsNull(h)) {
				var = h;
				return;
			}
		}
		throw JsonizeError("ожидалось число");
	}
	else
		io.Set(var);
}

template<> void Jsonize(JsonIO& io, i32& var)
{
	double v = IntDbl(var);
	Jsonize(io, v);
	if(io.IsLoading()) {
		if(IsNull(v))
			var = Null;
		else
		if(v >= INT_MIN && v <= INT_MAX && (i32)v == v)
			var = (i32)v;
		else
			throw JsonizeError("число не целое");
	}
}

template<> void Jsonize(JsonIO& io, u8& var)
{
	double v = var;
	Jsonize(io, v);
	if(io.IsLoading()) {
		if(v >= 0 && v <= 255 && (i32)v == v)
			var = (u8)v;
		else
			throw JsonizeError("ожидалось целое 0-255");
	}
}

template<> void Jsonize(JsonIO& io, i16& var)
{
	double v = var;
	Jsonize(io, v);
	if(io.IsLoading()) {
		if(v >= -32768 && v <= 32767 && (i32)v == v)
			var = (i16)v;
		else
			throw JsonizeError("ожидалось 16-битное целое");
	}
}

template<> void Jsonize(JsonIO& io, bool& var)
{
	if(io.IsLoading()) {
		const Val& v = io.Get();
		if(IsNumber(v) && !IsNull(v))
			var = (bool)v;
		else
			throw JsonizeError("ожидалось булево");
	}
	else
		io.Set(var);
}

template<> void Jsonize(JsonIO& io, z64& var)
{
	if(io.IsLoading()) {
		const Val& v = io.Get();
		if(IsNull(v)) {
			var = Null;
			return;
		}
		if(v.Is<z64>() || v.Is<i32>()) {
			var = v;
			return;
		}
		if(IsNumber(v)) {
			double d = v;
			if(FitsInInt64(d)) {
				var = (z64)d;
				return;
			}
		}
		else
		if(IsTxt(v)) {
			z64 h = ScanInt64((Txt)v);
			if(!IsNull(h)) {
				var = h;
				return;
			}
		}
		throw JsonizeError("неверное значение z64");
	}
	else
		if(IsNull(var))
			io.Set(Null);
		else
		if(var >= I64(-9007199254740992) && var <= I64(9007199254740991))
			io.Set(var);
		else
			io.Set(AsTxt(var));
}

template<> void Jsonize(JsonIO& io, Txt& var)
{
	if(io.IsLoading()) {
		const Val& v = io.Get();
		if(IsNull(v))
			var = Null;
		else
		if(IsTxt(v))
			var = v;
		else
			throw JsonizeError("ожидался ткст");
	}
	else
		io.Set(var);
}

template<> void Jsonize(JsonIO& io, WTxt& var)
{
	if(io.IsLoading()) {
		const Val& v = io.Get();
		if(IsNull(v))
			var = Null;
		else
		if(IsTxt(v))
			var = v;
		else
			throw JsonizeError("ожидался ткст");
	}
	else
		io.Set(var);
}

template<> void Jsonize(JsonIO& io, Date& var)
{
	if(io.IsLoading()) {
		const Val& v = io.Get();
		if(IsNull(v)) {
			var = Null;
			return;
		}
		if(IsTxt(v)) {
			Txt text = Filter(~v, CharFilterDigit);
			if(text.GetCount() >= 8) {
				Date d;
				d.year = ScanInt(text.Left(4));
				d.month = ScanInt(text.Mid(4, 2));
				d.day = ScanInt(text.Mid(6, 2));
				if(d.IsValid()) {
					var = d;
					return;
				}
			}
		}
		throw JsonizeError("ожидался ткст со значением Даты");
	}
	else
		if(IsNull(var))
			io.Set(Null);
		else
			io.Set(Format("%04d-%02d-%02d", var.year, var.month, var.day));
}

template<> void Jsonize(JsonIO& io, Time& var)
{
	if(io.IsLoading()) {
		const Val& v = io.Get();
		if(IsNull(v)) {
			var = Null;
			return;
		}
		if(IsTxt(v)) {
			Txt text = Filter(~v, CharFilterDigit);
			if(text.GetCount() >= 12) { //seconds may be missing
				Time tm;
				tm.year = ScanInt(text.Left(4));
				tm.month = ScanInt(text.Mid(4, 2));
				tm.day = ScanInt(text.Mid(6, 2));
				tm.hour = ScanInt(text.Mid(8, 2));
				tm.minute = ScanInt(text.Mid(10, 2));
				tm.second = text.GetCount() > 12 ? ScanInt(text.Mid(12)) : 0;
				if(tm.IsValid()) {
					var = tm;
					return;
				}
			}
		}
		throw JsonizeError("ожидался ткст со значением Времени");
	}
	else
		if(IsNull(var))
			io.Set(Null);
		else
			io.Set(Format("%04d-%02d-%02d`T%02d:%02d:%02d",
				          var.year, var.month, var.day, var.hour, var.minute, var.second));
}

Txt sJsonFile(tukk file)
{
	return file ? Txt(file) : ConfigFile(GetExeTitle() + ".json");
}

}