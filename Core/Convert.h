unsigned      stou(tukk ptr, uk endptr = NULL, unsigned radix = 10);
inline unsigned stou(u8k *ptr, uk endptr = NULL, unsigned radix = 10) { return stou((tukk )ptr, endptr, radix); }
unsigned      stou(wtukk ptr, uk endptr = NULL, unsigned radix = 10);

zu64        stou64(tukk s, uk endptr = NULL, unsigned radix = 10);
zu64        stou64(wtukk s, uk endptr = NULL, unsigned radix = 10);

#include "Convert.hpp"

i32           ScanInt(wtukk ptr, wtukk *endptr = NULL, i32 radix = 10);

i32           ScanInt(tukk ptr, tukk *endptr, i32 radix);
i32           ScanInt(tukk ptr, tukk *endptr);
i32           ScanInt(tukk ptr);

z64         ScanInt64(tukk ptr, tukk *endptr, i32 radix);
z64         ScanInt64(tukk ptr, tukk *endptr);
z64         ScanInt64(tukk ptr);

i32           Atoi(tukk ptr);
z64         Atoi64(tukk ptr);

double        ScanDouble(tukk ptr, tukk *endptr, bool accept_comma);
double        ScanDouble(wtukk ptr, wtukk *endptr, bool accept_comma);
double        ScanDouble(tukk ptr, tukk *endptr);
double        ScanDouble(wtukk ptr, wtukk *endptr);
double        ScanDouble(tukk ptr);
double        ScanDouble(wtukk ptr);

double        Atof(tukk s);

Val         StrIntVal(tukk s);

inline i32     StrInt(tukk s)   { return ScanInt(s); }
inline Txt  IntStr(i32 i)           { return FormatInt(i); }

inline z64   StrInt64(tukk s) { return ScanInt64(s); }
inline Txt  IntStr64(z64 i)       { return FormatInt64(i); }

inline double  StrDbl(tukk s)   { return ScanDouble(s); }
inline Txt  DblStr(double d)        { return IsNull(d) ? Txt() : FormatDouble(d); }

inline double  IntDbl(i32 i)           { return IsNull(i) ? double(Null) : double(i); }
inline i32     DblInt(double d)        { return IsNull(d) ? i32(Null) : fround(d); }

Val          StrDblVal(tukk s);


Val NotNullError();

class Convert {
public:
	Convert();
	virtual ~Convert();

	virtual Val  Format(const Val& q) const;
	virtual Val  Scan(const Val& text) const;
	virtual wchar    Filter(wchar chr) const;

	Val operator()(const Val& q) const              { return Format(q); }
};

const Convert& StdConvert();

Txt StdFormat(const Val& q);

class ConvertInt : public Convert {
public:
	virtual Val Scan(const Val& text) const;
	virtual wchar   Filter(wchar chr) const;

protected:
	z64 minval, maxval;
	bool  notnull;

public:
	ConvertInt& MinMax(i32 _min, i32 _max)        { minval = _min; maxval = _max; return *this; }
	ConvertInt& Min(i32 _min)                     { minval = _min; return *this; }
	ConvertInt& Max(i32 _max)                     { maxval = _max; return *this; }
	ConvertInt& NotNull(bool b = true)            { notnull = b; return *this; }
	ConvertInt& NoNotNull()                       { return NotNull(false); }
	i32         GetMin() const                    { return (i32)minval; }
	i32         GetMax() const                    { return (i32)maxval; }
	bool        IsNotNull() const                 { return notnull; }

	static i32  GetDefaultMin()                   { return -INT_MAX; }
	static i32  GetDefaultMax()                   { return INT_MAX; }

	ConvertInt(i32 minval = -INT_MAX, i32 maxval = INT_MAX, bool notnull = false)
		: minval(minval), maxval(maxval), notnull(notnull) {}
};

const ConvertInt& StdConvertInt();
const ConvertInt& StdConvertIntNotNull();

struct ConvertInt64 : public ConvertInt {
	ConvertInt64& MinMax(z64 _min, z64 _max)    { minval = _min; maxval = _max; return *this; }
	ConvertInt64& Min(z64 _min)                   { minval = _min; return *this; }
	ConvertInt64& Max(z64 _max)                   { maxval = _max; return *this; }
	z64         GetMin() const                    { return minval; }
	z64         GetMax() const                    { return maxval; }

	static z64  GetDefaultMin()                   { return -INT64_MAX; }
	static z64  GetDefaultMax()                   { return INT64_MAX; }

	ConvertInt64(z64 minval = -INT64_MAX, z64 maxval = INT64_MAX, bool notnull = false) {
		MinMax(minval, maxval); NotNull(notnull);
	}
};

