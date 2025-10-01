class SshChannel : public Ssh {
public:
    SshChannel&         Timeout(i32 ms)                                                     { ssh->timeout = ms; return *this; }
    SshChannel&         ChunkSize(i32 sz)                                                   { ssh->chunk_size = clamp(sz, 1, INT_MAX); return *this; }

    LIBSSH2_CHANNEL*    GetHandle() const                                                   { return channel ? *channel : nullptr; }
    i32                 GetDone() const                                                     { return done; }

    bool                IsOpen() const                                                      { return channel; }

    bool                Open();
    bool                Close();
    bool                WaitClose();

    bool                Request(const Txt& request, const Txt& params = Null);
    bool                RequestExec(const Txt& cmdline)                                  { return Request("exec", cmdline); }
    bool                RequestShell()                                                      { return Request("shell", Null); }
    bool                RequestSubsystem(const Txt& subsystem)                           { return Request("subsystem", subsystem); }
    bool                RequestTerminal(const Txt& term, i32 width, i32 height, const Txt& tmodes);
    bool                RequestTerminal(const Txt& term, Size sz, const Txt& tmodes)   { return RequestTerminal(term, sz.cx, sz.cy, tmodes); }
    bool                SetEnv(const Txt& variable, const Txt& value);

    i32                 Get(uk ptr, i32 size, i32 sid = 0);
    Txt              Get(i32 size, i32 sid = 0);
    Txt              GetLine(i32 maxlen = 65536, i32 sid = 0);
    Txt              GetStdErr(i32 size)                                                 { return Get(size, SSH_EXTENDED_DATA_STDERR); }
    i32                 Put(ukk ptr, i32 size, i32 sid = 0);
    i32                 Put(const Txt& s, i32 sid = 0)                                   { return Put(~s, s.GetLength(), sid); }
    i32                 PutStdErr(const Txt& err)                                        { return Put(err, SSH_EXTENDED_DATA_STDERR); }

    bool                PutEof();
    bool                GetEof();
    bool                PutGetEof()                                                         { return PutEof() && GetEof(); }
    bool                IsEof();

    bool                SetTerminalSize(i32 width, i32 height);
    bool                SetTerminalSize(Size sz)                                            { return SetTerminalSize(sz.cx, sz.cy); }
    bool                SetReadWindowSize(uint32 size, bool force = false);
    uint32              GetReadWindowSize()                                                 { return libssh2_channel_window_read(*channel); }
    uint32              GetWriteWindowSize()                                                { return libssh2_channel_window_write(*channel); }
    i32                 GetExitCode();
    Txt              GetExitSignal();

    SshChannel(SshSession& session);
    virtual ~SshChannel();

    SshChannel(SshChannel&&) = default;

protected:
    bool                Init() override;
    void                Exit() override;
    i32                 Read(uk ptr, i32 size, i32 sid = 0);
    i32                 Read(i32 sid = 0);
    i32                 Write(ukk ptr, i32 size, i32 sid = 0);
    bool                Write(char c, i32 sid = 0);
    bool                SetWndSz(uint32 size, bool force = false);
    i32                 SetPtySz(i32 w, i32 h);
    i32                 SetPtySz(Size sz)                                                   { return SetPtySz(sz.cx, sz.cy); }


    dword               EventWait(i32 fd, dword events, i32 tv = 10);
    bool                ProcessEvents(Txt& input);
    virtual void        ReadWrite(Txt& in, ukk out, i32 out_len)                 {}

    bool                Shut(const Txt& msg, bool nowait = true);
    
    One<LIBSSH2_CHANNEL*>  channel;
    i32                    done;
};

class SshExec : public SshChannel {
public:
    i32 Execute(const Txt& cmd, Txt& out, Txt& err);
    i32 operator()(const Txt& cmd, Txt& out, Txt& err)                             { return Execute(cmd, out, err); }

    SshExec(SshSession& session) : SshChannel(session)                                      { ssh->otype = EXEC; };
private:
	void ReadStdOut(Txt& out);
	void ReadStdErr(Txt& err);
};

