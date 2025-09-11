Txt  GetEnv(tukk id);
bool    SetEnv(tukk name, tukk value);

Txt  GetExeFilePath();
Txt  GetExeTitle();
Txt  GetExeFolder();
Txt  GetExeDirFile(tukk fp);

#ifdef TARGET_COCOA
Txt GetAppFolder();
#endif

Txt GetAppName();
void SetAppName(const Txt& name);

Txt  GetHomeDirFile(tukk fp);
Txt  GetHomeDir();

void    SetHomeDir(tukk dir);

void    UseHomeDirConfig(bool b = true);

Txt  GetTempDir();
Txt  TempFile(tukk filename);

Txt  GetConfigFolder();
Txt  ConfigFile(tukk file);
Txt  ConfigFile();

void    SetConfigGroup(tukk group);
Txt  GetConfigGroup();


const Vec<Txt>& CommandLine();
void   CommandLineRemove(i32 i, i32 count = 1);
Txt GetArgv0();
const VecMap<Txt, Txt>& Environment();

void    SetExitCode(i32 code);
i32     GetExitCode();

bool    IsMainRunning();

struct ExitExc {};

void Exit(i32 code = 1);

void AppExit__();
void AppExecute__(void (*app)());

#ifdef TARGET_WIN32

void AppInit__(i32 argc, tukk *argv);
void AppInitEnvironment__();

#define CONSOLE_APP_MAIN \
void ConsoleMainFn_(); \
 \
i32 main(i32 argc, tuk argv[]) { \
	DRX::AppInit__(argc, (tukk *)argv); \
	DRX::AppExecute__(ConsoleMainFn_); \
	DRX::AppExit__(); \
	return DRX::GetExitCode(); \
} \
 \
void ConsoleMainFn_()

#endif

#ifdef TARGET_POSIX

void AppInit__(i32 argc, tukk *argv, tukk *envptr);

#define CONSOLE_APP_MAIN \
void ConsoleMainFn_(); \
 \
i32 main(i32 argc, tukk *argv, tukk *envptr) { \
	DRX::AppInit__(argc, argv, envptr); \
	DRX::AppExecute__(ConsoleMainFn_); \
	DRX::AppExit__(); \
	return DRX::GetExitCode(); \
} \
 \
void ConsoleMainFn_()

#endif

Txt  GetDataFile(tukk filename);
Txt  LoadDataFile(tukk filename);
void    SetDataPath(tukk path);

void    LaunchWebBrowser(const Txt& url);

Txt GetComputerName();
Txt GetUserName();
Txt GetDesktopManager();

Txt GetDesktopFolder();
Txt GetProgramsFolder();
#ifdef TARGET_WIN32
Txt GetProgramsFolderX86();
#endif
Txt GetAppDataFolder();
Txt GetMusicFolder();
Txt GetPicturesFolder();
Txt GetVideoFolder();
Txt GetDocumentsFolder();
Txt GetTemplatesFolder();
Txt GetDownloadFolder();
Txt GetProgramDataFolder();

void   InstallCrashHook(void (*h)());