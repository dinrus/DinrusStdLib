// SPDX-License-Identifier: Apache-2.0
// Copyright 2021 - 2022, the Anboto author and contributors
#include <drx/Core/Core.h>
#include <drx/Funcs4U/Funcs4U.h>


#ifdef TARGET_WIN32 // || defined (TARGET_WIN64)
    #define Ptr Ptr_
    #define u8 byte_
    #ifndef win32_CY_
        #define win32_CY_ long
    #endif
    #define CY win32_CY_

    #include <shellapi.h>
    #include <wincon.h>
    #include <shlobj.h>

    #undef Ptr
    #undef u8
    #undef CY
#endif

#define TFILE <drx/Funcs4U/Funcs4U.t>
#include <drx/Core/t.h>


namespace drx {

/////////////////////////////////////////////////////////////////////
// LaunchFile

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
bool LaunchFileCreateProcess(tukk file, tukk , tukk directory) {
    STARTUPINFOW startInfo;
    PROCESS_INFORMATION procInfo;

    ZeroMemory(&startInfo, sizeof(startInfo));
    startInfo.cb = sizeof(startInfo);
    ZeroMemory(&procInfo, sizeof(procInfo));

    WTxt wexec;
    wexec = Format("\"%s\" \"%s\"", GetExtExecutable(GetFileExt(file)), file).ToWTxt();
    WTxtBuf wsbexec(wexec);

    if (!CreateProcessW(NULL, (LPWSTR)wsbexec.Begin(), NULL, NULL, FALSE, 0, NULL, ToSystemCharsetW(directory), &startInfo, &procInfo))
        return false;

    WaitForSingleObject(procInfo.hProcess, 0);

    CloseHandle(procInfo.hProcess);
    CloseHandle(procInfo.hThread);
    return true;
}

bool LaunchFileShellExecute(tukk file, tukk params, tukk directory) {
    zu64 ret = zu64(ShellExecuteW(NULL, L"open", ToSystemCharsetW(file), ToSystemCharsetW(params), ToSystemCharsetW(directory), SW_SHOWNORMAL));
    return 32 < ret;
}

bool LaunchFile(tukk file, tukk params, tukk directory) {
    Txt _file = Trim(WinPath(file));
    Txt _params, _directory;
    if (params)
        _params = WinPath(params);
    if (directory)
        _directory = WinPath(directory);
    if (!LaunchFileShellExecute(_file, _params, _directory))            // First try
        return LaunchFileCreateProcess(_file, _params, _directory);     // Second try
    return true;
}
#endif

#ifdef TARGET_POSIX

Txt GetDesktopManagerNew() {
    if(GetEnv("GNOME_DESKTOP_SESSION_ID").GetCount() || GetEnv("GNOME_KEYRING_SOCKET").GetCount())
        return "gnome";
    else if(GetEnv("KDE_FULL_SESSION").GetCount() || GetEnv("KDEDIR").GetCount() || GetEnv("KDE_MULTIHEAD").GetCount())
        return "kde";
    else {
        TxtParse desktopStr;
        if (Sys("xprop -root _DT_SAVE_MODE").Find("xfce") >= 0)
            return "xfce";
        else if ((desktopStr = Sys("xprop -root")).Find("ENLIGHTENMENT") >= 0)
            return "enlightenment";
        else
            return GetEnv("DESKTOP_SESSION");
    }
}

bool LaunchFile(tukk _file, tukk _params, tukk ) {
    Txt file = UnixPath(_file);
    Txt params = _params == nullptr ? "" : UnixPath(_params);
    i32 ret;
    if (GetDesktopManagerNew() == "gnome")
        ret = system("gnome-open \"" + file + "\" " + params);
    else if (GetDesktopManagerNew() == "kde")
        ret = system("kfmclient exec \"" + file + "\" " + params + " &");
    else if (GetDesktopManagerNew() == "enlightenment") {
        Txt mime = GetExtExecutable(GetFileExt(file));
        Txt program = mime.Left(mime.Find("."));     // Left side of mime executable is the program to run
        ret = system(program + " \"" + file + "\" " + params + " &");
    } else
        ret = system("xdg-open \"" + Txt(file) + "\"");
    return (ret >= 0);
}
#endif

/////////////////////////////////////////////////////////////////////
// General utilities

bool FileCat(tukk file, tukk appendFile) {
    if (!FileExists(file))
        return FileCopy(appendFile, file);
    FileAppend f(file);
    if(!f.IsOpen())
        return false;
    FileIn fi(appendFile);
    if(!fi.IsOpen())
        return false;
    CopyStream(f, fi, fi.GetLeft());
    fi.Close();
    f.Close();
    if(f.IsError())
        return false;
    return true;
}

bool FileStrAppend(tukk file, tukk str) {
    FileAppend f(file);
    if(!f.IsOpen())
        return false;
    f << str;
    f.Close();
    if(f.IsError())
        return false;
    return true;
}

bool AppendFile(tukk file, tukk str) {return FileStrAppend(file, str);}

Txt FormatLong(long a) {
    return Sprintf("%ld", a);
}

Txt Replace(Txt str, Txt find, Txt replace) {
    Txt ret;

    i32 lenStr = str.GetCount();
    i32 lenFind = find.GetCount();
    i32 i = 0, j;
    while ((j = str.Find(find, i)) >= i) {
        ret += str.Mid(i, j-i) + replace;
        i = j + lenFind;
        if (i >= lenStr)
            break;
    }
    ret += str.Mid(i);
    return ret;
}

Txt Replace(Txt str, char find, char replace) {
    TxtBuf ret(str);
    for (i32 i = 0; i < ret.GetCount(); ++i) {
        if (ret[i] == find)
            ret[i] = replace;
    }
        return static_cast<Txt>(ret);
}

// Rename file or folder
/*
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
*/
bool FileDeleteX(tukk path, EXT_FILE_FLAGS flags) {
    if (flags & USE_TRASH_BIN)
        return FileToTrashBin(path);
    else {
        if (flags & DELETE_READ_ONLY)
            SetReadOnly(path, false, false, false);
        return FileDelete(path);
    }
}

bool FolderDeleteX(tukk path, EXT_FILE_FLAGS flags) {
    if (flags & USE_TRASH_BIN)
        return FileToTrashBin(path);
    else {
        if (flags & DELETE_READ_ONLY)
            SetReadOnly(path, false, false, false);
        return DirDelete(path);
    }
}

bool DirCreateX(tukk path) {
    DirCreate(path);
    return DirExists(path);
}

bool DirExistsX_Each(tukk name) {
#if defined(TARGET_WIN32)
    if(name[0] && name[1] == ':' && name[2] == '\\' && name[3] == 0 &&
       GetDriveType(name) != DRIVE_NO_ROOT_DIR)
        return true;
    DWORD res = GetFileAttributes(ToSystemCharset(name));
    if (!(res & FILE_ATTRIBUTE_DIRECTORY))
        return false;
    if (res != INVALID_FILE_ATTRIBUTES)
        return true;
    if (!(name[0] && name[1] == ':'))
        return false;
    if (!(ERROR_PATH_NOT_FOUND == GetLastError()))
        return false;

    Txt localName = Txt(name, 2);
    char remoteName[256];
    DWORD lenRemoteName = sizeof(remoteName);
    res = WNetGetConnection(localName, remoteName, &lenRemoteName);
    if (res != ERROR_CONNECTION_UNAVAIL)
        return false;

    NETRESOURCE nr;
    memset(&nr, 0, sizeof(NETRESOURCE));
    nr.dwType = RESOURCETYPE_DISK;
    nr.lpLocalName = const_cast<tuk >(localName.Begin());
    nr.lpRemoteName = remoteName;
    nr.lpProvider = NULL;
    DWORD dwFlags = CONNECT_UPDATE_PROFILE;
    res = WNetAddConnection2(&nr, NULL, NULL, dwFlags);
    if (res != NO_ERROR)
        return false;

    res = GetFileAttributes(ToSystemCharset(name));
    return (res != INVALID_FILE_ATTRIBUTES &&
           (res & FILE_ATTRIBUTE_DIRECTORY));

#else
    FindFile ff(name);
    return ff && ff.IsDir();
#endif
}

bool DirExistsX(tukk path, EXT_FILE_FLAGS flags) {
    Txt spath = path;
    if (spath.EndsWith(DIR_SEPS))
        spath = spath.Left(spath.GetCount() - 1);
    if (!(flags & BROWSE_LINKS))
        return DirExistsX_Each(spath);
    if (DirExistsX_Each(spath))
        return true;
    if (!IsSymLink(spath))
        return false;
    Txt filePath;
    filePath = GetSymLinkPath(spath);
    if (filePath.IsEmpty())
        return false;
    return DirExistsX_Each(filePath);
}

bool IsFile(tukk fileName) {
    FindFile ff;
    if(ff.Search(fileName) && ff.IsFile())
        return true;
    return false;
}

bool IsFolder(tukk fileName) {
    FindFile ff;
    if(ff.Search(fileName) && ff.IsFolder())
        return true;
    return false;
}

Txt GetRelativePath(Txt from, Txt path, bool normalize) {
    Txt ret, dir_seps;
    Txt creplace = DIR_SEP == '\\' ? "/" : "\\";
    if (normalize) {
        from.Replace(creplace, DIR_SEPS);
        path.Replace(creplace, DIR_SEPS);
        if (!TARGET_PATH_HAS_CASE) {
            from = ToLower(from);
            path = ToLower(path);
        }
        dir_seps = DIR_SEPS;
    } else {
        bool seplinux = from.Find('/') >= 0 || path.Find('/') >= 0;
        bool sepwindows = from.Find('\\') >= 0 || path.Find('\\') >= 0;
        if (seplinux && sepwindows) {
            dir_seps = DIR_SEPS;
            from.Replace(creplace, DIR_SEPS);
            from.Replace(creplace, DIR_SEPS);
        } else if (seplinux)
            dir_seps = "/";
        else
            dir_seps = "\\";
    }
    i32 pos_from = 0, pos_path = 0;
    bool first = true;
    while (!IsNull(pos_from)) {
        Txt f_from = Tokenize2(from, dir_seps, pos_from);
        Txt f_path = Tokenize2(path, dir_seps, pos_path);
        if (f_from != f_path) {
            if (first)
                return Txt::GetVoid();
            ret << f_path;
            Txt fileName = path.Mid(pos_path);
            if (!fileName.IsEmpty())
                ret << dir_seps << fileName;
            while (!IsNull(pos_from)) {
                ret.Insert(0, Txt("..") + dir_seps);
                Tokenize2(from, dir_seps, pos_from);
            }
            ret.Insert(0, Txt("..") + dir_seps);
            return ret;
        }
        first = false;
    }
    return path.Mid(pos_path);
}

Txt GetAbsolutePath(Txt from, Txt relative) {
    from = Trim(from);
    relative = Trim(relative);
    if (!relative.StartsWith("."))
        return relative;
    while (!from.IsEmpty() && !relative.IsEmpty()) {
        if (relative.StartsWith("./") || relative.StartsWith(".\\"))
            relative = relative.Mid(2);
        else if (relative.StartsWith("../") || relative.StartsWith("..\\")) {
            relative = relative.Mid(3);
            from = GetUpperFolder(from);
        } else
            break;
    }
    return AppendFNameX(from, relative);
}
/*
bool SetReadOnly(tukk path, bool readOnly) {
    return SetReadOnly(path, readOnly, readOnly, readOnly);
}

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
*/

#ifdef TARGET_POSIX

i32 GetUid() {
    Txt proc = LoadFile_Safe("/etc/passwd");
    i32 pos = proc.Find(GetUserName());
    if (pos < 0)
        return -1;
    pos = proc.Find(':', pos);
    if (pos < 0)
        return -1;
    pos = proc.Find(':', pos+1);
    if (pos < 0)
        return -1;
    i32 posend = proc.Find(':', pos+1);
    if (posend < 0)
        return -1;
    return ScanInt(proc.Mid(pos+1, posend-pos-1));
}

Txt GetMountDir(const Txt &path) {
    Vec<Txt> drives = GetDriveList();
    for (i32 i = 0; i < drives.GetCount(); ++i) {
        if (path.Find(drives[i]) == 0)
            return drives[i];
    }
    Txt localPath = AppendFNameX(GetCurDir(), path);
    if (!FileExists(localPath) && !DirExists(localPath))
        return "";
    for (i32 i = 0; i < drives.GetCount(); ++i) {
        if (localPath.Find(drives[i]) == 0)
            return drives[i];
    }
    return "";
}

Txt GetTrashBinDir()
{
    Txt ret = GetEnv("XDG_DATA_HOME");
    if (ret.IsEmpty())
        ret = AppendFNameX(GetHomeDir(), ".local/share/Trash");
    else
        ret = AppendFNameX(ret, "Trash");
    return ret;
}

bool FileToTrashBin(tukk path)
{
    Txt newPath = AppendFNameX(GetTrashBinDir(), GetFName(path));
    return FileMove(path, newPath);
}

z64 TrashBinGetCount()
{
    z64 ret = 0;
    FindFile ff;
    if(ff.Search(AppendFNameX(GetTrashBinDir(), "*"))) {
        do {
            Txt name = ff.GetName();
            if (name != "." && name != "..")
                ret++;
        } while(ff.Next());
    }
    return ret;
}

bool TrashBinClear()
{
    FindFile ff;
    Txt trashBinDir = GetTrashBinDir();
    if(ff.Search(AppendFNameX(trashBinDir, "*"))) {
        do {
            Txt name = ff.GetName();
            if (name != "." && name != "..") {
                Txt path = AppendFNameX(trashBinDir, name);
                if (ff.IsFile())
                    FileDelete(path);
                else if (ff.IsDir()) {
                    DeleteFolderDeep(path);     Sleep(100);
                }
            }
        } while(ff.Next());
    }
    return true;
}

#endif
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

bool DirMove(tukk dir, tukk newPlace) {
    if (strcmp(dir, newPlace) == 0)
        return true;

    WTxt wDir(dir), wNewPlace(newPlace);
    wDir.Cat(L'\0');
    wNewPlace.Cat(L'\0');

    SHFILEOPSTRUCTW fileOp = {};
    fileOp.hwnd = NULL;
    fileOp.wFunc = FO_MOVE;
    fileOp.pFrom = (PCZZWSTR)~wDir;
    fileOp.pTo = (PCZZWSTR)~wNewPlace;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

    i32 ret = SHFileOperationW(&fileOp);
    return ret == 0;
}

bool FileToTrashBin(tukk path) {
    if (!FileExists(path) && !DirExists(path))
        return false;

    WTxt wpath(path);
    wpath.Cat(L'\0');

    SHFILEOPSTRUCTW fileOp = {};
    fileOp.hwnd = NULL;
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = (PCZZWSTR)~wpath;
    fileOp.pTo = NULL;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SILENT;

    i32 ret = SHFileOperationW(&fileOp);
    return ret == 0;
}

z64 TrashBinGetCount() {
    SHQUERYRBINFO shqbi;

    shqbi.cbSize = sizeof(SHQUERYRBINFO);
    if (S_OK != SHQueryRecycleBin(0, &shqbi))
        return -1;
    return shqbi.i64NumItems;
}

bool TrashBinClear() {
    if (S_OK != SHEmptyRecycleBin(0, 0, SHERB_NOCONFIRMATION | SHERB_NOPROGRESSUI | SHERB_NOSOUND))
        return false;
    return true;
}

#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

Txt LoadFile_Safe(const Txt fileName)
{
#ifdef TARGET_POSIX
    i32 fid = open(fileName, O_RDONLY);
#else
    i32 fid = _wopen((const wchar_t *)fileName.ToWTxt().Begin(), O_RDONLY|O_BINARY);
#endif
    if (fid < 0)
        return Txt();
    i32k size = 1024;
    i32 nsize;
    TxtBuf s;
    char buf[size];
    while((nsize = read(fid, buf, size)) == size)
        s.Cat(buf, size);
    if (nsize > 1)
        s.Cat(buf, nsize-1);
    close(fid);
    return static_cast<Txt>(s);
}

Txt LoadFile(tukk fileName, off_t from, size_t len)
{
#ifdef TARGET_POSIX
    i32 fid = open(fileName, O_RDONLY);
#else
    i32 fid = _wopen((const wchar_t *)Txt(fileName).ToWTxt().Begin(), O_RDONLY|O_BINARY);
#endif
    if (fid < 0)
        return Txt();
    if (0 > lseek(fid, from, SEEK_SET))
        return Txt();
    size_t size = 1024;
    if (len != 0 && size > len)
        size = len;
    size_t nsize;
    TxtBuf s;
    Buffer<char> buf(size);
    size_t loaded;
    for (loaded = 0; (nsize = read(fid, buf, unsigned(size))) == size && (len == 0 || loaded < len); loaded += nsize) {
        if (len != 0 && loaded + size > len)
            size = len - loaded;
        s.Cat(buf, i32(size));
    }
    if (nsize > 1 && (len == 0 || loaded < len))
        s.Cat(buf, i32(nsize-1));
    close(fid);
    return static_cast<Txt>(s);
}

Txt GetExtExecutable(const Txt _ext)
{
    Txt ext = _ext;
    Txt exeFile = "";
    if (ext[0] != '.')
        ext = Txt(".") + ext;
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
    Txt file = AppendFNameX(GetHomeDir(), Txt("dummy") + ext); // Required by FindExecutableW
    SaveFile(file, "   ");
    if (!FileExists(file))
        return "";
    WCHAR exe[MAX_PATH];
    HINSTANCE ret = FindExecutableW(ToSystemCharsetW(file), NULL, exe);
    if (reinterpret_cast<zu64>(ret) > 32)
        exeFile = WTxt(exe).ToTxt();
    DeleteFile(file);
#endif
#ifdef TARGET_POSIX
    TxtParse mime;
    //if (LaunchCommand(Txt("xdg-mime query filetype ") + file, mime) >= 0)  // xdg-mime query filetype does not work properly in Enlightenment
    mime = LoadFile_Safe("/etc/mime.types");    // Search in /etc/mime.types the mime type for the extension
    if ((mime.GoAfter_Init(Txt(" ") + ext.Right(ext.GetCount()-1))) || (mime.GoAfter_Init(Txt("\t") + ext.Right(ext.GetCount()-1)))) {
        mime.GoBeginLine();
        mime = mime.GetText();
        exeFile = TrimRight(Sys(Txt("xdg-mime query default ") + mime));
    }
#endif
    return exeFile;
}

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
Vec<Txt> GetDriveList() {
    char drvStr[26*4+1];        // A, B, C, ...
    Vec<Txt> ret;

    i32 lenDrvStrs = ::GetLogicalDriveTxts(sizeof(drvStr), drvStr);
    // To get the error call GetLastError()
    if (lenDrvStrs == 0)
        return ret;

    ret.Add(drvStr);
    for (i32 i = 0; i < lenDrvStrs-1; ++i) {
        if (drvStr[i] == '\0')
            ret.Add(drvStr + i + 1);
    }
    return ret;
}
#endif
#if defined(TARGET_POSIX)
Vec<Txt> GetDriveList() {
    Vec<Txt> ret;
    // Search for mountable file systems
    Txt mountableFS = "cofs.";
    TxtParse sfileSystems(LoadFile_Safe("/proc/filesystems"));
    Txt str;
    while (true) {
        str = sfileSystems.GetText();
        if (str == "")
            break;
        else if (str != "nodev")
            mountableFS << str << ".";
        else
            str = sfileSystems.GetText();
    }
    // Get mounted drives
    TxtParse smounts(LoadFile_Safe("/proc/mounts"));
    TxtParse smountLine(Trim(smounts.GetText("\r\n")));
    do {
        Txt devPath   = smountLine.GetText();
        Txt mountPath = smountLine.GetText();
        Txt fs        = smountLine.GetText();
        if ((mountableFS.Find(fs) >= 0) && (mountPath.Find("/dev") < 0)
         && (mountPath.Find("/rofs") < 0) && (mountPath != "/"))    // Is mountable
            ret.Add(mountPath);
        smountLine = Trim(smounts.GetText("\r\n"));
    } while (smountLine != "");
    ret.Add("/");   // Last but not least
    return ret;
}
#endif


#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
Txt GetShellFolder2(i32 clsid)
{
    wchar path[MAX_PATH];
    if(SHGetFolderPathW(NULL, clsid, NULL, //SHGFP_TYPE_CURRENT
                                            0, (LPWSTR)path) == S_OK)
        return FromUnicodeBuffer(path);
    return Null;
}

Txt GetShellFolder2(tukk local, tukk users)
{
    Txt ret = FromSystemCharset(GetWinRegTxt(local, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                                       HKEY_CURRENT_USER));
    if (ret == "" && *users != '\0')
        return FromSystemCharset(GetWinRegTxt(users, "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",
                                       HKEY_LOCAL_MACHINE));
    return ret;
}

Txt GetPersonalFolder()  {return GetShellFolder2("Personal", 0);}
Txt GetStartupFolder()   {return GetShellFolder2(CSIDL_STARTUP);}

Txt GetTempFolder()
{
    Txt ret;
    if ((ret = GetEnv("TEMP")) == "")   // One or the other one
        ret = GetEnv("TMP");
    return ret;
}

Txt GetOsFolder()
{
    char ret[MAX_PATH];
    ::GetWindowsDir(ret, MAX_PATH);
    return Txt(ret);
}
Txt GetSystemFolder()
{
    char ret[MAX_PATH];
    ::GetSystemDir(ret, MAX_PATH);
    return Txt(ret);
}

#ifdef TARGET_WIN32
Txt GetCommonAppDataFolder() {
    wchar path[MAX_PATH];
    if(SHGetFolderPathW(NULL, CSIDL_COMMON_APPDATA, NULL, 0, (LPWSTR)path) == S_OK)
        return FromUnicodeBuffer(path);
    return Null;
}
#endif
/*
bool SetEnv(tukk id, tukk val)
{
//  EnvMap().Put(WTxt(id), WTxt(val));
#ifdef TARGET_POSIX
    return setenv(id, val, 1) == 0;
#else
    WTxt str = WTxt(id) + "=" + WTxt(val);
    return _wputenv((const wchar_t *)str.Begin()) == 0;
#endif
}
*/

#endif
#ifdef TARGET_POSIX

Txt GetPathXdg2(Txt xdgConfigHome, Txt xdgConfigDirs)
{
    Txt ret;
    if (FileExists(ret = AppendFNameX(xdgConfigHome, "user-dirs.dirs")))
        ;
    else if (FileExists(ret = AppendFNameX(xdgConfigDirs, "user-dirs.defaults")))
        ;
    else if (FileExists(ret = AppendFNameX(xdgConfigDirs, "user-dirs.dirs")))
        ;
    return ret;
}
Txt GetPathDataXdg2(Txt fileConfig, tukk folder)
{
    TxtParse fileData = LoadFile(fileConfig);

    if (!fileData.GoAfter(folder)) return "";
    if (!fileData.GoAfter("=")) return "";

    Txt ret = "";
    TxtParse path = fileData.GetText();
    if(path.GoAfter("$HOME/"))
        ret = AppendFNameX(GetHomeDir(), path.Right());
    else if (!FileExists(path))
        ret = AppendFNameX(GetHomeDir(), path);

    return ret;
}
Txt GetShellFolder2(tukk local, tukk users)
{
    Txt xdgConfigHome = GetEnv("XDG_CONFIG_HOME");
    if (xdgConfigHome == "")        // By default
        xdgConfigHome = AppendFNameX(GetHomeDir(), ".config");
    Txt xdgConfigDirs = GetEnv("XDG_CONFIG_DIRS");
    if (xdgConfigDirs == "")            // By default
        xdgConfigDirs = "/etc/xdg";
    Txt xdgFileConfigData = GetPathXdg2(xdgConfigHome, xdgConfigDirs);
    Txt ret = GetPathDataXdg2(xdgFileConfigData, local);
    if (ret == "" && *users != '\0')
        return GetPathDataXdg2(xdgFileConfigData, users);
    else
        return ret;
}

Txt GetTempFolder()
{
    return GetHomeDir();
}
Txt GetOsFolder()
{
    return Txt("/bin");
}
Txt GetSystemFolder()
{
    return Txt("");
}

Txt GetPersonalFolder()  {return GetShellFolder2("XDG_DOCUMENTS_DIR", "DOCUMENTS");}

#endif

struct TxtNormalCompare__ {
    i32 operator()(char a, char b) const { return a - b; }
};

i32 Compare(const Txt& a, i32 i0, const Txt& b, i32 len) {
    return IterCompare(a.Begin() + i0, a.Begin() + i0 + len, b.Begin(), b.Begin() + len, TxtNormalCompare__());
}

i32 ReverseFind(const Txt& s, const Txt& toFind, i32 from) {
    ASSERT(from >= 0 && from <= s.GetLength());
    i32 lc = toFind.GetLength();
    for (i32 i = from; i >= 0; --i) {
        if (Compare(s, i, toFind, lc) == 0)
            return i;
    }
    return -1;
}

Time StrToTime(tukk s) {
    Time ret;
    if (StrToTime(ret, s))
        return ret;
    else
        return Null;
}

Date StrToDate(tukk s) {
    Time ret;
    if (StrToDate(ret, s))
        return ret;
    else
        return Null;
}

void TxtToHMS(Txt durat, i32 &hour, i32 &min, double &seconds) {
    TxtParse duration(durat);
    Txt s1, s2, s3;
    s1 = duration.GetText(":");
    s2 = duration.GetText(":");
    s3 = duration.GetText();

    if (s3 != "") {
        hour = ScanInt(s1);
        min = ScanInt(s2);
        seconds = ScanDouble(s3);
    } else if (s2 != "") {
        hour = 0;
        min = ScanInt(s1);
        seconds = ScanDouble(s2);
    } else {
        hour = 0;
        min = 0;
        seconds = ScanDouble(s1);
    }
    if (IsNull(hour) || IsNull(min) || IsNull(seconds)) {
        hour = min = Null;
        seconds = Null;
    } else if (hour < 0 || min < 0 || seconds < 0) {
        hour = Neg(hour);
        min = Neg(min);
        seconds = Neg(seconds);
    }
}
double TxtToSeconds(Txt duration) {
    i32 hour, min;
    double secs;
    TxtToHMS(duration, hour, min, secs);
    return 3600.*hour + 60.*min + secs;
}

Txt formatSeconds(double seconds, i32 dec, bool fill) {
    i32 iseconds = i32(seconds);
    Txt ret;
    if (fill)
        ret = FormatIntDec(iseconds, 2, '0');
    else
        ret = FormatInt(iseconds);
    double decs = seconds - iseconds;
    if (decs > 0 && dec > 0)
        ret << "." << FormatIntDec(static_cast<i32>(decs*pow(10, dec)), dec, '0');
    return ret;
}
/*
Txt HMSToTxt0(i32 hour, i32 min, double seconds, bool units, i32 dec) {
    Txt sunits;
    if (units) {
        if (hour >= 1)
            sunits = t_("hours");
        else if (min >= 2)
            sunits = t_("mins");
        else if (min == 1)
            sunits = t_("min");
        else if (seconds > 1)
            sunits = t_("secs");
        else
            sunits = t_("sec");
    }
    Txt ret;
    if (hour > 0)
        ret << hour << ":";
    if (min > 0 || hour > 0)
        ret << (ret.IsEmpty() ? FormatInt(min) : FormatIntDec(min, 2, '0')) << ":";

    ret << formatSeconds(seconds, dec, min > 0 || hour > 0);
    if (units)
        ret << " " << sunits;
    return ret;
}*/

Txt HMSToTxt(i32 hour, i32 min, double seconds, i32 dec, bool units, bool space, bool tp,
                    bool longUnits, bool forcesec) {
    Txt ret;
    bool isneg = hour < 0 || min < 0 || seconds < 0;

    if (hour > 0) {
        ret << hour;
        if (space)
            ret << " ";
        if (tp)
            ret << ":";
        if (units)
            ret << (longUnits ? ((hour > 1) ? t_("часов") : t_("час")) : t_("ч"));
    }

    if (min > 0) {
        if (tp) {
            Txt fmt = hour > 0 ? "%02d" : "%d";
            ret << Format(fmt, min);
        } else
            ret << (ret.IsEmpty() ? "" : " ") << min;
        if (space)
            ret << " ";
        if (tp && forcesec)
            ret << ":";
        if (units)
            ret << (longUnits ? ((min > 1) ? t_("минут") : t_("мин")) : t_("м"));
    } else if (tp) {
        if (hour > 0) {
            ret << "00";
            if (forcesec)
                ret << ":";
        }
    }

    if (forcesec || (hour == 0 && (seconds > 1 || (seconds > 0 && dec > 0)))) {
        ret << (ret.IsEmpty() || tp ? "" : " ") << formatSeconds(seconds, dec, tp && min > 0);
        if (space)
            ret << " ";
        if (units)
            ret << (longUnits ? ((seconds > 1) ? t_("секунд") : t_("сек")) : t_("с"));
    }

    if (isneg)
        ret = "-" + ret;
    return ret;
}

Txt SecondsToTxt(double seconds, i32 dec, bool units, bool space, bool tp,
                    bool longUnits, bool forcesec) {
    i32 hour, min;
    hour = static_cast<i32>(seconds/3600.);
    seconds -= hour*3600;
    min = static_cast<i32>(seconds/60.);
    seconds -= min*60;
    return HMSToTxt(hour, min, seconds, dec, units, space, tp, longUnits, forcesec);
}

Txt SeasonName(i32 iseason) {
    static tukk season[] = {t_("зима"), t_("весна"), t_("лето"), t_("осень")};
    return iseason >= 0 && iseason < 4 ? season[iseason] : "";
}

i32 GetSeason(const Date &date) {
    return i32((date.month - 1)/3.);
}

Txt BytesToTxt(zu64 _bytes, bool units)
{
    Txt ret;
    zu64 bytes = _bytes;

    if (bytes >= 1024) {
        bytes /= 1024;
        if (bytes >= 1024) {
            bytes /= 1024;
            if (bytes >= 1024) {
                bytes /= 1024;
                if (bytes >= 1024) {
                    //bytes /= 1024;
                    ret = Format("%.1f %s", _bytes/(1024*1024*1024*1024.), units ? "Tb" : "");
                } else
                    ret = Format("%.1f %s", _bytes/(1024*1024*1024.), units ? "Gb" : "");
            } else
                ret = Format("%.1f %s", _bytes/(1024*1024.), units ? "Mb" : "");
        } else
            ret = Format("%.1f %s", _bytes/1024., units ? "Kb" : "");
    } else
        ret << _bytes << (units ? "b" : "");
    return ret;
}

i32 NumAdvicedDigits(double d, double range) {
    if      (0.001 <= range && range < 0.01)    return 7;
    else if (0.01  <= range && range < 0.1)     return 6;
    else if (0.1   <= range && range < 10)      return 5;
    else if (10    <= range && range < 100)     return 5;
    else if (100   <= range && range < 10000)   return 6;
    else if (10000 <= range && range < 100000)  return 7;
    else return 6;
}

Txt FormatDoubleAutosize(double d) {
    return FormatDoubleAutosize(d, d);
}

Txt FormatDoubleAutosize(double d, double range) {
    return FDS(d, NumAdvicedDigits(d, range));
}

/*
Txt FormatDoubleSize(double d, i32 fieldWidth, bool fillSpaces) {
    Txt format, data;
    for (i32 res = 0; fieldWidth > res; res++) {
        format = S("%.") + FormatInt(fieldWidth-res) + S("G");
        data = Format(format, d);
        if (data.GetCount() <= fieldWidth) {
            if (fillSpaces)
                data = Txt(' ', fieldWidth - data.GetCount()) + data;
            break;
        }
    }
    return data;
}*/

inline bool IsNum_(double n) {return drx::IsFin(n) && !IsNull(n);}

Txt FormatDoubleSize(double d, i32 fieldWidth, bool fillSpaces) {
    Txt data;
    if (!IsNum_(d)) {
        if (fillSpaces)
            data = Txt(' ', fieldWidth - 3);
        data.Cat("nan");
    } else {
        i32 actualWidth = fieldWidth;
        data = FormatDouble(d, fieldWidth, FD_CAP_E|FD_SPECIAL|FD_MINIMAL_EXP);
        while (data.GetCount() > fieldWidth && actualWidth >= 3) {
            actualWidth--;
            data = FormatDouble(d, actualWidth, FD_CAP_E|FD_SPECIAL|FD_MINIMAL_EXP);
        }
        if (fillSpaces && data.GetCount() <= fieldWidth)
            data = Txt(' ', fieldWidth - data.GetCount()) + data;
    }
    return data;
}

Txt RemoveAccent(wchar c) {
    WTxt wsret;

    if (IsAlNum(c) || IsSpace(c)) {
        wsret.Cat(c);
        return wsret.ToTxt();
    }
    //const WTxt accented = "ÂÃÀÁÇÈÉÊËẼÌÍÎÏÑÒÓÔÕÙÚÛÝàáâãçèéêëẽìíîïñòóôõøùúûýÿ";
    const WTxt accented = "\303\202\303\203\303\200\303\201\303\207\303\210\303\211\303\212\303\213\341\272\274\303\214\303\215\303\216\303\217\303\221\303\222\303\223\303\224\303\225\303\231\303\232\303\233\303\235\303\240\303\241\303\242\303\243\303\247\303\250\303\251\303\252\303\253\341\272\275\303\254\303\255\303\256\303\257\303\261\303\262\303\263\303\264\303\265\303\270\303\271\303\272\303\273\303\275\303\277ω";
    tukk unaccented = "AAAACEEEEEIIIINOOOOUUUYaaaaceeeeeiiiinooooouuuyyw";

    for (i32 i = 0; accented[i]; ++i) {
        if (accented[i] == c) {
            wsret.Cat(unaccented[i]);
            return wsret.ToTxt();
        }
    }
    //const WTxt maccented = "ÅåÆæØøþÞßÐðÄäÖöÜü";
    const WTxt maccented = "\303\205\303\245\303\206\303\246\303\230\303\270\303\276\303\236\303\237\303\220\303\260\303\204\303\244\303\226\303\266\303\234\303\274∞";
    tukk unmaccented[] = {"AA", "aa", "AE", "ae", "OE", "oe", "TH", "th", "SS", "ETH",
                                 "eth", "AE", "ae", "OE", "oe", "UE", "ue", "inf"};
    for (i32 i = 0; maccented[i]; ++i) {
        if (maccented[i] == c)
            return unmaccented[i];
    }
    wsret.Cat(c);
    return wsret.ToTxt();
}

bool IsPunctuation(wchar c) {
    //const WTxt punct = "\"’'()[]{}<>:;,‒–—―….,¡!¿?«»-‐‘’“”/\\&@*\\•^©¤฿¢$€ƒ£₦¥₩₪†‡〃#№ºª\%‰‱ ¶′®§℠℗~™|¦=";
    const WTxt punct = "\"\342\200\231'()[]{}<>:;,\342\200\222\342\200\223\342\200\224\342\200\225\342\200\246.,\302\241!\302\277?\302\253\302\273-\342\200\220\342\200\230\342\200\231\342\200\234\342\200\235/\\&@*\\\342\200\242^\302\251\302\244\340\270\277\302\242$\342\202\254\306\222\302\243\342\202\246\302\245\342\202\251\342\202\252\342\200\240\342\200\241\343\200\203#\342\204\226\302\272\302\252%\342\200\260\342\200\261∞"
                          "\302\266\342\200\262\302\256\302\247\342\204\240\342\204\227~\342\204\242|\302\246=";
    for (i32 i = 0; punct[i]; ++i) {
        if (punct[i] == c)
            return true;
    }
    return false;
}

Txt RemoveAccents(Txt str) {
    Txt ret;
    WTxt wstr = str.ToWTxt();
    for (i32 i = 0; i < wstr.GetCount(); ++i) {
        Txt schar = RemoveAccent(wstr[i]);
        if (i == 0 || ((i > 0) && ((IsSpace(wstr[i-1]) || IsPunctuation(wstr[i-1]))))) {
            if (schar.GetCount() > 1) {
                if (IsUpper(schar[0]) && IsLower(wstr[1]))
                    schar = Txt(schar[0], 1) + ToLower(schar.Mid(1));
            }
        }
        ret += schar;
    }
    return ret;
}

Txt RemovePunctuation(Txt str) {
    Txt ret;
    WTxt wstr = str.ToWTxt();
    for (i32 i = 0; i < wstr.GetCount(); ++i) {
        if (!IsPunctuation(wstr[i]))
            ret += wstr[i];
    }
    return ret;
}

Txt FitFName(const Txt fileName, i32 len) {
    if (fileName.GetCount() <= len)
        return fileName;

    Vec<Txt> path;

    tukk s = fileName;
    char c;
    i32 pos0 = 0;
    for (i32 pos1 = 0; (c = s[pos1]) != '\0'; ++pos1) {
    #if defined(TARGET_WIN32) || defined (TARGET_WIN64)
        if(c == '\\' || c == '/') {
    #else
        if(c == '/') {
    #endif
            path.Add(fileName.Mid(pos0, pos1-pos0));
            pos0 = ++pos1;
        }
    }
    path.Add(fileName.Mid(pos0));

    Txt begin, end;
    i32 iBegin = 0;
    i32 iEnd = path.GetCount() - 1;

    if (path[iEnd].GetCount() >= len)
        return path[iEnd].Left(len);
    if (path[iEnd].GetCount() >= len-4)
        return path[iEnd];

    len -= 3;   // ...

    for (; iBegin <= iEnd; iBegin++, iEnd--) {
        if (path[iEnd].GetCount() + 1 > len)
            break;
        end = DIR_SEPS + path[iEnd] + end;
        len -= path[iEnd].GetCount() + 1;
        if (path[iBegin].GetCount() + 1 > len)
            break;
        begin += path[iBegin] + DIR_SEPS;
        len -= path[iBegin].GetCount() + 1;
    }
    return begin + "..." + end;
}

Txt Tokenize2(const Txt &str, const Txt &token, i32 &pos) {
    if (IsNull(pos) || pos >= str.GetCount()) {
        pos = Null;
        return Null;
    }
    i32 npos = str.Find(token, pos);
/*  for (i32 i = 0; i < token.GetCount(); ++i) {
        if ((npos = str.Find(token[i], pos)) >= 0)
            break;
    }*/
    i32 oldpos = pos;
    if (npos < 0) {
        pos = Null;
        return str.Mid(oldpos);
    } else {
        pos = npos + token.GetCount();
        return str.Mid(oldpos, npos - oldpos);
    }
}

Txt Tokenize2(const Txt &str, const Txt &token) {
    i32 dummy = 0;
    return Tokenize2(str, token, dummy);
}

Vec<Txt> Tokenize(const Txt &str, const Txt &token, i32 pos) {
    Vec<Txt> ret;

    Tokenize(str, token, ret, pos);

    return ret;
}

void Tokenize(const Txt &str, const Txt &token, Vec<Txt> &ret, i32 pos) {
    i32 _pos = pos;
    while (true) {
        Txt strRet = Tokenize2(str, token, _pos);
        if (IsNull(_pos)) {
            if (!IsNull(strRet))
                ret << strRet;
            break;
        }
        ret << strRet;
    }
}

Txt GetLine(const Txt &str, i32 &pos) {
    Txt ret;
    if (pos >= str.GetCount())
        return Txt::GetVoid();
    i32 npos = str.Find("\n", pos);
    if (npos == -1) {
        ret = str.Mid(pos);
        pos = -1;
    } else {
        ret = str.Mid(pos, npos - pos);
        pos = npos + 1;
    }
    return TrimBoth(ret);
}

Val GetField(const Txt &str, i32 &pos, char separator, char decimalSign, bool onlyTxts) {
    ASSERT(separator != '\"');
    Txt sret;
    i32 npos = str.Find(separator, pos);
    i32 spos = str.Find('\"', pos);
    if (spos < 0 || spos > npos) {
        if (npos < 0) {
            i32 lspos = str.Find('\"', spos + 1);
            if (lspos < 0)
                sret = str.Mid(max(pos, spos));
            else
                sret = str.Mid(spos + 1, lspos - spos - 1);
            pos = -1;
        } else {
            sret = Trim(str.Mid(pos, npos - pos));
            pos = npos + 1;
        }
    } else {
        i32 lspos = str.Find('\"', spos + 1);
        if (lspos < 0) {
            sret = str.Mid(spos);
            pos = -1;
        } else {
            sret = str.Mid(spos + 1, lspos - spos - 1);
            npos = str.Find(separator, lspos);
            pos = npos + 1;
        }
    }
    if (onlyTxts)
        return sret;

    if (sret.IsEmpty())
        return Null;

    bool hasDecimal = false, hasLetter = false;
    for (i32 i = 0; i < sret.GetCount(); ++i) {
        if (sret[i] == decimalSign)
            hasDecimal = true;
        else if (!IsNumber(sret[i]))
            hasLetter = true;
    }
    if (!hasLetter) {
        if (hasDecimal) {
            double dbl = ScanDouble(sret, NULL, decimalSign == ',');
            if (IsNull(dbl))
                return sret;
            else
                return dbl;
        } else {
            z64 it64 = ScanInt64(sret);
            if (IsNull(it64))
                return sret;
            i32 it = i32(it64);
            if (it64 != it)
                return it64;
            else
                return it;
        }
    } else {
        Time t = ScanTime(sret);
        if (IsNull(t))
            return sret;
        else if (t.hour == 0 && t.minute == 0 && t.second == 0)
            return Date(t);
        else
            return t;
    }
}

Vec<Vec <Val> > ReadCSV(const Txt &strFile, char separator, bool bycols, bool removeRepeated, char decimalSign, bool onlyTxts, i32 fromRow) {
    Vec<Vec<Val> > result;

    if (strFile.IsEmpty())
        return result;

    i32 posLine = 0;
    for (i32 i = 0; i < fromRow; ++i)
        GetLine(strFile, posLine);

    Txt line;
    i32 pos = 0;
    if (bycols) {
        line = GetLine(strFile, posLine);
        while (pos >= 0) {
            Val name = GetField(line, pos, separator, decimalSign, onlyTxts);
            if (/*pos >= 0 && */!IsNull(name)) {
                Vec<Val> &data = result.Add();
                data.Add(name);
            }
        }
        while (posLine >= 0) {
            pos = 0;
            line = GetLine(strFile, posLine);
            if (!line.IsEmpty()) {
                bool repeated = removeRepeated;
                i32 row = result[0].GetCount() - 1;
                for (i32 col = 0; col < result.GetCount(); col++) {
                    if (pos >= 0) {
                        Val data = GetField(line, pos, separator, decimalSign, onlyTxts);
                        result[col].Add(data);
                        if (row > 0 && result[col][row] != data)
                            repeated = false;
                    } else
                        result[col].Add();
                }
                if (row > 0 && repeated) {
                    for (i32 col = 0; col < result.GetCount(); col++)
                        result[col].Remove(row+1);
                }
            } else
                break;
        }
    } else {
        i32 row = 0;
        while (posLine >= 0) {
            pos = 0;
            line = GetLine(strFile, posLine);
            bool repeated = removeRepeated;
            if (!line.IsEmpty()) {
                Vec <Val> &linedata = result.Add();
                i32 col = 0;
                while (pos >= 0) {
                    Val val = GetField(line, pos, separator, decimalSign, onlyTxts);
                    if (val.IsNull())
                        linedata << "";
                    else
                        linedata << val;
                    if (row > 0 && (result[row - 1].GetCount() <= col || result[row - 1][col] != val))
                        repeated = false;
                    col++;
                }
            } else
                break;
            if (row > 0 && repeated)
                result.Remove(row);
            else
                row++;
        }
    }
    return result;
}

Vec<Vec <Val> > ReadCSVFile(const Txt &fileName, char separator, bool bycols, bool removeRepeated, char decimalSign, bool onlyTxts, i32 fromRow) {
    return ReadCSV(LoadFile(fileName), separator, bycols, removeRepeated,  decimalSign, onlyTxts, fromRow);
}

bool ReadCSVFileByLine(const Txt &fileName, Gate<i32, Vec<Val>&, Txt &> WhenRow, char separator, char decimalSign, bool onlyTxts, i32 fromRow) {
    Vec<Val> result;

    FindFile ff(fileName);
    if(!ff || !ff.IsFile())
        return false;

    FileIn in(fileName);
    in.ClearError();

    for (i32 i = 0; i < fromRow; ++i)
        in.GetLine();

    for (i32 row = 0; true; row++) {
        Txt line = in.GetLine();
        if (line.IsVoid()) {
            WhenRow(row, result, line);
            return true;
        }
        i32 pos = 0;
        while (pos >= 0) {
            Val val = GetField(line, pos, separator, decimalSign, onlyTxts);
            if (val.IsNull())
                result << "";
            else
                result << val;
        }
        if (!WhenRow(row, result, line))
            return false;
        result.Clear();
    }
    return false;
}

Txt ToTxtDecimalSign(Val &val, const Txt &decimalSign) {
    Txt ret = val.ToTxt();
    if (val.Is<double>() && decimalSign != ".")
        ret.Replace(".", decimalSign);
    return ret;
}

bool GuessCSV(const Txt &fileName, bool onlyNumbers, Txt &header, Vec<Txt> &parameters, char &separator, bool &repetition, char &decimalSign, z64 &beginData, i32 &beginDataRow) {
    const Array<char> separators = {',', ';', '\t', '|', '%'};

    i32 numLinesToDiscard = 10, numLinesToCheck = 5;

    FileIn in(fileName);
    if (!in)
        return false;

    // Get all lines and its positions
    Vec<Txt> lines;
    Vec<z64> linesPos;
    linesPos << 0;
    for (i32 i = 0; i < numLinesToDiscard+numLinesToCheck && !in.IsEof(); ++i) {
        lines << in.GetLine();
        linesPos << (in.GetPos()+1);
    }
    // Re adjust check window if the file is small
    if (lines.size() < numLinesToDiscard + numLinesToCheck) {
        numLinesToCheck = (numLinesToCheck*lines.size())/(numLinesToDiscard + numLinesToCheck);
        numLinesToDiscard = lines.size() - numLinesToCheck;
    }

    auto NumNum = [](const Vec<Txt> &a, char decimal)->i32 {  // Number of real numbers un a vector of strings
        tukk endptr;
        i32 num = 0;
        for (i32 i = 0; i < a.size(); ++i) {
            if (a[i].Find(decimal == '.' ? ',' : '.') < 0) {        // If ',' is the decimal, '.' is not allowed in a number, and the opposite
                if (!IsNull(ScanDouble(a[i], &endptr, decimal == ',')))
                    num++;
            }
        }
        return num;
    };

    auto CompareVector = [](const Vec<i32> &a)->i32 {            // Checks if all the values are the same, and returns it
        i32 n = a[0];

        for (i32 i = 1; i < a.size(); ++i) {
            if (n != a[i])
                return -1;
        }
        return n;
    };

    auto CompareVectors = [](const Vec<i32> &a, const Vec<i32> &b)->i32 { // Checks if all the values in 2 vectors are the same, and returns it
        if (a.size() != b.size())
            return -1;
        i32 n = a[0];
        if (n != b[0])
            return -1;

        for (i32 i = 1; i < a.size(); ++i) {
            if (n != a[i] || n != b[i])
                return -1;
        }
        return n;
    };

    // Gets the separator and decimal
    Vec<char> decimals = {'.', ','};
    Vec<bool> sepRepetition = {false, true};
    i32 numBest = -1;
    for (i32 irep = 0; irep < sepRepetition.size(); ++irep) {
        for (i32 idec = 0; idec < decimals.size(); ++idec) {
            for (i32 ih = 0; ih < separators.size(); ++ih) {
                Vec<i32> numF, numNum;
                for (i32 row = 0; row < numLinesToCheck; ++row) {
                    Vec<Txt> data = Split(lines[row+numLinesToDiscard], separators[ih], sepRepetition[irep]);
                    numF << data.size();
                    if (onlyNumbers)
                        numNum << NumNum(data, decimals[idec]);
                }
                i32 num;
                if (onlyNumbers)
                    num = CompareVectors(numF, numNum);
                else
                    num = CompareVector(numF);

                if (num > numBest) {
                    separator = separators[ih];
                    decimalSign = decimals[idec];
                    repetition = sepRepetition[irep];
                    numBest = num;
                }
            }
        }
    }
    if (numBest < 0)
        return false;

    // Analyses the header
    i32 beginHeader = -1, endHeader = -1;
    for (i32 r = numLinesToDiscard-1; r >= 0; --r) {
        Vec<Txt> data = Split(lines[r], separator, repetition);
        if (data.size() != numBest)
            break;

        beginHeader = r;

        if (onlyNumbers) {
            if (NumNum(data, decimalSign) == numBest)
                endHeader = r;
        } else
            endHeader = r;
    }

    beginData = linesPos[endHeader];
    beginDataRow = endHeader;

    header = "";
    for (i32 r = 0; r < beginHeader; ++r) {
        if (r > 0)
            header << "\n";
        header << lines[r];
    }

    parameters.SetCount(numBest);
    for (i32 r = beginHeader; r < endHeader; ++r) {
        Vec<Txt> data = Split(lines[r], separator);
        for (i32 i = 0; i < numBest; ++i) {
            if (r - beginHeader > 0)
                parameters[i] << "\n";
            parameters[i] << Trim(data[i]);
        }
    }

    return true;
}

Txt WriteCSV(Vec<Vec <Val> > &data, char separator, bool bycols, char decimalSign) {
    Txt ret;

    Txt _decimalSign(decimalSign, 1);

    if (bycols) {
        i32 maxr = 0;
        for (i32 c = 0; c < data.GetCount(); ++c)
            maxr = max(maxr, data[c].GetCount());

        for (i32 r = 0; r < maxr; ++r) {
            if (r > 0)
                ret << "\n";
            for (i32 c = 0; c < data.GetCount(); ++c) {
                if (c > 0)
                    ret << separator;
                if (r >= data[c].GetCount())
                    continue;
                Txt str = ToTxtDecimalSign(data[c][r], _decimalSign);
                if (str.Find(separator) >= 0)
                    ret << '\"' << str << '\"';
                else
                    ret << str;
            }
        }
    } else {
        for (i32 r = 0; r < data.GetCount(); ++r) {
            if (r > 0)
                ret << "\n";
            for (i32 c = 0; c < data[r].GetCount(); ++c) {
                if (c > 0)
                    ret << separator;
                Txt str = ToTxtDecimalSign(data[r][c], _decimalSign);
                if (str.Find(separator) >= 0)
                    ret << '\"' << str << '\"';
                else
                    ret << str;
            }
        }
    }
    return ret;
}

bool WriteCSVFile(const Txt fileName, Vec<Vec <Val> > &data, char separator, bool bycols, char decimalSign) {
    Txt str = WriteCSV(data, separator, bycols, decimalSign);
    return SaveFile(fileName, str);
}


#ifdef TARGET_POSIX
Txt FileRealName(tukk _fileName) {
    Txt fileName = GetFullPath(_fileName);
    FindFile ff(fileName);
    if (!ff)
        return Txt("");
    else
        return fileName;
}
#endif
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
bool GetRealName_Next(Txt &real, Txt file) {
    bool ret;
    Txt old;
    i32 from = real.GetCount()+1;
    i32 to = file.Find(DIR_SEP, from);
    if (to >= 0) {
        old = file.Mid(from, to-from);
        ret = true;
    } else {
        old = file.Mid(from);
        ret = false;
    }
    real += DIR_SEP;
    FindFile ff(real + old);
    real += ff.GetName();
    return ret;
}

Txt FileRealName(tukk _fileName) {
    Txt fileName = GetFullPath(_fileName);
    i32 len = fileName.GetCount();

    if (len == 3) {
        FindFile ff(fileName + "*");
        if (!ff)
            return Txt("");
        else
            return fileName;
    }
    FindFile ff(fileName);
    if (!ff)
        return Txt("");
    Txt ret;

    ret.Reserve(len);

    if (fileName.Left(2) == "\\\\")
        return Txt("");  // Not valid for UNC paths

    ret = ToUpper(fileName.Left(1)) + ":";

    while (GetRealName_Next(ret, fileName)) ;

    return ret;
}
#endif

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

#define Ptr Ptr_
#define u8 byte_
#define CY win32_CY_

#include <winnls.h>
#include <winnetwk.h>

#include <wincon.h>
#include <shlobj.h>

#undef Ptr
#undef u8
#undef CY

#endif

bool IsSymLink(tukk path) {
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
    return GetFileExt(path) == ".lnk";
#else
    struct stat stf;
    lstat(path, &stf);
    return S_ISLNK(stf.st_mode);
#endif
}

Txt GetNextFolder(const Txt &folder, const Txt &lastFolder) {
    i32 pos = lastFolder.Find(DIR_SEP, folder.GetCount()+1);
    if (pos >= 0)
        return lastFolder.Left(pos);
    else
        return lastFolder;
}

bool IsRootFolder(tukk folderName) {
    if (!folderName)
        return false;
    if (folderName[0] == '\0')
        return false;
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
    if (strlen(folderName) == 3 && folderName[1] == ':' && folderName[2] == DIR_SEP)
#else
    if (strlen(folderName) == 1 && folderName[0] == DIR_SEP)
#endif
        return true;
    return false;
}

Txt GetUpperFolder(const Txt &folderName) {
    if (IsRootFolder(folderName))
        return folderName;
    i32 len = folderName.GetCount();
    if (len == 0)
        return Txt();
    if (folderName[len-1] == DIR_SEP)
        len--;
    i32 pos = folderName.ReverseFind(DIR_SEP, len-1);
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
    if (pos == 2)
#else
    if (pos == 0)
#endif
        pos++;
    return folderName.Left(pos);
}

bool DeleteDeepWildcardsX(tukk pathwc, bool filefolder, EXT_FILE_FLAGS flags, bool deep)
{
    return DeleteDeepWildcardsX(GetFileFolder(pathwc), GetFName(pathwc), filefolder, flags, deep);
}

bool DeleteDeepWildcardsX(tukk path, tukk namewc, bool filefolder, EXT_FILE_FLAGS flags, bool deep)
{
    FindFile ff(AppendFNameX(path, "*.*"));
    while(ff) {
        Txt name = ff.GetName();
        Txt full = AppendFNameX(path, name);
        if (PatternMatch(namewc, name)) {
            if (ff.IsFolder() && !filefolder) {
                if (!DeleteFolderDeepX(full, flags))
                    return false;
                Sleep(100);
            } else if (ff.IsFile() && filefolder) {
                if (!FileDeleteX(full, flags))
                    return false;
            }
        } else if(deep && ff.IsFolder()) {
            if (!DeleteDeepWildcardsX(full, namewc, filefolder, flags))
                return false;
        }
        ff.Next();
    }
    return true;
}

bool DeleteFolderDeepWildcardsX(tukk path, EXT_FILE_FLAGS flags)
{
    return DeleteDeepWildcardsX(path, false, flags, true);
}

bool DeleteFolderDeepWildcardsX(tukk path, tukk name, EXT_FILE_FLAGS flags)
{
    return DeleteDeepWildcardsX(path, name, false, flags, true);
}

bool DeleteFileDeepWildcardsX(tukk path, EXT_FILE_FLAGS flags)
{
    return DeleteDeepWildcardsX(path, true, flags, true);
}

bool DeleteFileWildcardsX(tukk path, EXT_FILE_FLAGS flags)
{
    return DeleteDeepWildcardsX(path, true, flags, false);
}

bool DeleteFolderDeepX_Folder(tukk dir, EXT_FILE_FLAGS flags)
{
    FindFile ff(AppendFNameX(dir, "*.*"));
    while(ff) {
        Txt name = ff.GetName();
        Txt p = AppendFNameX(dir, name);
        if(ff.IsFile())
            FileDeleteX(p, flags);
        else
        if(ff.IsFolder())
            DeleteFolderDeepX_Folder(p, flags);
        ff.Next();
    }
    return FolderDeleteX(dir, flags);
}

bool DeleteFolderDeepX(tukk path, EXT_FILE_FLAGS flags)
{
    if (flags & USE_TRASH_BIN)
        return FileToTrashBin(path);
    return DeleteFolderDeepX_Folder(path, flags);
}

bool RenameDeepWildcardsX(tukk path, tukk namewc, tukk newname, bool forfile, bool forfolder, EXT_FILE_FLAGS flags)
{
    FindFile ff(AppendFNameX(path, "*.*"));
    while(ff) {
        Txt name = ff.GetName();
        Txt full = AppendFNameX(path, name);
        if(ff.IsFolder()) {
            if (!RenameDeepWildcardsX(full, namewc, newname, forfile, forfolder, flags))
                return false;
        }
        if (PatternMatch(namewc, name)) {
            if ((ff.IsFolder() && forfolder) || (ff.IsFile() && forfile)) {
                if (!FileMoveX(full, AppendFNameX(path, newname), flags))
                    return false;
            }
        }
        ff.Next();
    }
    return true;
}

void DirCopy_Each(tukk dir, tukk newPlace, Txt relPath, bool replaceOnlyNew, Txt filesToExclude, Txt &errorList)
{
    Txt dirPath = AppendFNameX(dir, relPath);
    Txt newPath = AppendFNameX(newPlace, relPath);
    LOG(dirPath);
    LOG(newPath);
    LOG (AppendFNameX(dirPath, "*.*"));
    FindFile ff(AppendFNameX(dirPath, "*.*"));
    while(ff) {
        Txt name = ff.GetName();
        Txt newFullPath = AppendFNameX(newPath, name);
        if(ff.IsFile()) {
            bool copy = !replaceOnlyNew;
            if (replaceOnlyNew) {
                Time newPathTime = FileGetTime(newFullPath);
                if (IsNull(newPathTime) || (newPathTime < Time(ff.GetLastWriteTime())))
                    copy = true;
            }
            if (copy) {
                if (!PatternMatchMulti(filesToExclude, name)) {
                    if (!FileCopy(ff.GetPath(), newFullPath))
                        errorList << "\n" << Format(t_("Невозможно копировать '%s' в '%s': %s"), ff.GetPath(), newFullPath, GetLastErrorMessage());
                }
            }
        } else if (ff.IsFolder()) {
            if (!DirExists(newFullPath)) {
                if (!DirCreate(newFullPath))
                    errorList << "\n" << Format(t_("Невозможно создать директорию '%s': %s"), newFullPath, GetLastErrorMessage());
            }
            DirCopy_Each(dir, newPlace, AppendFNameX(relPath, name), replaceOnlyNew, filesToExclude, errorList);
        }
        ff.Next();
    }
}

void DirCopyX(tukk dir, tukk newPlace, bool replaceOnlyNew, Txt filesToExclude, Txt &errorList) {
    if (!DirExists(dir))
        errorList << "\n" << Format(t_("Директории '%s' не существует"), dir);
    else
        DirCopy_Each(dir, newPlace, "", replaceOnlyNew, filesToExclude, errorList);
}

bool FolderIsEmpty(tukk path) {
    FindFile ff(AppendFNameX(path, "*.*"));
    while(ff) {
        if(ff.IsFile() || ff.IsFolder())
            return false;
        ff.Next();
    }
    return true;
}

#if defined(__MINGW32__)
    #define _SH_DENYNO 0x40
#endif

i32 FileCompare(tukk path1, tukk path2) {
    i32 fp1;
#ifdef TARGET_POSIX
    fp1 = open(ToSystemCharset(path1), O_RDONLY, S_IWRITE|S_IREAD);
#else
    fp1 = _wsopen(ToSystemCharsetW(path1), O_RDONLY|O_BINARY, _SH_DENYNO, _S_IREAD|_S_IWRITE);
#endif
    if (fp1 == -1)
        return -2;
    i32 fp2;
#ifdef TARGET_POSIX
    fp2 = open(ToSystemCharset(path2), O_RDONLY, S_IWRITE|S_IREAD);
#else
    fp2 = _wsopen(ToSystemCharsetW(path2), O_RDONLY|O_BINARY, _SH_DENYNO, _S_IREAD|_S_IWRITE);
#endif
    if (fp2 == -1) {
        close(fp1);
        return -2;
    }
    Buffer <u8> c1(8192), c2(8192);
    i32 ret = -1;
    while (true) {
        i32 n1 = read(fp1, c1, 8192);
        i32 n2 = read(fp2, c2, 8192);
        if (n1 == -1 || n2 == -1) {
            ret = -2;
            break;
        }
        if (n1 != n2)
            break;
        if (memcmp(c1, c2, n1) != 0)
            break;
        if (n1 == 0) {
            ret = 1;
            break;
        }
    }
#ifdef TARGET_POSIX
    if (-1 == close(fp1))
        ret = -2;
    if (-1 == close(fp2))
        ret = -2;
#else
    if (-1 == _close(fp1))
        ret = -2;
    if (-1 == _close(fp2))
        ret = -2;
#endif
    return ret;
}

z64 FindTxtInFile(tukk file, const Txt text, z64 pos0) {
#ifdef TARGET_POSIX
    FILE *fp = fopen(file, "rb");
#else
    FILE *fp = _wfopen((const wchar_t *)Txt(file).ToWTxt().Begin(), L"rb");
#endif
    if (fp != NULL) {
        z64 pos = 0;
        if (pos0 > 0) {
            pos = pos0;
            if (0 == fseek(fp, long(pos0), SEEK_SET)) {
                fclose(fp);
                return -2;
            }
        }
        i32 i = 0, c;
        for (; (c = fgetc(fp)) != EOF; pos++) {
            if (c == text[i]) {
                ++i;
                if (i == text.GetCount())
                    return pos - i;
            } else {
                if (i != 0)
                    if (0 == fseek(fp, -(i-1), SEEK_CUR))
                        return -2;
                i = 0;
            }
        }
        fclose(fp);
    } else
        return -2;
    return -1;
}

bool MatchPathName(tukk name, const Vec<Txt> &cond, const Vec<Txt> &ext) {
    for (i32 i = 0; i < cond.GetCount(); ++i) {
        if(!PatternMatch(cond[i], name))
            return false;
    }
    for (i32 i = 0; i < ext.GetCount(); ++i) {
        if(PatternMatch(ext[i], name))
            return false;
    }
    return true;
}

void SearchFile_Each(Txt dir, const Vec<Txt> &condFiles, const Vec<Txt> &condFolders,
                                 const Vec<Txt> &extFiles,  const Vec<Txt> &extFolders,
                                 const Txt text, Vec<Txt> &files, Vec<Txt> &errorList) {
    FindFile ff;
    if (ff.Search(AppendFNameX(dir, "*"))) {
        do {
            if(ff.IsFile()) {
                Txt name = AppendFNameX(dir, ff.GetName());
                if (MatchPathName(ff.GetName(), condFiles, extFiles)) {
                    if (text.IsEmpty())
                        files.Add(name);
                    else {
                        switch(FindTxtInFile(name, text)) {
                        case 1: files.Add(name);
                                break;
                        case -1:errorList.Add(AppendFNameX(dir, ff.GetName()) + Txt(": ") +
                                                                    t_("невозможно открыть файл"));
                                break;
                        }
                    }
                }
            } else if(ff.IsDir()) {
                Txt folder = ff.GetName();
                if (folder != "." && folder != "..") {
                    Txt name = AppendFNameX(dir, folder);
                    if (MatchPathName(name, condFolders, extFolders))
                        SearchFile_Each(name, condFiles, condFolders, extFiles, extFolders, text, files, errorList);
                }
            }
        } while (ff.Next());
    }
}

Vec<Txt> SearchFile(Txt dir, const Vec<Txt> &condFiles, const Vec<Txt> &condFolders,
                                 const Vec<Txt> &extFiles,  const Vec<Txt> &extFolders,
                                 const Txt text, Vec<Txt> &errorList) {
    Vec<Txt> files;
    errorList.Clear();

    SearchFile_Each(dir, condFiles, condFolders, extFiles, extFolders, text, files, errorList);

    return files;
}

Vec<Txt> SearchFile(Txt dir, Txt condFile, Txt text, Vec<Txt> &errorList)
{
    Vec<Txt> condFiles, condFolders, extFiles, extFolders, files;
    errorList.Clear();

    condFiles.Add(condFile);
    SearchFile_Each(dir, condFiles, condFolders, extFiles, extFolders, text, files, errorList);

    return files;
}

Vec<Txt> SearchFile(Txt dir, Txt condFile, Txt text)
{
    Vec<Txt> errorList;
    Vec<Txt> condFiles, condFolders, extFiles, extFolders, files;

    condFiles.Add(condFile);
    SearchFile_Each(dir, condFiles, condFolders, extFiles, extFolders, text, files, errorList);

    return files;
}

bool fileDataSortAscending;
char fileDataSortBy;

FileDataArray::FileDataArray(bool use, i32 _fileFlags)
{
    Clear();
    fileDataSortAscending = true;
    fileDataSortBy = 'n';
    useId = use;
    fileFlags = _fileFlags;
}

bool FileDataArray::Init(Txt , FileDataArray &orig, FileDiffArray &diff)
{
    basePath = orig.basePath;
    fileCount = orig.fileCount;
    folderCount = orig.folderCount;
    fileSize = orig.fileSize;
    useId = orig.useId;
    fileList.SetCount(orig.GetCount());
    for (i32 i = 0; i < orig.GetCount(); ++i)
        fileList[i] = orig[i];

    for (i32 i = 0; i < diff.GetCount(); ++i) {
        long id;
        if (diff[i].action != 'n') {
            id = Find(diff[i].relPath, diff[i].fileName, diff[i].isFolder);
            if (id < 0)
                return false;
        }
        switch(diff[i].action) {
        case 'u':
            fileList[id].id = diff[i].idMaster;
            fileList[id].length = diff[i].lengthMaster;
            fileList[id].t = diff[i].tMaster;
            break;
        case 'n':
            fileList.Add(FileData(diff[i].isFolder, diff[i].fileName, diff[i].relPath, diff[i].lengthMaster, diff[i].tMaster, diff[i].idMaster));
            if (diff[i].isFolder)
                folderCount++;
            else
                fileCount++;
            break;
        case 'd':
            fileList.Remove(id);
            if (diff[i].isFolder)
                folderCount--;
            else
                fileCount--;
            break;
        case 'p':
            SetLastError(t_("Обнаружена проблема"));      // To Fix
            //return false;
        }
    }
    return true;
}

void FileDataArray::Clear()
{
    fileList.Clear();
    errorList.Clear();
    fileCount = folderCount = 0;
    fileSize = 0;
    basePath = "";
}

bool FileDataArray::Search(Txt dir, Txt condFile, bool recurse, Txt text)
{
    Clear();
    if (fileFlags & BROWSE_LINKS) {
        if (IsSymLink(dir))
            dir = basePath = GetSymLinkPath(dir);
        else
            basePath = dir;
    } else
        basePath = dir;
    Search_Each(dir, condFile, recurse, text);
    return errorList.IsEmpty();
}

void FileDataArray::Search_Each(Txt dir, Txt condFile, bool recurse, Txt text)
{
    FindFile ff;
    if (ff.Search(AppendFNameX(dir, condFile))) {
        do {
            if(ff.IsFile()) {
                Txt p = AppendFNameX(dir, ff.GetName());
                //if (ff.IsSymLink()) {
                //  p = p;
                //}
                /*
                    fileList.Add(FileData(true, ff.GetName(), GetRelativePath(dir), 0, ff.GetLastWriteTime(), 0));
                    folderCount++;
                    if (recurse)
                        Search_Each(p, condFile, recurse, text);
                } else */ if (text.IsEmpty()) {
                    zu64 len = ff.GetLength();
                    fileList.Add(FileData(false, ff.GetName(), GetRelativePath(dir), len, ff.GetLastWriteTime(),
                                            (useId && len > 0) ? GetFileId(p) : 0));
                    fileCount++;
                    fileSize += len;
                } else {
                    FILE *fp = fopen(p, "r");
                    if (fp != NULL) {
                        i32 i = 0, c;
                        while ((c = fgetc(fp)) != EOF) {
                            if (c == text[i]) {
                                ++i;
                                if (i == text.GetCount()) {
                                    zu64 len = ff.GetLength();
                                    fileList.Add(FileData(false, ff.GetName(), GetRelativePath(dir), len, ff.GetLastWriteTime(), useId ? GetFileId(p) : 0));
                                    fileCount++;
                                    fileSize += len;
                                    break;
                                }
                            } else {
                                if (i != 0)
                                    fseek(fp, -(i-1), SEEK_CUR);
                                i = 0;
                            }
                        }
                        fclose(fp);
                    } else
                        errorList.Add(AppendFNameX(dir, ff.GetName()) + Txt(": ") + t_("Невозможно открыть файл"));
                }
            }
        } while (ff.Next());
    }
    ff.Search(AppendFNameX(dir, "*"));
    do {
        Txt name = ff.GetName();
        if(ff.IsDir() && name != "." && name != "..") {
            Txt p = AppendFNameX(dir, name);
            fileList.Add(FileData(true, name, GetRelativePath(dir), 0, ff.GetLastWriteTime(), 0));
            folderCount++;
            if (recurse)
                Search_Each(p, condFile, recurse, text);
        }
    } while (ff.Next());
}

z64 FileDataArray::GetFileId(Txt fileName)
{
    z64 id = -1;
#ifdef TARGET_POSIX
    FILE *fp = fopen(fileName, "rb");
#else
    FILE *fp = _wfopen((const wchar_t *)fileName.ToWTxt().Begin(), L"rb");
#endif
    if (fp != NULL) {
        i32 c;
        long i = 0;
        while ((c = fgetc(fp)) != EOF) {
            id += c*i;
            i++;
        }
        fclose(fp);
    }
    return id;
}

void FileDataArray::SortByName(bool ascending)
{
    fileDataSortBy = 'n';
    fileDataSortAscending = ascending;
    Sort(fileList);
}
void FileDataArray::SortByDate(bool ascending)
{
    fileDataSortBy = 'd';
    fileDataSortAscending = ascending;
    Sort(fileList);
}
void FileDataArray::SortBySize(bool ascending)
{
    fileDataSortBy = 's';
    fileDataSortAscending = ascending;
    Sort(fileList);
}

bool operator<(const FileData& a, const FileData& b)
{
    if ((a.isFolder && b.isFolder) || (!a.isFolder && !b.isFolder)) {
        switch (fileDataSortBy) {
        case 'n':   {
                        bool ais = IsDigit(a.fileName[0]);
                        bool bis = IsDigit(b.fileName[0]);
                        if (ais && bis)
                            return atoi(a.fileName) < atoi(b.fileName);
                        if (ais)
                            return true;
                        if (bis)
                            return false;
                    }
#ifdef TARGET_POSIX
                    return (a.fileName < b.fileName)&fileDataSortAscending;
#else
                    return (ToLower(a.fileName) < ToLower(b.fileName))&fileDataSortAscending;
#endif
        case 'd':   return (a.t < b.t)&fileDataSortAscending;
        default:    return (a.length < b.length)&fileDataSortAscending;
        }
    } else
        return a.isFolder;
}

bool CheckFileData(FileData &data, Txt &, Txt &, const Txt &lowrelFName, const Txt &lowfileName, bool isFolder) {
    if (data.isFolder == isFolder) {
        if (ToLower(data.fileName) == lowfileName) {
            if (ToLower(data.relFilename) == lowrelFName)
                return true;
        }
    }
    return false;
}

i32 FileDataArray::Find(Txt &relFName, Txt &fileName, bool isFolder) {
    Txt lowrelFName = ToLower(relFName);
    Txt lowfileName = ToLower(fileName);
    for (i32 i = 0; i < fileList.GetCount(); ++i) {
        if (CheckFileData(fileList[i], relFName, fileName, lowrelFName, lowfileName, isFolder))
            return i;
    }
    return -1;
}

/*
i32 FileDataArray::Find(FileDataArray &data, i32 id) {
    return Find(data[id].relFilename, data[id].fileName, data[id].isFolder);
}
*/

i32 FileDataArray::Find(FileDataArray &data, i32 id) {
    Txt lowrelFName = ToLower(data[id].relFilename);
    Txt lowfileName = ToLower(data[id].fileName);
    bool isFolder = data[id].isFolder;

    i32 num = fileList.GetCount();
    if (num == 0)
        return -1;
    if (num == 1) {
        if (CheckFileData(fileList[0], data[id].relFilename, data[id].fileName, lowrelFName, lowfileName, isFolder))
            return 0;
        else
            return -1;
    }
    i32 down, up;
    down = id < num-1 ? id : num-2;
    up = down + 1;
    while (down >= 0 || up < num) {
        if (down >= 0) {
            if (CheckFileData(fileList[down], data[id].relFilename, data[id].fileName, lowrelFName, lowfileName, isFolder))
                return down;
            down--;
        }
        if (up < num) {
            if (CheckFileData(fileList[up], data[id].relFilename, data[id].fileName, lowrelFName, lowfileName, isFolder))
                return up;
            up++;
        }
    }
    return -1;
}

Txt FileDataArray::GetFileText() {
    Txt ret;

    for (i32 i = 0; i < fileList.GetCount(); ++i) {
        ret << fileList[i].relFilename << "; ";
        ret << fileList[i].fileName << "; ";
        ret << fileList[i].isFolder << "; ";
        ret << fileList[i].length << "; ";
        ret << fileList[i].t << "; ";
        ret << fileList[i].id << "; ";
        ret << "\n";
    }
    return ret;
}

bool FileDataArray::SaveFile(tukk fileName) {
    return drx::SaveFile(fileName, GetFileText());
}

bool FileDataArray::AppendFile(tukk fileName) {
    return drx::AppendFile(fileName, GetFileText());
}

bool FileDataArray::LoadFile(tukk fileName)
{
    Clear();
    TxtParse in = drx::LoadFile(fileName);

    if (in == "")
        return false;

    i32 numData = in.Count("\n");
    fileList.SetCount(numData);
    for (i32 row = 0; row < numData; ++row) {
        fileList[row].relFilename = in.GetText(";");
        fileList[row].fileName = in.GetText(";");
        fileList[row].isFolder = in.GetText(";") == "true" ? true : false;
        if (fileList[row].isFolder)
            folderCount++;
        else
            fileCount++;
        fileList[row].length = in.GetUInt64(";");
        struct drx::Time t;
        StrToTime(t, in.GetText(";"));
        fileList[row].t = t;
        fileList[row].id = in.GetUInt64(";");
    }
    return true;
}

Txt FileDataArray::GetRelativePath(const Txt &fullPath) {
    if (basePath != fullPath.Left(basePath.GetCount()))
        return "";
    return fullPath.Mid(basePath.GetCount());
}

z64 GetDirLength(tukk directoryName) {
    FileDataArray files;
    files.Search(directoryName, "*.*", true);
    return files.GetSize();
}

z64 GetLength(tukk fileName) {
    if (FileExists(fileName))
        return GetFileLength(fileName);
    else
        return GetDirLength(fileName);
}

FileDiffArray::FileDiffArray() {
    Clear();
}

void FileDiffArray::Clear()
{
    diffList.Clear();
}

// True if equal
bool FileDiffArray::Compare(FileDataArray &master, FileDataArray &secondary, const Txt folderFrom,
                         Vec<Txt> &excepFolders, Vec<Txt> &excepFiles, i32 sensSecs) {
    if (master.GetCount() == 0) {
        if (secondary.GetCount() == 0)
            return true;
        else
            return false;
    } else if (secondary.GetCount() == 0)
        return false;

    bool equal = true;
    diffList.Clear();
    Vec<bool> secReviewed;
    secReviewed.SetCount(secondary.GetCount(), false);

    for (i32 i = 0; i < master.GetCount(); ++i) {
        bool cont = true;
        if (master[i].isFolder) {
            Txt fullfolder = AppendFNameX(folderFrom, master[i].relFilename, master[i].fileName);
            for (i32 iex = 0; iex < excepFolders.GetCount(); ++iex)
                if (PatternMatch(excepFolders[iex] + "*", fullfolder)) {// Subfolders included
                    cont = false;
                    break;
                }
        } else {
            Txt fullfolder = AppendFNameX(folderFrom, master[i].relFilename);
            for (i32 iex = 0; iex < excepFolders.GetCount(); ++iex)
                if (PatternMatch(excepFolders[iex] + "*", fullfolder)) {
                    cont = false;
                    break;
                }
            for (i32 iex = 0; iex < excepFiles.GetCount(); ++iex)
                if (PatternMatch(excepFiles[iex], master[i].fileName)) {
                    cont = false;
                    break;
                }
        }
        if (cont) {
            i32 idSec = secondary.Find(master, i);
            if (idSec >= 0) {
                bool useId = master.UseId() && secondary.UseId();
                secReviewed[idSec] = true;
                if (master[i].isFolder)
                    ;
                else if ((useId && (master[i].id == secondary[idSec].id)) ||
                         (!useId && (master[i].length == secondary[idSec].length) &&
                                     (abs(master[i].t - secondary[idSec].t) <= sensSecs)))
                    ;
                else {
                    equal = false;
                    FileDiffData &f = diffList.Add();
                    //bool isf = f.isFolder = master[i].isFolder;
                    f.relPath = master[i].relFilename;
                    Txt name = f.fileName = master[i].fileName;
                    f.idMaster = master[i].id;
                    f.idSecondary = secondary[idSec].id;
                    f.tMaster = master[i].t;
                    f.tSecondary = secondary[idSec].t;
                    f.lengthMaster = master[i].length;
                    f.lengthSecondary = secondary[idSec].length;
                    if (master[i].t > secondary[idSec].t)
                        f.action = 'u';
                    else
                        f.action = 'p';
                }
            } else {
                equal = false;
                FileDiffData &f = diffList.Add();
                f.isFolder = master[i].isFolder;
                f.relPath = master[i].relFilename;
                f.fileName = master[i].fileName;
                f.idMaster = master[i].id;
                f.idSecondary = 0;
                f.tMaster = master[i].t;
                f.tSecondary = Null;
                f.lengthMaster = master[i].length;
                f.lengthSecondary = 0;
                f.action = 'n';
            }
        }
    }
    for (i32 i = 0; i < secReviewed.GetCount(); ++i) {
        if (!secReviewed[i]) {
            bool cont = true;
            if (secondary[i].isFolder) {
                Txt fullfolder = AppendFNameX(folderFrom, secondary[i].relFilename, secondary[i].fileName);
                for (i32 iex = 0; iex < excepFolders.GetCount(); ++iex)
                    if (PatternMatch(excepFolders[iex] + "*", fullfolder)) {
                        cont = false;
                        break;
                    }
            } else {
                Txt fullfolder = AppendFNameX(folderFrom, secondary[i].relFilename);
                for (i32 iex = 0; iex < excepFolders.GetCount(); ++iex)
                    if (PatternMatch(excepFolders[iex] + "*", fullfolder)) {
                        cont = false;
                        break;
                    }
                for (i32 iex = 0; iex < excepFiles.GetCount(); ++iex)
                    if (PatternMatch(excepFiles[iex], secondary[i].fileName)) {
                        cont = false;
                        break;
                    }
            }
            if (cont) {
                equal = false;
                FileDiffData &f = diffList.Add();
                f.isFolder = secondary[i].isFolder;
                f.relPath = secondary[i].relFilename;
                f.fileName = secondary[i].fileName;
                f.idMaster = 0;
                f.idSecondary = secondary[i].id;
                f.tMaster = Null;
                f.tSecondary = secondary[i].t;
                f.lengthMaster = 0;
                f.lengthSecondary = secondary[i].length;
                f.action = 'd';
            }
        }
    }
    return equal;
}

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
Txt WinLastError() {
    LPVOID lpMsgBuf;
    Txt ret;

    FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                  FORMAT_MESSAGE_IGNORE_INSERTS,
                  NULL, ::GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  reinterpret_cast<LPTSTR>(&lpMsgBuf), 0, NULL);
    ret = static_cast<tuk >(lpMsgBuf);
    LocalFree(lpMsgBuf);
    return ret;
}
#endif

bool FileDiffArray::Apply(Txt toFolder, Txt fromFolder, EXT_FILE_FLAGS flags)
{
    for (i32 i = 0; i < diffList.GetCount(); ++i) {
        bool ok = true;
        Txt dest = AppendFNameX(toFolder, diffList[i].relPath, diffList[i].fileName);
        if (diffList[i].action == 'u' || diffList[i].action == 'd') {
            if (diffList[i].isFolder) {
                if (DirExists(dest)) {
                    if (!SetReadOnly(dest, false))
                        ok = false;
                }
            } else {
                if (FileExists(dest)) {
                    if (!SetReadOnly(dest, false))
                        ok = false;
                }
            }
        }
        if (!ok) {
            Txt strError = t_("Нельзя изменить ") + dest;
            SetLastError(strError);
            //return false;
        }

        switch (diffList[i].action) {
        case 'n': case 'u':
            if (diffList[i].isFolder) {
                if (!DirExists(dest)) {
                    ok = DirCreate(dest); ////////////////////////////////////////////////////////////////////////////////////////
                }
            } else {
                if (FileExists(dest)) {
                    if (!SetReadOnly(dest, false))
                        ok = false;
                }
                if (ok) {
                    ok = FileCopy(AppendFNameX(fromFolder, FormatInt(i)), dest);
                    diffList[i].tSecondary = diffList[i].tMaster;
                }
            }

            if (!ok) {
                Txt strError = t_("Невозможно создать ") + dest;
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
                strError += ". " + WinLastError();
#endif
                SetLastError(strError);
                //return false;
            }
            break;
        case 'd':
            if (diffList[i].isFolder) {
                if (DirExists(dest))
                    ok = DeleteFolderDeep(dest);    // Necessary to add the "X"
                Sleep(100);
            } else {
                if (FileExists(dest))
                    ok = FileDeleteX(dest, flags);
            }
            if (!ok) {
                Txt strError = t_("Нельзя удалить") + Txt(" ") + dest;
#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
                strError += ". " + WinLastError();
#endif
                SetLastError(strError);
                //return false;
            }
            break;
        case 'p':
            SetLastError(t_("Проблемы с копией"));
            //return false;
            break;
        }
    }
    return true;
}

bool FileDiffArray::SaveFile(tukk fileName)
{
    return drx::SaveFile(fileName, ToTxt());
}

Txt FileDiffArray::ToTxt()
{
    Txt ret;

    for (i32 i = 0; i < diffList.GetCount(); ++i) {
        ret << diffList[i].action << "; ";
        ret << diffList[i].isFolder << "; ";
        ret << diffList[i].relPath << "; ";
        ret << diffList[i].fileName << "; ";
        ret << diffList[i].idMaster << "; ";
        ret << diffList[i].idSecondary << "; ";
        ret << diffList[i].tMaster << "; ";
        ret << diffList[i].tSecondary << "; ";
        ret << diffList[i].lengthMaster << "; ";
        ret << diffList[i].lengthSecondary << "; ";
        ret << "\n";
    }
    return  ret;
}

bool FileDiffArray::LoadFile(tukk fileName)
{
    Clear();
    TxtParse in = drx::LoadFile(fileName);

    if (in == "")
        return false;

    i32 numData = in.Count("\n");
    diffList.SetCount(numData);
    for (i32 row = 0; row < numData; ++row) {
        diffList[row].action = TrimLeft(in.GetText(";"))[0];
        diffList[row].isFolder = in.GetText(";") == "true" ? true : false;
        diffList[row].relPath = in.GetText(";");
        diffList[row].fileName = in.GetText(";");
        diffList[row].idMaster = in.GetUInt64(";");
        diffList[row].idSecondary = in.GetUInt64(";");
        struct drx::Time t;
        StrToTime(t, in.GetText(";"));
        diffList[row].tMaster = t;
        StrToTime(t, in.GetText(";"));
        diffList[row].tSecondary = t;
        diffList[row].lengthMaster = in.GetUInt64(";");
        diffList[row].lengthSecondary = in.GetUInt64(";");
    }
    return true;
}


#if defined(TARGET_WIN32) || defined (TARGET_WIN64)
Val GetVARIANT(VARIANT &result)
{
    Val ret;
    switch(result.vt) {
    case VT_EMPTY:
    case VT_NULL:
    case VT_BLOB:
        break;
    case VT_BOOL:
         ret = result.boolVal;// ? "true" : "false";
         break;
    case VT_I2:
         ret = result.iVal;
         break;
    case VT_I4:
        ret = static_cast<z64>(result.lVal);
        break;
    case VT_R4:
        ret = AsTxt(result.fltVal);
        break;
    case VT_R8:
        ret = AsTxt(result.dblVal);
        break;
    case VT_BSTR:
        ret = WideToTxt(result.bstrVal);
        break;
    case VT_LPSTR:
         //ret = result.pszVal;
         ret = "Unknown VT_LPSTR";
         break;
    case VT_DATE:
        SYSTEMTIME stime;
        VariantTimeToSystemTime(result.date, &stime);
        {
            Time t;
            t.day    = static_cast<drx::u8>(stime.wDay);
            t.month  = static_cast<drx::u8>(stime.wMonth);
            t.year   = stime.wYear;
            t.hour   = static_cast<drx::u8>(stime.wHour);
            t.minute = static_cast<drx::u8>(stime.wMinute);
            t.second = static_cast<drx::u8>(stime.wSecond);
            ret = t;
        }
        break;
    case VT_CF:
        ret = "(Clipboard format)";
        break;
    }
    return ret;
}

Txt WideToTxt(LPCWSTR wcs, i32 len) {
    if (len == -1) {
        len = WideCharToMultiByte(CP_UTF8, 0, wcs, len, nullptr, 0, nullptr, nullptr);
        if (len == 0)
            return Null;
    }
    Buffer<char> w(len);
    WideCharToMultiByte(CP_UTF8, 0, wcs, len, w, len, nullptr, nullptr);
    return ~w;
}

bool TxtToWide(Txt str, LPCWSTR &wcs) {
    wchar_t *buffer;
    DWORD size = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
    if (!(buffer = (wchar_t *)GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size)))
        return false;

