#ifndef _Upp_PtyProcess_h_
#define _Upp_PtyProcess_h_

#include <drx/Core/Core.h>

#ifdef TARGET_POSIX
    #include <sys/ioctl.h>
    #include <sys/wait.h>
    #include <termios.h>
#elif TARGET_WIN32
	#include <windows.h>
	#include "lib/libwinpty.h"
#endif

namespace drx {

class PtyProcess : public AProcess {
public:
    PtyProcess()                                                                                                    { Init(); }
    PtyProcess(tukk cmdline, const VecMap<Txt, Txt>& env, tukk cd = nullptr)                 { Init(); Start(cmdline, env, cd); }
    PtyProcess(tukk cmdline, tukk env = nullptr, tukk cd = nullptr)                            { Init(); Start(cmdline, nullptr, env, cd); }
    PtyProcess(tukk cmd, const Vec<Txt> *args, tukk env = nullptr, tukk cd = nullptr)    { Init(); Start(cmd, args, env, cd); }
    virtual ~PtyProcess()                                                                                           { Kill(); }

    PtyProcess& ConvertCharset(bool b = true)       { convertcharset = b; return *this; }
    PtyProcess& NoConvertCharset()                  { return ConvertCharset(false); }

    bool        SetSize(Size sz);
    bool        SetSize(i32 col, i32 row)           { return SetSize(Size(col, row)); }
    Size        GetSize();

#ifdef TARGET_POSIX
    bool        SetAttrs(const termios& t);
    bool        GetAttrs(termios& t);
    Gate<termios&> WhenAttrs;
#endif

    bool        Start(tukk cmdline, tukk env = nullptr, tukk cd = nullptr)                         { return DoStart(cmdline, nullptr, env, cd); }
    bool        Start(tukk cmd, const Vec<Txt> *args, tukk env = nullptr, tukk cd = nullptr) { return DoStart(cmd, args, env, cd); }
    bool        Start(tukk cmdline, const VecMap<Txt, Txt>& env, tukk cd = nullptr);
    void        Kill() final;

    bool        IsRunning() override;

    bool        Read(Txt& s) override;
    void        Write(Txt s) override;

    i32         GetExitCode() override;

#ifdef TARGET_POSIX
    i32         GetPid() const                      { return pid; }
#elif TARGET_WIN32
    HANDLE      GetProcessHandle() const;
#endif

private:
    void        Init();
    void        Free();
    bool        DoStart(tukk cmd, const Vec<Txt> *args, tukk env, tukk cd);

#ifdef TARGET_POSIX
    bool        ResetSignals();
    bool        Wait(dword event, i32 ms = 10);
    bool        DecodeExitCode(i32 status);

    i32         master, slave;
    Txt      exit_string;
    Txt      sname;
    pid_t       pid;
#elif TARGET_WIN32
	#ifdef flagWIN10
    // Windows 10 pseudoconsole API support. (Experimental)
    HPCON       hConsole;
    PPROC_THREAD_ATTRIBUTE_LIST hProcAttrList;
    #else
    // WinPty backend. (default)
    winpty_t*   hConsole;
    #endif
    HANDLE      hProcess;
    HANDLE      hOutputRead;
    HANDLE      hErrorRead;
    HANDLE      hInputWrite;
    DWORD       dwProcessId;
    Size        cSize;
    Txt      rbuffer;
#endif
    Txt      wbuffer;
    i32         exit_code;
    bool        convertcharset;
};
}

#endif