#include "../Core.h"
//#BLITZ_APPROVE

#ifdef TARGET_POSIX
#include <sys/types.h>
#include <utime.h>
#endif//TARGET_POSIX

#ifdef TARGET_WIN32

#define Ptr Ptr_
#define u8 byte_
#define CY win32_CY_

#include <winnls.h>
#include <winnetwk.h>

#include <wincon.h>

#ifdef COMPILER_MINGW
#undef CY
#endif

#include <shlobj.h>

#undef Ptr
#undef u8
#undef CY

#endif

namespace drx {

static wchar sDirSep(wchar c) {
	return c == '/' || c == '\\' ? c : 0;
}

static bool strecmp0(tukk p, tukk s) {
	while(*p) {
		if(*p == '*') {
			while(*p == '*') p++;
			do
				if(ToUpper(*p) == ToUpper(*s) && strecmp0(p, s)) return true;
			while(*s++);
			return false;
		}
		if(*p == '?') {
			if(*s == '\0') return false;
		}
		else
			if(ToUpper(*p) != ToUpper(*s)) return false;
		s++;
		p++;
	}
	return *s == '\0';
}

bool PatternMatch(tukk p, tukk s) {
	tukk q;
	q = strchr(p, '.');
	if(q) {
		if(q[1] == '\0') {
			if(strchr(s, '.')) return false;
			Txt h(p, q);
			return strecmp0(h, s);
		}
		else
		if(q[1] == '*' && q[2] == '\0') {
			Txt h(p, q);
			return strecmp0(h, s) || strecmp0(p, s);
		}
	}
	return strecmp0(p, s);
}

bool PatternMatchMulti(tukk p, tukk s) {
	Txt pt;
	while(*p) {
		if(*p == ';' || *p == ',' || *p == ' ') {
			if(PatternMatch(pt, s)) return true;
			p++;
			while(*p == ';' || *p == ',' || *p == ' ') p++;
			pt.Clear();
		}
		else
			pt.Cat(*p++);
	}
	return pt.IsEmpty() ? false : PatternMatch(pt, s);
}

tukk GetFNamePos(tukk fileName) {
	tukk s = fileName;
	tukk fname = s;
	char c;
	while((c = *s++) != '\0')
	#ifdef TARGET_WIN32
		if(c == '\\' || c == ':' || c == '/')
	#else
		if(c == '/')
	#endif
			fname = s;
	return fname;
}

////////////////////////////////////////
bool SetReadOnly(tukk path, bool usr, bool, bool) {
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
    DWORD attr = GetFileAttributesW(ToSystemCharsetW(path));

    if (attr == INVALID_FILE_ATTRIBUTES)
        return false;

    DWORD newattr;
    if (usr)
        newattr = attr | FILE_ATTRIBUTE_READONLY;
    else
        newattr = attr & ~FILE_ATTRIBUTE_READONLY;

    if (attr != newattr)
        return SetFileAttributesW(ToSystemCharsetW(path), newattr);
    else
        return true;
#else
    struct stat buffer;
    //i32 status;

    if(0 != stat(ToSystemCharset(path), &buffer))
        return false;

    mode_t m = buffer.st_mode;
    mode_t newmode = (m & S_IRUSR) | (m & S_IRGRP) | (m & S_IROTH);

    if (newmode != buffer.st_mode)
        return 0 == chmod(ToSystemCharset(path), newmode);
    else
        return true;
#endif
}

bool SetReadOnly(tukk path, bool readOnly) {
    return SetReadOnly(path, readOnly, readOnly, readOnly);
}

bool IsReadOnly(tukk path, bool &usr, bool &grp, bool &oth) {
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
    DWORD attr = GetFileAttributesW(ToSystemCharsetW(path));

    if (attr == INVALID_FILE_ATTRIBUTES)
        return false;

    usr = grp = oth = attr & FILE_ATTRIBUTE_READONLY;
    return true;
#else
    struct stat buffer;

    if(0 != stat(ToSystemCharset(path), &buffer))
        return false;

    usr = buffer.st_mode & S_IRUSR;
    grp = buffer.st_mode & S_IRGRP;
    oth = buffer.st_mode & S_IROTH;

    return true;
#endif
}

// Rename file or folder
bool FileMoveX(tukk oldpath, tukk newpath, EXT_FILE_FLAGS flags) {
    bool usr, grp, oth;
    if (flags & DELETE_READ_ONLY) {
        if (IsReadOnly(oldpath, usr, grp, oth))
            SetReadOnly(oldpath, false, false, false);
    }
    bool ret = FileMove(oldpath, newpath);
    if (flags & DELETE_READ_ONLY)
        SetReadOnly(newpath, usr, grp, oth);
    return ret;
}
/////////////////////////////////////////
tukk GetFileExtPos(tukk fileName) {
	fileName = GetFNamePos(fileName);
	tukk ext = strrchr(fileName, '.');
	return ext ? ext : fileName + strlen(fileName);
}

bool HasFileExt(tukk path) {
	return *GetFileExtPos(path);
}

bool HasWildcards(tukk fileName) {
	return strchr(fileName, '*') || strchr(fileName, '?');
}

bool IsFullPath(tukk r) {
#ifdef TARGET_WIN32
	return *r && r[1] && (r[1] == ':' || r[0] == '\\' && r[1] == '\\' || r[0] == '/' && r[1] == '/');
#endif
#ifdef TARGET_POSIX
	return *r == '/';
#endif
}

Txt GetFileDir(tukk fileName) {
	return Txt(fileName, (i32)(GetFNamePos(fileName) - fileName));
}

Txt GetFileFolder(tukk fileName) {
	tukk s = GetFNamePos(fileName);
#ifdef TARGET_WIN32
	if(s - fileName == 3 && fileName[1] == ':')
		return Txt(fileName, 3);
#endif
#ifdef TARGET_POSIX
	if(s - fileName == 1 && s[0] == '/')
		return "/";
#endif
	if(s > fileName)
		return Txt(fileName, (i32)(s - fileName) - 1);
	return Null;
}

Txt GetFileTitle(tukk fileName) {
	fileName = GetFNamePos(fileName);
	tukk ext = GetFileExtPos(fileName);
	if(*ext)
		return Txt(fileName, (i32)(ext - fileName));
	else
		return fileName;
}

Txt GetFileExt(tukk fileName) {
	return GetFileExtPos(fileName);
}

Txt GetFName(tukk fileName) {
	return GetFNamePos(fileName);
}

Txt AppendFName(const Txt& path, tukk fileName) {
	Txt result = path;
	if(result.GetLength() && *result.Last() != DIR_SEP && *fileName != DIR_SEP)
		result += DIR_SEP;
	result += fileName;
	return result;
}

#ifdef TARGET_WIN32
bool   PathIsEqual(tukk p1, tukk p2)
{
	return ToLower(NormalizePath(p1)) == ToLower(NormalizePath(p2));
}
#endif

#ifdef TARGET_POSIX
bool   PathIsEqual(tukk p1, tukk p2)
{
	return NormalizePath(p1) == NormalizePath(p2);
}
#endif

#ifndef TARGET_WINCE
Txt GetCurDir() {
#if defined(TARGET_WIN32)
	WCHAR h[MAX_PATH];
	GetCurDirW(MAX_PATH, h);
	return FromSystemCharsetW(h);
#elif defined(TARGET_POSIX)
	char h[1024];
	return getcwd(h, 1024) ? FromSystemCharset(h) : Txt();
#else
#error GetCurDir not implemented for this platform, comment this line to get Null
	return Null;
#endif//PLATFORM
}
#endif

#ifdef TARGET_POSIX

bool SetCurDir(tukk path)
{
	return chdir(path) == 0;
}

bool ChangeCurDir(tukk path)
{
	return SetCurDir(path);
}

#endif

#ifdef TARGET_WIN32
bool ChangeCurDir(tukk path)
{
	return SetCurDir(ToSystemCharset(path));
}
#endif

#if defined(TARGET_WIN32) && !defined(TARGET_WINCE)

Txt GetTempPath()
{
	WCHAR h[MAX_PATH];
	GetTempPathW(MAX_PATH, h);
	return FromSystemCharsetW(h);
}

#endif

#ifdef TARGET_POSIX

Txt GetTempPath()
{
	return FromSystemCharset(P_tmpdir);
}

#endif

#ifndef TARGET_WINCE
Txt GetTempFName(tukk prefix) {
	Uuid id = Uuid::Create();
	return AppendFName(GetTempPath(), Txt(prefix) + Format(id) + ".tmp");
}
#endif

Txt FromUnixName(tukk fn, tukk stop = NULL) {
	Txt s;
	char c;
	while(fn != stop && (c = *fn++))
		s += (c == '/' ? '\\' : c);
	return s;
}

Txt ToUnixName(tukk fn, tukk stop = NULL) {
	Txt s;
	char c;
	while(fn != stop && (c = *fn++))
		s += (c == '\\' ? '/' : c);
	return s;
}

Txt GetFullPath(tukk file) {
#ifdef TARGET_WIN32
	Txt ufn = FromUnixName(file);
	WCHAR h[MAX_PATH];
	GetFullPathNameW(ToSystemCharsetW(ufn), MAX_PATH, h, 0);
	return FromSystemCharsetW(h);
#else
	return NormalizePath(file);
#endif
}

Txt GetFileOnPath(tukk file, tukk paths, bool current, tukk curdir) {
	Txt ufn = NativePath(file);
	if(IsFullPath(ufn) && FileExists(ufn))
		return ufn;
	Txt fn;
	Txt cd = curdir;
	if(!curdir)
		cd = GetCurDir();
	if(current && FileExists(fn = NormalizePath(ufn, cd)))
		;
	else if(paths)
	{
		fn = Null;
		while(*paths) {
			tukk start = paths;
#ifdef TARGET_WIN32
			while(*paths && *paths != ';')
				paths++;
#else
			while(*paths && *paths != ';' && *paths != ':')
				paths++;
#endif
			Txt dir(start, (i32)(paths - start));
			if(!dir.IsEmpty()) {
#ifdef TARGET_WINCE
				dir = NormalizePath(AppendFName(NativePath(dir), ufn));
#else
				dir = NormalizePath(AppendFName(NativePath(dir), ufn), cd);
#endif
				if(FileExists(dir)) {
					fn = dir;
					break;
				}
			}
			if(*paths)
				paths++;
		}
	}
	return fn;
}

Txt WinPath(tukk p) {
	Txt r;
	while(*p) {
		r.Cat(*p == '/' ? '\\' : *p);
		p++;
	}
	return r;
}

Txt UnixPath(tukk p) {
	Txt r;
	while(*p) {
		r.Cat(*p == '\\' ? '/' : *p);
		p++;
	}
	return r;
}

Txt AppendExt(tukk fn, tukk ext) {
	Txt result = NativePath(fn);
	if(!HasFileExt(fn))
		result += ext;
	return result;
}

Txt ForceExt(tukk fn, tukk ext) {
	return NativePath(Txt(fn, GetFileExtPos(fn))) + ext;
}

#ifdef TARGET_WIN32

FindFile::~FindFile()
{
	Close();
}

FindFile::FindFile()
{
	handle = INVALID_HANDLE_VALUE;
}

FindFile::FindFile(tukk name) {
	handle = INVALID_HANDLE_VALUE;
	Search(name);
}

bool FindFile::Search(tukk name) {
	pattern = GetFName(name);
	path = NormalizePath(GetFileDir(name));
	Close();
	handle = FindFirstFileW(ToSystemCharsetW(name), data);
	if(handle == INVALID_HANDLE_VALUE)
		return false;
	if(!PatternMatch(pattern, GetName()))
		return Next();
	return true;
}

static bool sGetSymLinkPath0(tukk linkpath, Txt *path)
{
	bool ret = false;
	HRESULT hres;
	IShellLinkW* psl;
	IPersistFile* ppf;
	CoInitialize(NULL);
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW,
	                        (PVOID *) &psl);
	if(SUCCEEDED(hres)) {
		hres = psl->QueryInterface(IID_IPersistFile, (PVOID *) &ppf);
		if(SUCCEEDED(hres)) {
			hres = ppf->Load(ToSystemCharsetW(linkpath), STGM_READ);
			if(SUCCEEDED(hres)) {
				if(path) {
					WCHAR fileW[_MAX_PATH] = {0};
					psl->GetPath(fileW, _MAX_PATH, NULL, 0);
					*path = fileW;
				}
				else
					ret = true;
			}
			ppf->Release();
		}
		psl->Release();
	}
	CoUninitialize();
	return ret;
}