    MultiByteToWideChar(CP_UTF8, 0, str, -1, buffer, size);
    wcs = SysAllocTxt(buffer);
    GlobalFree(buffer);
    if (!wcs)
        return false;
    return true;
}

bool BSTRSet(const Txt str, BSTR &bstr) {
    wchar_t *buffer;
    DWORD size = MultiByteToWideChar(CP_UTF8, 0, str, -1, 0, 0);
    if (!(buffer = (wchar_t *)GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size)))
        return false;

    MultiByteToWideChar(CP_UTF8, 0, str, -1, buffer, size);
    bstr = SysAllocTxt(buffer);
    GlobalFree(buffer);
    if (!bstr)
        return false;
    return true;
}

Txt BSTRGet(BSTR &bstr) {
    if (!bstr)
        return Null;

    tuk buffer;
    DWORD size = SysTxtLen(bstr);
    if (!(buffer = (tuk )GlobalAlloc(GMEM_FIXED, sizeof(wchar_t) * size)))
        return Null;

    size_t i = wcstombs(buffer, bstr, size);
    buffer[i] = 0;

    Txt ret = buffer;
    GlobalFree(buffer);
    return ret;
}

#endif

#if defined(TARGET_WIN32) || defined (TARGET_WIN64)

Dl::~Dl() {
    if (hinstLib)
        if (FreeLibrary(hinstLib) == 0)
            LOG(t_("Dl cannot be released"));
}

