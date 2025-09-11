class TopFrameDraw : public SystemDraw {
public:
	TopFrameDraw(Ctrl *ctrl, const Rect& r);
	~TopFrameDraw();

protected:
	bool   caret;
};


#define GUI_APP_MAIN \
void GuiMainFn_(); \
\
i32 main(i32 argc, tukk *argv, tukk *envptr) { \
	DRX::AppInit__(argc, argv, envptr); \
	GUI_APP_MAIN_HOOK \
	DRX::Ctrl::InitX11(NULL); \
	DRX::AppExecute__(GuiMainFn_); \
	DRX::Ctrl::ExitX11(); \
	DRX::Ctrl::ShutdownThreads(); \
	DRX::AppExit__(); \
	return DRX::GetExitCode(); \
} \
 \
void GuiMainFn_()

class DHCtrl : public Ctrl {
	i32 isError;
	bool isMapped;
	Size CurrentSize;
	XVisualInfo* UserVisualInfo;
	Txt ErrorMessage;

	void MapWindow(bool map);
	bool Init(void);
	void Terminate(void);

protected:
	Visual     *GetVisual(void);
	XVisualInfo GetVisualInfo(void);

	virtual XVisualInfo *CreateVisual(void) {return 0;}
	virtual void SetAttributes(u64 &ValMask, XSetWindowAttributes &attr) {}
	virtual void Paint(Draw &draw) {}
	virtual void BeforeInit(void) {}
	virtual void AfterInit(bool Error) {}
	virtual void BeforeTerminate(void) {}
	virtual void AfterTerminate(void) {}
	virtual void Resize(i32 w, i32 h) {}

	void SetError(bool err) { isError = err; }
	void SetErrorMessage(Txt const &msg) { ErrorMessage = msg; }

	virtual void State(i32 reason);

	Window   hwnd;
	bool isInitialized;

public:
	typedef DHCtrl CLASSNAME;

	bool   IsInitialized(void) { return isInitialized; }

	bool   GetError(void) { return isError; }
	Txt GetErrorMessage(void) { return ErrorMessage; }

	DHCtrl();
	~DHCtrl();
};
