#include "../Core.h"

#ifdef TARGET_MACOS
#include <mach-o/dyld.h>
#endif

#ifdef TARGET_WIN32
#define Ptr Ptr_
#define u8 byte_
#define CY win32_CY_

#include <shellapi.h>
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

static StaticMutex sHlock;

static char sHomeDir[_MAX_PATH + 1];
static char Argv0__[_MAX_PATH + 1];

void (*CrashHook)();

void InstallCrashHook(void (*h)())
{
	CrashHook = h;
}

void SetHomeDir(tukk dir)
{
	INTERLOCKED_(sHlock) {
		strcpy(sHomeDir, dir);
	}
}

Txt GetHomeDir() {
	Txt r;
	INTERLOCKED_(sHlock) {
		if(!*sHomeDir)
			strcpy(sHomeDir,
			#ifdef TARGET_WIN32
				GetEnv("HOMEDRIVE") + GetEnv("HOMEPATH")
			#else
				Nvl(GetEnv("HOME"), "/root")
			#endif
			);
		r = sHomeDir;
	}
	return r;
}

#ifdef TARGET_WIN32

Txt GetEnv(tukk id)
{
	return WTxt(_wgetenv(ToSystemCharsetW(id))).ToTxt();
}

bool SetEnv(tukk name, tukk value)
{
	Txt env;
	env << name << "=" << value;
	auto wenv = ToUtf16(env);
	wenv.Add(wchar_t(0));
	return _wputenv(wenv.begin()) == 0;
}

Txt GetExeFilePath()
{
	return GetModuleFName();
}

#endif

#ifdef TARGET_POSIX

Txt GetEnv(tukk id)
{
	return FromSystemCharset(getenv(id));
}

bool SetEnv(tukk name, tukk value)
{
	return setenv(name, value, 1) == 0;
}

static void sSetArgv0__(tukk title)
{
	strcpy(Argv0__, title);
}

tukk procexepath_() {
	static char h[_MAX_PATH + 1];
	ONCELOCK {
		char link[1024];
#ifdef TARGET_MACOS
		uint32_t sz = 1024;
		if(_NSGetExecutablePath(link, &sz))
			*link = 0;
#elif defined(TARGET_BSD)
		sprintf(link, "/proc/%d/file", getpid());
#else
		sprintf(link, "/proc/%d/exe", getpid());
#endif
		FindFile ff(link);
		if(ff) {
			if(ff.IsSymLink()) {
				i32 ret = readlink(link, h, _MAX_PATH);
				if(ret > 0 && ret < _MAX_PATH)
					h[ret] = '\0';
				else
					*h = '\0';
			}
			else
				strcpy(h, link);
		}
	}
	return h;
}

Txt GetExeFilePath()
{
	static char exepath[_MAX_PATH + 1];
	ONCELOCK {
		tukk exe = procexepath_();
		if(*exe)
			strcpy(exepath, exe);
		else {
			Txt x = Argv0__;
			if(IsFullPath(x) && FileExists(x))
				strcpy(exepath, x);
			else {
				strcpy(exepath, GetHomeDirFile("DinrusPro"));
				Vec<Txt> p = Split(FromSystemCharset(Environment().Get("PATH")), ':');
				if(x.Find('/') >= 0)
					p.Add(GetCurDir());
				for(i32 i = 0; i < p.GetCount(); i++) {
					Txt ep = NormalizePath(AppendFName(p[i], x));
					if(FileExists(ep))
						strcpy(exepath, ep);
				}
			}
		}
	}
	return exepath;
}
#endif

Txt GetExeDirFile(tukk filename)
{
	return AppendFName(GetFileFolder(GetExeFilePath()), filename);
}

Txt GetExeFolder()
{
	return GetFileFolder(GetExeFilePath());
}

Txt GetExeTitle()
{
	return GetFileTitle(GetExeFilePath());
}

#ifdef TARGET_COCOA
Txt GetAppFolder()
{
	Txt p = GetExeFolder();
	return p.TrimEnd("/Contents/MacOS") && GetFileExt(p) == ".app" ? p : Txt();
}
#endif

