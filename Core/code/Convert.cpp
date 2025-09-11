#include "../Core.h"

namespace drx {

double ipow10_table[601];

u32 stou(tukk s, uk endptr, u32 base)
{
	ASSERT(base >= 2 && base <= 36);
	u32 digit = ctoi(*s);
	if(digit >= base)
	{ // error
		if(endptr)
			*(tukk *)endptr = s;
		return ~0;
	}
	u32 value = digit;
	while((digit = ctoi(*++s)) < base) {
		u32 v0 = value;
		value = value * base + digit;
		if(v0 > value) // overflow
			return ~0;
	}
	if(endptr)
		*(tukk *)endptr = s;
	return value;
}

u32 stou(wtukk s, uk endptr, u32 base)
{
	ASSERT(base >= 2 && base <= 36);
	u32 digit = ctoi(*s);
	if(digit >= base)
	{ // error
		if(endptr)
			*(wtukk *)endptr = s;
		return ~0;
	}
	u32 value = digit;
	while((digit = ctoi(*++s)) < base) {
		u32 v0 = value;
		value = value * base + digit;
		if(v0 > value) // overflow
			return ~0;
	}
	if(endptr)
		*(wtukk *)endptr = s;
	return value;
}

zu64 stou64(tukk s, uk endptr, u32 base)
{
	ASSERT(base >= 2 && base <= 36);
	u32 digit = ctoi(*s);
	if(digit >= base)
	{ // error
		if(endptr)
			*(tukk *)endptr = s;
		return ~0;
	}
	zu64 value = digit;
	while((digit = ctoi(*++s)) < base) {
		zu64 v0 = value;
		value = value * base + digit;
		if(v0 > value) // overflow
			return ~0;
	}
	if(endptr)
		*(tukk *)endptr = s;
	return value;
}

i32 ScanInt(tukk ptr, tukk *endptr, i32 base)
{
	tukk s = ptr;
	bool minus = false;
	while(*s && (u8)*s <= ' ')
		s++;
	if(*s == '+' || *s == '-')
		minus = (*s++ == '-');
	u32 u = stou(s, endptr, base);
	if(~u)
		return (minus ? -(i32)u : (i32)u);
	else
		return Null;
}

i32 ScanInt(wtukk ptr, wtukk *endptr, i32 base)
{
	wtukk s = ptr;
	bool minus = false;
	while(*s && *s <= ' ')
		s++;
	if(*s == '+' || *s == '-')
		minus = (*s++ == '-');
	u32 u = stou(s, endptr, base);
	if(~u)
		return (minus ? -(i32)u : (i32)u);
	else
		return Null;
}

z64 ScanInt64(tukk ptr, tukk *endptr, i32 base)
{
	tukk s = ptr;
	bool minus = false;
	while(*s && *s <= ' ')
		s++;
	if(*s == '+' || *s == '-')
		minus = (*s++ == '-');
	zu64 u = stou64(s, endptr, base);
	if(~u)
		return (minus ? -(z64)u : (z64)u);
	else
		return Null;
}

i32 ScanInt(tukk ptr, tukk *endptr)
{
	i32 x;
	bool overflow = false;
	ptr = ScanInt<char, u8, u32, i32, 10>(x, ptr, overflow);
	if(ptr && endptr)
		*endptr = ptr;
	return !overflow && ptr ? x : Null;
}

i32 ScanInt(tukk ptr)
{
	i32 x;
	bool overflow = false;
	return ScanInt<char, u8, u32, i32, 10>(x, ptr, overflow) && !overflow ? x : Null;
}

i32 Atoi(tukk ptr)
{
	i32 x;
	bool overflow = false;
	return ScanInt<char, u8, u32, i32, 10>(x, ptr, overflow) && !overflow ? x : 0;
}

z64 ScanInt64(tukk ptr, tukk *endptr)
{
	z64 x;
	bool overflow = false;
	ptr = ScanInt<char, u8, zu64, z64, 10>(x, ptr, overflow);
	if(ptr && endptr)
		*endptr = ptr;
	return !overflow && ptr ? x : Null;
}

z64 ScanInt64(tukk ptr)
{
	z64 x;
	bool overflow = false;
	return ScanInt<char, u8, zu64, z64, 10>(x, ptr, overflow) && !overflow ? x : Null;
}

z64 Atoi64(tukk ptr)
{
	z64 x;
	bool overflow = false;
	return ScanInt<char, u8, zu64, z64, 10>(x, ptr, overflow) && !overflow ? x : 0;
}

Val StrIntVal(tukk s)
{
	if(s && *s) {
		tukk p;
		z64 q = ScanInt64(s, &p);
		if(!IsNull(q))
			while(*p) {
				if((u8)*p > ' ')
					return ErrorVal(t_("Неверное число !"));
				p++;
			}
		return IsNull(q) ? ErrorVal(t_("Неверное число !")) : Val(q);
	}
	return (i32)Null;
}

Val StrDblVal(tukk s)
{
	if(s && *s) {
		tukk p;
		double q = ScanDouble(s, &p);
		if(!IsNull(q))
			while(*p) {
				if((u8)*p > ' ')
					return ErrorVal(t_("Неверное число !"));
				p++;
			}
		return IsNull(q) ? ErrorVal(t_("Неверное число !")) : Val(q);
	}
	return (double)Null;
}

Val Scan(dword qtype, const Txt& text, const Val& def, bool *hastime) {
	Date date;
	tukk s;
	if(hastime)
		*hastime = false;
	switch(qtype) {
	case INT64_V:
	case INT_V:
	case BOOL_V:
		return StrIntVal(text);
	case DATE_V:
		if(text.IsEmpty()) return (Date) Null;
		s = StrToDate(date, text, (Date)def);
		if(s)
			for(;;) {
				if(IsDigit(*s))
					break;
				if(*s == '\0')
					return date;
				s++;
			}
		return ErrorVal(t_("Неверная дата !"));
	case TIME_V: {
		if(text.IsEmpty()) return (Time) Null;
		s = StrToDate(date, text, (Date)def);
		if(s)
			try {
				CParser p(s);
				Time tm = ToTime(date);
				Time d = (Time)def;
				tm.hour = d.hour;
				tm.minute = d.minute;
				tm.second = d.second;
				if(p.IsEof())
					return tm;
				if(hastime)
					*hastime = true;
				i32 q = p.ReadInt();
				if(q < 0 || q > 23)
						p.ThrowError();//throw CParser::Error("");
				tm.hour = q;
				if(p.IsEof())
					return tm;
				p.PassChar(':');
				q = p.ReadInt();
				if(q < 0 || q > 59)
						p.ThrowError();//throw CParser::Error("");
				tm.minute = q;
				if(p.IsEof())
					return tm;
				p.PassChar(':');
				q = p.ReadInt();
				if(q < 0 || q > 59)
					p.ThrowError();//throw CParser::Error("");
				tm.second = q;
				if(p.IsEof())
					return tm;
			}
			catch(CParser::Error) {}
		return ErrorVal(t_("Неверное время !"));
	}
	case TXT_V:
	case WTXT_V:
		return text;
	case DOUBLE_V:
		return StrDblVal(text);
	default:
		ASSERT(0);
		break;
	}
	return Null;
}

Convert::Convert() {}
Convert::~Convert() {}

Val  Convert::Format(const Val& q) const {
	if(IsVoid(q) || q.IsNull()) return Txt();
	switch(q.GetType()) {
	case INT64_V:
		return IntStr64((z64)q);
	case INT_V:
	case BOOL_V:
		return IntStr((i32)q);
	case DOUBLE_V:
		return DblStr((double)q);
	case DATE_V:
		return DRX::Format(Date(q));
	case TIME_V:
		return DRX::Format(Time(q));
	case TXT_V:
	case WTXT_V:
		return q;
	}
	return q.ToTxt();
}

Val  Convert::Scan(const Val& text) const {
	return text;
};

wchar    Convert::Filter(wchar chr) const {
	return chr;
}

const Convert& StdConvert()
{
	static Convert h;
	return h;
}

Txt StdFormat(const Val& q) {
	return StdConvert().Format(q);
}

Val NotNullError() {
	return ErrorVal(t_("Значение Null недопустимо."));
}

Val ConvertInt::Scan(const Val& text) const {
	Val v = DRX::Scan(INT_V, text);
	if(IsError(v)) return v;
	if(IsNull(v)) return notnull ? NotNullError() : v;
	z64 m = v;
	if(m >= minval && m <= maxval) {
		if(m >= INT_MIN && m <= INT_MAX)
			return (i32)m;
		else
			return v;
	}
	return ErrorVal(DRX::Format(t_("Число должно входить в диапазон между %d и %d."), minval, maxval));
}

i32   ConvertInt::Filter(i32 chr) const {
	return minval >= 0 ? CharFilterDigit(chr) : CharFilterInt(chr);
}

Val ConvertDouble::Format(const Val& q) const
{
	if(IsNull(q))
		return Null;
	return DRX::Format(pattern, (double)q);
}

Val ConvertDouble::Scan(const Val& txt) const {
	Txt text = txt;
	if(pattern.GetCount() && pattern != "%.10g") { // Fix text with patterns like "%2.!n EUR" (e.g. 1.2 EUR)
		text = DRX::Filter(text, CharFilterDouble);
		while(ToUpper(*text.Last()) == 'E')
			text.Trim(text.GetCount() - 1);
	}
	Val v = DRX::Scan(DOUBLE_V, text);
	if(IsError(v)) return v;
	if(IsNull(v)) return notnull ? NotNullError() : v;
	double m = v;
	if(m >= minval && m <= maxval) return v;
	return ErrorVal(DRX::Format(t_("Число должно входить в диапазон между %g и %g."), minval, maxval));
}

wchar   ConvertDouble::Filter(wchar chr) const {
	chr = CharFilterDouble(chr);
	return comma && chr == '.' ? ',' : chr;
}

ConvertDouble::ConvertDouble(double minval, double maxval, bool notnull)
  : minval(minval), maxval(maxval), notnull(notnull)
{
	pattern = "%.10g";
	comma = false;
}

ConvertDouble& ConvertDouble::Pattern(tukk p)
{
	pattern = p;
	comma = Txt(Format(1.1)).Find(',') >= 0;
	return *this;
}

Date& ConvertDate::default_min()
{
	static Date v = Date::Low();
	return v;
}

Date& ConvertDate::default_max()
{
	static Date v = Date::High();
	return v;
}

void ConvertDate::SetDefaultMinMax(Date min, Date max)
{
	default_min() = min;
	default_max() = max;
}

ConvertDate::ConvertDate(Date minval, Date maxval, bool notnull)
: minval(minval), maxval(maxval), notnull(notnull) {
	defaultval = Null;
}

Val ConvertDate::Format(const Val& q) const
{
	if(IsDateTime(q))
		return Convert::Format((Date)q);
	return Convert::Format(q);
}

Val ConvertDate::Scan(const Val& text) const {
	Val v = DRX::Scan(DATE_V, text, defaultval);
	if(IsError(v)) return v;
	if(IsNull(v)) return notnull ? NotNullError() : v;
	Date m = v;
	Date minval = GetMin();
	Date maxval = GetMax();
	if(m >= minval && m <= maxval) return v;
	return ErrorVal(t_("Дата должна быть в диапазоне между ") + DRX::Format(minval) + t_("\v и ") + DRX::Format(maxval) + ".");
}

wchar   ConvertDate::Filter(wchar chr) const {
	return CharFilterDate(chr);
}

ConvertTime::ConvertTime(Time minval, Time maxval, bool notnull)
: minval(minval), maxval(maxval), notnull(notnull), seconds(true) {
	defaultval = Null;
	timealways = false;
	dayend = false;
}

ConvertTime::~ConvertTime()
{
}

Val ConvertTime::Scan(const Val& text) const
{
	bool hastime;
	Val v = DRX::Scan(TIME_V, text, defaultval, &hastime);
	if(IsError(v)) return v;
	if(IsNull(v)) return notnull ? NotNullError() : v;
	Time m = v;
	if(!hastime && dayend) {
		m.hour = 23;
		m.minute = 59;
		m.second = 59;
		v = m;
	}
	Time minval = GetMin();
	Time maxval = GetMax();
	if(m >= minval && m <= maxval) return v;
	return ErrorVal(t_("Время должно быть в диапазоне между ") + DRX::Format(minval) + t_("\v и ") + DRX::Format(maxval) + ".");
}

wchar ConvertTime::Filter(wchar chr) const
{
	if(IsDigit(chr) || chr == ' ' || chr == '.' || chr == ':')
		return chr;
	if(chr == ',')
		return '.';
	return CharFilterDate(chr);
}

Val ConvertTime::Format(const Val& q) const
{
	if(IsVoid(q) || q.IsNull())
		return Txt();
	else
	if(q.GetType() == TIME_V || timealways)
		return ToTime((Date)q) != (Time)q || timealways ? DRX::Format(Time(q), seconds) : DRX::Format(Date(q));
	else
		return Convert::Format(q);
}

Val ConvertTxt::Scan(const Val& text) const {
	if(IsError(text)) return text;
	if(text.GetType() == TXT_V) {
		Txt s = text;
		if(trimleft)
			s = drx::TrimLeft(s);
		if(trimright)
			s = drx::TrimRight(s);
		if(IsNull(s)) return notnull ? NotNullError() : Val(s);
		if(s.GetLength() <= maxlen) return s;
	}
	if(text.GetType() == WTXT_V) {
		WTxt s = text;
		if(trimleft)
			s = drx::TrimLeft(s);
		if(trimright)
			s = drx::TrimRight(s);
		if(IsNull(s)) return notnull ? NotNullError() : Val(s);
		if(s.GetLength() <= maxlen) return s;
	}
	return ErrorVal(DRX::Format(t_("Пожалуйста, введите не более %d символов."), maxlen));
}

const ConvertInt& StdConvertInt() { static ConvertInt h; return h; }
const ConvertInt& StdConvertIntNotNull() { static ConvertInt h(-INT_MAX, INT_MAX, true); return h; }

const ConvertDouble& StdConvertDouble() { static ConvertDouble h; return h; }
const ConvertDouble& StdConvertDoubleNotNull() { static ConvertDouble h(-std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), true); return h; }