#ifndef LOAD_IGNORE_CODE_AUTHZ_LEVEL
    #define LOAD_IGNORE_CODE_AUTHZ_LEVEL    0x00000010
#endif

bool Dl::Load(const Txt &fileDll) {
    if (hinstLib)
        if (FreeLibrary(hinstLib) == 0)
            return false;

    hinstLib = LoadLibraryEx(TEXT(ToSystemCharset(fileDll)), nullptr, LOAD_IGNORE_CODE_AUTHZ_LEVEL);
    if (!hinstLib)
        return false;
    return true;
}

uk Dl::GetFunction(const Txt &functionName) const {
    if (!hinstLib)
        return nullptr;
    return reinterpret_cast<uk >(GetProcAddress(hinstLib, functionName));
}

uk Dl::GetFunction_throw(const Txt &functionName) const {
    uk ret;
    if (!(ret = GetFunction(functionName)))
        throw Exc(Format("Dl::GetFunction: %s не найдено", functionName));
    return ret;
}


#else

#include <dlfcn.h>

Dl::~Dl() {
    if (hinstLib)
        if (dlclose(hinstLib) == 0)
            ;   // Dl cannot be released
}

bool Dl::Load(const Txt &fileDll) {
    if (hinstLib)
        if (dlclose(hinstLib) == 0)
            return false;

    hinstLib = dlopen(fileDll, RTLD_LAZY);
    if (!hinstLib)
        return false;
    return true;
}