void SyncLogPath__();

static char sAppName[256];

Txt GetAppName()
{
	return Nvl(Txt(sAppName), GetExeTitle());
}

void SetAppName(const Txt& name)
{
	strcpy(sAppName, name);
	SyncLogPath__();
}

static char sConfigGroup[256] = "Dinrus";

void SetConfigGroup(tukk group)
{
	strcpy(sConfigGroup, group);
	SyncLogPath__();
}

Txt GetConfigGroup()
{
	return sConfigGroup;
}

Txt GetTempDir()
{
	return GetTempPath();
}

Txt TempFile(tukk filename)
{
	return AppendFName(GetTempDir(), filename);
}

Txt  GetHomeDirFile(tukk fp) {
	return AppendFName(GetHomeDir(), fp);
}

static bool sHomecfg;

void UseHomeDirConfig(bool b)
{
	sHomecfg = b;
}

static char sConfigFolder[_MAX_PATH + 1];

void SetConfigDir(const Txt& s)
{
	strcpy(sConfigFolder, s);
	SyncLogPath__();
}

void CopyFolder(tukk dst, tukk src)
{
	RealizeDir(dst);
	FindFile ff(Txt(src) + "/*.*");
	while(ff) {
		Txt s = AppendFName(src, ff.GetName());
		Txt d = AppendFName(dst, ff.GetName());
		if(ff.IsFile()) {
			FileIn in(s);
			FileOut out(d);
			CopyStream(out, in);
		}
		else
		if(ff.IsFolder() && *ff.GetName() != '.')
			CopyFolder(d, s);
		ff.Next();
	}
}

Txt  ConfigFile(tukk file) {
	if(*sConfigFolder)
		return AppendFName(sConfigFolder, file);
#if defined(TARGET_WIN32)
	if(sHomecfg) {
		Txt p = GetHomeDirFile(GetAppName());
		ONCELOCK
			RealizeDir(p);
		return AppendFName(p, file);
	}
	return GetExeDirFile(file);
#elif defined(TARGET_POSIX)
	static char cfgd[_MAX_PATH + 1];
	static bool sandboxed = true;
	ONCELOCK {
		Txt cfgdir;
		Txt h = GetExeFolder();
		if(!sHomecfg)
			while(h.GetCount() > 1 && DirExists(h)) {
				Txt pp = AppendFName(h, ".config");
				FindFile ff(pp);
				if(ff && ff.IsFolder() && ff.CanWrite()) {
					cfgdir = pp;
					break;
				}
				h = GetFileFolder(h);
			}
		if(IsNull(cfgdir)) {
			sandboxed = false;
			cfgdir = GetEnv("XDG_CONFIG_HOME");
		}
		if(IsNull(cfgdir) || !DirExists(cfgdir))
			cfgdir = GetHomeDirFile(".config");
		if(*sConfigGroup)
			cfgdir = AppendFName(cfgdir, GetConfigGroup());
		strcpy(cfgd, cfgdir);
	}
	Txt pp = AppendFName(cfgd, GetAppName());
	bool exists = DirExists(pp);
	RealizeDir(pp);
	if(!exists && !sandboxed) { // migrate config files from the old path
		Txt old = GetHomeDirFile(".upp/" + GetExeTitle());
		if(DirExists(old))
			CopyFolder(pp, old);
	}
	return AppendFName(pp, file);
#else
	NEVER();
	return GetExeDirFile(file);
#endif//PLATFORM
}

Txt GetConfigFolder()
{
	return GetFileFolder(ConfigFile("x"));
}

Txt  ConfigFile() {
	return ConfigFile(GetAppName() + ".cfg");
}

Txt argv0;

Vec<WTxt>& coreCmdLine__()
{
	static Vec<WTxt> h;
	return h;
}

static Mutex sCmdMutex;
static Vec<Txt> sCmd;

const Vec<Txt>& CommandLine()
{
	Mutex::Lock __(sCmdMutex);
	ONCELOCK {
		for(WTxt s : coreCmdLine__())
			sCmd.Add(s.ToTxt());
	}
	return sCmd;
}