const ConvertFloat& StdConvertFloat() { static ConvertFloat h; return h; }
const ConvertFloat& StdConvertFloatNotNull() { static ConvertFloat h(-std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), true); return h; }

const ConvertDate& StdConvertDate() { static ConvertDate h; return h; }
const ConvertDate& StdConvertDateNotNull() { static ConvertDate h(Date(0, 0, 0), Date(3000, 12, 31), true); return h; }

const ConvertTime& StdConvertTime() { static ConvertTime h; return h; }
const ConvertTime& StdConvertTimeNotNull() { static ConvertTime h(Null, Null, true); return h; }

const ConvertTxt& StdConvertTxt() { static ConvertTxt h; return h; }
const ConvertTxt& StdConvertTxtNotNull() { static ConvertTxt h(INT_MAX, true); return h; }

Val  MapConvert::Format(const Val& q) const {
	return map.Get(q, default_value);
}

NoConvertClass::NoConvertClass() {}

Val NoConvertClass::Format(const Val& q) const {
	return q;
}

const NoConvertClass& NoConvert() {
	return Single<NoConvertClass>();
}

Val ErrorConvertClass::Scan(const Val& v) const
{
	return ErrorVal();
}

const ErrorConvertClass& ErrorConvert()
{
	return Single<ErrorConvertClass>();
}

