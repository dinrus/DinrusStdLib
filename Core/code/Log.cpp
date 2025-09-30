#include "../Core.h"
//#include <drx/Core/Compress.h>

namespace drx {

StaticMutex log_mutex;

#ifdef TARGET_WINCE
tukk FromSysChrSet(wtukk s)
{
	static char out[256];
	FromUnicode(out, s, wstrlen(s), CHARSET_DEFAULT);
	return out;
}

wtukk ToSysChrSet(tukk s)
{
	static wchar out[1024];
	ToUnicode(out, s, strlen(s), CHARSET_DEFAULT);
	return out;
}
#endif

struct LogOut {
	dword options;
	i32   sizelimit;

#ifdef TARGET_WIN32
	HANDLE hfile;
#endif
#ifdef TARGET_POSIX
	enum { INVALID_HANDLE_VALUE = -1 };
	i32   hfile;
#endif

	char  filepath[512];
	i32   filesize;

	i32   part;

	bool  line_begin;

	i32   prev_msecs;

	void  Create(bool append);
	void  Create()                                     { Create(options & LOG_APPEND); }
	void  Close();
	void  Line(tukk buffer, i32 len, i32 depth);
	bool  IsOpen() const;
	void  Rotate();
};

bool LogOut::IsOpen() const
{
#ifdef TARGET_POSIX
	return hfile >= 0;
#else
	return hfile != INVALID_HANDLE_VALUE;
#endif
}

void LogOut::Rotate()
{
}

void LogOut::Create(bool append)
{
	Close();

	line_begin = true;

	i32 rotn = options >> 24;
	if(rotn) {
		char next[512];
		for(i32 rot = rotn; rot >= 0; rot--) {
			char current[512];
			if(rot == 0)
				strcpy(current, filepath);
			else
				snprintf(current, 512, rot > 1 && (options & LOG_ROTATE_GZIP) ? "%s.%d.gz" : "%s.%d",
				        filepath, rot);
			if(FileExists(current)) {
				if(rot == rotn)
					FileDelete(current);
				else
				if((options & LOG_ROTATE_GZIP) && rot == 1 && !IsPanicMode()) {
					GZCompressFile(next, current); // Should be OK to use heap in Create...
				}
				else
					FileMove(current, next);
			}
			strcpy(next, current);
		}
	}

	filesize = 0;

#ifdef TARGET_WIN32
	hfile = CreateFile(ToSysChrSet(filepath),
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		append ? OPEN_ALWAYS : CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if(append)
		filesize = (i32)SetFilePointer(hfile, 0, NULL, FILE_END);
#else
	hfile = open(filepath, append ? O_CREAT|O_RDWR|O_APPEND : O_CREAT|O_RDWR|O_TRUNC, 0644);
	if(append)
		filesize = (i32)lseek(hfile, 0, SEEK_END);
#endif
	Time t = GetSysTime();
#ifdef TARGET_WINCE
	wchar exe[512];
#else
	char exe[512];
#endif
	char user[500];
	*user = 0;

#ifdef TARGET_WIN32
	GetModuleFName(AppGetHandle(), exe, 512);
#ifndef TARGET_WINCE
	dword w = 2048;
	::GetUserNameA(user, &w);
#endif
#else //#
	tukk procexepath_(void);
	strcpy(exe, procexepath_());
	tukk uenv = getenv("USER");
	strcpy(user, uenv ? uenv : "boot");
#endif

	char h[1200];
	snprintf(h, 1200, "* %s %02d.%02d.%04d %02d:%02d:%02d, user: %s\n",
	         FromSysChrSet(exe),
	         t.day, t.month, t.year, t.hour, t.minute, t.second, user);
#ifdef TARGET_WIN32
	dword n;
	WriteFile(hfile, h, (dword)strlen(h), &n, NULL);
	if(part) {
		sprintf(h, ", #%d", part);
		WriteFile(hfile, h, (dword)strlen(h) , &n, NULL);
	}
	WriteFile(hfile, "\r\n", 2, &n, NULL);
#else
	IGNORE_RESULT(
		write(hfile, h, strlen(h))
	);
	if(part) {
		snprintf(h, 1200, ", #%d", part);
		IGNORE_RESULT(
			write(hfile, h, strlen(h))
		);
	}
	IGNORE_RESULT(
		write(hfile, "\r\n", 2)
	);
#endif
}

void LogOut::Close()
{
#ifdef TARGET_POSIX
	if(hfile >= 0)
		close(hfile);
	hfile = -1;
#else
	if(hfile != INVALID_HANDLE_VALUE)
		CloseHandle(hfile);
	hfile = INVALID_HANDLE_VALUE;
#endif
}

void LogOut::Line(tukk s, i32 len, i32 depth)
{
	Mutex::Lock __(log_mutex);

	ASSERT(len < 600);

	char h[1200]; // 2 * 600 to make snprintf easier
	tuk p = h;
	i32   ll = 0;
	if(options & LOG_ELAPSED) {
		i32 t = msecs();
		i32 e = prev_msecs ? t - prev_msecs : 0;
		ll = snprintf(p, 600, "[+%6d ms] ", e);
		if(ll < 0)
			return;
		p += ll;
		prev_msecs = t;
	}
	if((options & (LOG_TIMESTAMP|LOG_TIMESTAMP_UTC)) && line_begin) {
		Time t = (options & LOG_TIMESTAMP_UTC) ? GetUtcTime() : GetSysTime();
		ll = snprintf(p, 600, "%02d.%02d.%04d %02d:%02d:%02d ",
		              t.day, t.month, t.year, t.hour, t.minute, t.second);
		if(ll < 0)
			return;
		p += ll;
	}
	tuk beg = p;
	for(i32 q = min(depth, 99); q--;)
		*p++ = '\t';
	line_begin = len && s[len - 1] == '\n';
	memcpy(p, s, len);
	p += len;
	*p = '\0';
	i32 count = (i32)(p - h);
	if(count == 0) return;
	if(options & LOG_COUT)
		for(tukk s = beg; *s; s++)
			putchar(*s);
	if(options & LOG_CERR)
		for(tukk s = beg; *s; s++)
			putc(*s, stderr);
	if(options & LOG_COUTW)
		Cout().Put(h, count);
	if(options & LOG_CERRW)
		Cerr().Put(h, count);
#ifdef TARGET_WIN32
	if(options & LOG_FILE)
		if(hfile != INVALID_HANDLE_VALUE) {
			dword n;
			WriteFile(hfile, h, count, &n, NULL);
		}
	if(options & LOG_DBG) {
		*p = 0;
		::OutputDebugTxt((LPCSTR)h);
	}
#else
	if(options & LOG_FILE)
		if(hfile >= 0)
			IGNORE_RESULT(
				write(hfile, h, count)
			);
	if(options & LOG_DBG)
		Cerr().Put(h, count);
	if(options & LOG_SYS)
		syslog(LOG_INFO|LOG_USER, "%s", beg);
#endif
	filesize += count;
	if(sizelimit > 0 && filesize > sizelimit)
		Create(false);
}

#ifdef TARGET_POSIX
static LogOut sLog = { LOG_FILE, 10 * 1024 * 1024, -1 };
#else
static LogOut sLog = { LOG_FILE, 10 * 1024 * 1024 };
#endif

struct ThreadLog {
	char  buffer[512];
	i32   len;
	i32   line_depth;
	i32   depth;

	void  Put(i32 w);
};

static thread_local ThreadLog sTh;

static void sStdLogLine(tukk buffer, i32 len, i32 line_depth)
{
	sLog.Line(buffer, len, line_depth);
}

static void (*sLogLine)(tukk , i32, i32) = sStdLogLine;

LogLineFn SetUppLog(LogLineFn log_line)
{
	auto h = sLogLine;
	sLogLine = log_line;
	return h;
}

void ThreadLog::Put(i32 w)
{
	if(w == LOG_BEGIN)
		depth = min(depth + 1, 20);
	else
	if(w == LOG_END)
		depth = max(depth - 1, 0);
	else {
		buffer[len++] = w;
		if(w == '\n' || len > 500) {
			sLogLine(buffer, len, line_depth);
			len = 0;
		}
		if(w != '\r')
			line_depth = depth;
	}
}

class LogStream : public Stream {
protected:
	virtual void    _Put(i32 w);
	virtual void    _Put(ukk data, dword size);
	virtual z64   GetSize() const;

public:
	virtual   bool  IsOpen() const;
};

z64 LogStream::GetSize() const
{
	return sLog.filesize;
}

bool LogStream::IsOpen() const
{
	return sLog.IsOpen();
}

void LogStream::_Put(i32 w)
{
	sTh.Put(w);
}

void  LogStream::_Put(ukk data, dword size)
{
	u8k *q = (u8 *)data;
	while(size--)
		sTh.Put(*q++);
}

#ifdef flagCHECKINIT

void InitBlockBegin__(tukk fn, i32 line) {
	RLOG(fn << " " << line << " init block");
#ifdef HEAPDBG
	MemoryCheckDebug();
#else
	MemoryCheck();
#endif
}

void InitBlockEnd__(tukk fn, i32 line) {
	RLOG(fn << " " << line << " init block finished");
#ifdef HEAPDBG
	MemoryCheckDebug();
#else
	MemoryCheck();
#endif
}

#endif

#ifdef TARGET_WIN32
static void sLogFile(tuk fn, tukk app = ".log")
{
#ifdef TARGET_WINCE
	wchar wfn[256];
	::GetModuleFName(NULL, wfn, 512);
	strcpy(fn, FromSysChrSet(wfn));
#else
	::GetModuleFName(NULL, fn, 512);
#endif
	tuk e = fn + strlen(fn), *s = e;
	while(s > fn && *--s != '\\' && *s != '.')
		;
	strcpy(*s == '.' ? s : e, app);
}

void SyncLogPath__() {}

#endif

#ifdef TARGET_POSIX

static char sLogPath[1024];

void SyncLogPath__()
{
	Mutex::Lock __(log_mutex);
	strcpy(sLogPath, ConfigFile(GetExeTitle()));
}

static void sLogFile(tuk fn, tukk app = ".log")
{
	Mutex::Lock __(log_mutex);
	if(!*sLogPath)
		SyncLogPath__();
	strcpy(fn, sLogPath);
	strcat(fn, app);
}
#endif

static Stream *__logstream;

void SetVppLogSizeLimit(i32 limit) { sLog.sizelimit = limit; }
void SetVppLogNoDeleteOnStartup()  { sLog.options |= LOG_APPEND; }

LogStream& StdLogStream()
{
	static LogStream *s;
	ONCELOCK {
		static u8 lb[sizeof(LogStream)];
		LogStream *strm = new(lb) LogStream;
		if(*sLog.filepath == '\0')
			sLogFile(sLog.filepath);
		sLog.Create();
		s = strm;
	}
	return *s;
}

void CloseStdLog()
{
	StdLogStream().Close();
}

void ReopenLog()
{
	if(sLog.IsOpen()) {
		sLog.Close();
		sLog.Create();
	}
}

void StdLogSetup(dword options, tukk filepath, i32 filesize_limit)
{
	sLog.options = options;
	sLog.sizelimit = filesize_limit;
	if(filepath)
		strcpy(sLog.filepath, filepath);
	ReopenLog();
}

Txt GetStdLogPath()
{
	return sLog.filepath;
}

Stream& StdLog()
{
	return StdLogStream();
}

void SetVppLog(Stream& log) {
	__logstream = &log;
}

void SetUppLog(Stream& log) {
	__logstream = &log;
}

Stream& UppLog() {
	if(!__logstream) __logstream = &StdLog();
	return *__logstream;
}

Stream& VppLog() {
	return UppLog();
}

void SetVppLogName(const Txt& file) {
	strcpy(sLog.filepath, file);
	ReopenLog();
}

namespace Ini {
	INI_BOOL(user_log, false, "Activates logging of user actions");
};

}