#include "../Core.h"

#ifdef TARGET_WIN32
#	include <winnls.h>
#endif

#if defined(TARGET_POSIX) && defined(COMPILER_GCC) && !defined(TARGET_ANDROID) && defined(flagSTACKTRACE)
#	include <execinfo.h>
#	include <cxxabi.h>
#endif

namespace drx {

bool PanicMode;

bool    IsPanicMode() { return PanicMode; }

static  void (*sPanicMessageBox)(tukk title, tukk text);

void InstallPanicMessageBox(void (*mb)(tukk title, tukk text))
{
	sPanicMessageBox = mb;
}

void PanicMessageBox(tukk title, tukk text)
{
	PanicMode = true;
	if(sPanicMessageBox)
		(*sPanicMessageBox)(title, text);
	else {
		IGNORE_RESULT(
			write(2, text, (i32)strlen(text))
		);
		IGNORE_RESULT(
			write(2, "\n", 1)
		);
	}
}


#if defined(TARGET_LINUX) && defined(COMPILER_GCC) && !defined(TARGET_ANDROID) && defined(flagSTACKTRACE)
void AddStackTrace(tuk  str, i32 len)
{
	const size_t max_depth = 100;
    uk stack_addrs[max_depth];
    tuk* stack_strings;
    const char msg[] = "\nТрассировка стэка:\n";

    size_t stack_depth = backtrace(stack_addrs, max_depth);
    stack_strings = backtrace_symbols(stack_addrs, stack_depth);

	i32 space = len - strlen(str);
	strncat(str, msg, max(space, 0));
	space -= sizeof(msg) - 1;

    for (size_t i = 0; i < stack_depth && space > 0; i++) {

		tuk  start = strchr(stack_strings[i], '(');
		if (start == NULL) continue;

		size_t len;
		i32 stat;

		tuk  end = strchr(start, '+');
		if (end != NULL) *end = '\0';

		tuk  demangled = abi::__cxa_demangle(start+1, NULL, &len, &stat);

		if (stat == 0 && demangled != NULL){
			strncat(str, demangled, max(space, 0));
			space -= len;
		}else{
			strncat(str, start, max(space, 0));
			space -= strlen(start);
		}
		if (demangled != NULL) free(demangled);

		strncat(str, "\n", max(space, 0));
		space -= 1;
    }
    
    free(stack_strings);
}
#endif


void    Panic(tukk msg)
{
#ifdef TARGET_POSIX
	signal(SIGILL, SIG_DFL);
	signal(SIGSEGV, SIG_DFL);
	signal(SIGBUS, SIG_DFL);
	signal(SIGFPE, SIG_DFL);
#endif
	if(PanicMode)
		return;
	PanicMode = true;
	RLOG("****************** PANIC: " << msg << "\n");
	PanicMessageBox("Фатальная ошибка", msg);

#ifdef TARGET_WIN32
#	ifdef __NOASSEMBLY__
#		if defined(TARGET_WINCE) || defined(WIN64)
			DebugBreak();
#		endif
#	else
#		if defined(_DEBUG) && defined(CPU_X86)
#			ifdef COMPILER_MSC
				_asm i32 3
#			endif
#			ifdef COMPILER_GCC
				asm("i32 $3");
#			endif
#		endif
#	endif
#else
#endif
#ifdef TARGET_POSIX
	raise(SIGTRAP);
#endif
#ifdef _DEBUG
	__BREAK__;
#endif
	abort();
}

static void (*s_assert_hook)(tukk );

void    SetAssertFailedHook(void (*h)(tukk ))
{
	s_assert_hook = h;
}

void    AssertFailed(tukk file, i32 line, tukk cond)
{
	if(PanicMode)
		return;
	PanicMode = true;
	char s[2048];
	snprintf(s, 2048, "Assertion failed in %s, line %d\n%s\n", file, line, cond);
#if defined(TARGET_LINUX) && defined(COMPILER_GCC) && defined(flagSTACKTRACE)
	AddStackTrace(s, sizeof(s));
#endif

	if(s_assert_hook)
		(*s_assert_hook)(s);
	RLOG("****************** ASSERT FAILED: " << s << "\n");
#ifdef TARGET_POSIX
	RLOG("LastErrorMessage: " << strerror(errno)); // do not translate
#else
	RLOG("LastErrorMessage: " << GetLastErrorMessage());
#endif

	PanicMessageBox("Fatal error", s);

#ifdef TARGET_WIN32
#	ifdef __NOASSEMBLY__
#		if defined(TARGET_WINCE) || defined(WIN64)
			DebugBreak();
#		endif
#	else
#		if defined(_DEBUG) && defined(CPU_X86)
#			ifdef COMPILER_MSC
				_asm i32 3
#			endif
#			ifdef COMPILER_GCC
				asm("i32 $3");
#			endif
#		endif
#	endif
#else
#endif
#ifdef TARGET_POSIX
	raise(SIGTRAP);
#endif
#ifdef _DEBUG
	__BREAK__;
#endif
	abort();
}

#ifdef TARGET_POSIX
dword GetTickCount() {
#if _POSIX_C_SOURCE >= 199309L
	struct timespec tp;
	if (clock_gettime(CLOCK_MONOTONIC, &tp) == 0)
	{
		return (dword)((tp.tv_sec * 1000) + (tp.tv_nsec / 1000000));
	}
	return 0; // ?? (errno is set)
#else
	struct timeval tv[1];
	struct timezone tz[1];
	memset(tz, 0, sizeof(tz));
	gettimeofday(tv, tz);
	return (dword)tv->tv_sec * 1000 + tv->tv_usec / 1000;
#endif
}
#endif

z64 usecs(z64 prev)
{
	auto p2 = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(p2.time_since_epoch()).count() - prev;
}

i32 msecs(i32 from) { return GetTickCount() - (dword)from; }

/*// it looks like there might be a problem with std::chrono in llvm-mingw, reverting to original implementation for now
i32 msecs(i32 prev)
{
	auto p2 = std::chrono::steady_clock::now();
	return (i32)std::chrono::duration_cast<std::chrono::milliseconds>(p2.time_since_epoch()).count() - prev;
}
*/

void TimeStop::Reset()
{
	starttime = (double)usecs();
}

TimeStop::TimeStop()
{
	Reset();
}

Txt TimeStop::ToTxt() const
{
	double time = Elapsed();
	if(time < 1e3)
		return Txt() << time << " us";
	if(time < 1e6)
		return Txt() << time / 1e3 << " ms";
	return Txt() << time / 1e6 << " s";
}

i32 RegisterTypeNo__(tukk type)
{
	INTERLOCKED {
		static Index<Txt> types;
		return types.FindAdd(type);
	}
	return -1;
}

tuk PermanentCopy(tukk s)
{
	tuk t = (tuk )MemoryAllocPermanent(strlen(s) + 1);
	strcpy(t, s);
	return t;
}

#ifndef TARGET_WIN32
void Sleep(i32 msec)
{
	::timespec tval;
	tval.tv_sec = msec / 1000;
	tval.tv_nsec = (msec % 1000) * 1000000;
	nanosleep(&tval, NULL);
}
#endif

i32 MemICmp(ukk dest, ukk src, i32 count)
{

	u8k *a = (u8k *)dest;
	u8k *b = (u8k *)src;
	u8k *l = a + count;
	while(a < l) {
		if(ToUpper((i8) *a) != ToUpper((i8)*b))
			return ToUpper((i8)*a) - ToUpper((i8)*b);
		a++;
		b++;
	}
	return 0;
}

Stream& Pack16(Stream& s, Point& p) {
	return Pack16(s, p.x, p.y);
}

Stream& Pack16(Stream& s, Size& sz) {
	return Pack16(s, sz.cx, sz.cy);
}

Stream& Pack16(Stream& s, Rect& r) {
	return Pack16(s, r.left, r.top, r.right, r.bottom);
}

i32 InScListIndex(tukk s, tukk list)
{
	i32 ii = 0;
	for(;;) {
		tukk q = s;
		for(;;) {
			if(*q == '\0' && *list == '\0') return ii;
			if(*q != *list) {
				if(*q == '\0' && *list == ';') return ii;
				if(*list == '\0') return -1;
				break;
			}
			q++;
			list++;
		}
		while(*list && *list != ';') list++;
		if(*list == '\0') return -1;
		list++;
		ii++;
	}
}

bool InScList(tukk s, tukk list)
{
	return InScListIndex(s, list) >= 0;
}

Txt timeFormat(double s) {
	if(s < 0.000001) return Sprintf("%5.2f ns", s * 1.0e9);
	if(s < 0.001) return Sprintf("%5.2f us", s * 1.0e6);
	if(s < 1) return Sprintf("%5.2f ms", s * 1.0e3);
	return Sprintf("%5.2f s ", s);
}

Txt Garble(tukk s, tukk e)
{
	i32 c = 0xAA;
	Txt result;
	if(!e)
		e = s + strlen(s);
	while(s != e)
	{
		result.Cat(*s++ ^ (char)c);
		if((c <<= 1) & 0x100)
			c ^= 0x137;
	}
	return result;
}

Txt Garble(const Txt& s)
{
	return Garble(~s, ~s + s.GetLength());
}

Txt Encode64(const Txt& s)
{
	Txt enc;
	i32 l = s.GetLength();
	enc << l << ':';
	for(i32 i = 0; i < l;)
	{
		char a = 0, b = 0, c = 0;
		if(i < l) a = s[i++];
		if(i < l) b = s[i++];
		if(i < l) c = s[i++];
		enc.Cat(' ' + 1 + ((a >> 2) & 0x3F));
		enc.Cat(' ' + 1 + ((a << 4) & 0x30) + ((b >> 4) & 0x0F));
		enc.Cat(' ' + 1 + ((b << 2) & 0x3C) + ((c >> 6) & 0x03));
		enc.Cat(' ' + 1 + (c & 0x3F));
	}
	return enc;
}

Txt Decode64(const Txt& s)
{
	if(!IsDigit(*s))
		return s;
	tukk p = s;
	tuk h;
	i32 len = strtol(p, &h, 10);
	p = h;
	if(*p++ != ':' || len < 0 || (len + 2) / 3 * 4 > (s.End() - p))
		return s; // invalid encoding
	if(len == 0)
		return Null;
	Txt dec;
	for(;;)
	{
		u8 ea = *p++ - ' ' - 1, eb = *p++ - ' ' - 1, ec = *p++ - ' ' - 1, ed = *p++ - ' ' - 1;
		u8 out[3] = { u8((ea << 2) | (eb >> 4)), u8((eb << 4) | (ec >> 2)), u8((ec << 6) | (ed >> 0)) };
		switch(len)
		{
		case 1:  dec.Cat(out[0]); return dec;
		case 2:  dec.Cat(out, 2); return dec;
		case 3:  dec.Cat(out, 3); return dec;
		default: dec.Cat(out, 3); len -= 3; break;
		}
	}
}

Txt HexTxt(u8k *s, i32 count, i32 sep, i32 sepchr)
{
	ASSERT(count >= 0);
	if(count == 0)
		return Txt();
	TxtBuf b(2 * count + (count - 1) / sep);
	static const char itoc[] = "0123456789abcdef";
	i32 i = 0;
	tuk t = b;
	for(;;) {
		for(i32 q = 0; q < sep; q++) {
			if(i >= count)
				return Txt(b);
			*t++ = itoc[(s[i] & 0xf0) >> 4];
			*t++ = itoc[s[i] & 0x0f];
			i++;
		}
		if(i >= count)
			return Txt(b);
		*t++ = sepchr;
	}
}

Txt HexTxt(const Txt& s, i32 sep, i32 sepchr)
{
	return HexTxt(~s, s.GetCount(), sep, sepchr);
}

Txt HexEncode(u8k *s, i32 count, i32 sep, i32 sepchr)
{
	return HexTxt(s, count, sep, sepchr);
}

Txt HexEncode(const Txt& s, i32 sep, i32 sepchr)
{
	return HexTxt(s, sep, sepchr);
}

Txt ScanHexTxt(tukk s, tukk lim)
{
	Txt r;
	r.Reserve(i32(lim - s) / 2);
	for(;;) {
		u8 b = 0;
		while(!IsXDigit(*s)) {
			if(s >= lim)
				return r;
			s++;
		}
		b = ctoi(*s++);
		if(s >= lim)
			return r;
		while(!IsXDigit(*s)) {
			if(s >= lim) {
				r.Cat(b);
				return r;
			}
			s++;
		}
		b = (b << 4) + ctoi(*s++);
		r.Cat(b);
		if(s >= lim)
			return r;
	}
}

Txt HexDecode(tukk s, tukk lim)
{
	return ScanHexTxt(s, lim);
}


Txt NormalizeSpaces(tukk s)
{
	TxtBuf r;
	while(*s && (u8)*s <= ' ')
		s++;
	while(*s) {
		if((u8)*s <= ' ') {
			while(*s && (u8)*s <= ' ')
				s++;
			if(*s)
				r.Cat(' ');
		}
		else
			r.Cat(*s++);
	}
	return Txt(r);
}

Txt NormalizeSpaces(tukk s, tukk end)
{
	TxtBuf r;
	while(*s && (u8)*s <= ' ')
		s++;
	while(s < end) {
		if((u8)*s <= ' ') {
			while(s < end && (u8)*s <= ' ')
				s++;
			if(*s)
				r.Cat(' ');
		}
		else
			r.Cat(*s++);
	}
	return Txt(r);
}

Txt CsvTxt(const Txt& text)
{
	Txt r;
	r << '\"';
	tukk s = text;
	while(*s) {
		if(*s == '\"')
			r << "\"\"";
		else
			r.Cat(*s);
		s++;
	}
	r << '\"';
	return r;
}

Vec<Txt> GetCsvLine(Stream& s, i32 separator, u8 charset)
{
	Vec<Txt> r;
	bool instring = false;
	Txt val;
	u8 dcs = GetDefaultCharset();
	for(;;) {
		i32 c = s.Get();
		if(c == '\n' && instring)
			val.Cat(c);
		else
		if(c == '\n' || c < 0) {
			if(val.GetCount())
				r.Add(ToCharset(dcs, val, charset));
			return r;
		}
		else
		if(c == separator && !instring) {
			r.Add(ToCharset(dcs, val, charset));
			val.Clear();
		}
		else
		if(c == '\"') {
			if(instring && s.Term() == '\"') {
				s.Get();
				val.Cat('\"');
			}
			else
				instring = !instring;
		}
		else
		if(c != '\r')
			val.Cat(c);
	}
}

Txt CompressLog(tukk s)
{
	static bool breaker[256];
	ONCELOCK {
		for(i32 i = 0; i < 256; i++)
		breaker[i] = IsSpace(i) || findarg(i, '<', '>', '\"', '\'', ',', '.', '[', ']', '{', '}', '(', ')') >= 0;
	}

	TxtBuf result;
	while(*s) {
		tukk b = s;
		while(breaker[(u8)*s])
			s++;
		result.Cat(b, s);
		if(!*s)
			break;
		b = s;
		while(*s && !breaker[(u8)*s])
			s++;
		if(s - b > 200) {
			result.Cat(b, 20);
			result.Cat("....", 4);
			result << "[" << i32(s - b) << " байтов]";
			result.Cat("....", 4);
			result.Cat(s - 20, 20);
		}
		else
			result.Cat(b, s);
	}
	return Txt(result);
}

wchar ChNoInvalid(wchar c)
{
	return c == DEFAULTCHAR ? '_' : c;
}

#ifdef TARGET_WIN32
Txt ToSystemCharset(const Txt& src, i32 cp)
{
	Vec<char16> s = ToUtf16(src);
	i32 l = s.GetCount() * 8;
	TxtBuf b(l);
	i32 q = WideCharToMultiByte(cp, 0, s, s.GetCount(), b, l, NULL, NULL);
	if(q <= 0)
		return src;
	b.SetCount(q);
	return Txt(b);
}

Txt ToSystemCharset(const Txt& src)
{
	return ToSystemCharset(src, CP_ACP);
}

Txt FromWin32Charset(const Txt& src, i32 cp)
{
	Buffer<char16> b(src.GetLength());
	i32 q = MultiByteToWideChar(cp, MB_PRECOMPOSED, ~src, src.GetLength(), b, src.GetLength());
	if(q <= 0)
		return src;
	return ToUtf8(b, q);
}

Txt FromOEMCharset(const Txt& src)
{
	return FromWin32Charset(src, CP_OEMCP);
}

Txt FromSystemCharset(const Txt& src)
{
	return FromWin32Charset(src, CP_ACP);
}

#else
Txt ToSystemCharset(const Txt& src)
{
	return IsMainRunning() ? Filter(ToCharset(GetLNGCharset(GetSystemLNG()), src), ChNoInvalid)
	                       : src;
}

Txt FromSystemCharset(const Txt& src)
{
	return IsMainRunning() ? Filter(ToCharset(CHARSET_DEFAULT, src, GetLNGCharset(GetSystemLNG())), ChNoInvalid) : src;
}
#endif

Vec<char16> ToSystemCharsetW(const WTxt& src)
{
	Vec<char16> h = ToUtf16(src);
	h.Add(0);
	return h;
}

Vec<char16> ToSystemCharsetW(const Txt& src)
{
	Vec<char16> h = ToUtf16(src);
	h.Add(0);
	return h;
}

Vec<char16> ToSystemCharsetW(wtukk src)
{
	Vec<char16> h = ToUtf16(src);
	h.Add(0);
	return h;
}

Vec<char16> ToSystemCharsetW(tukk src)
{
	Vec<char16> h = ToUtf16(src);
	h.Add(0);
	return h;
}

Txt FromSystemCharsetW(const char16 *src)
{
	return ToUtf8(src);
}

static StaticMutex sGCfgLock;

static VecMap<Txt, Txt>& sGCfg()
{
	static VecMap<Txt, Txt> h;
	return h;
}

static Vec<Event<>>& sGFlush()
{
	static Vec<Event<>> h;
	return h;
}

static VecMap<Txt, Event<Stream&>>& sGSerialize()
{
	static VecMap<Txt, Event<Stream&>> h;
	return h;
}

void    RegisterGlobalConfig(tukk name)
{
	Mutex::Lock __(sGCfgLock);
	ASSERT(sGCfg().Find(name) < 0);
	sGCfg().Add(name);
}

void    RegisterGlobalSerialize(tukk name, Event<Stream&> WhenSerialize)
{
	Mutex::Lock __(sGCfgLock);
	RegisterGlobalConfig(name);
	sGSerialize().Add(name, WhenSerialize);
}

void    RegisterGlobalConfig(tukk name, Event<>  WhenFlush)
{
	Mutex::Lock __(sGCfgLock);
	RegisterGlobalConfig(name);
	sGFlush().Add(WhenFlush);
}

Txt GetGlobalConfigData(tukk name)
{
	Mutex::Lock __(sGCfgLock);
	return sGCfg().GetAdd(name);
}

void SetGlobalConfigData(tukk name, const Txt& data)
{
	Mutex::Lock __(sGCfgLock);
	sGCfg().GetAdd(name) = data;
}

bool LoadFromGlobal(Event<Stream&> x, tukk name)
{
	TxtStream ss(GetGlobalConfigData(name));
	return ss.IsEof() || Load(x, ss);
}

void StoreToGlobal(Event<Stream&> x, tukk name)
{
	TxtStream ss;
	Store(x, ss);
	SetGlobalConfigData(name, ss);
}

void  SerializeGlobalConfigs(Stream& s)
{
	Mutex::Lock __(sGCfgLock);
	for(i32 i = 0; i < sGFlush().GetCount(); i++)
		sGFlush()[i]();
	i32 version = 0;
	s / version;
	i32 count = sGCfg().GetCount();
	s / count;
	for(i32 i = 0; i < count; i++) {
		Txt name;
		if(s.IsStoring())
			name = sGCfg().GetKey(i);
		s % name;
		i32 q = sGCfg().Find(name);
		if(q >= 0) {
			i32 w = sGSerialize().Find(name);
			if(w >= 0) {
				Txt h;
				if(s.IsStoring()) {
					TxtStream ss;
					sGSerialize()[w](ss);
					h = ss;
				}
				s % h;
				if(s.IsLoading()) {
					TxtStream ss(h);
					sGSerialize()[w](ss);
				}
			}
			else
				s % sGCfg()[q];
		}
		else {
			Txt dummy;
			s % dummy;
		}
	}
	s.Magic();
}

AbortExc::AbortExc() :
	Exc(t_("Прервано пользователем.")) {}

#ifdef TARGET_WIN32

Txt GetErrorMessage(DWORD dwError) {
	char h[2048];
	sprintf(h, "%08x", (i32)dwError);
#ifdef TARGET_WINCE //СДЕЛАТЬ
	return h;
#else
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		          NULL, dwError, 0, h, 2048, NULL);
	Txt result = h;
	Txt modf;
	tukk s = result;
	BYTE c;
	while((c = *s++) != 0)
		if(c <= ' ') {
			if(!modf.IsEmpty() && modf[modf.GetLength() - 1] != ' ')
				modf += ' ';
		}
		else if(c == '%' && *s >= '0' && *s <= '9') {
			s++;
			modf += "<###>";
		}
		else
			modf += (char)c;
	tukk p = modf;
	for(s = p + modf.GetLength(); s > p && s[-1] == ' '; s--);
	return FromSystemCharset(modf.Left((i32)(s - p)));
#endif
}

