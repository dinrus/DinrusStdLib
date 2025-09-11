class TopFrameDraw : public SystemDraw {
public:
	TopFrameDraw(Ctrl *ctrl, const Rect& r);
	~TopFrameDraw();
};

Vec<WTxt>& coreCmdLine__();
Vec<WTxt> SplitCmdLine__(tukk cmd);

void CocoInit(i32 argc, tukk *argv, tukk *envptr);
void CocoExit();

#ifdef TARGET_POSIX
#define GUI_APP_MAIN \
void GuiMainFn_(); \
\
i32 main(i32 argc, tukk *argv, tukk *envptr) { \
	DRX::AppInit__(argc, (tukk *)argv, envptr); \
	GUI_APP_MAIN_HOOK \
	DRX::CocoInit(argc, argv, envptr); \
	GuiMainFn_(); \
	DRX::Ctrl::CloseTopCtrls(); \
	DRX::CocoExit(); \
	DRX::Ctrl::ShutdownThreads(); \
	DRX::AppExit__(); \
	return DRX::GetExitCode(); \
} \
 \
void GuiMainFn_()

#endif

class DHCtrl : Ctrl {};

enum {
	SF_NSDocumentDir,
	SF_NSUserDir,
	SF_NSDesktopDir,
	SF_NSDownloadsDir,
	SF_NSMoviesDir,
	SF_NSMusicDir,
	SF_NSPicturesDir,
};

Txt GetSpecialDir(i32 i);

void   CocoBeep();