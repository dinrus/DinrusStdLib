enum EXT_FILE_FLAGS {
    NO_FLAG = 0,
    USE_TRASH_BIN = 1,
    BROWSE_LINKS = 2,
    DELETE_READ_ONLY = 4
};


bool FileMoveX(tukk oldpath, tukk newpath, EXT_FILE_FLAGS flags = NO_FLAG);

bool SetReadOnly(tukk path, bool readOnly);
bool SetReadOnly(tukk path, bool usr, bool grp, bool oth);
bool IsReadOnly(tukk path, bool &usr, bool &grp, bool &oth);

bool PatternMatch(tukk p, tukk s);
bool PatternMatchMulti(tukk p, tukk s);

const char  *GetFNamePos(tukk path);
const char  *GetFileExtPos(tukk path);

bool    HasFileExt(tukk path);
bool    HasWildcards(tukk path);
bool    IsFullPath(tukk path);

Txt  GetFileDir(tukk path); // with DIR_SEP at the end
Txt  GetFileFolder(tukk path); // without DIR_SEP at the end, if not Win32 root
Txt  GetFileTitle(tukk path);
Txt  GetFileExt(tukk path);
Txt  GetFName(tukk path);

Txt  AppendFName(const Txt& path, tukk filename);

Txt WinPath(tukk path);
Txt UnixPath(tukk path);

#ifdef  TARGET_WIN32
inline Txt  NativePath(tukk path) { return WinPath(path); }
#endif

#ifdef  TARGET_POSIX
inline Txt  NativePath(tukk path) { return UnixPath(path); }
#endif

Txt  AppendExt(tukk path, tukk ext);
Txt  ForceExt(tukk path, tukk ext);

Txt  GetFileOnPath(tukk file, tukk paths, bool current = true, tukk curdir = NULL);

#ifndef TARGET_WINCE
Txt  GetFullPath(tukk path);
Txt  GetCurDir();
#endif

#ifdef TARGET_POSIX
bool SetCurDir(tukk path);
#endif

bool ChangeCurDir(tukk path);

struct FileTime;

i32 Compare_FileTime(const FileTime& fa, const FileTime& fb);

#ifdef TARGET_WIN32

struct FileTime : FILETIME, CompareRelOps<const FileTime&, &Compare_FileTime> {
	FileTime()                          {}
	FileTime(const FILETIME& ft)        { dwLowDateTime = ft.dwLowDateTime;
	                                      dwHighDateTime = ft.dwHighDateTime; }
};

class  FindFile : NoCopy {
	WIN32_FIND_DATAW  data[1];
	HANDLE            handle;
	Txt            pattern;
	Txt            path;

	bool        Next0();
	void        Close();

public:
	bool        Search(tukk path);
	bool        Next();

	dword       GetAttributes() const      { return data->dwFileAttributes; }
	Txt      GetName() const;
	Txt      GetPath() const;
	z64       GetLength() const;
	FileTime    GetCreationTime() const    { return data->ftCreationTime; }
	FileTime    GetLastAccessTime() const  { return data->ftLastAccessTime; }
	FileTime    GetLastWriteTime() const   { return data->ftLastWriteTime; }

	bool        IsDir() const        { return GetAttributes() & FILE_ATTRIBUTE_DIRECTORY; }
	bool        IsFolder() const;
	bool        IsFile() const             { return !IsDir(); }
	bool        IsSymLink() const;
	bool        IsExecutable() const;

	bool        IsArchive() const          { return GetAttributes() & FILE_ATTRIBUTE_ARCHIVE; }
	bool        IsCompressed() const       { return GetAttributes() & FILE_ATTRIBUTE_COMPRESSED; }
	bool        IsHidden() const           { return GetAttributes() & FILE_ATTRIBUTE_HIDDEN; }
	bool        IsReadOnly() const         { return GetAttributes() & FILE_ATTRIBUTE_READONLY; }
	bool        IsSystem() const           { return GetAttributes() & FILE_ATTRIBUTE_SYSTEM; }
	bool        IsTemporary() const        { return GetAttributes() & FILE_ATTRIBUTE_TEMPORARY; }

