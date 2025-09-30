i32 utoa32(dword value, tuk buffer);
i32 utoa64(zu64 value, tuk buffer);

Txt FormatUnsigned(dword w);
Txt FormatInt(i32 i);
Txt FormatUInt64(zu64 w);
Txt FormatInt64(z64 i);

Txt         FormatIntBase(i32 i, i32 base, i32 width = 0, char lpad = ' ', i32 sign = 0, bool upper = false);
Txt         FormatIntDec(i32 i, i32 width, char lpad = ' ', bool always_sign = false);
Txt         FormatIntHex(i32 i, i32 width = 8, char lpad = '0');
Txt         FormatIntHexUpper(i32 i, i32 width = 8, char lpad = '0');
Txt         FormatIntOct(i32 i, i32 width = 12, char lpad = '0');
Txt         FormatIntRoman(i32 i, bool upper = false);
Txt         FormatIntAlpha(i32 i, bool upper = true);
Txt         Format64Hex(zu64 a);

#ifdef CPU_64
inline Txt  FormatIntHex(ukk ptr) { return Format64Hex((z64)(uintptr_t)ptr); }
inline Txt  FormatHex(ukk ptr)    { return Format64Hex((z64)(uintptr_t)ptr); }
#else
inline Txt  FormatIntHex(ukk ptr) { return FormatIntHex((i32)(uintptr_t)ptr); }
inline Txt  FormatHex(ukk ptr)    { return FormatIntHex((i32)(uintptr_t)ptr); }
#endif

enum
{
	FD_SIGN     = 0x01,  // always prepend sign (+10)
	FD_MINUS0   = 0x02,  // show sign for negative zero (-0)
	FD_SIGN_EXP = 0x04,  // always prepend sign to exponent (1e+2)
	FD_CAP_E    = 0x08,  // capital E for exponent (1E10)
	FD_ZEROS    = 0x10,  // keep trailing zeros (1.25000)
	FD_FIX      = 0x20,  // always use fixed notation (FormatF)
	FD_EXP      = 0x40,  // always use exponential notation (FormatG)
	FD_COMMA       = 0x80,  // use ',' instead of '.'
	FD_NOTHSEPS    = 0x100, // In i18n, do not use thousands separators
	FD_MINIMAL_EXP = 0x1000, // Use minimal exp (1e5 instead 1e+05)
	FD_SPECIAL     = 0x2000, // Print nan, inf (if not specified output is empty for nan/inf)
	FD_SIGN_SPACE  = 0x4000, // Add space in place of sign for positive numbers ( 10)
	FD_POINT       = 0x8000, // Always add decimal point
};

inline constexpr dword
FD_TOLERANCE(i32 x) // how many zeroes can be on the right side of '.' until changing to E format
{ return x << 16; }

char  *FormatE(tuk t, double x, i32 precision, dword flags = FD_SPECIAL|FD_MINUS0|FD_SIGN_EXP);
Txt   FormatE(double x, i32 precision, dword flags = FD_SPECIAL|FD_MINUS0|FD_SIGN_EXP);

tuk FormatF(tuk t, double x, i32 precision, dword flags = FD_SPECIAL|FD_MINUS0|FD_SIGN_EXP|FD_ZEROS);
Txt   FormatF(double x, i32 precision, dword flags = FD_SPECIAL|FD_MINUS0|FD_SIGN_EXP|FD_ZEROS);

tuk FormatG(tuk t, double x, i32 precision, dword flags = FD_SPECIAL|FD_MINUS0|FD_SIGN_EXP|FD_TOLERANCE(3));
Txt   FormatG(double x, i32 precision, dword flags = FD_SPECIAL|FD_MINUS0|FD_SIGN_EXP|FD_TOLERANCE(3));

tuk FormatDouble(tuk t, double x, i32 precision, dword flags = FD_TOLERANCE(6)|FD_MINIMAL_EXP|FD_SPECIAL);
Txt   FormatDouble(double x, i32 precision, dword flags = FD_TOLERANCE(6)|FD_MINIMAL_EXP|FD_SPECIAL);

char  *FormatDouble(tuk t, double x);
Txt   FormatDouble(double x);

Txt   FormatDoubleN(double x);

inline Txt FormatFloat(float x) { return FormatDouble(x, 7, FD_TOLERANCE(6)|FD_MINIMAL_EXP|FD_SPECIAL); }
inline Txt FormatFloatN(float x) { return FormatDouble(x, 7, FD_TOLERANCE(6)|FD_MINIMAL_EXP); }

Txt         FormatDate(Date date, tukk format, i32 language = 0);
Txt         FormatTime(Time time, tukk format, i32 language = 0);

template<> inline Txt AsTxt(const short& a)           { return FormatInt(a); }
template<> inline Txt AsTxt(u16k& a)  { return FormatUnsigned(a); }
template<> inline Txt AsTxt(i32k& a)             { return FormatInt(a); }
template<> inline Txt AsTxt(u32k& a)    { return FormatUnsigned(a); }
template<> inline Txt AsTxt(const long& a)            { return FormatInt64(a); }
template<> inline Txt AsTxt(const u64& a)   { return FormatUInt64(a); }
template<> inline Txt AsTxt(const z64& a)           { return FormatInt64(a); }
template<> inline Txt AsTxt(zu64k& a)          { return FormatUInt64(a); }
template<> inline Txt AsTxt(const double& a)          { return FormatDoubleN(a); }
template<> inline Txt AsTxt(const float& a)           { return FormatFloatN(a); }

/*
Date        ScanDate(tukk text, tukk *endptr, tukk format, i32 language, Date base_date);
Time        ScanTime(tukk text, tukk *endptr, tukk format, i32 language, Time base_time);
*/

struct Formatting
{
	i32    language;
	Val  arg;
	Txt format;
	Txt id;
};

typedef Txt (*Formatter)(const Formatting& fmt);

void RegisterFormatter(i32 type, tukk id, Formatter f);
void RegisterNullFormatter(tukk id, Formatter f);
void RegisterNumberFormatter(tukk id, Formatter f);
void RegisterTxtFormatter(tukk id, Formatter f);
void RegisterDateTimeFormatter(tukk id, Formatter f);
void RegisterValFormatter(tukk id, Formatter f);

Txt Format(tukk s, const Vec<Val>& v);
Txt Format(i32 language, tukk s, const Vec<Val>& v);

template <typename... Args>
Txt Format(i32 language, tukk s, const Args& ...args)
{
	return Format(language, s, gather<Vec<Val>>(args...));
}

template <typename... Args>
Txt Format(tukk s, const Args& ...args)
{
	return Format(s, gather<Vec<Val>>(args...));
}

Txt VFormat(tukk fmt, va_list args);
Txt Sprintf(tukk fmt, ...);

Txt DeFormat(tukk text);

// DEPRECATED

template <typename... Args>
Txt NFormat(i32 language, tukk s, const Args& ...args)
{
	return Format(language, s, gather<Vec<Val>>(args...));
}

template <typename... Args>
Txt NFormat(tukk s, const Args& ...args)
{
	return Format(s, gather<Vec<Val>>(args...));
}

inline Txt         FormatInteger(i32 a)                  { return FormatInt(a); }
inline Txt         Format64(zu64 a)                    { return FormatUInt64(a); }

inline Txt         FormatDoubleFix(double x, i32 digits) { return FormatF(x, digits); }
inline Txt         FormatDoubleExp(double x, i32 digits) { return FormatE(x, digits); }