Txt GetLastErrorMessage() {
	return GetErrorMessage(GetLastError()) + " (" + AsTxt(GetLastError()) + ")";
}

#endif

#ifdef TARGET_POSIX

Txt GetErrorMessage(i32 errorno)
{
	// Linux strerror_r declaration might be different than posix
	// hence we are using strerror with mutex... (cxl 2008-07-17)
	static StaticMutex m;
	Mutex::Lock __(m);
	return FromSystemCharset(strerror(errorno));
}

Txt GetLastErrorMessage() {
	return GetErrorMessage(errno) + " (" + AsTxt(errno) + ")";
}

#endif

#ifdef TARGET_POSIX
#ifndef TARGET_COCOA

Txt CurrentSoundTheme = "freedesktop";

static void LinuxBeep(tukk name)
{
	static Txt player;
	ONCELOCK {
		tukk players[] = { "play", "ogg123", "gst123", "gst-play-1.0" };
		for(i32 i = 0; i < __countof(players); i++)
			if(Sys("which " + Txt(players[i])).GetCount()) {
				player = players[i];
				break;
			}
	}

	if(player.GetCount()) {
		Txt fn = "/usr/share/sounds/" + CurrentSoundTheme + "/stereo/dialog-" + name;
		IGNORE_RESULT(system(player + " -q " + fn +
		              (FileExists(fn + ".ogg") ? ".ogg" :
		               FileExists(fn + ".oga") ? ".oga" :
	                       FileExists(fn + ".wav") ? ".wav" :
	                       ".*")
		             + " >/dev/null 2>/dev/null&"));
	}
}

