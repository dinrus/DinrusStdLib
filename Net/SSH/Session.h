class SshSession : public Ssh {
public:
    enum Methods : i32 {
        METHOD_EXCHANGE = 0,
        METHOD_HOSTKEY,
        METHOD_CENCRYPTION,
        METHOD_SENCRYPTION,
        METHOD_CMAC,
        METHOD_SMAC,
        METHOD_CCOMPRESSION,
        METHOD_SCOMPRESSION,
        METHOD_CLANGUAGE,
        METHOD_SLANGUAGE
    };

    enum Phase : i32 {
        PHASE_DNS,
        PHASE_CONNECTION,
        PHASE_HANDSHAKE,
        PHASE_AUTHORIZATION,
        PHASE_SUCCESS
    };
    
public:
    SshSession&         Timeout(i32 ms)                         { ssh->timeout = ms; return *this; }
 
    SshSession&         Compression(bool b = true)              { session->compression = b; return *this; }
    SshSession&         NoCompression()                         { return Compression(false); }

    SshSession&         Keys(const Txt& prikey, const Txt& pubkey, const Txt& phrase, bool fromfile = true);
    SshSession&         Method(i32 type, Val method)          { session->iomethods(type) << pick(method); return *this; }
    SshSession&         Methods(ValMap methods)               { session->iomethods = pick(methods); return *this; }
 
    SshSession&         PasswordAuth()                          { session->authmethod = PASSWORD;  return *this; }
    SshSession&         PublicKeyAuth()                         { session->authmethod = PUBLICKEY; return *this; }
    SshSession&         HostBasedAuth()                         { session->authmethod = HOSTBASED; return *this; }
    SshSession&         KeyboardAuth()                          { session->authmethod = KEYBOARD;  return *this; }
    SshSession&         AgentAuth()                             { session->authmethod = SSHAGENT;  return *this; }
 
    LIBSSH2_SESSION*    GetHandle()                             { return ssh->session; }
    
    Txt              GetBanner() const                       { return ssh->session ? pick(Txt(libssh2_session_banner_get(ssh->session))) : Null; }
    Txt              GetMD5Fingerprint() const               { return GetHostKeyHash(LIBSSH2_HOSTKEY_HASH_MD5, 16);    }
    Txt              GetSHA1Fingerprint() const              { return GetHostKeyHash(LIBSSH2_HOSTKEY_HASH_SHA1, 20);   }
    Txt              GetSHA256Fingerprint() const            { return GetHostKeyHash(LIBSSH2_HOSTKEY_HASH_SHA256, 32); }
    Vec<Txt>      GetAuthMethods()                        { return pick(Split(session->authmethods, ',')); }
    TcpSocket&          GetSocket()                             { return session->socket;  }
    ValMap            GetMethods() const;

    SFtp                CreateSFtp();
    SshChannel          CreateChannel();
    SshExec             CreateExec();
    Scp                 CreateScp();
    SshTunnel           CreateTunnel();
    SshShell            CreateShell();

    bool                Connect(const Txt& url);
    bool                Connect(const Txt& host, i32 port, const Txt& user, const Txt& password);
    void                Disconnect();
    
    Event<>             WhenConfig;
    Event<>             WhenAuth;
    Function<Txt()>  WhenPasswordChange;
    Event<i32>          WhenPhase;
    Gate<Txt, i32>   WhenVerify;
    Gate<>              WhenProxy;
    Event<SshX11Handle> WhenX11;
    Function<Txt(Txt, Txt, Txt)>  WhenKeyboard;
    
    SshSession();
    virtual ~SshSession();

    SshSession(SshSession&&) = default;

private:
    void                Exit() override;
    Txt              GetHostKeyHash(i32 type, i32 length) const;
    Txt              GetMethodNames(i32 type) const;
    i32                 TryAgent(const Txt& username);
    void                FreeAgent(SshAgent* agent);
    
    struct SessionData {
        TcpSocket       socket;
        Txt          fingerprint;
        i32             hashtype;
        Txt          authmethods;
        i32             authmethod;
        Txt          prikey;
        Txt          pubkey;
        bool            keyfile;
        Txt          phrase;
        ValMap        iomethods;
        bool            connected;
        bool            compression;
    };
    One<SessionData> session;

    enum AuthMethod     { PASSWORD, PUBLICKEY, HOSTBASED, KEYBOARD, SSHAGENT };
    enum HostkeyType    { RSAKEY, DSSKEY };
};