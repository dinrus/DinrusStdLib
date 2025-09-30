class TopFrameDraw : public SystemDraw {
public:
	TopFrameDraw(Ctrl *ctrl, const Rect& r);
	~TopFrameDraw();

protected:
	HWND   hwnd;
};


Vec<WTxt>& coreCmdLine__();
Vec<WTxt> SplitCmdLine__(tukk cmd);

#ifdef TARGET_WINCE

#define GUI_APP_MAIN \
void GuiMainFn_();\
\
i32 APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, i32 nCmdShow) \
{ \
	DRX::coreCmdLine__() = DRX::SplitCmdLine__(DRX::FromSystemCharset(lpCmdLine)); \
	DRX::AppInitEnvironment__(); \
	DRX::Ctrl::InitWin32(hInstance); \
	try { \
		GuiMainFn_(); \
	} \
	DRX::Ctrl::ExitWin32(); \
	DRX::Ctrl::ShutdownThreads(); \
	DRX::AppExit__(); \
	return DRX::GetExitCode(); \
} \
\
void GuiMainFn_()

#else

#define GUI_APP_MAIN \
void GuiMainFn_();\
\
i32 APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR lpCmdLine, i32 nCmdShow) \
{ \
	DRX::AppInitEnvironment__(); \
	GUI_APP_MAIN_HOOK \
	DRX::Ctrl::InitWin32(hInstance); \
	DRX::AppExecute__(GuiMainFn_); \
	DRX::Ctrl::CloseTopCtrls(); \
	DRX::Ctrl::ExitWin32(); \
	DRX::Ctrl::ShutdownThreads(); \
	DRX::AppExit__(); \
	return DRX::GetExitCode(); \
} \
\
void GuiMainFn_()

#define DLL_APP_MAIN \
void _DllMainAppInit(); \
\
BOOL WINAPI DllMain(HINSTANCE hinstDll, DWORD fdwReason, LPVOID lpReserved) \
{ \
	if(fdwReason == DLL_PROCESS_ATTACH) { \
		AppInitEnvironment__(); \
		Ctrl::InitWin32(AppGetHandle()); \
		DRX::AppExecute__(_DllMainAppInit); \
	} \
	else \
	if(fdwReason == DLL_PROCESS_DETACH) { \
		Ctrl::ExitWin32(); \
	} \
	return true; \
} \
\
void _DllMainAppInit()

#endif

#ifndef TARGET_WINCE

class DHCtrl : public Ctrl {
public:
	virtual void    State(i32 reason);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void    NcCreate(HWND hwnd);
	virtual void    NcDestroy();
	virtual bool    PreprocessMessage(MSG& msg);

private:
	void OpenHWND();
	void SyncHWND();
	
	void RemoveActive();

	static Vec<DHCtrl *> all_active;

	static bool PreprocessMessageAll(MSG& msg);
	
	Rect current_pos = Null;
	i32  current_visible = Null;

	friend class Ctrl;

protected:
	void CloseHWND();
	HWND   hwnd;

public:
	HWND    GetHWND()                              { return hwnd; }
//	void    Refresh()                              { InvalidateRect(GetHWND(), NULL, false); }

	DHCtrl();
	~DHCtrl();
};

#endif