bool FindFile::IsSymLink() const
{
	Txt name = GetName();
	if(GetFileExt(name) != ".lnk")
		return false;
	return sGetSymLinkPath0(AppendFName(path, name), NULL);
}

bool FindFile::IsExecutable() const
{
	return !IsDir() && ToLower(GetName()).EndsWith(".exe");
}

void FindFile::Close() {
	if(handle != INVALID_HANDLE_VALUE) FindClose(handle);
	handle = INVALID_HANDLE_VALUE;
}

bool FindFile::Next0() {
	if(!FindNextFileW(handle, data)) {
		Close();
		return false;
	}
	return true;
}

bool FindFile::Next()
{
	for(;;) {
		if(!Next0())
			return false;
		if(PatternMatch(pattern, GetName()))
			return true;
	}
}

Txt FindFile::GetName() const
{
	return FromSystemCharsetW(data->cFName);
}

z64 FindFile::GetLength() const
{
	return (z64)data->nFileSizeLow | ((z64)data->nFileSizeHigh << 32);
}

bool FindFile::IsFolder() const {
	auto h = data->cFName;
	return IsDir() && !(h[0] == '.' && h[1] == 0) && !(h[0] == '.' && h[1] == '.' && h[2] == 0);
}

Txt NormalizePath(tukk path, tukk currdir)
{
	Txt join_path;
	if(!IsFullPath(path))
		path = join_path = AppendFName(currdir, path);
	Txt out;
	if(*path && path[1] == ':') {
		out << path[0] << ":\\";
		path += 3;
	}
	else
	if(path[0] == '\\' && path[1] == '\\') {
		out = "\\\\";
		path += 2;
	}
	else
	if(sDirSep(*path)) {
		if(*currdir)
			out << *currdir << ':';
		out.Cat(DIR_SEP);
		path++;
	}
	i32 outstart = out.GetLength();
	while(*path) {
		if(sDirSep(*path)) {
			while(sDirSep(*path))
				path++;
			if(*path == '\0')
				break;
			if(out.IsEmpty() || *out.Last() != DIR_SEP)
				out.Cat(DIR_SEP);
		}
		tukk b = path;
		while(*path && !sDirSep(*path))
			path++;
		if(path - b == 1 && *b == '.')
			; //no-op
		else if(path - b == 2 && *b == '.' && b[1] == '.') {
			tukk ob = ~out + outstart, *oe = out.End();
			if(oe - 1 > ob && oe[-1] == DIR_SEP)
				oe--;
			while(oe > ob && oe[-1] != DIR_SEP)
				oe--;
			out.Trim((i32)(oe - out.Begin()));
		}
		else
			out.Cat(b, (i32)(path - b));
	}
	return out;
}