void CommandLineRemove(i32 i, i32 count)
{
	CommandLine();
	sCmd.Remove(i, count);
}

Txt GetArgv0()
{
	return Argv0__;
}

VecMap<WTxt, WTxt>& EnvMap()
{
	static VecMap<WTxt, WTxt> x;
	return x;
}

const VecMap<Txt, Txt>& Environment()
{
	VecMap<Txt, Txt> *ptr;
	INTERLOCKED {
		static ArrayMap< u8, VecMap<Txt, Txt> > charset_env;
		u8 cs = GetDefaultCharset();
		i32 f = charset_env.Find(cs);
		if(f >= 0)
			ptr = &charset_env[f];
		else {
			ptr = &charset_env.Add(cs);
			const VecMap<WTxt, WTxt>& env_map = EnvMap();
			for(i32 i = 0; i < env_map.GetCount(); i++)
				ptr->Add(env_map.GetKey(i).ToTxt(), env_map[i].ToTxt());
		}
	}
	return *ptr;
}

static i32 exitcode;
static bool sMainRunning;

void  SetExitCode(i32 code) { exitcode = code; }
i32   GetExitCode()         { return exitcode; }

bool  IsMainRunning()
{
	return sMainRunning;
}

void LoadLangFiles(tukk dir)
{
	FindFile ff(AppendFName(dir, "*.tr"));
	while(ff) {
		LoadLngFile(AppendFName(dir, ff.GetName()));
		ff.Next();
	}
}

void CommonInit()
{
#ifdef TARGET_WIN32
	LoadLangFiles(GetFileFolder(GetExeFilePath()));
#else
	LoadLangFiles(GetHomeDir());
#endif

	Vec<WTxt>& cmd = coreCmdLine__();
	static WTxt exp_cmd = "--export-tr";
	static WTxt brk_cmd = "--memory-breakpoint__";

	for(i32 i = 0; i < cmd.GetCount();) {
		if(cmd[i] == exp_cmd) {
			{
				i++;
				i32 lang = 0;
				i32 lang2 = 0;
				u8 charset = CHARSET_UTF8;
				Txt fn = "Все";
				if(i < cmd.GetCount())
					if(cmd[i].GetLength() == 4 || cmd[i].GetLength() == 5) {
						lang = LNGFromText(cmd[i].ToTxt());
						fn = cmd[i].ToTxt();
						i32 c = cmd[i][4];
						if(c >= '0' && c <= '8')
							charset = c - '0' + CHARSET_WIN1250;
						if(c >= 'A' && c <= 'J')
							charset = c - 'A' + CHARSET_ISO8859_1;
					}
				fn << ".tr";
				if(++i < cmd.GetCount() && (cmd[i].GetLength() == 4 || cmd[i].GetLength() == 5))
					lang2 = LNGFromText(cmd[i].ToTxt());
			#ifdef TARGET_WIN32
				FileOut out(GetExeDirFile(fn));
			#else
				FileOut out(GetHomeDirFile(fn));
			#endif
				if(lang) {
					if(lang2)
						SaveLngFile(out, SetLNGCharset(lang, charset), SetLNGCharset(lang2, charset));
					else
						SaveLngFile(out, SetLNGCharset(lang, charset));
				}
				else {
					Index<i32> l = GetLngSet();
					for(i32 i = 0; i < l.GetCount(); i++)
						SaveLngFile(out, SetLNGCharset(l[i], charset));
				}
			}
			exit(0);
		}
	#if defined(_DEBUG) && defined(DRX_HEAP)
		if(cmd[i] == brk_cmd && i + 1 < cmd.GetCount()) {
			MemoryBreakpoint(atoi(cmd[i + 1].ToTxt()));
			cmd.Remove(i, 2);
		}
		else
			i++;
	#else
		i++;
	#endif
	}
	sMainRunning = true;
}

void Exit(i32 code)
{
	SetExitCode(code);
	throw ExitExc();
}

void MemorySetMainBegin__();
void MemorySetMainEnd__();