uk Dl::GetFunction(const Txt &functionName) const {
    if (!hinstLib)
        return nullptr;
    return dlsym(hinstLib, functionName);
}

#endif

Color RandomColor() {
    return Color().FromRaw(Random());
}

Image GetRect(const Image& orig, const Rect &r) {
    if(r.IsEmpty())
        return Image();
    ImageBuffer ib(r.GetSize());
    for(i32 y = r.top; y < r.bottom; y++) {
        const RGBA *s = orig[y] + r.left;
        const RGBA *e = orig[y] + r.right;
        RGBA *t = ib[y - r.top];
        while(s < e) {
            *t = *s;
            t++;
            s++;
        }
    }
    return static_cast<Image>(ib);
}


double tmGetTimeX() {
#ifdef __linux__
    timespec t;
    if (0 != clock_gettime(CLOCK_REALTIME, &t))
        return Null;
    return t.tv_sec + (double)t.tv_nsec/1000000000.;
#elif defined(_WIN32) || defined(WIN32)
    LARGE_INTEGER clock;
    LARGE_INTEGER freq;
    if(!QueryPerformanceCounter(&clock) || !QueryPerformanceFrequency(&freq))
        return Null;
    return double(clock.QuadPart)/freq.QuadPart;
#else
    return double(time(0));     // Low resolution
#endif
}