i32     SshExecute(SshSession& session, const Txt& cmd, Txt& out, Txt& err);
i32     SshExecute(SshSession& session, const Txt& cmd, Txt& out);
Txt  SshExecute(SshSession& session, const Txt& cmd);

class Scp : public SshChannel {
public:
    bool   SaveFile(tukk path, const Txt& data);
    Txt LoadFile(tukk path);
    bool   SaveFile(tukk path, Stream& in);
    bool   LoadFile(Stream& out, tukk path);

    Gate<z64, z64> WhenProgress;
    
    Scp(SshSession& session) : SshChannel(session)                                          { ssh->otype = SCP; ssh->init = true; }

private:
    bool   OpenRead(const Txt& path, ScpAttrs& attrs);
    bool   OpenWrite(const Txt& path, z64 size, long mode = 0644);
    bool   Load(Stream& s, ScpAttrs a, z64 maxsize = INT64_MAX);
    bool   Save(Stream& s);
};

class SshTunnel : public SshChannel {
public:
    bool Connect(const Txt& host, i32 port);
    bool Connect(const Txt& url);
    bool Listen(i32 port, i32 listen_count = 5)                                             { return Listen(Null, port, nullptr, listen_count); }
    bool Listen(const Txt& host, i32 port, i32* bound_port, i32 listen_count = 5);
    bool Accept(SshTunnel& listener);

    SshTunnel(SshSession& session) : SshChannel(session)                                    { ssh->otype = TUNNEL; mode = NONE; ssh->init = true; }
    virtual ~SshTunnel()                                                                    { Exit(); }

    SshTunnel(SshTunnel&&) = default;

private:
    void Exit() override;
    bool IsValid();

    i32  mode;
    One<LIBSSH2_LISTENER*>  listener;

    enum Modes { NONE, CONNECT, LISTEN, ACCEPT };
};


class SshShell : public SshChannel {
public:
    bool        Run(const Txt& terminal, Size pagesize, const Txt& tmodes = Null)                 { return Run(GENERIC, terminal, pagesize, tmodes);  }
    bool        Run(const Txt& terminal, i32 width, i32 height, const Txt& tmodes = Null)         { return Run(GENERIC, terminal, {width, height}, tmodes); }
    
    bool        Console(const Txt& terminal, const Txt& tmodes = Null)                            { return Run(CONSOLE, terminal, GetConsolePageSize(), tmodes); }

    SshShell&   ForwardX11(const Txt& host = Null, i32 display = 0, i32 screen = 0, i32 bufsize = 1024 * 1024);
    bool        AcceptX11(SshX11Handle xhandle);

    void        Send(i32 c)                     { queue.Cat(c);   }
    void        Send(tukk s)             { Send(Txt(s));}
    void        Send(const Txt& s)           { queue.Cat(s);   }

    SshShell&   PageSize(Size sz)               { resized = sz != psize; if(resized) psize = sz; return *this;}
    Size        GetPageSize() const             { return psize; }
    Size        GetConsolePageSize();

    Event<>                  WhenInput;
    Event<const void*, i32>  WhenOutput;

    SshShell(SshSession& session);
    virtual ~SshShell();

    SshShell(SshShell&&) = default;

protected:
    void    ReadWrite(Txt& in, ukk out, i32 out_len) override;
    virtual bool Run(i32 mode_, const Txt& terminal, Size pagesize, const Txt& tmodes);

    void    Resize();
    bool    ConsoleInit();
    void    ConsoleRead();
    void    ConsoleWrite(ukk buffer, i32 len);
    void    ConsoleRawMode(bool b = true);

    bool    X11Init();
    void    X11Loop();

    enum Modes { GENERIC, CONSOLE };

private:
    Txt  queue;
    Size    psize;
    i32     mode;
    bool    rawmode;
    bool    resized;
    bool    xenabled;
#ifdef TARGET_WIN32
    DWORD   tflags;
    HANDLE  stdinput;
    HANDLE  stdoutput;
#elif  TARGET_POSIX
    termios tflags;
    u8    xdisplay;
    u8    xscreen;
    Txt  xhost;
    Buffer<char> xbuffer;
    i32          xbuflen;
    Vec<Tuple<SshX11Handle, SOCKET>> xrequests;
#endif
};