#endif
#endif

#ifdef TARGET_COCOA
void (*CocoBeepFn)();

void DoCocoBeep()
{
	if(CocoBeepFn)
		(*CocoBeepFn)();
}

#endif

void BeepInformation()
{
#ifdef TARGET_WIN32
	MessageBeep(MB_ICONINFORMATION);
#elif defined(TARGET_COCOA)
	DoCocoBeep();
#else
	LinuxBeep("information");
#endif
}

void BeepExclamation()
{
#ifdef TARGET_WIN32
	MessageBeep(MB_ICONEXCLAMATION);
#elif defined(TARGET_COCOA)
	DoCocoBeep();
#else
	LinuxBeep("warning");
#endif
}

void BeepError()
{
#ifdef TARGET_WIN32
	MessageBeep(MB_ICONERROR);
#elif defined(TARGET_COCOA)
	DoCocoBeep();
#else
	LinuxBeep("error");
#endif
}

void BeepQuestion()
{
#ifdef TARGET_WIN32
	MessageBeep(MB_ICONQUESTION);
#elif defined(TARGET_COCOA)
	DoCocoBeep();
#else
	LinuxBeep("question");
#endif
}

#if defined(COMPILER_MSC) && (_MSC_VER < 1300)
//hack for linking libraries built using VC7 with VC6 standard lib's
extern "C" long _ftol( double );
extern "C" long _ftol2( double dblSource ) { return _ftol( dblSource ); }
#endif