class ConvertDouble : public Convert {
public:
	virtual Val Format(const Val& q) const;
	virtual Val Scan(const Val& text) const;
	virtual wchar   Filter(wchar chr) const;

protected:
	double      minval, maxval;
	bool        notnull, comma;
	Txt      pattern;

public:
	ConvertDouble& Pattern(tukk p);
	ConvertDouble& MinMax(double _min, double _max)  { minval = _min; maxval = _max; return *this; }
	ConvertDouble& Min(double _min)                  { minval = _min; return *this; }
	ConvertDouble& Max(double _max)                  { maxval = _max; return *this; }
	ConvertDouble& NotNull(bool b = true)            { notnull = b; return *this; }
	ConvertDouble& NoNotNull()                       { return NotNull(false); }
	double         GetMin() const                    { return minval; }
	double         GetMax() const                    { return maxval; }
	bool           IsNotNull() const                 { return notnull; }

	static double  GetDefaultMin()                   { return -std::numeric_limits<double>::max(); }
	static double  GetDefaultMax()                   { return std::numeric_limits<double>::max(); }

	ConvertDouble(double minval = GetDefaultMin(), double maxval = GetDefaultMax(),
		          bool notnull = false);
};

const ConvertDouble& StdConvertDouble();
const ConvertDouble& StdConvertDoubleNotNull();

struct ConvertFloat : public ConvertDouble {
	static double  GetDefaultMin()                   { return -std::numeric_limits<float>::max(); }
	static double  GetDefaultMax()                   { return std::numeric_limits<float>::max(); }

	ConvertFloat(double minval = GetDefaultMin(), double maxval = GetDefaultMax(), bool notnull = false)
	:	ConvertDouble(minval, maxval, notnull) { Pattern("%.7g"); }
};

const ConvertFloat& StdConvertFloat();
const ConvertFloat& StdConvertFloatNotNull();

class ConvertDate : public Convert {
public:
	virtual Val Format(const Val& q) const;
	virtual Val Scan(const Val& text) const;
	virtual wchar   Filter(wchar chr) const;

protected:
	Date minval, maxval, defaultval;
	bool notnull;

	static Date& default_min();
	static Date& default_max();

public:
	ConvertDate& MinMax(Date _min, Date _max)      { minval = _min; maxval = _max; return *this; }
	ConvertDate& Min(Date _min)                    { minval = _min; return *this; }
	ConvertDate& Max(Date _max)                    { maxval = _max; return *this; }
	ConvertDate& NotNull(bool b = true)            { notnull = b; return *this; }
	ConvertDate& NoNotNull()                       { return NotNull(false); }
	ConvertDate& Default(Date d)                   { defaultval = d; return *this; }
	bool         IsNotNull() const                 { return notnull; }
	Date         GetMin() const                    { return max(GetDefaultMin(), minval); }
	Date         GetMax() const                    { return min(GetDefaultMax(), maxval); }

	static void  SetDefaultMinMax(Date min, Date max);
	static Date  GetDefaultMin()                   { return default_min(); }
	static Date  GetDefaultMax()                   { return default_max(); }

	ConvertDate(Date minval = Date::Low(), Date maxval = Date::High(), bool notnull = false);
};

const ConvertDate& StdConvertDate();
const ConvertDate& StdConvertDateNotNull();

class ConvertTime : public Convert {
public:
	virtual Val Scan(const Val& text) const;
	virtual wchar   Filter(wchar chr) const;
	virtual Val Format(const Val& q) const;

protected:
	Time minval, maxval, defaultval;
	bool notnull;
	bool seconds;
	bool timealways;
	bool dayend;

public:
	ConvertTime& MinMax(Time _min, Time _max)      { minval = _min; maxval = _max; return *this; }
	ConvertTime& Min(Time _min)                    { minval = _min; return *this; }
	ConvertTime& Max(Time _max)                    { maxval = _max; return *this; }
	ConvertTime& NotNull(bool b = true)            { notnull = b; return *this; }
	ConvertTime& NoNotNull()                       { return NotNull(false); }
	ConvertTime& Seconds(bool b = true)            { seconds = b; return *this; }
	ConvertTime& NoSeconds()                       { return Seconds(false); }
	bool         IsSeconds() const                 { return seconds; }
	ConvertTime& TimeAlways(bool b = true)         { timealways = b; return *this; }
	bool         IsTimeAlways() const              { return timealways; }
	ConvertTime& DayEnd(bool b = true)             { dayend = b; return *this; }
	bool         IsDayEnd() const                  { return dayend; }
	ConvertTime& Default(Time d)                   { defaultval = d; return *this; }
	bool         IsNotNull() const                 { return notnull; }