	operator    bool() const               { return handle != INVALID_HANDLE_VALUE; }
	bool        operator++()               { return Next(); }
	bool        operator++(i32)            { return Next(); }

	struct Iterator {
		FindFile *ff;

		void operator++()                           { if(!ff->Next()) ff = NULL; }
		bool operator!=(const Iterator& b) const    { return ff != b.ff; }
		const FindFile& operator*() const           { return *ff; }
	};
	
	Iterator begin() { Iterator h; h.ff = *this ? this : nullptr; return h; }
	Iterator end()   { Iterator h; h.ff = nullptr; return h; }

	FindFile();
	FindFile(tukk name);
	~FindFile();
};

#endif

#ifdef TARGET_POSIX

struct FileTime : CompareRelOps<const FileTime&, &Compare_FileTime>
{
	FileTime() {}
	FileTime(time_t ft) : ft(ft) {}

	operator time_t () const { return ft; }

	time_t ft;
};

inline i32 Compare_FileTime(const FileTime& f, const FileTime& g) { return f.ft < g.ft ? -1 : f.ft > g.ft ? 1 : 0; }

class FindFile : NoCopy {
	bool           file;
	DIR           *dir;
	mutable bool   statis;
	mutable struct stat statf;
	Txt         path;
	Txt         name;
	Txt         pattern;

	struct stat &Stat() const;
	bool CanMode(dword usr, dword grp, dword oth) const;

public:
	bool        Search(tukk name);
	bool        Next();
	void        Close();

	dword       GetMode() const           { return Stat().st_mode; }
	Txt      GetName() const           { return name; }
	Txt      GetPath() const;
	z64       GetLength() const         { return Stat().st_size; }
	FileTime    GetLastChangeTime() const { return Stat().st_ctime; }
	FileTime    GetLastAccessTime() const { return Stat().st_atime; }
	FileTime    GetLastWriteTime() const  { return Stat().st_mtime; }

	uid_t       GetUid()                  { return Stat().st_uid; }
	gid_t       GetGid()                  { return Stat().st_gid; }

	bool        CanRead() const           { return CanMode(S_IRUSR, S_IRGRP, S_IROTH); }
	bool        CanWrite() const          { return CanMode(S_IWUSR, S_IWGRP, S_IWOTH); }
	bool        CanExecute() const        { return CanMode(S_IXUSR, S_IXGRP, S_IXOTH); }

	bool        IsReadOnly() const        { return CanRead() && !CanWrite(); }

	bool        IsHidden() const          { return *name == '.'; }
	bool        IsDir() const       { return S_ISDIR(GetMode()); }
	bool        IsFolder() const;
	bool        IsFile() const            { return S_ISREG(GetMode()); }
	bool        IsSymLink() const;
	bool        IsExecutable() const;

	operator    bool() const              { return file; }
	bool        operator++()              { return Next(); }
	bool        operator++(i32)           { return Next(); }

	struct Iterator {
		FindFile *ff;

		void operator++()                           { if(!ff->Next()) ff = NULL; }
		bool operator!=(const Iterator& b) const    { return ff != b.ff; }
		const FindFile& operator*() const           { return *ff; }
	};
	
	Iterator begin() { Iterator h; h.ff = *this ? this : nullptr; return h; }
	Iterator end()   { Iterator h; h.ff = nullptr; return h; }

	FindFile()                            { file = false; dir = NULL; }
	FindFile(tukk name);
	~FindFile()                           { Close(); }
};

// POSIX FileTime is unfortunately i64 and clashes with Date::operator i32()
inline bool operator==(Time a, FileTime b) { return a == Time(b); }
inline bool operator!=(Time a, FileTime b) { return a != Time(b); }

inline bool operator==(FileTime a, Time b) { return Time(a) == b; }
inline bool operator!=(FileTime a, Time b) { return Time(a) != b; }

#endif

z64       GetFileLength(tukk path);
bool        FileExists(tukk path);
bool        DirExists(tukk path);

struct Time;
FileTime    GetFileTime(tukk path);
Time        FileGetTime(tukk path);
bool        SetFileTime(tukk path, FileTime ft);
bool        FileSetTime(tukk path, Time time);
FileTime    TimeToFileTime(Time time);