i32 SysX(tukk cmd, Txt& out, Txt& err, double timeOut,
            Gate3<double, Txt&, Txt&> progress, bool convertcharset) {
    out.Clear();
    LocalProcess p;
    p.ConvertCharset(convertcharset);
    double t0 = tmGetTimeX();
    if(!p.Start2(cmd))
        return -1;
    i32 ret = Null;
    Txt os, es;
    while(p.IsRunning()) {
        if (p.Read2(os, es)) {
            out.Cat(os);
            err.Cat(es);
        }
        double elapsed = tmGetTimeX() - t0;
        if (!IsNull(timeOut) && elapsed > timeOut) {
            ret = -2;
            break;
        }
        if (progress(elapsed, out, err)) {
            ret = -3;
            break;
        }
        Sleep(1);
    }
    out.Cat(os);
    err.Cat(es);
    if (!IsNull(ret))
        p.Kill();

    return IsNull(ret) ? 0 : ret;
}


i32 LevenshteinDistance(tukk s, tukk t) {
    i32 lens = i32(strlen(s));
    i32 lent = i32(strlen(t));

    Buffer<i32> v0(lent + 1);
    Buffer<i32> v1(lent + 1);

    for (i32 i = 0; i <= lent; ++i)
        v0[i] = i;

    for (i32 i = 0; i < lens; ++i) {
        v1[0] = i + 1;

        for (i32 j = 0; j < lent; ++j) {
            i32 deletionCost = v0[j + 1] + 1;
            i32 insertionCost = v1[j] + 1;
            i32 substitutionCost;
            if (s[i] == t[j])
                substitutionCost = v0[j];
            else
                substitutionCost = v0[j] + 1;

            v1[j + 1] = min(deletionCost, insertionCost, substitutionCost);
        }
        Swap(v0, v1);
    }
    return v0[lent];
}

