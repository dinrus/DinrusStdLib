class Exc : public Txt {
public:
	Exc() {}
	Exc(const Txt& desc) : Txt(desc) {}
};

const char LOG_BEGIN = '\x1e';
const char LOG_END = '\x1f';

enum LogOptions {
	LOG_FILE = 1, LOG_COUT = 2, LOG_CERR = 4, LOG_DBG = 8, LOG_SYS = 16, LOG_ELAPSED = 128,
	LOG_TIMESTAMP = 256, LOG_TIMESTAMP_UTC = 512, LOG_APPEND = 1024, LOG_ROTATE_GZIP = 2048,
	LOG_COUTW = 4096, LOG_CERRW = 8192
};

inline i32 LOG_ROTATE(i32 x) { return x << 24; }

void     StdLogSetup(dword options, tukk filepath = NULL,
                     i32 filesize_limit = 10 * 1024 * 1024);
Stream&  StdLog();

Txt      GetStdLogPath();

Stream&  UppLog();
void     SetUppLog(Stream& log);

typedef void (*LogLineFn)(tukk buffer, i32 len, i32 depth);

LogLineFn SetUppLog(LogLineFn log_line);

Stream&  VppLog();
void     SetVppLog(Stream& log);

void     SetVppLogName(const Txt& file);
void     SetVppLogSizeLimit(i32 limit);
void     SetVppLogNoDeleteOnStartup();

void     HexDumpData(Stream& s, ukk ptr, i32 size, bool adr, i32 maxsize);
void     HexDump(Stream& s, ukk ptr, i32 size, i32 maxsize = INT_MAX);

void     LogHex(const Txt& s);
void     LogHex(const WTxt& s);
void     LogHex(zu64 i);
void     LogHex(uk p);

void     SetMagic(u8 *t, i32 count);
void     CheckMagic(u8 *t, i32 count);

Txt        GetTypeName(tukk type_name);
inline Txt GetTypeName(const ::std::type_info& tinfo)   { return GetTypeName(tinfo.name()); }

void __LOGF__(tukk format, ...);

#ifndef STATIC_ASSERT
#define STATIC_ASSERT( expr ) { struct __static_assert { unsigned __static_assert_tst:(expr); }; }
#endif

#ifdef _DEBUG

#define LOG(a)           DRX::VppLog() << a << DRX::EOL
#define LOGF             DRX::__LOGF__
#define LOGBEGIN()       DRX::VppLog() << DRX::LOG_BEGIN
#define LOGEND()         DRX::VppLog() << DRX::LOG_END
#define LOGBLOCK(n)      RLOGBLOCK(n)
#define LOGHEXDUMP(s, a) DRX::HexDump(VppLog(), s, a)
#define LOGHEX(x)        DRX::LogHex(x)
#define QUOTE(a)         { LOG(#a); a; }
#define LOGSRCPOS()      DRX::VppLog() << __FILE__ << '#' << __LINE__ << DRX::EOL
#define DUMP(a)          DRX::VppLog() << #a << " = " << (a) << DRX::EOL
#define DUMPC(c)         DRX::DumpContainer(DRX::VppLog() << #c << ':' << DRX::EOL, (c))
#define DUMPCC(c)        DRX::DumpContainer2(DRX::VppLog() << #c << ':' << DRX::EOL, (c))
#define DUMPCCC(c)       DRX::DumpContainer3(DRX::VppLog() << #c << ':' << DRX::EOL, (c))
#define DUMPM(c)         DRX::DumpMap(DRX::VppLog() << #c << ':' << DRX::EOL, (c))
#define DUMPHEX(x)       DRX::VppLog() << #x << " = ", DRX::LogHex(x)
#define XASSERT(c, d)    if(!bool(c)) { LOG("XASSERT failed"); LOGSRCPOS(); LOG(d); ASSERT(0); } else
#define NEVER()          ASSERT(0)
#define NEVER_(msg)      ASSERT_(0, msg)
#define XNEVER(d)        if(1) { LOG("NEVER failed"); LOGSRCPOS(); LOG(d); ASSERT(0); } else
#define CHECK(c)         if(!bool(c)) { ASSERT(0); } else
#define XCHECK(c, d)     if(!bool(c)) { LOG("XCHECK failed"); LOGSRCPOS(); LOG(d); ASSERT(0); } else