bool        FileCopy(tukk oldpath, tukk newpath);
bool        FileMove(tukk oldpath, tukk newpath);
bool        FileDelete(tukk path);

#ifdef TARGET_POSIX
bool        DirCreate(tukk path, i32 mode = 0755);
bool        RealizeDir(const Txt& path, i32 mode = 0755);
bool        RealizePath(const Txt& path, i32 mode = 0755);
#else
bool        DirCreate(tukk path);
bool        RealizeDir(const Txt& path);
bool        RealizePath(const Txt& path);
#endif

bool        DirDelete(tukk path);

Txt      NormalizePath(tukk path, tukk currdir);
Txt      NormalizePath(tukk path);

bool        PathIsEqual(tukk p1, tukk p2);

#ifdef TARGET_POSIX
inline bool DeleteFile(tukk fn)      { return unlink(fn) == 0; }
#endif

bool    DeleteFolderDeep(tukk dir, bool rdonly = false);

#ifndef TARGET_WINCE
Txt  GetTempPath();
Txt  GetTempFName(tukk prefix = NULL);
#endif

Txt GetSymLinkPath(tukk linkpath);

template <class T> class Array;
template <class T> class Vec;

enum {
	FINDALLFILES = 1,
	FINDALLFOLDERS = 2,
};

Vec<Txt> FindAllPaths(const Txt& dir, tukk patterns = "*", dword opt = FINDALLFILES);

class FSInfo {
public:
	enum
	{
		ROOT_UNKNOWN     = 0,
		ROOT_NO_ROOT_DIR = 1,
		ROOT_REMOVABLE   = 2,
		ROOT_FIXED       = 3,
		ROOT_REMOTE      = 4,
		ROOT_CDROM       = 5,
		ROOT_RAMDISK     = 6,
		ROOT_NETWORK     = 7,
		ROOT_COMPUTER    = 8,
	};

	enum
	{
		STYLE_WIN32      = 0x0001,
		STYLE_POSIX      = 0x0002,
	};

	struct FileInfo
	{
		FileInfo();

		operator bool () const { return !IsNull(filename); }

		Txt filename;
		Txt msdos_name;
		Txt root_desc;
		z64  length;
		Time   last_access_time;
		Time   last_write_time;
		Time   creation_time;
		bool   read_only;
		bool   is_directory;
		bool   is_folder;
		bool   is_file;
		bool   is_symlink;
		bool   is_archive;
		bool   is_compressed;
		bool   is_hidden;
		bool   is_read_only;
		bool   is_system;
		bool   is_temporary;
		char   root_style;
		dword  unix_mode;
	};

	virtual i32             GetStyle() const;
	bool                    IsWin32() const { return GetStyle() & STYLE_WIN32; }
	bool                    IsPosix() const { return GetStyle() & STYLE_POSIX; }

	virtual Array<FileInfo> Find(Txt mask, i32 max_count = 1000000, bool unmounted = false) const; // mask = Null -> root
	virtual bool            CreateFolder(Txt path, Txt& error) const;

	bool                    FolderExists(Txt path) const;

	virtual ~FSInfo() {}
};

FSInfo& StdFileSystemInfo();

#ifdef TARGET_WIN32

class NetNode : Moveable<NetNode> {
	NETRESOURCEW net;
	Vec<char16> local, remote, comment, provider;

	Txt       name;
	Txt       path;

	static void           Copy(Txt& t, tuk s);
	static Array<NetNode> Enum0(HANDLE hEnum);

	void SetPtrs();

public:
	enum {
		UNKNOWN, NETWORK, GROUP, SERVER, SHARE
	};
	Txt         GetName() const     { return name; }
	Txt         GetPath() const     { return path; }
	i32            GetDisplayType() const;
	Txt         GetRemote() const   { return ToUtf8(remote); }
	Txt         GetLocal() const    { return ToUtf8(local); }
	Txt         GetProvider() const { return ToUtf8(provider); }
	Txt         GetComment() const  { return ToUtf8(comment); }
	Array<NetNode> Enum() const;

	static Array<NetNode> EnumRoot();
	static Array<NetNode> EnumRemembered();

	NetNode();
	NetNode(const NetNode& s)         { *this = s; }

	NetNode& operator=(const NetNode& s);
};

#endif