i32 DamerauLevenshteinDistance(tukk s, tukk t, i32 alphabetLength) {
    i32 lens = i32(strlen(s));
    i32 lent = i32(strlen(t));
    i32 lent2 = lent + 2;
    Buffer<i32> H((lens+2)*lent2);

    i32 infinity = lens + lent;
    H[0] = infinity;
    for(i32 i = 0; i <= lens; i++) {
        H[lent2*(i+1)+1] = i;
        H[lent2*(i+1)+0] = infinity;
    }
    for(i32 j = 0; j <= lent; j++) {
        H[lent2*1+(j+1)] = j;
        H[lent2*0+(j+1)] = infinity;
    }
    Buffer<i32> DA(alphabetLength, 0);

    for(i32 i = 1; i <= lens; i++) {
        i32 DB = 0;
        for(i32 j = 1; j <= lent; j++) {
            i32 i1 = DA[i32(t[j-1])];
            i32 j1 = DB;
            i32 cost = (s[i-1] == t[j-1]) ? 0 : 1;
            if(cost == 0)
                DB = j;
            H[lent2*(i+1)+j+1] =
              min(H[lent2*i     + j] + cost,
                  H[lent2*(i+1) + j] + 1,
                  H[lent2*i     + j+1] + 1,
                  H[lent2*i1    + j1] + (i-i1-1) + 1 + (j-j1-1));
        }
        DA[i32(s[i-1])] = i;
    }
    return H[lent2*(lens+1)+lent+1];
}