#ifdef TARGET_WINCE
i32 errno; // missing and zlib needs it
#endif


template <class CHR, class T>
T Replace__(const T& s, const Vec<T>& find, const Vec<T>& replace)
{
	ASSERT(find.GetCount() == replace.GetCount());

	T r;
	i32 i = 0;
	while(i < s.GetCount()) {
		i32 best = -1;
		i32 bestlen = 0;
		i32 len = s.GetCount() - i;
		const CHR *q = ~s + i;
		for(i32 j = 0; j < replace.GetCount(); j++) {
			const T& m = find[j];
			i32 l = m.GetCount();
			if(l <= len && l > bestlen && memcmp(~m, q, l * sizeof(CHR)) == 0) {
				bestlen = l;
				best = j;
			}
		}
		if(best >= 0) {
			i += bestlen;
			r.Cat(replace[best]);
		}
		else {
			r.Cat(*q);
			i++;
		}
	}
	return r;
}

Txt Replace(const Txt& s, const Vec<Txt>& find, const Vec<Txt>& replace)
{
	return Replace__<char>(s, find, replace);
}

Txt Replace(const Txt& s, const VecMap<Txt, Txt>& fr)
{
	return Replace__<char>(s, fr.GetKeys(), fr.GetVals());
}

WTxt Replace(const WTxt& s, const Vec<WTxt>& find, const Vec<WTxt>& replace)
{
	return Replace__<wchar>(s, find, replace);
}

WTxt Replace(const WTxt& s, const VecMap<WTxt, WTxt>& fr)
{
	return Replace__<wchar>(s, fr.GetKeys(), fr.GetVals());
}


Txt (*GetP7Signature__)(ukk data, i32 length, const Txt& cert_pem, const Txt& pkey_pem);

Txt GetP7Signature(ukk data, i32 length, const Txt& cert_pem, const Txt& pkey_pem)
{
	ASSERT_(GetP7Signature__, "Отсутствует поддержка SSL (Core/SSL)");
	return (*GetP7Signature__)(data, length, cert_pem, pkey_pem);
}

Txt GetP7Signature(const Txt& data, const Txt& cert_pem, const Txt& pkey_pem)
{
	return GetP7Signature(data, data.GetLength(), cert_pem, pkey_pem);
}

}