#define TIMING(x)        RTIMING(x)
#define HITCOUNT(x)      RHITCOUNT(x)
#define ACTIVATE_TIMING()    DRX::TimingInspector::Activate(true);
#define DEACTIVATE_TIMING()  DRX::TimingInspector::Activate(false);
#define TIMESTOP(x)      RTIMESTOP(x)

#else

inline void LOGF(tukk format, ...) {}

#define LOG(a)           LOG_NOP
#define LOGBEGIN()       LOG_NOP
#define LOGEND()         LOG_NOP
#define LOGBLOCK(n)      LOG_NOP
#define LOGHEXDUMP(s, a) LOG_NOP
#define LOGHEX(x)        LOG_NOP
#define QUOTE(a)         a
#define LOGSRCPOS()      LOG_NOP
#define DUMP(a)          LOG_NOP
#define DUMPC(a)         LOG_NOP
#define DUMPCC(a)        LOG_NOP
#define DUMPCCC(a)       LOG_NOP
#define DUMPM(a)         LOG_NOP
#define DUMPHEX(nx)      LOG_NOP
#define XASSERT(c, d)    LOG_NOP
#define NEVER()          LOG_NOP
#define NEVER_(msg)      LOG_NOP
#define XNEVER(d)        LOG_NOP
#define CHECK(c)         (void)(c)
#define XCHECK(c, d)     (void)(c)

#define TIMING(x)           LOG_NOP
#define HITCOUNT(x)         LOG_NOP
#define ACTIVATE_TIMING()   LOG_NOP
#define DEACTIVATE_TIMING() LOG_NOP

#define TIMESTOP(x)      LOG_NOP

#endif

struct DebugLogBlock
{
	DebugLogBlock(tukk name) : name(name) { VppLog() << name << EOL << LOG_BEGIN; }
	~DebugLogBlock()                             { VppLog() << LOG_END << "//" << name << EOL; }
	tukk name;
};

#define RLOG(a)           DRX::VppLog() << a << DRX::EOL
#define RLOGBEGIN()       DRX::VppLog() << LOG_BEGIN
#define RLOGEND()         DRX::VppLog() << LOG_END
#define RLOGBLOCK(n)      DRX::DebugLogBlock MK__s(n)
#define RLOGHEXDUMP(s, a) DRX::HexDump(DRX::VppLog(), s, a)
#define RQUOTE(a)         { LOG(#a); a; }
#define RLOGSRCPOS()      DRX::VppLog() << __FILE__ << '#' << __LINE__ << DRX::EOL
#define RDUMP(a)          DRX::VppLog() << #a << " = " << (a) << DRX::EOL
#define RDUMPC(c)         DRX::DumpContainer(DRX::VppLog() << #c << ':' << DRX::EOL, (c))
#define RDUMPM(c)         DRX::DumpMap(VppLog() << #c << ':' << DRX::EOL, (c))
#define RLOGHEX(x)        DRX::LogHex(x)
#define RDUMPHEX(x)       DRX::VppLog() << #x << " = ", DRX::LogHex(x)

#if defined(_DEBUG) || defined(flagDEBUGCODE)

#define DLOG(x)          RLOG(x)
#define DDUMP(x)         RDUMP(x)
#define DDUMPC(x)        RDUMPC(x)
#define DDUMPM(x)        RDUMPM(x)
#define DTIMING(x)       RTIMING(x)
#define DLOGHEX(x)       RLOGHEX(x)
#define DDUMPHEX(x)      RDUMPHEX(x)
#define DTIMESTOP(x)     RTIMESTOP(x)
#define DHITCOUNT(x)     RHITCOUNT(x)

#define DEBUGCODE(x)     x

#define _DBG_

#else

