class Ssh {
public:
    void                Abort()                                 { ssh->status = ABORTED; }
    Ssh&                WaitStep(i32 ms)                        { ssh->waitstep = clamp(ms, 0, INT_MAX); return *this; }
    i32                 GetTimeout() const                      { return ssh->timeout; }
    i32                 GetWaitStep() const                     { return ssh->waitstep; }
    bool                InProgress() const                      { return ssh->status == WORKING; }
    bool                IsError() const                         { return ssh->status == FAILED || ssh->status == ABORTED; }
    i32                 GetError() const                        { return ssh->error.Get<i32>(); }
    Txt              GetErrorDesc() const                    { return ssh->error.Get<Txt>(); }
    z64               GetId() const                           { return ssh->oid;   }
    i32                 GetType() const                         { return ssh->otype; }
    template <class T>  T&   To()                               { auto* t = dynamic_cast<T*>(this); ASSERT(t); return *t; }
    template <class T>  bool Is() const                         { return dynamic_cast<const T*>(this); }

    operator bool() const                                       { return ssh; }

    Event<>             WhenWait;
    
    static void         Trace(bool b = true)                    { SSH::sTrace = b; }
    static void         TraceVerbose(i32 level)                 { Trace((bool)level); SSH::sTraceVerbose = level; }

    Ssh();
    virtual ~Ssh();

    Ssh(Ssh&&) = default;

    struct Error : Exc {
        i32 code;
        Error() : Exc(Null), code(-1) {}
        Error(const Txt& reason) : Exc(reason), code(-1) {}
        Error(i32 rc, const Txt& reason) : Exc(reason), code(rc) {}
    };
    enum Type  { CORE, SESSION, SFTP, CHANNEL, SCP, EXEC, SHELL, TUNNEL };

protected:
    struct CoreData {
        LIBSSH2_SESSION*    session;
        TcpSocket*          socket;
        Tuple<i32, Txt>  error;
        Event<>             whenwait;
        bool                init;
        z64               oid;
        i32                 otype;
        i32                 timeout;
        i32                 start_time;
        i32                 waitstep;
        i32                 chunk_size;
        i32                 status;
    };
    One<CoreData> ssh;

    i32k           CHUNKSIZE = 1024 * 64;

    enum Status         { IDLE, WORKING, FAILED, ABORTED };

    virtual bool        Init()                                  { return true; }
    virtual void        Exit()                                  {}
    void                Wait();
    bool                Run(Gate<>&& fn, bool abortable = true);
    bool                WouldBlock(i32 rc)                      { return rc == LIBSSH2_ERROR_EAGAIN; }
    bool                WouldBlock()                            { return ssh->session && WouldBlock(libssh2_session_last_errno(ssh->session)); }
    bool                IsTimeout() const                       { return !IsNull(ssh->timeout) && ssh->timeout > 0 &&  msecs(ssh->start_time) >= ssh->timeout; }
    void                ThrowError(i32 rc, const Txt& reason = Null);
    void                SetError(i32 rc, const Txt& reason);
    void                UpdateClient()                          { WhenWait  ? WhenWait() : ssh->whenwait(); }
    
private:
    static z64        GetNewId();

};