Val JoinConvert::Format(const Val& v) const {
	Txt r;
	ValArray a = v;
	for(i32 i = 0; i < item.GetCount(); i++) {
		const Item& m = item[i];
		if(m.pos < 0)
			r << m.text;
		else
			r << (Txt) StdConvert().Format(m.convert->Format(a[m.pos]));
	}
	return r;
}

i32 JoinConvert::NextPos() const {
	for(i32 i = item.GetCount() - 1; i >= 0; i--)
		if(item[i].pos >= 0) return item[i].pos + 1;
	return 0;
}

JoinConvert& JoinConvert::Add(tukk text) {
	Item& m = item.Add();
	m.pos = -1;
	m.text = text;
	return *this;
}

JoinConvert& JoinConvert::Add(i32 pos, const Convert& cv) {
	ASSERT(pos >= 0);
	Item& m = item.Add();
	m.pos = pos;
	m.convert = &cv;
	return *this;
}

JoinConvert& JoinConvert::Add(i32 pos) {
	Add(pos, StdConvert());
	return *this;
}

JoinConvert& JoinConvert::Add(const Convert& cv) {
	Add(NextPos(), cv);
	return *this;
}

JoinConvert& JoinConvert::Add() {
	Add(NextPos());
	return *this;
}

Val FormatConvert::Format(const Val& v) const
{
	ValArray va;
	if(IsValArray(v))
		va = v;
	else
		va.Add(v);
	return DRX::Format(format, va.Get());
}

}//endns drx