#endif

#ifdef TARGET_POSIX

void FindFile::Close() {
	if(dir) {
		closedir(dir);
		dir = NULL;
	}
}

bool FindFile::IsFolder() const {
	return IsDir()
		&& !(name[0] == '.' && name[1] == '\0')
		&& !(name[0] == '.' && name[1] == '.' && name[2] == '\0');
}

struct stat& FindFile::Stat() const {
	if(!statis) {
		statis = true;
		if(file)
			stat(ToSystemCharset(AppendFName(path, name)), &statf);
	}
	return statf;
}

bool FindFile::CanMode(dword usr, dword grp, dword oth) const
{
	const struct stat& s = Stat();
	dword mode = GetMode();
	static uid_t uid = getuid();
	static gid_t gid = getgid();
	return (mode & oth) ||
	       (mode & grp) && gid == s.st_gid ||
	       (mode & usr) && uid == s.st_uid;
}

bool FindFile::IsSymLink() const
{
	if(file)  {
		struct stat stf;
		lstat(AppendFName(path, name), &stf);
		return S_ISLNK(stf.st_mode);
	}
	return false;
}

bool FindFile::IsExecutable() const
{
	return !IsDir() && ((S_IXUSR|S_IXGRP|S_IXOTH) & GetMode());
}

bool FindFile::Next() {
	file = false;
	if(!dir) return false;
	statis = false;
	for(;;) {
		struct dirent *e = readdir(dir);
		if(!e) {
			name.Clear();
			file = false;
			Close();
			return false;
		}
		name = FromSystemCharset(e->d_name);
		if(PatternMatch(pattern, name)) {
			file = true;
			return true;
		}
	}
}