i32 SentenceSimilitude(tukk s, tukk t) {
    i32 ls = i32(strlen(s));
    i32 lt = i32(strlen(t));
    if (ls > lt) {
        Swap(s, t);
        Swap(ls, lt);
    }
    i32 mind = ls;
    for (i32 i = 0; i < t - s; ++i) {
        i32 d = DamerauLevenshteinDistance(s, Txt(t).Mid(i, ls));
        if (d < mind)
            mind = d;
    }
    return (100*mind)/ls;
}

// Dummy functions added after TheIDE change
drx::Txt GetCurrentMainPackage() {return "dummy";}
drx::Txt GetCurrentBuildMethod() {return "dummy";}
void IdePutErrorLine(const drx::Txt& ) {}

size_t GetNumLines(Stream &stream) {
    size_t res = 0;
    i32 c;

    if ((c = stream.Get()) < 0)
        return 0;
    if (c == '\n')
        res++;
    while ((c = stream.Get()) > 0)
        if (c == '\n')
            res++;
    if (c != '\n')
        res++;
    return res;
}


bool SetConsoleColor(CONSOLE_COLOR color) {
    static Vec<i32> colors;
    if (color == RESET)
        colors.Clear();
    else if(color == PREVIOUS) {
        i32 num = colors.size();
        if (num == 0)
            color = RESET;
        else {
            colors.Remove(num-1);
            if (num-2 < 0)
                color = RESET;
            else
                color = static_cast<CONSOLE_COLOR>(colors[num-2]);
        }
    } else
        colors << color;

#ifdef TARGET_WIN32
    static HANDLE hstdout = 0;
    static CONSOLE_SCREEN_BUFFER_INFO csbiInfo = {};
    static WORD woldattrs;

    if (hstdout == 0) {
        hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
        //if (!GetConsoleScreenBufferInfo(hstdout, &csbiInfo)) {
        //  hstdout = 0;
        //  return false;
        //}
        woldattrs = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;  //csbiInfo.wAttributes;
    }
    if (color == RESET)
        return SetConsoleTextAttribute(hstdout, woldattrs);
    else
        return SetConsoleTextAttribute(hstdout, color);
#else
    if (color < 100)
        printf("\x1b[%dm", color);
    else
        printf("\x1b[1;%dm", color-100);
    return true;
#endif
}

