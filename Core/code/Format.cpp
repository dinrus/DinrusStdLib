#include "../Core.h"
//#BLITZ_APPROVE

namespace drx {

// Old format ---------------------------

Txt  VFormat(tukk fmt, va_list ptr) {
	i32 limit = 2 * (i32)strlen(fmt) + 1024;
	if(limit < 1500) {
		char buffer[1500];
		vsnprintf(buffer, 1500, fmt, ptr);
		va_end(ptr);
		i32 len = (i32)strlen(buffer);
		ASSERT(len <= limit);
		return Txt(buffer, len);
	}
	else {
		Buffer<char> buffer(limit);
		vsnprintf(buffer, 1500, fmt, ptr);
		va_end(ptr);
		i32 len = (i32)strlen(buffer);
		ASSERT(len <= limit);
		return Txt(buffer, len);
	}
}

// Formatting routines ---------------------------

// utoa32, utoa64 inspired by
// https://github.com/miloyip/itoa-benchmark/blob/940542a7770155ee3e9f2777ebc178dc899b43e0/src/branchlut.cpp
// by Milo Yip

const char s100[] =
    "00010203040506070809"
    "10111213141516171819"
    "20212223242526272829"
    "30313233343536373839"
    "40414243444546474849"
    "50515253545556575859"
    "60616263646566676869"
    "70717273747576777879"
    "80818283848586878889"
    "90919293949596979899"
;

namespace utoa_private {

force_inline
void  Do2(tuk t, dword d) {
	memcpy(t, s100 + 2 * d, 2);
};

force_inline
void  Do4(tuk t, dword value) {
	Do2(t, value / 100);
	Do2(t + 2, value % 100);
}

force_inline
void  Do8(tuk t, dword value) {
	Do4(t, value / 10000);
	Do4(t + 4, value % 10000);
}

};

i32 utoa32(dword value, tuk buffer)
{
	using namespace utoa_private;

	if (value < 10000) {
		if(value < 100) {
			if(value < 10) {
				*buffer = char(value + '0');
				return 1;
			}
			Do2(buffer, value % 100);
			return 2;
		}

		if(value < 1000) {
			*buffer = char(value / 100 + '0');
			Do2(buffer + 1, value % 100);
			return 3;
		}

		Do4(buffer, value);
		return 4;
	}
	else if (value < 100000000) {
		if(value < 10000000) {
			if(value < 100000) {
				*buffer = char(value / 10000 + '0');
				Do4(buffer + 1, value % 10000);
				return 5;
			}
			if(value < 1000000) {
				Do2(buffer, value / 10000);
				Do4(buffer + 2, value % 10000);
				return 6;
			}
			*buffer = char(value / 1000000 + '0');
			Do2(buffer + 1, value / 10000 % 100);
			Do4(buffer + 3, value % 10000);
			return 7;
		}

		Do8(buffer, value);
		return 8;
	}
	else {
		dword a = value / 100000000; // 2 digits
		value %= 100000000;

		if(a < 10) {
			*buffer = char(a + '0');
			Do8(buffer + 1, value);
			return 9;
		}

		Do2(buffer, a);
		Do8(buffer + 2, value);
		return 10;
	}
}

i32 utoa64(zu64 value, tuk buffer)
{
	using namespace utoa_private;

	if(value <= 0xffffffff)
		return utoa32((dword)value, buffer);
	if(value < (zu64)1000000000 * 100000000) {
		i32 q = utoa32(dword(value / 100000000), buffer);
		Do8(buffer + q, value % 100000000);
		return q + 8;
	}
	i32 q = utoa32(dword(value / ((zu64)100000000 * 100000000)), buffer);
	Do8(buffer + q, value / 100000000 % 100000000);
	Do8(buffer + 8 + q, value % 100000000);
	return q + 16;
}

Txt FormatUInt64(zu64 w)
{
	if(w < 100000000000000)
		return Txt::Make(14, [&](tuk s) { return utoa64(w, s); });
	else
		return Txt::Make(20, [&](tuk s) { return utoa64(w, s); });
}

Txt FormatInt64(z64 i)
{
	if(IsNull(i))
		return Txt();
	if(i < 0) {
		i = -i;
		if(i < 10000000000000)
			return Txt::Make(14, [&](tuk s) {
				*s++ = '-';
				return utoa64(i, s) + 1;
			});
		return Txt::Make(20, [&](tuk s) {
			*s++ = '-';
			return utoa64(i, s) + 1;
		});
	}
	if(i < 100000000000000)
		return Txt::Make(14, [&](tuk s) { return utoa64(i, s); });
	return Txt::Make(20, [&](tuk s) { return utoa64(i, s); });
}

Txt FormatIntBase(i32 i, i32 base, i32 width, char lpad, i32 sign, bool upper)
{
	enum { BUFFER = sizeof(i32) * 8 + 1 };
	if(base < 2 || base > 36)
		return "<invalid base>";
	char buffer[BUFFER];
	tuk const e = buffer + (i32)BUFFER;
	tuk p = e;
	tukk itoc = upper ? "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" : "0123456789abcdefghijklmnopqrstuvwxyz";
	if(sign < 0 || !IsNull(i))
	{
		unsigned x = i;
		if(sign >= 0 && i < 0)
			x = -i;
		do
			*--p = itoc[x % base];
		while(x /= base);
	}
	bool minus = (sign >= 0 && i < 0 && !IsNull(i));
	bool do_sign = (sign > 0 || sign >= 0 && minus);
	if(do_sign && lpad != '0')
		*--p = (minus ? '-' : '+');
	if(width > e - p)
	{
		tuk b = e - min<i32>(width, BUFFER);
		while(p > b)
			*--p = lpad;
	}
	if(do_sign && lpad == '0')
		*--p = (minus ? '-' : '+');
	i32 dwd = (i32)(e - p);
	i32 pad = (width = max(width, dwd)) - dwd;
	TxtBuf out(width);
	tuk o = out;
	if(dwd < width)
		memset(o, lpad, pad);
	memcpy8(o + pad, p, dwd);
	return Txt(out);
}

Txt FormatIntDec(i32 i, i32 width, char lpad, bool always_sign)
{
	return FormatIntBase(i, 10, width, lpad, always_sign ? 1 : 0);
}

Txt FormatIntHex(i32 i, i32 width, char lpad)
{
	return FormatIntBase(i, 16, width, lpad, -1);
}

Txt FormatIntHexUpper(i32 i, i32 width, char lpad)
{
	return FormatIntBase(i, 16, width, lpad, -1, true);
}

Txt FormatIntOct(i32 i, i32 width, char lpad)
{
	return FormatIntBase(i, 8, width, lpad, -1);
}

Txt FormatIntAlpha(i32 i, bool upper)
{
	if(IsNull(i) || i == 0)
		return Null;
	Txt out;
	if(i < 0)
	{
		out << '-';
		i = -i;
	}
	char temp[10], *p = temp + 10;
	tukk itoc = upper ? "ZABCDEFGHIJKLMNOPQRSTUVWXYZ" : "zabcdefghijklmnopqrstuvwxyz";
	do
		*--p = itoc[i-- % 26];
	while(i /= 26);
	out.Cat(p, temp + 10);
	return out;
}

Txt FormatIntRoman(i32 i, bool upper)
{
	if(IsNull(i) || i == 0)
		return Null;

	Txt out;
	if(i < 0)
	{
		out << '-';
		i = -i;
	}
	i32 m = i / 1000;
	if(m)
	{
		out.Cat('M', m);
		i -= 1000 * m;
	}

	char shift = upper ? 0 : 'a' - 'A';
	static i32k  value[] =  { 1000, 500, 100, 50,  10,  5,   1 };
	static const char letter[] = { 'M',  'D', 'C', 'L', 'X', 'V', 'I' };
	for(i32 n = 0; i && n < __countof(value); n++)
	{
		i32 v = value[n];
		while(i >= v)
		{
			out << (char)(letter[n] + shift);
			i -= v;
		}
		if(n < __countof(value) - 1)
			for(i32 j = n + (value[n + 1] * 2 >= v ? 2 : 1); j < __countof(value); j++)
				if(i >= v - value[j])
				{ // subtraction scheme
					out << (char)(letter[j] + shift) << (char)(letter[n] + shift);
					i -= v - value[j];
					break;
				}
	}
	return out;
}

Txt Format64Hex(zu64 a)
{
	char b[50];
	tuk p = b + 50;
	do {
		*--p = "0123456789abcdef"[a & 15];
		a >>= 4;
	}
	while(a);
	return Txt(p, b + 50);
}

Txt FormatBool(bool a)              { return a ? "true" : "false"; }
Txt FormatPtr(ukk p)        { return "0x" + FormatHex(p); }

Txt FormatDate(Date date, tukk format, i32 language)
{
	if(IsNull(date))
		return Null;
	if(!format || !*format)
		return Format(date);
	return FormatTime(ToTime(date), format, language);
}

Txt FormatTime(Time t, tukk s, i32 language)
{
	if(IsNull(t))
		return Null;
	Txt result;
	if(!s || !*s)
		return Format(t);
	while(*s) {
		i32 q = 0;
		if(*s == 'M') {
			while(*s == 'M') { s++; q++; }
			if(q == 1)
				result.Cat(Format("%d", t.month));
			else
				result.Cat(Format("%02d", t.month));
		}
		else
		if(*s == 'D') {
			while(*s == 'D') { s++; q++; }
			if(q == 1)
				result.Cat(Format("%d", t.day));
			else
				result.Cat(Format("%02d", t.day));
		}
		else
		if(*s == 'Y') {
			while(*s == 'Y') { s++; q++; }
			if(q == 1)
				result.Cat(Format("%d", t.year % 100));
			else
			if(q == 2)
				result.Cat(Format("%02d", t.year % 100));
			else
				result.Cat(Format("%d", t.year));
		}
		else
		if(*s == 'h') {
			while(*s == 'h') { s++; q++; }
			if(q == 1)
				result.Cat(Format("%d", t.hour));
			else
				result.Cat(Format("%02d", t.hour));
		}
		else
		if(*s == 'H') {
			while(*s == 'H') { s++; q++; }
			i32 h = ((t.hour + 11) % 12 + 1);
			if(q == 1)
				result.Cat(Format("%d", h));
			else
				result.Cat(Format("%02d", h));
		}
		else
		if(*s == '<') {
			s++;
			while(*s && *s != '/') {
				if(t.hour <= 12)
					result.Cat(*s);
				s++;
			}
			if(!*s) break;
			s++;
			while(*s && *s != '>') {
				if(t.hour > 12)
					result.Cat(*s);
				s++;
			}
			if(!*s) break;
			s++;
		}
		else
		if(*s == 'm') {
			while(*s == 'm') { s++; q++; }
			if(q == 1)
				result.Cat(Format("%d", t.minute));
			else
				result.Cat(Format("%02d", t.minute));
		}
		else
		if(*s == 's') {
			while(*s == 's') { s++; q++; }
			if(q == 1)
				result.Cat(Format("%d", t.second));
			else
				result.Cat(Format("%02d", t.second));
		}
		else
		if(*s == '`' && s[1]) {
			s++;
			result.Cat(*s++);
		}
		else
			result.Cat(*s++);
	}
	return result;
}

// New format ----------------------------


void TrimChar(Txt& s, i32 n)
{
	if(GetDefaultCharset() == CHARSET_UTF8) {
		WTxt h(s);
		h.Trim(n);
		s = h.ToTxt();
	}
	else
		s.Trim(n);
}

struct FormId : Moveable<FormId> {
	FormId(Txt id, i32 type) : id(id), type(type) {}
	Txt id;
	i32    type;
};

hash_t GetHashVal(const FormId& fid)
{
	return CombineHash(fid.type, GetHashVal(fid.id));
}

bool operator==(const FormId& a, const FormId& b)
{
	return a.type == b.type && a.id == b.id;
}

VecMap<FormId, Formatter>& formatmap()
{
	return Single< VecMap<FormId, Formatter> > ();
}

void RegisterFormatter(i32 type, tukk id, Formatter f)
{
	AssertST();
	INTERLOCKED {
		FormId fid(id, type);
		formatmap().FindAdd(fid, f);
		formatmap().Find(fid);
	}
}

void RegisterValFormatter(tukk id, Formatter f)
{
	RegisterFormatter(VALUE_V, id, f);
}

void RegisterNullFormatter(tukk id, Formatter f)
{
	RegisterFormatter(VOID_V, id, f);
	RegisterFormatter(ERROR_V, id, f);
}

void RegisterNumberFormatter(tukk id, Formatter f)
{
	RegisterFormatter(DOUBLE_V, id, f);
	RegisterFormatter(INT64_V, id, f);
	RegisterFormatter(INT_V, id, f);
	RegisterFormatter(BOOL_V, id, f);
	RegisterNullFormatter(id, f);
}

void RegisterTxtFormatter(tukk id, Formatter f)
{
	RegisterFormatter(WTXT_V, id, f);
	RegisterFormatter(TXT_V, id, f);
	RegisterNullFormatter(id, f);
}

void RegisterDateTimeFormatter(tukk id, Formatter f)
{
	RegisterFormatter(TIME_V, id, f);
	RegisterFormatter(DATE_V, id, f);
	RegisterNullFormatter(id, f);
}

Txt IntFormatter(const Formatting& f)
{
	if(f.format.GetCount() == 0 && f.id[0] == 'd' && f.id[1] == 0)
		return AsTxt((i32)f.arg);
	TxtBuf q;
	q.SetLength(1000);
	q.SetLength(sprintf(q, '%' + f.format + f.id, (i32)f.arg));
	return Txt(q);
}

Txt Int64Formatter(const Formatting& f)
{
	TxtBuf q;
	q.SetLength(1000);
	q.SetLength(sprintf(q, '%' + f.format + f.id, (z64)f.arg));
	return Txt(q);
}

Txt IntLowerAlphaFormatter(const Formatting& f)
{
	return FormatIntAlpha(f.arg, false);
}

Txt IntUpperAlphaFormatter(const Formatting& f)
{
	return FormatIntAlpha(f.arg, true);
}

Txt IntLowerRomanFormatter(const Formatting& f)
{
	return FormatIntRoman(f.arg, false);
}

Txt IntUpperRomanFormatter(const Formatting& f)
{
	return FormatIntRoman(f.arg, true);
}

Txt DoubleFormatter(const Formatting& f)
{
	tukk s = f.format;

	bool fillz = false;
	bool wd = true;
	bool left = false;
	i32 width = 0;
	i32 precision = 6;

	i32 flags = FD_SIGN_EXP|FD_SPECIAL|FD_MINUS0;
	tukk id = f.id;
	if(*id++ == 'M')
		flags |= FD_CAP_E;
	bool lng = false;
	if(*id == 'l') {
		lng = true;
		id++;
	}
	if(*id == 'E') flags |= FD_EXP|FD_CAP_E;
	if(*id == 'e') flags |= FD_EXP;
	if(*id == 'f') flags |= FD_FIX|FD_ZEROS;

	while(*s) {
		if(IsDigit(*s)) {
			if(wd && *s == '0')
				fillz = true;
			dword n;
			bool overflow = false;
			s = ScanUint<char, u8, dword, 10>(n, s, overflow);
			if(overflow || !s || n > (wd ? 1000u : 100u))
				return Null;
			(wd ? width : precision) = n;
		}
		else
		switch(*s++) {
		case '-': left = true; break;
		case '+': flags |= FD_SIGN; break;
		case ' ': flags |= FD_SIGN_SPACE; break;
		case ',': flags |= FD_COMMA; wd = false; break;
		case '.': flags &= ~FD_COMMA; wd = false; break;
		case '!': flags |= FD_ZEROS; break;
		case '?': flags &= ~FD_SPECIAL; break;
		case '_': flags &= ~FD_MINUS0; break;
		case '^': flags &= ~FD_SIGN_EXP; break;
		case '&': flags |= FD_MINIMAL_EXP; break;
		case '#': flags |= FD_ZEROS|FD_POINT; break;
		}
	}
	Txt r = FormatDouble(f.arg, precision, flags);
	if(lng) {
		i32 q = r.Find('.');
		if(q >= 0)
			r = r.Mid(0, q) + GetLangInfo(f.language).decimal_point + r.Mid(q + 1);
	}
	if(width > r.GetCount()) {
		if(fillz && !left && !IsNull(f.arg))
			return IsDigit(*r) ? Txt('0', width - r.GetCount()) + r
			                   : r.Mid(0, 1) + Txt('0', width - r.GetCount()) + r.Mid(1);
		return left ? r + Txt(' ', width - r.GetCount()) : Txt(' ', width - r.GetCount()) + r;
	}
	return r;
}

Txt RealFormatter(const Formatting& f)
{
	if(IsNull(f.arg))
		return Null;
	double value = f.arg;
	tukk s = f.format;
	i32 digits = 6;
	tukk id = f.id;
	bool fn = *id++ != 'v';
	i32 flags = 0;
	if(*s == '+') {
		flags |= FD_SIGN;
		s++;
	}
	if(IsDigit(*s) || *s == '-' && IsDigit(s[1])) {
		digits = (i32)strtol(s, NULL, 10);
		while(IsDigit(*++s))
			;
	}
	if(*s == '@') { s++; flags |= FD_NOTHSEPS; }
	if(*s == ',') { s++; flags |= FD_COMMA; }
	if(*s == '!') { s++; flags |= FD_ZEROS; }
	if(*s == '^') {
		if(*++s == '+') {
			flags |= FD_SIGN_EXP;
			s++;
		}
		while(IsDigit(*++s))
			;
	}
	bool lng = false;
	if(*id == 'l') {
		lng = true;
		id++;
	}
	if(*id == 'e') flags |= FD_EXP;
	else if(*id == 'f') flags |= FD_FIX;
	if(fn && value >= 1e-15 && value <= 1e15)
		flags |= FD_FIX;
	if(lng)
		return GetLangInfo(f.language).FormatDouble(value, digits, flags, 0);
	else
		return FormatDouble(value, digits, flags);
}

Txt TxtFormatter(const Formatting& f)
{
	const Txt& s = f.arg;
	if(f.format.GetCount() == 0 && f.id[0] == 's' && f.id[1] == 0)
		return s;
	i32 len = s.GetCharCount();
	i32 width = len;
	i32 precision = len;
	bool lpad = false;
	CParser p(f.format);
	if(p.Char('-'))
		lpad = true;
	if(p.IsNumber())
		width = p.ReadInt();
	if(p.Char('.') && p.IsNumber())
		precision = p.ReadInt();
//	if(precision > len)
//		return WTxt(~s, precision).ToTxt();
	Txt q = s;
	if(precision < len) {
		TrimChar(q, precision);
		len = precision;
	}
	Txt r;
	if(lpad)
		r.Cat(q);
	if(width > len)
		r.Cat(' ', width - len);
	if(!lpad)
		r.Cat(q);
	return r;
}

static inline
void sFixPoint(tuk s) // We do not want locale to affect decimal point, convert ',' to '.'
{
	while(*s) {
		if(*s == ',')
			*s = '.';
		s++;
	}
}

Txt FloatFormatter(const Formatting& f)
{
	double d = f.arg;
	Txt fmt = '%' + f.format + f.id;
	char h[256];
	i32 n = snprintf(h, 255, fmt, d);
	if(n >= 254) {
		Buffer<char> ah(n + 1);
		snprintf(ah, n + 1, fmt, d);
		sFixPoint(ah);
		return Txt(ah, n);
	}
	if(n < 0)
		return Txt();
	sFixPoint(h);
	return Txt(h, n);
}

Txt DateFormatter(const Formatting& f)
{
	return GetLangInfo(f.language).FormatDate(f.arg);
}

Txt TimeFormatter(const Formatting& f)
{
	return GetLangInfo(f.language).FormatTime(f.arg);
}

Txt SwitchFormatter(const Formatting& f)
{
	tukk s = f.format;
	i32 i = f.arg;
	i32 o = i;
	for(;;) {
		i32 n = 0;
		while(IsDigit(*s))
			n = 10 * n + *s++ - '0';
		if(!*s) return Null;
		if(*s == '%') {
			o = i % max(n, 1);
			s++;
		}
		else
		if(*s == ',' || *s == ':') {
			if(o == n) {
				while(*s && *s != ':')
					s++;
				if(!*s) return Null;
				++s;
				tukk b = s;
				while(*s && *s != ';')
					s++;
				return Txt(b, s);
			}
			if(*s == ':')
				while(*s && *s != ';')
					s++;
			if(!*s) return Null;
			s++;
		}
		else
			return s;
	}
	return Txt();
}

Txt StdFormatFormatter(const Formatting& f)
{
	Txt out = StdFormat(f.arg);
	if(!IsNull(out))
		return out;
	return f.format;
}

Txt MonthFormatter(const Formatting& f)
{
	return MonthName((i32)f.arg - 1, f.language);
}

Txt MONTHFormatter(const Formatting& f)
{
	return ToUpper(MonthFormatter(f), GetLNGCharset(f.language));
}

Txt monthFormatter(const Formatting& f)
{
	return ToLower(MonthFormatter(f), GetLNGCharset(f.language));
}

Txt MonFormatter(const Formatting& f)
{
	return MonName((i32)f.arg - 1, f.language);
}

Txt MONFormatter(const Formatting& f)
{
	return ToUpper(MonFormatter(f), GetLNGCharset(f.language));
}

Txt monFormatter(const Formatting& f)
{
	return ToLower(MonFormatter(f), GetLNGCharset(f.language));
}

Txt DayFormatter(const Formatting& f)
{
	return DayName((i32)f.arg, f.language);
}

Txt DAYFormatter(const Formatting& f)
{
	return ToUpper(DayFormatter(f), GetLNGCharset(f.language));
}

Txt dayFormatter(const Formatting& f)
{
	return ToLower(DayFormatter(f), GetLNGCharset(f.language));
}

Txt DyFormatter(const Formatting& f)
{
	return DyName((i32)f.arg, f.language);
}

Txt DYFormatter(const Formatting& f)
{
	return ToUpper(DyFormatter(f), GetLNGCharset(f.language));
}

Txt dyFormatter(const Formatting& f)
{
	return ToLower(DyFormatter(f), GetLNGCharset(f.language));
}

Txt twFormatter(const Formatting& f)
{
	i32 q = f.arg;
	return Sprintf(*f.format == '0' ? "%02d" : "%d", q ? q % 12 : 12);
}

Txt NumberFormatter(const Formatting& f)
{
	i32 q = f.arg;
	return AsTxt(q);
}

void IntDoubleRegister(i32 type)
{
	RegisterFormatter(type, "", &NumberFormatter);

	RegisterFormatter(type, "c", &IntFormatter);
	RegisterFormatter(type, "d", &IntFormatter);
	RegisterFormatter(type, "i", &IntFormatter);
	RegisterFormatter(type, "o", &IntFormatter);
	RegisterFormatter(type, "x", &IntFormatter);
	RegisterFormatter(type, "X", &IntFormatter);
	RegisterFormatter(type, "ld", &IntFormatter);
	RegisterFormatter(type, "li", &IntFormatter);
	RegisterFormatter(type, "lo", &IntFormatter);
	RegisterFormatter(type, "lx", &IntFormatter);
	RegisterFormatter(type, "lX", &IntFormatter);

	RegisterFormatter(type, "lld", &Int64Formatter);
	RegisterFormatter(type, "lli", &Int64Formatter);
	RegisterFormatter(type, "llo", &Int64Formatter);
	RegisterFormatter(type, "llx", &Int64Formatter);
	RegisterFormatter(type, "llX", &Int64Formatter);

	RegisterFormatter(type, "e", &FloatFormatter);
	RegisterFormatter(type, "E", &FloatFormatter);
	RegisterFormatter(type, "f", &FloatFormatter);
	RegisterFormatter(type, "g", &FloatFormatter);
	RegisterFormatter(type, "G", &FloatFormatter);

	RegisterFormatter(type, "s", &SwitchFormatter);

	RegisterFormatter(type, "month", &monthFormatter);
	RegisterFormatter(type, "Month", &MonthFormatter);
	RegisterFormatter(type, "MONTH", &MONTHFormatter);
	RegisterFormatter(type, "mon", &monFormatter);
	RegisterFormatter(type, "Mon", &MonFormatter);
	RegisterFormatter(type, "MON", &MONFormatter);
	RegisterFormatter(type, "Day", &DayFormatter);
	RegisterFormatter(type, "DAY", &DAYFormatter);
	RegisterFormatter(type, "day", &dayFormatter);
	RegisterFormatter(type, "Dy", &DyFormatter);
	RegisterFormatter(type, "DY", &DYFormatter);
	RegisterFormatter(type, "dy", &dyFormatter);
	RegisterFormatter(type, "tw", &twFormatter);
}

static void sRegisterFormatters()
{
	ONCELOCK {
		IntDoubleRegister(BOOL_V);
		IntDoubleRegister(INT_V);
		IntDoubleRegister(INT64_V);
		IntDoubleRegister(DOUBLE_V);

		RegisterTxtFormatter("s", &TxtFormatter);
		RegisterNullFormatter("", &DateFormatter);
		RegisterFormatter(DATE_V, "", &DateFormatter);
		RegisterFormatter(TIME_V, "", &TimeFormatter);

		RegisterNumberFormatter("n",  &RealFormatter);
		RegisterNumberFormatter("ne", &RealFormatter);
		RegisterNumberFormatter("nf", &RealFormatter);
		RegisterNumberFormatter("nl", &RealFormatter);
		RegisterNumberFormatter("nle", &RealFormatter);
		RegisterNumberFormatter("nlf", &RealFormatter);
		RegisterNumberFormatter("v",  &RealFormatter);
		RegisterNumberFormatter("ve", &RealFormatter);
		RegisterNumberFormatter("vf", &RealFormatter);
		RegisterNumberFormatter("vl", &RealFormatter);
		RegisterNumberFormatter("vle", &RealFormatter);
		RegisterNumberFormatter("vlf", &RealFormatter);

		// real number formats (n = fixed decimals, v = valid decimals)
		// ne, ve - force exponential notation; nf, vf - force fixed notation; nl, vl - language-based formatting
		// Options: [+][[-]<digits>][!][^[+]<expdig>]
		// + .. always prepend sign
		// [-]<digits> .. number of decimals to print (negative = left of decimal point, default = 6)
		// ! .. keep insignificant zeros
		// ^ .. exponent options:
		// + .. always prepend sign to exponent
		// <expdig> exponent padding width

		RegisterNumberFormatter("m",  &DoubleFormatter);
		RegisterNumberFormatter("me", &DoubleFormatter);
		RegisterNumberFormatter("mf", &DoubleFormatter);
		RegisterNumberFormatter("ml", &DoubleFormatter);
		RegisterNumberFormatter("mle", &DoubleFormatter);
		RegisterNumberFormatter("mlf", &DoubleFormatter);
		RegisterNumberFormatter("M",  &DoubleFormatter);
		RegisterNumberFormatter("mE", &DoubleFormatter);
		RegisterNumberFormatter("Ml", &DoubleFormatter);
		RegisterNumberFormatter("mlE", &DoubleFormatter);

		RegisterNumberFormatter("a", &IntLowerAlphaFormatter);
		RegisterNumberFormatter("A", &IntUpperAlphaFormatter);
		RegisterNumberFormatter("r", &IntLowerRomanFormatter);
		RegisterNumberFormatter("R", &IntUpperRomanFormatter);

		RegisterValFormatter("vt", &StdFormatFormatter);
		RegisterValFormatter("", &StdFormatFormatter);
	}
}

INITBLOCK {
	sRegisterFormatters();
}

Txt Format(i32 language, tukk s, const Vec<Val>& v)
{
	sRegisterFormatters();
	Formatting f;
	f.language = language;
	Txt result;
	i32 pos = 0;
	tukk b;
	for(;;) {
		i32 n = 0;
		b = s;
		for(;;) {
			while(*s && *s != '%')
				++s;
			result.Cat(b, (i32)(s - b));
			if(*s == '\0')
				return result;
			++s;
			if(*s == '%') {
				result.Cat('%');
				++s;
			}
			else
				break;
			b = s;
		}
		f.format.Clear();
		f.id.Clear();
		b = s;
		i32 pad = -1;
		i32 padn = 0;
		Txt nvl_value = Txt::GetVoid();
		for(;;) {
			if(*s == '$') {
				pos = n - 1;
				b = ++s;
				n = 0;
			}
			else
			if(*s == ':') {
				pos = n - 1;
				b = ++s;
				n = 0;
			}
			else
			if(*s == '*') {
				f.format.Cat(b, (i32)(s - b));
				b = ++s;
				i32 i = v[pos++];
				if(*s == ':' || *s == '$' || *s == '<' || *s == '>' || *s == '=')
					n = i;
				else
					f.format.Cat(FormatInt(i));
			}
			else
			if(*s == '<') {
				padn = n;
				pad = ALIGN_LEFT;
				b = ++s;
				n = 0;
			}
			else
			if(*s == '>') {
				padn = n;
				pad = ALIGN_RIGHT;
				b = ++s;
				n = 0;
			}
			else
			if(*s == '=') {
				padn = n;
				pad = ALIGN_CENTER;
				b = ++s;
				n = 0;
			}
			else
			if(*s == '[') {
				f.format.Cat(b, (i32)(s - b));
				s++;
				b = s;
				while(*s && *s != ']')
					s++;
				f.format.Cat(b, (i32)(s - b));
				if(*s) s++;
				b = s;
				if(!IsAlpha(*s) && *s != '~') break;
			}
			else if(*s == '~') {
				nvl_value = f.format;
				f.format = Null;
				b = ++s;
			}
			else
			if(!*s || *s == '`' || IsAlpha(*s))
				break;
			else {
				if(!*s)
					return result + "<unexpected end>";
				if(IsDigit(*s))
					n = 10 * n + *s - '0';
				else
					n = 0;
				s++;
			}
		}
		f.format.Cat(b, (i32)(s - b));
		b = s;
		while(IsAlpha(*s))
			s++;
		f.id = Txt(b, s);
		if(pos < 0 || pos >= v.GetCount())
		{
			result << "<invalid pos=" << pos << ">";
			if(*s == '`')
				s++;
			continue;
		}
		f.arg = v[pos++];
		Txt r;
		if(!nvl_value.IsVoid() && IsNull(f.arg))
			r = nvl_value;
		else
		{
			Formatter ft = NULL;
#ifdef _DEBUG
			i32 fi = formatmap().Find(FormId(f.id, f.arg.GetType()));
			if(fi < 0) fi = formatmap().Find(FormId(f.id, VALUE_V));
			if(fi >= 0)
				ft = formatmap()[fi];
#else
			for(;;) {
				i32 fi = formatmap().Find(FormId(f.id, f.arg.GetType()));
				if(fi < 0) fi = formatmap().Find(FormId(f.id, VALUE_V));
				if(fi >= 0)
				{
					ft = formatmap()[fi];
					break;
				}
				if(f.id.GetLength() == 0) break;
				f.id.Trim(f.id.GetLength() - 1);
				s--;
			}
#endif
			if(ft)
				r = (*ft)(f);
			else
				r << "<N/A '" << f.id << "' for type " << (i32)f.arg.GetType() << ">";
		}
		i32 len;
		if(padn < 0 || padn > 1000)
			r << "<invalid padding>";
		else
		switch(pad) {
		case ALIGN_LEFT:
			len = r.GetCharCount();
			if(len < padn)
				r.Cat(' ', padn - len);
			else
				TrimChar(r, padn);
			break;
		case ALIGN_RIGHT:
			len = r.GetCharCount();
			if(len < padn) {
				Txt fill(' ', padn - len);
				r = fill + r;
			}
			else
				TrimChar(r, padn);
			break;
		case ALIGN_CENTER:
			len = r.GetCharCount();
			if(len < padn) {
				i32 ll = (padn - len) / 2;
				r = Txt(' ', ll) + r;
				r.Cat(' ', padn - len - ll);
			}
			else
				TrimChar(r, padn);
			break;
		}
		result.Cat(r);
		if(*s == '`')
			s++;
	}
}

Txt Format(tukk s, const Vec<Val>& v) { return Format(GetCurrentLanguage(), s, v); }

Txt Sprintf(tukk fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
	return VFormat(fmt, argptr);
}

Txt DeFormat(tukk text)
{
	TxtBuf x;
	while(*text) {
		if(*text == '%')
			x.Cat('%');
		x.Cat(*text++);
	}
	return Txt(x);
}

}