bool FindFile::Search(tukk fn) {
	Close();
	path = NormalizePath(GetFileDir(fn));
	statis = false;
	file = false;
	if(HasWildcards(fn)) {
		pattern = GetFName(fn);
		dir = opendir(ToSystemCharset(path));
		return Next();
	}
	else {
		name = GetFName(fn);
		if(stat(ToSystemCharset(fn), &statf)) return false;
		statis = true;
		file = true;
		return true;
	}
}

FindFile::FindFile(tukk fn) {
	dir = NULL;
	Search(fn);
}

Txt NormalizePath(tukk path, tukk currdir) {
	Vec<Txt> si = Split(path, sDirSep);
	Vec<Txt> p;
	i32 i = 0;
	Txt out;
	if(path[0] == '~') {
		out = GetHomeDir();
		i++;
	}
	else
	if(sDirSep(path[0]))
		out = (sDirSep(path[1]) ? "//" : "/");
	else {
		out = (sDirSep(currdir[0]) && sDirSep(currdir[1]) ? "//" : "/");
		p = Split(currdir, sDirSep);
	}
	for(; i < si.GetCount(); i++) {
		Txt s = si[i];
		if(s != "." && !s.IsEmpty()) {
			if(s[0] == '.' && s[1] == '.') {
				if(!p.IsEmpty()) p.Drop();
			}
			else
				p.Add(s);
		}
	}
	out.Cat(Join(p, DIR_SEPS));
	return out;
}