	Time         GetMin() const                    { return max(minval, GetDefaultMin()); }
	Time         GetMax() const                    { return min(maxval, GetDefaultMax()); }

	static Time  GetDefaultMin()                   { return ToTime(ConvertDate::GetDefaultMin()); }
	static Time  GetDefaultMax()                   { return ToTime(ConvertDate::GetDefaultMax()); }

	ConvertTime(Time minval = ToTime(Date::Low()), Time maxval = ToTime(Date::High()), bool notnull = false);
	virtual ~ConvertTime();
};

const ConvertTime& StdConvertTime();
const ConvertTime& StdConvertTimeNotNull();

class ConvertTxt : public Convert {
public:
	virtual Val Scan(const Val& text) const;

protected:
	i32  maxlen;
	bool notnull, trimleft, trimright;

public:
	ConvertTxt& MaxLen(i32 _maxlen)             { maxlen = _maxlen; return *this; }
	i32            GetMaxLength() const            { return maxlen; }
	ConvertTxt& NotNull(bool b = true)          { notnull = b; return *this; }
	ConvertTxt& NoNotNull()                     { return NotNull(false); }
	bool           IsNotNull() const               { return notnull; }
	ConvertTxt& TrimLeft(bool b = true)         { trimleft = b; return *this; }
	ConvertTxt& TrimRight(bool b = true)        { trimright = b; return *this; }
	ConvertTxt& TrimBoth(bool b = true)         { return TrimLeft(b).TrimRight(b); }
	bool           IsTrimLeft() const              { return trimleft; }
	bool           IsTrimRight() const             { return trimright; }

	ConvertTxt(i32 maxlen = INT_MAX, bool notnull = false)
		: maxlen(maxlen), notnull(notnull) { trimleft = trimright = false; }
};

const ConvertTxt& StdConvertTxt();
const ConvertTxt& StdConvertTxtNotNull();

class NoConvertClass : public Convert {
public:
	NoConvertClass();

	virtual Val  Format(const Val& q) const;
};

const NoConvertClass& NoConvert();

class ErrorConvertClass : public Convert {
public:
	Val Scan(const Val& v) const;
};

const ErrorConvertClass& ErrorConvert();

class MapConvert : public Convert {
public:
	virtual Val  Format(const Val& q) const;

protected:
	VecMap<Val, Val> map;
	Val                   default_value;

public:
	void         Clear()                                 { map.Clear(); }
	MapConvert&  Add(const Val& a, const Val& b)     { map.Add(a, b); return *this; }
	MapConvert&  Default(const Val& v)                 { default_value = v; return *this; }

	i32          GetCount() const                        { return map.GetCount(); }
	i32          Find(const Val& v) const              { return map.Find(v); }
	const Val& GetKey(i32 i) const                     { return map.GetKey(i); }
	const Val& GetVal(i32 i) const                   { return map[i]; }
	const Val& operator[](i32 i) const                 { return map[i]; }

	virtual ~MapConvert() {}
};

class JoinConvert : public Convert {
public:
	virtual Val Format(const Val& v) const;

protected:
	struct Item {
		i32            pos;
		const Convert *convert;
		Txt         text;
	};
	Array<Item> item;

	i32 NextPos() const;

public:
	JoinConvert& Add(tukk text);
	JoinConvert& Add(i32 pos, const Convert& cv);
	JoinConvert& Add(i32 pos);
	JoinConvert& Add(const Convert& cv);
	JoinConvert& Add();
};

class FormatConvert : public Convert {
public:
	virtual Val Format(const Val& v) const;

private:
	Txt format;

public:
	void   SetFormat(tukk fmt)           { format = fmt; }
};

Convert& LNGConvert();

template <typename F, typename S, class R>
struct LambdaConvertClass : Convert {
	Function<Val(const Val& w)> format;
	Function<Val(const Val& text)> scan;
	Function<wchar(wchar)> filter;

	virtual Val Format(const Val& q) const { return format(q); }
	virtual Val Scan(const Val& text) const { return scan(text); }
	virtual wchar Filter(wchar chr) const { return filter(chr); }

	LambdaConvertClass(F format, S scan, R filter) : format(format), scan(scan), filter(filter) {}
};

template <typename F, typename S, class R>
const auto& LambdaConvert(F format, S scan, R filter)
{
	static LambdaConvertClass<F, S, R> x(format, scan, filter);
	return x;
}

template <typename F, typename S>
const auto& LambdaConvert(F format, S scan)
{
	return LambdaConvert(format, scan, [](i32 ch) { return ch; });
}

template <typename F>
const auto& LambdaConvert(F format)
{
	return LambdaConvert(format, [](const Val& v) { return v; });
}