void AppExecute__(void (*app)())
{
	try {
		MemorySetMainBegin__();
		(*app)();
		MemorySetMainEnd__();
	}
	catch(ExitExc) {
		return;
	}
}

#ifdef TARGET_POSIX

void s_ill_handler(i32)
{
	CrashHook();
	Panic("Нелегальная инструкция!");
}

void s_segv_handler(i32)
{
	CrashHook();
	Panic("Неправильный доступ к  памяти!");
}

void s_fpe_handler(i32)
{
	CrashHook();
	Panic("Неверная арифметическая операция!");
}

void AppInit__(i32 argc, tukk *argv, tukk *envptr)
{
#if defined(TARGET_WIN32)
	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
#endif
	SetLanguage(LNG_CURRENT);
	sSetArgv0__(argv[0]);
	for(tukk var; (var = *envptr) != 0; envptr++)
	{
		tukk b = var;
		while(*var && *var != '=')
			var++;
		Txt varname(b, var);
		if(*var == '=')
			var++;
		EnvMap().Add(varname.ToWTxt(), Txt(var).ToWTxt());
	}
	Vec<WTxt>& cmd = coreCmdLine__();
	for(i32 i = 1; i < argc; i++)
		cmd.Add(FromSystemCharset(argv[i]).ToWTxt());
	CommonInit();
	signal(SIGILL, s_ill_handler);
	signal(SIGSEGV, s_segv_handler);
	signal(SIGBUS, s_segv_handler);
	signal(SIGFPE, s_fpe_handler);
}
#endif

#if defined(TARGET_WIN32)

void AppInitEnvironment__()
{

	SetConsoleCP(65001);
	SetConsoleOutputCP(65001);
	SetLanguage(LNG_('R', 'U', 'R', 'U'));
	i32 nArgs;
    LPWSTR *szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);
	if(szArglist) {
		strcpy(Argv0__, FromSystemCharsetW(szArglist[0]));
		for(i32 i = 1; i < nArgs; i++)
			coreCmdLine__().Add(FromSystemCharsetW(szArglist[i]).ToWTxt());
		LocalFree(szArglist);
    }

	WCHAR *env = GetEnvironmentTxtsW();
	for(WCHAR *ptr = env; *ptr; ptr++)
	{
		const WCHAR *b = ptr;
		if(*ptr)
			ptr++;
		while(*ptr && *ptr != '=')
			ptr++;
		WTxt varname = ToUtf32(b, i32(ptr - b));
		if(*ptr)
			ptr++;
		b = ptr;
		while(*ptr)
			ptr++;
		EnvMap().GetAdd(ToUpper(varname)) = ToUtf32(b, i32(ptr - b));
	}
	FreeEnvironmentTxtsW(env);

	CommonInit();
}

void AppInit__(i32 argc, tukk *argv)
{
	AppInitEnvironment__();
}
#endif

void AppExit__()
{
	Thread::ShutdownThreads();
	sMainRunning = false;
#ifdef TARGET_POSIX
	MemoryIgnoreLeaksBegin(); // Qt leaks on app exit...
#endif
}

#ifdef flagTURTLE

void Turtle_PutLink(const Txt& link);

void LaunchWebBrowser(const Txt& url)
{
	Turtle_PutLink(url);
}

#else

#if defined(TARGET_WIN32) && !defined(TARGET_WINCE)
static auxthread_t auxthread__ sShellExecuteOpen(uk str)
{
	ShellExecuteW(NULL, L"open", (WCHAR *)str, NULL, L".", SW_SHOWDEFAULT);
	free(str);
	return 0;
}

void LaunchWebBrowser(const Txt& url)
{
	Vec<WCHAR> wurl = ToSystemCharsetW(url);
	if ((z64)(ShellExecuteW(NULL, L"open", wurl, NULL, L".", SW_SHOWDEFAULT)) <= 32) {
		i32 l = sizeof(wchar) * wurl.GetCount() + 1;
		tuk curl = (tuk )malloc(l);
		memcpy(curl, wurl, l);
		StartAuxThread(sShellExecuteOpen, curl);
	}
}
#endif