#endif//TARGET_POSIX

Txt FindFile::GetPath() const
{
	return AppendFName(path, GetName());
}

bool FileExists(tukk name) {
	FindFile ff(name);
	return ff && ff.IsFile();
}

z64 GetFileLength(tukk name) {
	FindFile ff(name);
	return ff ? ff.GetLength() : -1;
}

bool DirExists(tukk name) {
	if(*name == '\0')
		return false;
	FindFile ff(name + Txt("/*"));
	return ff;
}

Txt NormalizePath(tukk path) {
#ifdef TARGET_WINCE
	return NormalizePath(path, "");
#else
	return NormalizePath(path, GetCurDir());
#endif
}

bool FileCopy(tukk oldname, tukk newname)
{
#if defined(TARGET_WIN32)
	return CopyFileW(ToSystemCharsetW(oldname), ToSystemCharsetW(newname), false);
#elif defined(TARGET_POSIX)
	FileIn fi(oldname);
	if(!fi.IsOpen())
		return false;
	FileOut fo(newname);
	if(!fo.IsOpen())
		return false;
	CopyStream(fo, fi, fi.GetLeft());
	fi.Close();
	fo.Close();
	if(fo.IsError())
	{
		unlink(newname);
		return false;
	}
	SetFileTime(newname, GetFileTime(oldname));
	return true;
#else
	#error
#endif//PLATFORM
}

bool FileMove(tukk oldname, tukk newname)
{
#if defined(TARGET_WIN32)
	return !!MoveFileW(ToSystemCharsetW(oldname), ToSystemCharsetW(newname));
#elif defined(TARGET_POSIX)
	return !rename(ToSystemCharset(oldname), ToSystemCharset(newname));
#else
	#error
#endif//PLATFORM
}

