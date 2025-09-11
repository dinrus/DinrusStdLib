class SshHosts {
public:
    struct Info {
        Txt          key;
        i32             type;
        i32             status;

        bool            IsRSA() const                   { return type == LIBSSH2_KNOWNHOST_KEY_SSHRSA;       }
        bool            IsDSS() const                   { return type == LIBSSH2_KNOWNHOST_KEY_SSHDSS;       }
        bool            IsECDSA256() const              { return type == LIBSSH2_KNOWNHOST_KEY_ECDSA_256;    }
        bool            IsECDSA384() const              { return type == LIBSSH2_KNOWNHOST_KEY_ECDSA_384;    }
        bool            IsECDSA521() const              { return type == LIBSSH2_KNOWNHOST_KEY_ECDSA_521;    }
        bool            IsED25519() const               { return type == LIBSSH2_KNOWNHOST_KEY_ED25519;      }
        bool            IsUnknown() const;

        bool            IsFailure() const               { return status == LIBSSH2_KNOWNHOST_CHECK_FAILURE;  }
        bool            IsNotFound() const              { return status == LIBSSH2_KNOWNHOST_CHECK_NOTFOUND; }
        bool            IsMismatch() const              { return status == LIBSSH2_KNOWNHOST_CHECK_MISMATCH; }
        bool            IsMatch() const                 { return status == LIBSSH2_KNOWNHOST_CHECK_MATCH;    }
    };

public:
    bool                Add(const Txt& host, i32 port, const Info& info, const Txt& comment);
    bool                Add(const Txt& host, const Info& info, const Txt& comment);
    bool                Remove(SshHost* host);
    bool                Load(const Txt& filename);
    bool                Save();
    bool                SaveAs(const Txt& filename);
    Info                Check(const Txt& host, i32 port);

    Vec<SshHost*>    GetHosts();

    i32                 GetError() const                { return error.a; }
    Txt              GetErrorDesc() const            { return error.b; }

    SshHosts(SshSession& session);
    virtual ~SshHosts();

private:
    bool                Error();
    void                Clear()                         { error.a = 0; error.b = Null; }

    Txt              file_path;
    Tuple<i32,Txt>   error;
    LIBSSH2_SESSION*    ssh_session;
    LIBSSH2_KNOWNHOSTS* handle;
};
