#ifdef TARGET_WIN32
#ifndef _MAX_PATH
static i32k _MAX_PATH = MAX_PATH;
#endif
#endif

#ifdef TARGET_POSIX
static i32k _MAX_PATH = PATH_MAX;
dword  GetTickCount();
#endif

z64 usecs(z64 prev = 0);
i32 msecs(i32 prev = 0);

class TimeStop : Moveable<TimeStop> {
	double starttime;

public:
	double Elapsed() const           { return usecs() - starttime; }
	double Seconds() const           { return (double)Elapsed() / 1000000; }
	Txt ToTxt() const;
	void   Reset();

	TimeStop();
};

struct TimeStopper {
	Txt   name;
	TimeStop tm;

	TimeStopper(const Txt& name) : name(name) {}
	~TimeStopper() { RLOG(name << " " << tm); }
};

#define RTIMESTOP(name) TimeStopper COMBINE(sTmStop, __LINE__)(name);

void   SetAssertFailedHook(void (*h)(tukk ));

void   ReloadIniFile();
void   SetIniFile(tukk path = NULL);
Txt GetIniFile();
Txt GetIniKey(tukk id, const Txt& def);
Txt GetIniKey(tukk id);

VecMap<Txt, Txt> GetIniKeys();

extern i32  ini_version__;
inline bool IniChanged__(i32 version) { return version != ini_version__; }

struct IniTxt {
// "private":
	const char   *id;
	Txt      (*def)();
	Txt&     (*ref_fn)();
	i32           version;

// "public:"
	operator Txt();
	Txt   operator=(const Txt& s);
	Txt   ToTxt() const;
};

struct IniInt {
// "private":
	tukk id;
	i32       (*def)();
	i32         version;
	i32         value;
	i32         Load();

// "public:"
	operator    i32()             { i32 h = value; if(IniChanged__(version)) return Load(); return h; }
	i32         operator=(i32 b);
	Txt      ToTxt() const;
};

struct IniInt64 {
// "private":
	tukk id;
	z64     (*def)();
	i32         version;
	z64       value;

// "public:"
	operator    z64();
	z64       operator=(z64 b);
	Txt      ToTxt() const;
};

struct IniDouble {
// "private":
	tukk id;
	double    (*def)();
	i32         version;
	double      value;
	double      Load();

// "public:"
	operator    double()           { double h = value; if(IniChanged__(version)) return Load(); return h; }
	double      operator=(double b);
	Txt      ToTxt() const;
};

struct IniBool {
// "private":
	tukk id;
	bool      (*def)();
	i32         version;
	bool        value;
	bool        Load();

// "public:"
	operator     bool()            { bool h = value; if(IniChanged__(version)) return Load(); return h; }
	bool         operator=(bool b);
	Txt       ToTxt() const;
};

void AddIniInfo(tukk id, Txt (*current)(), Txt (*def)(), tukk info);

struct IniInfo {
	Txt id;
	Txt info;
	Txt (*current)();
	Txt (*def)();
};

const Array<IniInfo>& GetIniInfo();
Txt GetIniInfoFormatted();
Txt DefaultIniFileContent();
Txt CurrentIniFileContent(bool comment_defaults);

