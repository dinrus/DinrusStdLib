class TopFrameDraw : public SystemDraw {
#if GTK_CHECK_VERSION(3, 22, 0)
	GdkDrawingContext *ctx;
#endif
public:
	TopFrameDraw(Ctrl *ctrl, const Rect& r);
	~TopFrameDraw();
};

class DHCtrl : Ctrl {};

void InitGtkApp(i32 argc, tuk *argv, tukk *envptr);
void ExitGtkApp();

#define GUI_APP_MAIN \
void GuiMainFn_(); \
\
i32 main(i32 argc, tuk *argv, tukk *envptr) { \
	DRX::AppInit__(argc, (tukk *)argv, envptr); \
	GUI_APP_MAIN_HOOK \
	DRX::InitGtkApp(argc, argv, envptr); \
	DRX::AppExecute__(GuiMainFn_); \
	DRX::Ctrl::CloseTopCtrls(); \
	DRX::ExitGtkApp(); \
	DRX::Ctrl::ShutdownThreads(); \
	DRX::AppExit__(); \
	return DRX::GetExitCode(); \
} \
 \
void GuiMainFn_()
