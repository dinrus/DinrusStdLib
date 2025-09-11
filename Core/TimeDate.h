class Nuller;
class Stream;
struct FileTime;

struct Date : RelOps< Date, Moveable<Date> > {
	u8     day;
	u8     month;
	i16  year;

	void     Serialize(Stream& s);

	bool     IsValid() const;
	void     Set(i32 scalar);
	i32      Get() const;

	static Date Low()                    { return Date(-4000, 1, 1); }
	static Date High()                   { return Date(4000, 1, 1); }

	i32      Compare(Date b) const;

	Date& operator++()                   { if(day < 28) day++; else Set(Get() + 1); return *this; }
	Date& operator--()                   { if(day > 1) day--; else Set(Get() - 1); return *this; }

	Date  operator++(i32)                { Date d = *this; operator++(); return d; }
	Date  operator--(i32)                { Date d = *this; operator--(); return d; }

	Date()                               { year = -32768; day = month = 0; }
	Date(const Nuller&)                  { year = -32768; day = month = 0; }
	Date(i32 y, i32 m, i32 d)            { day = d; month = m; year = y; }
};

inline hash_t GetHashVal(Date t) {
	return 512 * t.year + 32 * t.month + t.day;
}

inline bool operator==(Date a, Date b) {
	return a.day == b.day && a.month == b.month && a.year == b.year;
}

template<> inline bool  IsNull(const Date& d)    { return d.year == -32768; }

bool operator<(Date a, Date b);

i32   operator-(Date a, Date b);
Date  operator+(Date a, i32 b);
Date  operator+(i32 a, Date b);
Date  operator-(Date a, i32 b);
Date& operator+=(Date& a, i32 b);
Date& operator-=(Date& a, i32 b);

bool IsLeapYear(i32 year);

i32  GetDaysOfMonth(i32 month, i32 year);

i32  DayOfWeek(Date date);
Date LastDayOfMonth(Date d);
Date FirstDayOfMonth(Date d);
Date LastDayOfYear(Date d);
Date FirstDayOfYear(Date d);
i32  DayOfYear(Date d);

Date AddMonths(Date date, i32 months);
i32  GetMonths(Date since, Date till);
i32  GetMonthsP(Date since, Date till);
Date AddYears(Date date, i32 years);

Date GetWeekDate(i32 year, i32 week);
i32  GetWeek(Date d, i32& year);

Date EasterDay(i32 year);

Date GetSysDate();

Txt    DayName(i32 i, i32 lang = 0);
Txt    DyName(i32 i, i32 lang = 0);
Txt    MonthName(i32 i, i32 lang = 0);
Txt    MonName(i32 i, i32 lang = 0);

void   SetDateFormat(tukk fmt);
void   SetDateScan(tukk scan);
void   SetDateFilter(tukk seps);

Txt        GetDateFormat();

tukk        StrToDate(tukk fmt, Date& d, tukk s, Date def = Null);
tukk        StrToDate(Date& d, tukk s, Date def);
tukk        StrToDate(Date& d, tukk s);
Date        ScanDate(tukk fmt, tukk s, Date def = Null);
Date        ScanDate(tukk s, Date def = Null);
Txt         Format(Date date, tukk fmt);
Txt         Format(Date date);
wchar       CharFilterDate(wchar c);

template<>
inline Txt AsTxt(const Date& date) { return Format(date); }

struct Time : Date, RelOps< Time, Moveable<Time> > {
	u8   hour;
	u8   minute;
	u8   second;

	void     Serialize(Stream& s);

	static Time High()       { return Time(4000, 1, 1); }
	static Time Low()        { return Time(-4000, 1, 1); }

	void   Set(z64 scalar);
	z64  Get() const;

	bool   IsValid() const;

	i32    Compare(Time b) const;

	Time()                   { hour = minute = second = 0; }
	Time(const Nuller&)      { hour = minute = second = 0; }
	Time(i32 y, i32 m, i32 d, i32 h = 0, i32 n = 0, i32 s = 0)
		{ day = d; month = m; year = y; hour = h; minute = n; second = s; }

	Time(FileTime filetime);
	FileTime AsFileTime() const;
};

inline Time ToTime(const Date& d) {
	return Time(d.year, d.month, d.day);
}

inline hash_t GetHashVal(Time t) {
	return t.second +
		   32 * (t.minute + 32 * (t.hour + 16 * (t.day + 32 * (t.month + 8 * t.year))));
}

template<> inline bool  IsNull(const Time& t)    { return t.year == -32768; }

bool operator==(Time a, Time b);
bool operator<(Time a, Time b);

z64  operator-(Time a, Time b);
Time   operator+(Time a, z64 seconds);
Time   operator+(z64 seconds, Time a);
Time   operator-(Time a, z64 secs);
Time&  operator+=(Time& a, z64 secs);
Time&  operator-=(Time& a, z64 secs);

inline Time   operator+(Time a, i32 i)   { return a +  z64(i); }
inline Time   operator-(Time a, i32 i)   { return a -  z64(i); }
inline Time&  operator+=(Time& a, i32 i) { return a += z64(i); }
inline Time&  operator-=(Time& a, i32 i) { return a -= z64(i); }

inline Time   operator+(Time a, double i)   { return a +  z64(i); }
inline Time   operator-(Time a, double i)   { return a -  z64(i); }
inline Time&  operator+=(Time& a, double i) { return a += z64(i); }
inline Time&  operator-=(Time& a, double i) { return a -= z64(i); }

Time  GetSysTime();
Time  GetUtcTime();

Txt Format(Time time, bool seconds = true);
tukk StrToTime(tukk datefmt, Time& d, tukk s);
tukk StrToTime(Time& d, tukk s);
Time        ScanTime(tukk datefmt, tukk s, Time def = Null);
Time        ScanTime(tukk s, Time def = Null);

template<>
inline Txt AsTxt(const Time& time) { return Format(time); }

bool SetSysTime(Time time); // only root/sysadmin can do this...

i32    GetTimeZone();
Txt GetTimeZoneText();
i32    ScanTimeZoneText(tukk s);
i32    ScanTimeZone(tukk s);

i32   GetLeapSeconds(Date dt);
z64 GetUTCSeconds(Time tm);
Time  TimeFromUTC(z64 seconds);