#define INI_TYPE(var, def, info, type, decl, ref)\
type DefIni_##var() { return def; }\
decl var = { #var, DefIni_##var, ref };\
Txt AsTxtIniCurrent_##var() { return AsTxt(var); } \
Txt AsTxtIniDefault_##var() { return AsTxt(DefIni_##var()); } \
INITBLOCK { AddIniInfo(#var, AsTxtIniCurrent_##var, AsTxtIniDefault_##var, info); }

#define INI_BOOL(var, def, info)   INI_TYPE(var, def, info, bool, IniBool, 0);
#define INI_INT(var, def, info)    INI_TYPE(var, def, info, i32, IniInt, 0);
#define INI_INT64(var, def, info)  INI_TYPE(var, def, info, z64, IniInt64, 0);
#define INI_DOUBLE(var, def, info) INI_TYPE(var, def, info, double, IniDouble, 0);

#define INI_TXT(var, def, info) Txt& DefRef_##var() { static Txt x; return x; }\
                                   INI_TYPE(var, def, info, Txt, IniTxt, DefRef_##var);

VecMap<Txt, Txt> LoadIniStream(Stream &in);
VecMap<Txt, Txt> LoadIniFile(tukk filename);

Txt timeFormat(double second);

Txt HexEncode(u8k *s, i32 count, i32 sep = INT_MAX, i32 sepchr = ' ');
inline Txt HexEncode(tukk s, i32 count, i32 sep = INT_MAX, i32 sepchr = ' ') { return HexEncode((u8 *)s, count, sep, sepchr); }
inline Txt HexEncode(ukk s, i32 count, i32 sep = INT_MAX, i32 sepchr = ' ') { return HexEncode((u8 *)s, count, sep, sepchr); }
Txt HexEncode(const Txt& s, i32 sep = INT_MAX, i32 sepchr = ' ');

Txt HexDecode(tukk s, tukk lim);
inline Txt HexDecode(tukk s, i32 len) { return HexDecode(s, s + len); }
inline Txt HexDecode(const Txt& s)        { return HexDecode(~s, s.GetCount()); }

Txt  ToSystemCharset(const Txt& src, i32 cp);
Txt  ToSystemCharset(const Txt& src);
Txt  FromWin32Charset(const Txt& src, i32 cp);
Txt  FromSystemCharset(const Txt& src);

Vec<char16> ToSystemCharsetW(const WTxt& src);
Vec<char16> ToSystemCharsetW(const Txt& src);
Vec<char16> ToSystemCharsetW(wtukk src);
Vec<char16> ToSystemCharsetW(tukk src);

Txt         FromSystemCharsetW(const char16 *src);

#ifdef TARGET_WIN32
Txt FromOEMCharset(const Txt& src);
Txt GetErrorMessage(dword dwError);
#endif

#ifdef TARGET_POSIX
inline i32 GetLastError() { return errno; }
Txt GetErrorMessage(i32 errorno);
#endif

Txt GetLastErrorMessage();

void   BeepInformation();
void   BeepExclamation();
void   BeepQuestion();
void   BeepError();

inline
void memsetex(uk t, ukk item, i32 item_size, i32 count) {
	ASSERT(count >= 0);
	u8 *q = (u8 *)t;
	while(count--) {
		memcpy8(q, item, item_size);
		q += item_size;
	}
}

tuk PermanentCopy(tukk s);

i32 MemICmp(ukk dest, ukk src, i32 count);

Txt NormalizeSpaces(tukk s);
Txt NormalizeSpaces(tukk begin, tukk end);

Txt         CsvTxt(const Txt& text);
Vec<Txt> GetCsvLine(Stream& s, i32 separator, u8 charset);

Txt         CompressLog(tukk s);

#ifndef TARGET_WIN32
void Sleep(i32 msec);
#endif

template <class T>
void Zero(T& obj)
{
	::memset(&obj, 0, sizeof(obj));
}

template <class T>
T& Reconstruct(T& object)
{
	object.~T();
	::new(&object) T;
	return object;
}

template <class T>
inline void Dbl_Unlink(T *x)
{
	x->prev->next = x->next; x->next->prev = x->prev;
}

template <class T>
inline void Dbl_LinkAfter(T *x, T *lnk)
{
	x->prev = lnk; x->next = lnk->next; x->next->prev = x; lnk->next = x;
}

template <class T>
inline void Dbl_Self(T *x)
{
	x->prev = x->next = x;
}

#define ZeroArray(x)       memset((x), 0, sizeof(x))

dword   Random();
dword   Random(dword n);
zu64  Random64();
zu64  Random64(zu64 n);
void    Random64(zu64 *t, i32 n);
double  Randomf();

void  SeedRandom(dword *seed, i32 len);
void  SeedRandom(dword seed);
void  SeedRandom();

// Math utils

inline double  sqr          (double a)                      { return a * a; }
inline double  argsinh      (double s)                      { return log(s + sqrt(s * s + 1)); }
inline double  argcosh      (double c)                      { ASSERT(c >= 1); return log(c + sqrt(c * c - 1)); }
inline double  argtanh      (double t)                      { ASSERT(fabs(t) < 1); return log((1 + t) / (1 - t)) / 2; }

i32            iscale(i32 x, i32 y, i32 z);
i32            iscalefloor(i32 x, i32 y, i32 z);
i32            iscaleceil(i32 x, i32 y, i32 z);
i32            idivfloor(i32 x, i32 y);
i32            idivceil(i32 x, i32 y);
i32            itimesfloor(i32 x, i32 y);
i32            itimesceil(i32 x, i32 y);

i32            fround(double x);
i32            ffloor(double x);
i32            fceil(double x);

z64          fround64(double x);
z64          ffloor64(double x);
z64          fceil64(double x);

Txt         AsTxt(double x, i32 nDigits);
double         modulo(double x, double y);

i32            ilog10       (double d);
double         ipow10       (i32 i);
double         normalize    (double d, i32& exponent);

double         roundr       (double d, i32 digits);
double         floorr       (double d, i32 digits);
double         ceilr        (double d, i32 digits);

i32 SolveQuadraticEquation(double A, double B, double C, double *r);

// Constants rounded for 21 decimals.

#ifndef M_E

#define M_E         2.71828182845904523536
#define M_LOG2E     1.44269504088896340736
#define M_LOG10E    0.434294481903251827651
#define M_LN2       0.693147180559945309417
#define M_LN10      2.30258509299404568402
#define M_PI        3.14159265358979323846
#define M_PI_2      1.57079632679489661923
#define M_PI_4      0.785398163397448309616
#define M_1_PI      0.318309886183790671538
#define M_2_PI      0.636619772367581343076
#define M_1_SQRTPI  0.564189583547756286948
#define M_2_SQRTPI  1.12837916709551257390
#define M_SQRT2     1.41421356237309504880
#define M_SQRT_2    0.707106781186547524401

#endif

#define M_2PI       (2 * M_PI)

// ------

//# System dependent !
class BitAndPtr {
	uintptr_t bap;

public:
	void  SetBit(bool b)  { bap = (~1 & bap) | (uintptr_t)b; }
	void  SetPtr(uk p) { bap = (1 & bap) | (uintptr_t)p; }

	bool  GetBit() const  { return bap & 1; }
	uk GetPtr() const  { return (uk ) (bap & ~1); }

	void  Set0(uk ptr) { bap = (uintptr_t)ptr; }
	void  Set1(uk ptr) { bap = (uintptr_t)ptr | 1; }

	BitAndPtr()           { bap = 0; }
};

class AbortExc : public Exc {
public:
	AbortExc();
};

// ---------------

i32  InScListIndex(tukk s, tukk list);
bool InScList(tukk s, tukk list);

// ------------------- Linux style text settings -------------

class TextSettings {
	VecMap< Txt, VecMap< Txt, Txt > > settings;

public:
	Txt Get(tukk group, tukk key) const;
	Txt Get(tukk key) const                            { return Get("", key); }
	Txt Get(i32 groupIndex, tukk key) const;
	Txt Get(i32 groupIndex, i32 keyIndex) const;

	Txt operator()(tukk group, tukk key) const  { return Get(group, key); }
	Txt operator()(tukk key) const                     { return Get(key); }

	void Clear()                                                 { settings.Clear(); }
	void Load(tukk filename);

	i32 GetGroupCount()                                          { return settings.GetCount(); }
	i32 GetKeyCount(i32 group)                                   { return settings[group].GetCount(); }

	Txt GetGroupName(i32 groupIndex)                          { return settings.GetKey(groupIndex); }
	Txt GetKey(i32 groupIndex, i32 keyIndex)                  { return settings[groupIndex].GetKey(keyIndex); }
};

// ------------------- Advanced streaming --------------------

void CheckedSerialize(const Event<Stream&> serialize, Stream& stream, i32 version = Null);

bool Load(Event<Stream&> serialize, Stream& stream, i32 version = Null);
bool Store(Event<Stream&> serialize, Stream& stream, i32 version = Null);
bool LoadFromFile(Event<Stream&> serialize, tukk file = NULL, i32 version = Null);
bool StoreToFile(Event<Stream&> serialize, tukk file = NULL, i32 version = Null);

template <class T>
void SerializeTFn(Stream &s, T *x)
{
	s % *x;
}

template <class T>
Event<Stream&> SerializeCb(T& x)
{
	return callback1(SerializeTFn<T>, &x);
}

template <class T>
bool Load(T& x, Stream& s, i32 version = Null) {
	return Load(SerializeCb(x), s, version);
}

template <class T>
bool Store(T& x, Stream& s, i32 version = Null) {
	return Store(SerializeCb(x), s, version);
}

template <class T>
bool LoadFromFile(T& x, tukk name = NULL, i32 version = Null) {
	return LoadFromFile(SerializeCb(x), name, version);
}

template <class T>
bool StoreToFile(T& x, tukk name = NULL, i32 version = Null) {
	return StoreToFile(SerializeCb(x), name, version);
}

template <class T>
Txt StoreAsTxt(T& x) {
	TxtStream ss;
	Store(x, ss);
	return ss;
}

template <class T>
bool LoadFromTxt(T& x, const Txt& s) {
	TxtStream ss(s);
	return Load(x, ss);
}

void             RegisterGlobalConfig(tukk name);
void             RegisterGlobalSerialize(tukk name, Event<Stream&> WhenSerialize);
void             RegisterGlobalConfig(tukk name, Event<>  WhenFlush);

Txt           GetGlobalConfigData(tukk name);
void             SetGlobalConfigData(tukk name, const Txt& data);

template <class T>
bool LoadFromGlobal(T& x, tukk name)
{
	TxtStream ss(GetGlobalConfigData(name));
	return ss.IsEof() || Load(x, ss);
}

template <class T>
void StoreToGlobal(T& x, tukk name)
{
	TxtStream ss;
	Store(x, ss);
	SetGlobalConfigData(name, ss);
}

bool LoadFromGlobal(Event<Stream&> serialize, tukk name);
void StoreToGlobal(Event<Stream&> serialize, tukk name);

void SerializeGlobalConfigs(Stream& s);

#ifdef TARGET_WINCE
inline void abort() { TerminateProcess(NULL, -1); }
#endif

template <class T>
hash_t HashBySerialize(const T& cont)
{
	TimeStop tm;
	xxHashStream xxh;
	const_cast<T&>(cont).Serialize(xxh);
	return xxh.Finish();
}

template <class T>
size_t SizeBySerialize(const T& cont)
{
	TimeStop tm;
	SizeStream szs;
	const_cast<T&>(cont).Serialize(szs);
	return (size_t)szs;
}

template <class T>
bool IsEqualBySerialize(const T& a, const T& b)
{
	TxtStream sa;
	const_cast<T&>(a).Serialize(sa);

	TxtStream ss(sa.GetResult());
	CompareStream cs(ss);

	const_cast<T&>(b).Serialize(cs);
	return cs;
}

Txt  Replace(const Txt& s, const Vec<Txt>& find, const Vec<Txt>& replace);
Txt  Replace(const Txt& s, const VecMap<Txt, Txt>& fr);
WTxt Replace(const WTxt& s, const Vec<WTxt>& find, const Vec<WTxt>& replace);
WTxt Replace(const WTxt& s, const VecMap<WTxt, WTxt>& fr);

bool SpellWordRaw(const WTxt& wrd, i32 lang, Vec<Txt> *withdia = NULL);
bool SpellWord(const WTxt& ws, i32 lang);
bool SpellWord(wtukk ws, i32 len, i32 lang);
void SpellerAdd(const WTxt& w, i32 lang);
bool AllSpellerWords(i32 lang, Gate<Txt> iter);
Vec<Txt> SpellerFindCloseWords(i32 lang, const Txt& w, i32 n);

Txt GetP7Signature(ukk data, i32 length, const Txt& cert_pem, const Txt& pkey_pem);
Txt GetP7Signature(const Txt& data, const Txt& cert_pem, const Txt& pkey_pem);

// deprecated
Txt HexTxt(u8k *s, i32 count, i32 sep = INT_MAX, i32 sepchr = ' ');
inline Txt HexTxt(tukk s, i32 count, i32 sep = INT_MAX, i32 sepchr = ' ') { return HexTxt((u8 *)s, count, sep, sepchr); }
inline Txt HexTxt(ukk s, i32 count, i32 sep = INT_MAX, i32 sepchr = ' ') { return HexTxt((u8 *)s, count, sep, sepchr); }
Txt HexTxt(const Txt& s, i32 sep = INT_MAX, i32 sepchr = ' ');

Txt ScanHexTxt(tukk s, tukk lim);
inline Txt ScanHexTxt(tukk s, i32 len) { return ScanHexTxt(s, s + len); }
inline Txt ScanHexTxt(const Txt& s)        { return ScanHexTxt(~s, s.GetCount()); }

Txt Garble(tukk s, tukk e);
Txt Garble(const Txt& s);

Txt Encode64(const Txt& s);
Txt Decode64(const Txt& s);