void ConsoleOutputDisable(bool disable) {
#ifdef TARGET_WIN32
    if (disable)
        freopen("nul", "w", stdout);
    else
        freopen("CONOUT$", "w", stdout);
#else
    static i32 saved_id = Null;
    static fpos_t saved_pos;

    if (disable) {
        fflush(stdout);
        fgetpos(stdout, &saved_pos);
        saved_id = dup(fileno(stdout));
        close(fileno(stdout));
    } else {
        if (!IsNull(saved_id)) {
            fflush(stdout);
            dup2(saved_id, fileno(stdout));
            close(saved_id);
            clearerr(stdout);
            fsetpos(stdout, &saved_pos);
        }
    }
#endif
}

static void ListArgsCFunction(const Txt &strargs, const Vec <Txt> &ctypes,
                        Vec<i32> &argTypeId, Vec<Txt> &argVars) {
    Vec<Txt> args = Split(strargs, ",");
    argTypeId.Clear();
    argVars.Clear();
    for (auto &arg : args) {
        arg = Trim(arg);
        i32 j;
        for (j = 0; j < ctypes.size(); ++j) {
            if (arg.StartsWith(ctypes[j]))
                break;
        }
        if (j >= ctypes.size())
            throw Exc(Format("Тип в аргументе '%s' не найден", arg));
        argTypeId << j;
        argVars   << Trim(arg.Mid(ctypes[j].GetCount()));
    }
}

static Txt ToArgs(const Vec<Txt> &args) {
    Txt ret;
    for (i32 i = 0; i < args.size(); ++i) {
        if (i > 0)
            ret << ", ";
        ret << args[i];
    }
    return ret;
}

Txt GetPythonDeclaration(const Txt &name, const Txt &include) {
    static Txt str;
    const Vec<Txt> ctypes = {"double **",                                       "i32 *",                       "i32",           "double",          "tukk ",    "bool",          "const double *"};
    const Vec<Txt> ptypes = {"ctypes.POINTER(ctypes.POINTER(ctypes.c_double))", "ctypes.POINTER(ctypes.c_int)", "ctypes.c_int", "ctypes.c_double", "ctypes.c_char_p", "ctypes.c_bool", "np.ctypeslib.ndpointer(dtype=np.float64)"};
    const Vec<bool> isPy_C   = {false,                                             false,                         false,           false,             false,             false,           true};
    const Vec<bool> isC_Py   = {true,                                              false,                         false,           false,             false,             false,           false};

    str << "# " << name << " список функций Питона\n\n"
           "class " << name << ":\n"
           "    def __init__(self, path_dll):\n"
           "        self.libc = ctypes.CDLL(dll)\n\n";

    Txt strIn  = "        # INPUT TYPES\n";
    Txt strOut = "        # OUTPUT TYPES\n";
    Txt strFun;

    Txt cleaned = CleanCFromDeclaration(include);

    Vec<Txt> lines = Split(cleaned, "\n");
    for (const auto &line : lines) {
        i32 pospar = line.Find("(");
        Txt function;

        for (i32 i = 0; i < ctypes.size(); ++i) {
            const auto &type = ctypes[i];
            if (line.StartsWith(type)) {
                function = Trim(line.Mid(type.GetCount(), pospar - type.GetCount()));
                strOut << "        self.libc." << function << ".restype = " << ptypes[i] << "\n";
                break;
            }
        }
        if (function.IsEmpty() && line.StartsWith("void"))
            function = Trim(line.Mid(Txt("void").GetCount(), pospar - Txt("void").GetCount()));

        if (function.IsEmpty())
            continue;

        i32 posparout = line.Find(")");
        Txt strargs = line.Mid(pospar+1, posparout - pospar-1);

        Vec<i32> argTypeId;
        Vec<Txt> argVars;
        ListArgsCFunction(strargs, ctypes, argTypeId, argVars);

        if (argTypeId.IsEmpty())
            continue;

        Vec<Txt> pargs, cargs, pargTypes;
        Txt pre, post, returns;
        i32 idata = 0;
        for (i32 i = 0; i < argTypeId.size(); ++i) {
            pargTypes << ptypes[argTypeId[i]];
            if (isC_Py[argTypeId[i]]) {
                cargs << Format("ctypes.byref(_data%d)", idata);
                //pargs << argVars[i];
                pre  << Format("        _data%d = ctypes.POINTER(ctypes.c_double)()\n", idata)
                     << Format("        _size%d = ctypes.c_int()\n", idata);
                post << Format("        _arraySize%d = ctypes.c_double * _size%d.value\n", idata, idata)
                     << Format("        _data%d_pointer = ctypes.cast(_data%d, ctypes.POINTER(_arraySize%d))\n", idata, idata, idata)
                     << Format("        %s = np.frombuffer(_data%d_pointer.contents)\n", argVars[i], idata);
                returns << ", " << argVars[i];
            } else if (isPy_C[argTypeId[i]]) {
                cargs << argVars[i];
                pargs << argVars[i];
            } else {
                if (i > 0 && isC_Py[argTypeId[i-1]]) {
                    cargs << Format("ctypes.byref(_size%d)", idata);
                    idata++;
                } else if (i > 0 && isPy_C[argTypeId[i-1]])
                    cargs << Format("len(%s)", argVars[i-1]);
                else {
                    if (ctypes[argTypeId[i]] == "tukk ")
                        cargs << Format("str.encode(%s, 'UTF-8')", argVars[i]);
                    else
                        cargs << argVars[i];
                    pargs << argVars[i];
                }
            }
        }
        strIn << Format("        self.libc.%s.argtypes = [%s]\n", function, ToArgs(pargTypes));

        Txt fname = function;
        fname.Replace("DLL_", "");
        strFun << "    def " << fname << "(self";
        if (!pargs.IsEmpty())
            strFun << ", " << ToArgs(pargs);
        strFun << "):\n";
        if (!pre.IsEmpty())
            strFun << "        # Подготовка аргумента\n"
                   << pre
                   << "        # Вызов функции из DLL\n"
                   << Format("        ret = self.libc.%s(%s)\n", function, ToArgs(cargs));
        else
            strFun << Format("        self.libc.%s(%s)\n", function, ToArgs(cargs));
        if (!post.IsEmpty())
            strFun << "        # Обработка вектора\n"
                   << post
                   << "        return ret" << returns << "\n";

        strFun << "\n";
    }
    str << strIn << "\n" << strOut << "\n" << strFun;

    return str = Trim(str);
}

Txt CleanCFromDeclaration(const Txt &include, bool removeSemicolon) {
    Txt str = include;

    str.Replace("   __declspec(dllexport) ", "");
    str.Replace("extern \"C\" {", "");
    str.Replace("};", "");
    str.Replace("\r\n\r\n", "\r\n");
    str.Replace(" noexcept", "");
    str.Replace("  ", " ");
    str.Replace("\t", " ");
    str.Replace(" ;", ";");
    str.Replace(" (", "(");
    str.Replace(" )", ")");

    if (removeSemicolon)
        str.Replace(");", ")");

    return str;
}

Txt S(tukk s)     {return s;}
Txt S(const Val &v)    {return v.ToTxt();}

bool CoutStreamX::noprint = false;

Stream& CoutX() {
    return Single<CoutStreamX>();
}

}