#define DLOG(x)          @ // To clean logs after debugging, this produces error in release mode
#define DDUMP(x)         @ // To clean logs after debugging, this produces error in release mode
#define DDUMPC(x)        @ // To clean logs after debugging, this produces error in release mode
#define DDUMPM(x)        @ // To clean logs after debugging, this produces error in release mode
#define DTIMING(x)       @ // To clean logs after debugging, this produces error in release mode
#define DLOGHEX(x)       @ // To clean logs after debugging, this produces error in release mode
#define DDUMPHEX(nx)     @ // To clean logs after debugging, this produces error in release mode
#define DTIMESTOP(x)     @ // To clean logs after debugging, this produces error in release mode
#define DHITCOUNT(x)     @ // To clean logs after debugging, this produces error in release mode

#define DEBUGCODE(x)     LOG_NOP

#endif

// Conditional logging

#define LOG_(flag, x)       do { if(flag) RLOG(x); } while(false)
#define LOGBEGIN_(flag)     do { if(flag) RLOGBEGIN(x); } while(false)
#define LOGEND_(flag)       do { if(flag) RLOGEND(x); } while(false)
#define DUMP_(flag, x)      do { if(flag) RDUMP(x); } while(false)
#define LOGHEX_(flag, x)    do { if(flag) RLOGHEX(x); } while(false)
#define DUMPHEX_(flag, x)   do { if(flag) RDUMPHEX(x); } while(false)
	
// USRLOG


struct IniBool;

namespace Ini {
extern IniBool user_log;
}

#define USRLOG(x)         LOG_(Ini::user_log, x)

// Crash support

void InstallCrashDump(tukk app_info = NULL);
void SetCrashFName(tukk cfile);

// Dumping templates

template <class T>
void DumpContainer(Stream& s, T ptr, T end) {
	i32 i = 0;
	s << LOG_BEGIN;
	while(ptr != end)
		s << '[' << i++ << "] = " << *ptr++ << EOL;
	s << LOG_END;
}

template <class C>
void DumpContainer(Stream& s, const C& c) {
	DumpContainer(s, c.begin(), c.end());
}

template <class T>
void DumpContainer2(Stream& s, T ptr, T end) {
	i32 i = 0;
	s << LOG_BEGIN;
	while(ptr != end) {
		s << '[' << i++ << "] =" << EOL;
		DumpContainer(s, (*ptr).begin(), (*ptr).end());
		ptr++;
	}
	s << LOG_END;
}

template <class C>
void DumpContainer2(Stream& s, const C& c) {
	DumpContainer2(s, c.begin(), c.end());
}

template <class T>
void DumpContainer3(Stream& s, T ptr, T end) {
	i32 i = 0;
	s << LOG_BEGIN;
	while(ptr != end) {
		s << '[' << i++ << "] =" << EOL;
		DumpContainer2(s, (*ptr).begin(), (*ptr).end());
		ptr++;
	}
	s << LOG_END;
}

template <class C>
void DumpContainer3(Stream& s, const C& c) {
	DumpContainer3(s, c.begin(), c.end());
}

template <class T>
void DumpMap(Stream& s, const T& t) {
	s << LOG_BEGIN;
	for(i32 i = 0; i < t.GetCount(); i++)
		s << '[' << i << "] = (" << t.GetKey(i) << ") " << t[i] << EOL;
	s << LOG_END;
}

void CheckLogEtalon(tukk etalon_path);
void CheckLogEtalon();

#ifdef DRX_HEAP
Txt AsTxt(const MemoryProfile& mem);
#else
inline Txt AsTxt(const MemoryProfile&) { return "Используется malloc - профиль памяти недоступен"; }
#endif

Txt CppDemangle(tukk name);

#ifdef CPU_X86
Txt AsTxt(__m128i x);
#endif

#if 0 // rare cases when release-mode DLOG/DDUMP is needed

#undef  DLOG
#undef  DDUMP
#define DLOG(x)  RLOG(x)
#define DDUMP(x) RDUMP(x)

#endif
