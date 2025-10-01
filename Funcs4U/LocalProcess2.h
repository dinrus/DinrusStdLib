// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#ifndef _Functions4U_LocalProcess2_h
#define _Functions4U_LocalProcess2_h

namespace drx {

class LocalProcess2 : public AProcess {
public:
    virtual void Kill();
    virtual bool IsRunning();
    virtual void Write(Txt s);
    virtual bool Read(Txt& s);
    virtual bool Read(Txt& os, Txt &es);
    virtual Txt GetExitMessage();
    virtual i32  GetExitCode();
#ifdef TARGET_WIN32
    void Pause();
    bool IsPaused()     {return paused;}
#endif

private:
    virtual void CloseRead();
    virtual void CloseWrite();
    virtual void Detach();

    void         Init();
    void         Free();
#ifdef TARGET_POSIX
    bool         DecodeExitCode(i32 code);
#endif
    bool         convertcharset;

#ifdef TARGET_WIN32
    HANDLE       hProcess;
    DWORD        dwProcessId;
    HANDLE       hOutputRead;
    HANDLE       hErrorRead;
    HANDLE       hInputWrite;
    bool         paused;
#endif
#ifdef TARGET_POSIX
    Buffer<char> cmd_buf;
    Vec<tuk > args;
    pid_t        pid;
    i32          rpipe[2], wpipe[2], epipe[2];
    Txt       exit_string;
    bool         doublefork;
#endif
    i32          exit_code;
    Txt       wreso, wrese; // Output fetched during Write

    typedef LocalProcess2 CLASSNAME;

    bool DoStart(tukk cmdline, const Vec<Txt> *arg, bool spliterr, tukk envptr = NULL, tukk dir = NULL);

public:
    bool Start(tukk cmdline, tukk envptr = NULL, tukk dir = NULL)  { return DoStart(cmdline, NULL, false, envptr, dir); }
    bool Start2(tukk cmdline, tukk envptr = NULL, tukk dir = NULL) { return DoStart(cmdline, NULL, true, envptr, dir); }

    bool Start(tukk cmd, const Vec<Txt>& arg, tukk envptr = NULL, tukk dir = NULL)  { return DoStart(cmd, &arg, false, envptr, dir); }
    bool Start2(tukk cmd, const Vec<Txt>& arg, tukk envptr = NULL, tukk dir = NULL) { return DoStart(cmd, &arg, true, envptr, dir); }

#ifdef TARGET_POSIX
    LocalProcess2& DoubleFork(bool b = true)                    { doublefork = b; return *this; }

    i32  GetPid()  const                                        { return pid; }
#endif

#ifdef TARGET_WIN32
    HANDLE  GetProcessHandle()  const                           { return hProcess; }
    DWORD   GetPid()  const                                      { return dwProcessId; }
#endif

    i32  Finish(Txt& out);

    LocalProcess2& ConvertCharset(bool b = true)                { convertcharset = b; return *this; }
    LocalProcess2& NoConvertCharset()                           { return ConvertCharset(false); }

    LocalProcess2()                                                                          { Init(); }
    LocalProcess2(tukk cmdline, tukk envptr = NULL, tukk dir = NULL)    { Init(); Start(cmdline, envptr, dir); }
    LocalProcess2(tukk cmdline, const Vec<Txt>& arg, tukk envptr = NULL, tukk dir = NULL) { Init(); Start(cmdline, arg, envptr, dir); }
    virtual ~LocalProcess2()                                                                 { Kill(); }
};

class LocalProcessSet {
public:
    void SetMaxProcesses(i32 n) {mxProcess = n;}
    i32 GetCount()              {return processes.size();}

    void Add(Txt cmdline, Txt dir = Null, Function<void(Txt&)> WhenLog = Null, Txt envptr = Null) {
        Process &p = processes.Insert(0);
        p.cmdline = cmdline;
        p.dir = dir;
        p.envptr = envptr;
        p.WhenLog = WhenLog;
        p.isPending = true;
    }
    void Perform() {
        while (processes.size() > 0) {
            for (i32 i = processes.size()-1; i >= 0; --i) {
                Process &p = processes[i];
                if (p.isPending) {
                    if (nmProcess < mxProcess) {
                        p.process.Start(p.cmdline, p.envptr, p.dir);
                        nmProcess++;
                        p.isPending = false;
                        Cout() << "\nДобавлен процесс: " << nmProcess;
                    }
                } else {
                    if (p.process.IsRunning()) {
                        Txt str;
                        if (p.process.Read(str))
                            p.WhenLog(str);
                    } else {
                        processes.Remove(i);
                        nmProcess--;
                        Cout() << "\nУдалён процесс: " << nmProcess;
                    }
                }
            }
            Sleep(100);
        }
    }

private:
    struct Process {
        LocalProcess2 process;
        Txt cmdline, dir, envptr;
        Function<void(Txt&)> WhenLog;
        bool isPending;
    };
    Array<Process> processes;
    i32 mxProcess = 1, nmProcess = 0;
};

}

#endif