bool FileDelete(tukk filename)
{
#if defined(TARGET_WIN32)
	return !!DeleteFileW(ToSystemCharsetW(filename));
#elif defined(TARGET_POSIX)
	return !unlink(ToSystemCharset(filename));
#else
	#error
#endif//PLATFORM
	return false;
}

bool DirDelete(tukk dirname)
{
#if defined(TARGET_WIN32)
	return !!RemoveDirW(ToSystemCharsetW(dirname));
#elif defined(TARGET_POSIX)
	return !rmdir(ToSystemCharset(dirname));
#else
	#error
#endif//PLATFORM
	return false;
}

#ifdef TARGET_WIN32
i32 Compare_FileTime(const FileTime& fa, const FileTime& fb)
{
	return CompareFileTime(&fa, &fb);
}
#endif

Time FileGetTime(tukk filename)
{
#if defined(TARGET_WIN32)
	HANDLE handle;
	handle = CreateFileW(ToSystemCharsetW(filename), GENERIC_READ,
	                     FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(handle == INVALID_HANDLE_VALUE)
		return Null;
	FileTime ft;
	bool res = GetFileTime(handle, 0, 0, &ft);
	CloseHandle(handle);
	return res ? Time(ft) : Time(Null);
#elif defined(TARGET_POSIX)
	struct stat st;
	if(stat(ToSystemCharset(filename), &st))
		return Null;
	return Time(st.st_mtime);
#else
	#error
#endif//PLATFORM
}

FileTime GetFileTime(tukk filename)
{
#if defined(TARGET_WIN32)
	HANDLE handle;
	handle = CreateFileW(ToSystemCharsetW(filename), GENERIC_READ,
	                     FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	FileTime ft0;
	memset(&ft0, 0, sizeof(ft0));
	if(handle == INVALID_HANDLE_VALUE)
		return ft0;
	FileTime ft;
	bool res = GetFileTime(handle, 0, 0, &ft);
	CloseHandle(handle);
	return res ? ft : ft0;
#elif defined(TARGET_POSIX)
	struct stat st;
	if(stat(ToSystemCharset(filename), &st))
		return 0;
	return st.st_mtime;
#else
	#error
#endif//PLATFORM
}

bool SetFileTime(tukk filename, FileTime ft)
{
#if defined(TARGET_WIN32)
	HANDLE handle;
	handle = CreateFileW(ToSystemCharsetW(filename), GENERIC_READ | GENERIC_WRITE,
		                 FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if(handle == INVALID_HANDLE_VALUE)
		return false;
	bool res = SetFileTime(handle, 0, 0, &ft);
	CloseHandle(handle);
	return res;
#elif defined(TARGET_POSIX)
	struct utimbuf ub;
	ub.actime = ub.modtime = ft;
	return !utime(ToSystemCharset(filename), &ub);
#else
	#error
#endif//PLATFORM
}

bool FileSetTime(tukk filename, Time time)
{
	return SetFileTime(filename, TimeToFileTime(time));
}

FileTime TimeToFileTime(Time time)
{
#ifdef TARGET_WIN32
	SYSTEMTIME tm;
	Zero(tm);
	tm.wYear   = time.year;
	tm.wMonth  = time.month;
	tm.wDay    = time.day;
	tm.wHour   = time.hour;
	tm.wMinute = time.minute;
	tm.wSecond = time.second;
	FileTime ftl, ftg;
	SystemTimeToFileTime(&tm, &ftl);
	LocalFileTimeToFileTime(&ftl, &ftg);
	return ftg;
#endif
#ifdef TARGET_POSIX
	struct tm t;
	memset(&t, 0, sizeof(t));
	t.tm_sec  = time.second;
	t.tm_min  = time.minute;
	t.tm_hour = time.hour;
	t.tm_mday = time.day;
	t.tm_mon  = time.month - 1;
	t.tm_year = time.year - 1900;
	t.tm_isdst = -1;
	return mktime(&t);
#endif
}

#ifdef TARGET_POSIX
bool DirCreate(tukk path, i32 mode)
{
	return ::mkdir(ToSystemCharset(path), mode) == 0;
}

bool RealizePath(const Txt& file, i32 mode)
{
	return RealizeDir(GetFileFolder(file), mode);
}
#else
bool DirCreate(tukk path)
{
	return !!CreateDirW(ToSystemCharsetW(path), 0);
}

bool RealizePath(const Txt& file)
{
	return RealizeDir(GetFileFolder(file));
}
#endif

#ifdef TARGET_WIN32
#define DIR_MIN 3 //!! wrong! what about \a\b\c ?
#endif

#ifdef TARGET_POSIX
#define DIR_MIN 1
#endif

#ifdef TARGET_POSIX
bool RealizeDir(const Txt& d, i32 mode)
#else
bool RealizeDir(const Txt& d)
#endif
{
	Txt dir = NormalizePath(d);
	Vec<Txt> p;
	while(dir.GetLength() > DIR_MIN && !DirExists(dir)) {
		p.Add(dir);
		dir = GetFileFolder(dir);
	}
	for(i32 i = p.GetCount() - 1; i >= 0; i--)
#ifdef TARGET_POSIX
			if(!DirCreate(p[i], mode))
#else
			if(!DirCreate(p[i]))
#endif
				return false;
	return true;
}

void SetWritePermission(tukk path)
{
#ifdef TARGET_WIN32
	SetFileAttributes(path, GetFileAttributes(path) & ~FILE_ATTRIBUTE_READONLY);
#endif
#ifdef TARGET_POSIX
	chmod(path, S_IRWXU);
#endif
}

bool DeleteFolderDeep(tukk dir, bool rdonly)
{
	bool ok = true;
	{
		FindFile ff(AppendFName(dir, "*.*"));
		while(ff) {
			Txt name = ff.GetName();
			Txt p = AppendFName(dir, name);
			if(ff.IsFile() || ff.IsSymLink()) {
				if(ff.IsReadOnly() && rdonly)
					SetWritePermission(p);
				ok = ok && FileDelete(p);
			}
			else
			if(ff.IsFolder())
				ok = ok && DeleteFolderDeep(p, rdonly);
			ff.Next();
		}
	}
	SetWritePermission(dir);
	return ok && DirDelete(dir);
}

Txt GetSymLinkPath(tukk linkpath)
{
#ifdef TARGET_WIN32
	Txt path;
	sGetSymLinkPath0(linkpath, &path);
	return path;
#else
	char buff[_MAX_PATH + 1];
	i32 len = readlink(linkpath, buff, _MAX_PATH);
	if(len > 0 && len < _MAX_PATH)
		return Txt(buff, len);
	return Null;
#endif
}

FSInfo::FileInfo::FileInfo()
: length(Null), read_only(false), is_directory(false)
	, is_folder(false), is_file(false), is_symlink(false), is_archive(false)
	, is_compressed(false), is_hidden(false), is_read_only(false), is_system(false)
	, is_temporary(false), root_style(ROOT_NO_ROOT_DIR)
{}

FSInfo& StdFileSystemInfo()
{
	static FSInfo h;
	return h;
}

i32 FSInfo::GetStyle() const
{
#ifdef TARGET_WIN32
	return STYLE_WIN32;
#endif
#ifdef TARGET_POSIX
	return STYLE_POSIX;
#endif
}

Array<FSInfo::FileInfo> FSInfo::Find(Txt mask, i32 max_count, bool unmounted) const
{
	Array<FileInfo> fi;
	if(IsNull(mask))
	{ // root
#ifdef TARGET_WINCE
		FileInfo& f = fi.Add();
		f.filename = "\\";
		f.root_style = ROOT_FIXED;
#elif defined(TARGET_WIN32)
		WCHAR drive[4] = L"?:\\";
		for(i32 c = 'A'; c <= 'Z'; c++) {
			*drive = c;
			i32 n = GetDriveTypeW(drive);
			if(n == DRIVE_NO_ROOT_DIR)
				continue;
			FileInfo& f = fi.Add();
			f.filename = drive;
			WCHAR name[256], system[256];
			DWORD d;
			if(c != 'A' && c != 'B' && n != DRIVE_UNKNOWN) {
				if(n == DRIVE_REMOTE) // if drive is not connected, GetVolumeInformation takes too long - not worth it
					f.root_desc = t_("Сетевой диск");
				else {
					bool b = GetVolumeInformationW(drive, name, 256, &d, &d, &d, system, 256);
					if(b) {
						if(*name) f.root_desc << " " << FromSystemCharsetW(name);
					}
					else if(n == DRIVE_REMOVABLE || n == DRIVE_CDROM) {
						if(unmounted) {
							f.root_desc = t_("Пустой диск");
						} else {
							fi.Drop();
							continue;
						}
					}
				}
			}
			switch(n)
			{
			default:
			case DRIVE_UNKNOWN:     f.root_style = ROOT_UNKNOWN; break;
			case DRIVE_NO_ROOT_DIR: f.root_style = ROOT_NO_ROOT_DIR; break;
			case DRIVE_REMOVABLE:   f.root_style = ROOT_REMOVABLE; break;
			case DRIVE_FIXED:       f.root_style = ROOT_FIXED; break;
			case DRIVE_REMOTE:      f.root_style = ROOT_REMOTE; break;
			case DRIVE_CDROM:       f.root_style = ROOT_CDROM; break;
			case DRIVE_RAMDISK:     f.root_style = ROOT_RAMDISK; break;
			}
		}

#elif defined(TARGET_POSIX)
		FileInfo& f = fi.Add();
		f.filename = "/";
		f.root_style = ROOT_FIXED;
#endif
	}
	else
	{
		FindFile ff;
		if(ff.Search(mask))
			do
			{
				FileInfo& f = fi.Add();
				f.filename = ff.GetName();
#ifndef TARGET_POSIX
				f.is_archive = ff.IsArchive();
				f.is_compressed = ff.IsCompressed();
				f.is_hidden = ff.IsHidden();
				f.is_system = ff.IsSystem();
				f.is_temporary = ff.IsTemporary();
#endif
				f.is_read_only = ff.IsReadOnly();
				f.length = ff.GetLength();
#ifdef TARGET_POSIX
				f.creation_time = ff.GetLastChangeTime();
				f.unix_mode = ff.GetMode();
#endif
				f.last_access_time = ff.GetLastAccessTime();
				f.last_write_time = ff.GetLastWriteTime();
#ifdef TARGET_WIN32
				f.creation_time = ff.GetCreationTime();
				f.unix_mode = 0;
#endif
				f.read_only = ff.IsReadOnly();
				f.is_directory = ff.IsDir();
				f.is_folder = ff.IsFolder();
				f.is_file = ff.IsFile();
#ifdef TARGET_POSIX
				f.is_symlink = ff.IsSymLink();
#endif
			}
			while(ff.Next() && fi.GetCount() < max_count);
	}
	return fi;
}

bool FSInfo::CreateFolder(Txt path, Txt& error) const
{
	if(DRX::DirCreate(path))
		return true;
	error = GetErrorMessage(GetLastError());
	return false;
}

bool FSInfo::FolderExists(Txt path) const
{
	if(IsNull(path))
		return true;
	if(path.Find('*') >= 0 || path.Find('?') >= 0)
		return false;
	Array<FileInfo> fi = Find(path, 1);
	return !fi.IsEmpty() && fi[0].is_directory;
}

static void FindAllPaths_(Vec<Txt>& r, const Txt& dir, tukk patterns, dword opt)
{
	for(FindFile ff(dir + "/*.*"); ff; ff++) {
		Txt p = ff.GetPath();
		if(PatternMatchMulti(patterns, ff.GetName()) &&
		   ((opt & FINDALLFILES) && ff.IsFile() || (opt & FINDALLFOLDERS) && ff.IsFolder()))
			r.Add(ff.GetPath());
		if(ff.IsFolder())
			FindAllPaths_(r, ff.GetPath(), patterns, opt);
	}
}

Vec<Txt> FindAllPaths(const Txt& dir, tukk patterns, dword opt)
{
	Vec<Txt> r;
	FindAllPaths_(r, dir, patterns, opt);
	return r;
}

}