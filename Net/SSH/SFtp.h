class SFtp : public Ssh {
public:
    enum Flags {
        READ        = LIBSSH2_FXF_READ,
        WRITE       = LIBSSH2_FXF_WRITE,
        APPEND      = LIBSSH2_FXF_APPEND,
        CREATE      = LIBSSH2_FXF_CREAT,
        TRUNCATE    = LIBSSH2_FXF_TRUNC,
        EXCLUSIVE   = LIBSSH2_FXF_EXCL
     };

    enum Permissions {
        IRUSR = LIBSSH2_SFTP_S_IRUSR,
        IWUSR = LIBSSH2_SFTP_S_IWUSR,
        IXUSR = LIBSSH2_SFTP_S_IXUSR,
        IRWXU = LIBSSH2_SFTP_S_IRWXU,
        IRGRP = LIBSSH2_SFTP_S_IRGRP,
        IWGRP = LIBSSH2_SFTP_S_IWGRP,
        IXGRP = LIBSSH2_SFTP_S_IXGRP,
        IRWXG = LIBSSH2_SFTP_S_IRWXG,
        IROTH = LIBSSH2_SFTP_S_IROTH,
        IWOTH = LIBSSH2_SFTP_S_IWOTH,
        IXOTH = LIBSSH2_SFTP_S_IXOTH,
        IRWXO = LIBSSH2_SFTP_S_IRWXO,
        IRALL = IRUSR | IRGRP | IROTH,
        IWALL = IWUSR | IWGRP | IWOTH,
        IXALL = IXUSR | IXGRP | IXOTH,
        IRWXA = IRALL | IWALL | IXALL
    };

    class DirEntry : public Moveable<DirEntry> {
        friend class SFtp;
        public:
            Txt GetName() const                  { return filename; }
            z64  GetUid() const                   { return (a->flags & LIBSSH2_SFTP_ATTR_UIDGID) ? a->uid : -1; }
            z64  GetGid() const                   { return (a->flags & LIBSSH2_SFTP_ATTR_UIDGID) ? a->gid : -1; }
            z64  GetSize() const                  { return (a->flags & LIBSSH2_SFTP_ATTR_SIZE) ? a->filesize : -1; }
            Time   GetLastModified() const          { return (a->flags & LIBSSH2_SFTP_ATTR_ACMODTIME) ? TimeFromUTC(a->mtime) : Null; }
            Time   GetLastAccessed() const          { return (a->flags & LIBSSH2_SFTP_ATTR_ACMODTIME) ? TimeFromUTC(a->atime) : Null; }
            dword  GetPermissions() const           { return a->permissions; }
            SFtpAttrs& GetAttrs()                   { return *a; }

            const SFtpAttrs& operator~() const      { return *a; }
            SFtpAttrs&  operator*()                 { return *a; }
            operator bool() const                   { return valid; }

            bool IsFile() const                     { return LIBSSH2_SFTP_S_ISREG(a->permissions); }
            bool IsDir() const                { return LIBSSH2_SFTP_S_ISDIR(a->permissions); }
            bool IsSymLink() const                  { return LIBSSH2_SFTP_S_ISLNK(a->permissions); }
            bool IsSpecial() const                  { return LIBSSH2_SFTP_S_ISCHR(a->permissions); }
            bool IsBlock() const                    { return LIBSSH2_SFTP_S_ISBLK(a->permissions); }
            bool IsPipe() const                     { return LIBSSH2_SFTP_S_ISFIFO(a->permissions); }
            bool IsSocket() const                   { return LIBSSH2_SFTP_S_ISSOCK(a->permissions); }
            bool IsReadable() const                 { return CanMode(IRUSR, IRGRP, IROTH); }
            bool IsWriteable() const                { return CanMode(IWUSR, IWGRP, IWOTH); }
            bool IsReadOnly() const                 { return IsReadable() && !IsWriteable(); }
            bool IsExecutable() const               { return !IsDir() && CanMode(IXUSR, IXGRP, IXOTH); }

            Txt ToTxt() const;
            Txt ToXml() const;
            FSInfo::FileInfo ToFileInfo() const;

            void Serialize(Stream& s);
            
            DirEntry(const Txt& path);
            DirEntry(const Txt& path, const SFtpAttrs& attrs);
            DirEntry()                              { Zero();  }
            DirEntry(const Nuller&)                 { Zero();  }

            DirEntry(DirEntry&&) = default;
            DirEntry& operator=(DirEntry&&) = default;
            
        private:
            bool CanMode(dword u, dword g, dword o) const;
            void Zero();

            bool valid;
            Txt filename;
            One<SFtpAttrs> a;
     };
    using DirList = Vec<DirEntry>;

public:
    SFtp&                   Timeout(i32 ms)                                         { ssh->timeout = ms; return *this; }
    SFtp&                   ChunkSize(i32 sz)                                       { ssh->chunk_size = clamp(sz, 1, INT_MAX); return *this; }

    // File
    SFtpHandle              Open(const Txt& path, dword flags, long mode);
    SFtpHandle              OpenRead(const Txt& path)                            { return Open(path, READ, IRALL); }
    SFtpHandle              OpenWrite(const Txt& path)                           { return Open(path, CREATE | WRITE, IRALL | IWUSR); }
    void                    Close(SFtpHandle handle);
    bool                    Rename(const Txt& oldpath, const Txt& newpath);
    bool                    Delete(const Txt& path);
    bool                    Sync(SFtpHandle handle);
    SFtp&                   Seek(SFtpHandle handle, z64 position);
    z64                   GetPos(SFtpHandle handle);

    // Read/Write
    i32                     Get(SFtpHandle handle, uk ptr, i32 size = INT_MAX);
    i32                     Put(SFtpHandle handle, ukk ptr, i32 size);
    bool                    SaveFile(tukk path, const Txt& data);
    Txt                  LoadFile(tukk path);
    bool                    SaveFile(tukk path, Stream& in);
    bool                    LoadFile(Stream& out, tukk path);
    
    i32                     GetDone() const                                         { return done; }
    
    // Dir
    SFtpHandle              OpenDir(const Txt& path);
    bool                    MakeDir(const Txt& path, long mode);
    bool                    RemoveDir(const Txt& path);
    bool                    ListDir(SFtpHandle handle, DirList& list);
    bool                    ListDir(const Txt& path, DirList& list);
    // Symlink
    bool                    MakeLink(const Txt& orig, Txt& target)            { return SymLink(orig, target, LIBSSH2_SFTP_SYMLINK); }
    bool                    ReadLink(const Txt& path, Txt& target)            { return SymLink(path, target, LIBSSH2_SFTP_READLINK); }
    bool                    RealizePath(const Txt& path, Txt& target)         { return SymLink(path, target, LIBSSH2_SFTP_REALPATH); }

    Txt                  GetDefaultDir()                                         { Txt s; return RealizePath(".", s) ? s : Txt::GetVoid(); }
    
    // Attributes
    bool                    GetAttrs(SFtpHandle handle, SFtpAttrs& attrs);
    bool                    GetAttrs(const Txt& path, SFtpAttrs& attrs);
    bool                    SetAttrs(SFtpHandle handle, const SFtpAttrs& attrs);
    bool                    SetAttrs(const Txt& path, const SFtpAttrs& attrs);
    DirEntry                GetInfo(const Txt& path);
    bool                    SetInfo(const DirEntry& entry)                          { return SetAttrs(entry.GetName(), ~entry); }
    z64                   GetSize(const Txt& path)                             { return QueryAttr(path, SFTP_ATTR_SIZE); }
    bool                    SetSize(const Txt& path, z64 size)                 { return ModifyAttr(path, SFTP_ATTR_SIZE, size); }
    Time                    GetLastModifyTime(const Txt& path)                   { return QueryAttr(path, SFTP_ATTR_LAST_MODIFIED); }
    bool                    SetLastModifyTime(const Txt& path, const Time& time) { return ModifyAttr(path, SFTP_ATTR_LAST_MODIFIED, time); }
    Time                    GetLastAccessTime(const Txt& path)                   { return QueryAttr(path, SFTP_ATTR_LAST_ACCESSED); }
    bool                    SetLastAccessTime(const Txt& path, const Time& time) { return ModifyAttr(path, SFTP_ATTR_LAST_ACCESSED, time); }

    // Tests
    bool                    FileExists(const Txt& path)                          { return QueryAttr(path, SFTP_ATTR_FILE); }
    bool                    DirExists(const Txt& path)                     { return QueryAttr(path, SFTP_ATTR_DIRECTORY); }
    bool                    SymLinkExists(const Txt& path)                       { return QueryAttr(path, SFTP_ATTR_SYMLINK); }
    bool                    SocketExists(const Txt& path)                        { return QueryAttr(path, SFTP_ATTR_SOCKET); }
    bool                    PipeExists(const Txt& path)                          { return QueryAttr(path, SFTP_ATTR_PIPE); }
    bool                    BlockExists(const Txt& path)                         { return QueryAttr(path, SFTP_ATTR_BLOCK); }
    bool                    SpecialFileExists(const Txt& path)                   { return QueryAttr(path, SFTP_ATTR_SPECIAL); }

    Gate<z64, z64>      WhenProgress;
    
    SFtp(SshSession& session);
    virtual ~SFtp();

    SFtp(SFtp&&) = default;

private:
    bool                    Init() override;
    void                    Exit() override;

    i32                     FStat(SFtpHandle handle, SFtpAttrs& a, bool set);
    i32                     LStat(const Txt& path, SFtpAttrs& a, i32 type);
    Val                   QueryAttr(const Txt& path, i32 attr);
    bool                    ModifyAttr(const Txt& path, i32 attr, const Val& v);
    bool                    SymLink(const Txt& path, Txt& target, i32 type);
    i32                     Read(SFtpHandle handle, uk ptr, i32 size);
    i32                     Write(SFtpHandle handle, ukk ptr, i32 size);
    bool                    CopyData(Stream& dest, Stream& src, z64 maxsize = INT64_MAX);
  
    One<LIBSSH2_SFTP*>      sftp_session;
    i32                     done;

    enum FileAttributes {
        SFTP_ATTR_FILE,
        SFTP_ATTR_DIRECTORY,
        SFTP_ATTR_SYMLINK,
        SFTP_ATTR_SOCKET,
        SFTP_ATTR_PIPE,
        SFTP_ATTR_BLOCK,
        SFTP_ATTR_SPECIAL,
        SFTP_ATTR_UID,
        SFTP_ATTR_GID,
        SFTP_ATTR_PERMISSIONS,
        SFTP_ATTR_SIZE,
        SFTP_ATTR_LAST_MODIFIED,
        SFTP_ATTR_LAST_ACCESSED
    };
};