#ifdef TARGET_POSIX
void    LaunchWebBrowser(const Txt& url)
{
#ifdef TARGET_MACOS
	IGNORE_RESULT(system("open " + url));
#else
	tukk  browser[] = {
		"htmlview", "xdg-open", "x-www-browser", "firefox", "konqueror", "opera", "epiphany", "galeon", "netscape"
	};
	for(i32 i = 0; i < __countof(browser); i++)
		if(system("which " + Txt(browser[i])) == 0) {
			Txt u = url;
			u.Replace("'", "'\\''");
			IGNORE_RESULT(
				system(Txt(browser[i]) + " '" + u + "' &")
			);
			break;
		}
#endif
}
#endif
#endif

Txt sDataPath;

void SetDataPath(tukk path)
{
	sDataPath = path;
}

Txt GetDataFile(tukk filename)
{
	if(sDataPath.GetCount())
		return AppendFName(sDataPath, filename);
	Txt s = GetEnv("DINRUS_MAIN__");
	return s.GetCount() ? AppendFName(s, filename) : GetExeDirFile(filename);
}

Txt LoadDataFile(tukk filename)
{
	return LoadFile(GetDataFile(filename));
}

Txt GetComputerName()
{
#if defined(TARGET_WIN32)
	WCHAR temp[256];
	*temp = 0;
	dword w = 255;
	::GetComputerNameW(temp, &w);
#else
	char temp[256];
	gethostname(temp, sizeof(temp));
#endif
	return temp;
}

Txt GetUserName()
{
#if defined(TARGET_WIN32)
	WCHAR temp[256];
	*temp = 0;
	dword w = 255;
	::GetUserNameW(temp, &w);
	return temp;
#else
	return Nvl(GetEnv("USER"), "root");
#endif
}

Txt GetDesktopManager()
{
#if defined(TARGET_WIN32) && !defined(TARGET_WINCE)
	return "windows";
#endif
#ifdef TARGET_POSIX
    if(GetEnv("GNOME_DESKTOP_SESSION_ID").GetCount())
		return "gnome";
	if(GetEnv("KDE_FULL_SESSION").GetCount() || GetEnv("KDEDIR").GetCount())
        return "kde";
	return GetEnv("DESKTOP_SESSION");
#endif
}

#if defined(TARGET_WIN32)

Txt GetShellFolder(i32 clsid)
{
	WCHAR path[MAX_PATH];
	if(SHGetFolderPathW(NULL, clsid, NULL, /*SHGFP_TYPE_CURRENT*/0, path) == S_OK)
		return FromSystemCharsetW(path);
	return Null;
}

Txt GetDesktopFolder()	  { return GetShellFolder(CSIDL_DESKTOP); }
Txt GetProgramsFolder()	  { return GetShellFolder(CSIDL_PROGRAM_FILES); }
Txt GetProgramsFolderX86() { return GetShellFolder(0x2a); }
Txt GetAppDataFolder()	  { return GetShellFolder(CSIDL_APPDATA);}
Txt GetMusicFolder()		  { return GetShellFolder(CSIDL_MYMUSIC);}
Txt GetPicturesFolder()	  { return GetShellFolder(CSIDL_MYPICTURES);}
Txt GetVideoFolder()		  { return GetShellFolder(CSIDL_MYVIDEO);}
Txt GetDocumentsFolder()	  { return GetShellFolder(/*CSIDL_MYDOCUMENTS*/0x0005);}
Txt GetTemplatesFolder()	  { return GetShellFolder(CSIDL_TEMPLATES);}
Txt GetProgramDataFolder() { return GetShellFolder(CSIDL_COMMON_APPDATA); }

#define MY_DEFINE_KNOWN_FOLDER(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
static const GUID name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }

MY_DEFINE_KNOWN_FOLDER(MY_FOLDERID_Downloads, 0x374de290, 0x123f, 0x4565, 0x91, 0x64, 0x39, 0xc4, 0x92, 0x5e, 0x46, 0x7b);

