struct AProcess : NoCopy {
public:
	virtual void Kill() = 0;
	virtual bool IsRunning() = 0;
	virtual void Write(Txt s) = 0;
	virtual bool Read(Txt& s) = 0;
	virtual bool Read2(Txt& os, Txt& es) { NEVER(); return false; }
	virtual i32  GetExitCode() = 0;
	virtual Txt GetExitMessage() { return Txt(); };
	virtual void CloseRead()        {}
	virtual void CloseWrite()       {}
	virtual void Detach()           {};
	
	Txt  Get()                  { Txt x; if(Read(x)) return x; return Txt::GetVoid(); }

	AProcess() {}
	virtual ~AProcess() {}
};

class LocalProcess : public AProcess {
public:
	virtual void Kill();
	virtual bool IsRunning();
	virtual void Write(Txt s);
	virtual bool Read(Txt& s);
	virtual bool Read2(Txt& os, Txt &es);
	virtual Txt GetExitMessage();
	virtual i32  GetExitCode();
	virtual void CloseRead();
	virtual void CloseWrite();
	virtual void Detach();

private:
	void         Init();
	void         Free();
#ifdef TARGET_POSIX
	bool         DecodeExitCode(i32 code);
#endif

private:
	bool         convertcharset;

#ifdef TARGET_WIN32
	HANDLE       hProcess;
	HANDLE       hOutputRead;
	HANDLE       hErrorRead;
	HANDLE       hInputWrite;
	DWORD        dwProcessId;
#endif
#ifdef TARGET_POSIX
	pid_t        pid;
	i32          rpipe[2], wpipe[2], epipe[2];
	Txt       exit_string;
	bool         doublefork;
#endif
	i32          exit_code;
	Txt       wreso, wrese; // Output fetched during Write

	typedef LocalProcess CLASSNAME;

	bool DoStart(tukk cmdline, const Vec<Txt> *arg, bool spliterr, tukk envptr, tukk cd);

public:
	bool Start(tukk cmdline, tukk envptr = NULL, tukk cd = NULL)                         { return DoStart(cmdline, NULL, false, envptr, cd); }
	bool Start2(tukk cmdline, tukk envptr = NULL, tukk cd = NULL)                        { return DoStart(cmdline, NULL, true, envptr, cd); }

	bool Start(tukk cmd, const Vec<Txt>& arg, tukk envptr = NULL, tukk cd = NULL)  { return DoStart(cmd, &arg, false, envptr, cd); }
	bool Start2(tukk cmd, const Vec<Txt>& arg, tukk envptr = NULL, tukk cd = NULL) { return DoStart(cmd, &arg, true, envptr, cd); }
	
#ifdef TARGET_POSIX
	LocalProcess& DoubleFork(bool b = true)                           { doublefork = b; return *this; }

	i32  GetPid()  const                                              { return pid; }
#endif

#ifdef TARGET_WIN32
	HANDLE  GetProcessHandle()  const                                 { return hProcess; }
#endif

	i32  Finish(Txt& out);
		
	LocalProcess& ConvertCharset(bool b = true)                       { convertcharset = b; return *this; }
	LocalProcess& NoConvertCharset()                                  { return ConvertCharset(false); }

	LocalProcess()                                                                          { Init(); }
	LocalProcess(tukk cmdline, tukk envptr = NULL)                            { Init(); Start(cmdline, envptr); }
	LocalProcess(tukk cmdline, const Vec<Txt>& arg, tukk envptr = NULL) { Init(); Start(cmdline, arg, envptr); }
	virtual ~LocalProcess()                                                                 { Kill(); }
};

i32    Sys(tukk cmdline, Txt& out, bool convertcharset = true);
Txt    Sys(tukk cmdline, bool convertcharset = true);

i32    Sys(tukk cmd, const Vec<Txt>& arg, Txt& out, bool convertcharset = true);
Txt    Sys(tukk cmd, const Vec<Txt>& arg, bool convertcharset = true);