class SFtpStream : public BlockStream {
protected:
    virtual  void  SetStreamSize(z64 size);
    virtual  dword Read(z64 at, uk ptr, dword size);
    virtual  void  Write(z64 at, ukk data, dword size);

public:
    virtual  void  Close();
    virtual  bool  IsOpen() const;

protected:
    SFtp       *sftp;
    SFtpHandle  handle;

    void      SetPos(z64 pos);
    void      Init(z64 size);

public:
    operator  bool() const                 { return IsOpen(); }

    bool       Open(SFtp& sftp, tukk filename, dword mode, i32 acm = 0644);
    SFtpStream(SFtp& sftp, tukk filename, dword mode, i32 acm = 0644);
    SFtpStream();
    ~SFtpStream();
    SFtpHandle GetHandle() const            { return handle; }
};

class SFtpFileOut : public SFtpStream {
public:
    bool Open(SFtp& sftp, tukk fn, i32 acm = 0644) { return SFtpStream::Open(sftp, fn, CREATE|NOWRITESHARE, acm); }

    SFtpFileOut(SFtp& sftp, tukk fn)    { Open(sftp, fn); }
    SFtpFileOut()                              {}
};

class SFtpFileAppend : public SFtpStream {
public:
    bool Open(SFtp& sftp, tukk fn)      { return SFtpStream::Open(sftp, fn, APPEND|NOWRITESHARE); }

    SFtpFileAppend(SFtp& sftp, tukk fn) { Open(sftp, fn); }
    SFtpFileAppend()                           {}
};

class SFtpFileIn : public SFtpStream {
public:
    bool Open(SFtp& sftp, tukk fn)      { return SFtpStream::Open(sftp, fn, READ); }

    SFtpFileIn(SFtp& sftp, tukk fn)     { Open(sftp, fn); }
    SFtpFileIn()                               {}
};

class SFtpFSInfo : public FSInfo {
public:
    SFtpFSInfo& Mount(SFtp& sftp)       { browser = &sftp; return *this; }
    i32 GetStyle() const override               { return STYLE_POSIX; }
    bool CreateFolder(Txt path, Txt& error) const override;
    Array<FSInfo::FileInfo> Find(Txt mask, i32 max_count = INT_MAX, bool unmounted = false) const override;
    
    SFtpFSInfo()                        { browser = nullptr; }
    SFtpFSInfo(SFtp& sftp)              { Mount(sftp); }
    virtual ~SFtpFSInfo()               {}
    
private:
    SFtp *browser = nullptr;
};