Txt GetDownloadFolder()
{
	static HRESULT (STDAPICALLTYPE * SHGetKnownFolderPath)(ukk rfid, DWORD dwFlags, HANDLE hToken, PWSTR *ppszPath);
	ONCELOCK {
		DllFn(SHGetKnownFolderPath, "shell32.dll", "SHGetKnownFolderPath");
	}
	if(SHGetKnownFolderPath) {
		PWSTR path = NULL;
		if(SHGetKnownFolderPath(&MY_FOLDERID_Downloads, 0, NULL, &path) == S_OK && path) {
			Txt s = FromSystemCharsetW(path);
			CoTaskMemFree(path);
			return s;
		}
	}
	return Null;
};
#endif

#ifdef TARGET_POSIX

Txt GetPathXdg(Txt xdgConfigHome, Txt xdgConfigDirs)
{
	Txt ret;
	if(FileExists(ret = AppendFName(xdgConfigHome, "user-dirs.dirs")))
		return ret;
	if(FileExists(ret = AppendFName(xdgConfigDirs, "user-dirs.defaults")))
		return ret;
	if(FileExists(ret = AppendFName(xdgConfigDirs, "user-dirs.dirs")))
		return ret;
	return Null;
}

Txt GetPathDataXdg(Txt fileConfig, tukk folder)
{
	TextSettings settings;
	settings.Load(fileConfig);
	Txt v = settings.Get(folder);
	if(*v == '\"')
		v = v.Mid(1);
	if(*v.Last() == '\"')
		v.Trim(v.GetCount() - 1);
	Txt r;
	tukk s = v;
	while(*s) {
		if(*s == '$') {
			CParser p(s + 1);
			p.NoSkipSpaces();
			p.Spaces();
			if(p.IsId()) {
				Txt id = p.ReadId();
				r.Cat(GetEnv(id));
				s = p.GetPtr();
			}
			else {
				r.Cat('$');
				s++;
			}
		}
		else
			r.Cat(*s++);
	}
	return r;
}

Txt GetShellFolder(tukk local, tukk users)
{
	Txt xdgConfigHome = GetEnv("XDG_CONFIG_HOME");
	if (xdgConfigHome == "")		// By default
		xdgConfigHome = AppendFName(GetHomeDir(), ".config");
	Txt xdgConfigDirs = GetEnv("XDG_CONFIG_DIRS");
	if (xdgConfigDirs == "")			// By default
		xdgConfigDirs = "/etc/xdg";
	Txt xdgFileConfigData = GetPathXdg(xdgConfigHome, xdgConfigDirs);
	Txt ret = GetPathDataXdg(xdgFileConfigData, local);
	if (ret == "" && *users != '\0')
		return GetPathDataXdg(xdgFileConfigData, users);
	else
		return ret;
}

Txt GetDesktopFolder()
{
	Txt ret = GetShellFolder("XDG_DESKTOP_DIR", "DESKTOP");
	if (ret.IsEmpty())
		return AppendFName(GetHomeDir(), "Desktop");
	else
		return ret;
}

Txt GetProgramsFolder()    { return Txt("/usr/bin"); }
Txt GetAppDataFolder()     { return GetHomeDir(); }
Txt GetMusicFolder()	      { return GetShellFolder("XDG_MUSIC_DIR", "MUSIC"); }
Txt GetPicturesFolder()    { return GetShellFolder("XDG_PICTURES_DIR", "PICTURES"); }
Txt GetVideoFolder()       { return GetShellFolder("XDG_VIDEOS_DIR", "VIDEOS"); }
Txt GetDocumentsFolder()   { return GetShellFolder("XDG_DOCUMENTS_DIR", "DOCUMENTS"); }
Txt GetTemplatesFolder()   { return GetShellFolder("XDG_TEMPLATES_DIR", "XDG_TEMPLATES_DIR"); }
Txt GetDownloadFolder()    { return GetShellFolder("XDG_DOWNLOAD_DIR", "XDG_DOWNLOAD_DIR"); }
Txt GetProgramDataFolder() { return Txt("/var/opt"); }

#endif


}