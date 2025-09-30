	bool         ignoretakefocus:1;
protected:
	struct XWindow {
		Ptr<Ctrl>    ctrl;
		bool         exposed;
		Vec<Rect>    invalid;
		Ptr<Ctrl>    owner;
		Ptr<Ctrl>    last_active;
		XIC          xic;
	};

private:
	static ArrayMap<Window, XWindow>& Xwindow();
	static i32       Xbuttons;
	static i32       Xbuttontime;
	static Point     Xbuttonpos;
	static Window    grabWindow, focusWindow;
	static Point     mousePos;
	static i32       PopupGrab;
	static Ptr<Ctrl> popupWnd;
	static Index<Txt> sel_formats;
	static Ptr<Ctrl>     sel_ctrl;
	static void     ProcessEvent(XEvent *event);
	static void     TimerAndPaint();
	static void     ProcessEvent(XEvent& event);
	       void     Invalidate(XWindow& xw, const Rect& r);
	       void     DoPaint(const Vec<Rect>& invalid);
	       void     SetLastActive(XWindow *w, Ctrl *la);
	       XWindow *GetXWindow();
	static void     SyncMousePos();
	static void     ReleaseGrab();
	static Vec<Event<> > hotkey;
	static Vec<dword> modhot;
	static Vec<dword> keyhot;

	       void    StartPopupGrab();
	static void    EndPopupGrab();
	static void    SyncIMPosition();

	friend bool    GetMouseRight();
	friend bool    GetMouseLeft();
	friend bool    GetMouseMiddle();
	friend Point   GetMousePos();
	friend void    sPanicMessageBox(tukk title, tukk text);

protected:
	       void    Create(Ctrl *owner, bool redirect, bool savebits);
	       void    SyncExpose();
	       void    TakeFocus();
	static Window  GetXServerFocusWindow();
	       void    AddGlobalRepaint();
	static void    KillFocus(Window w);
	static void    FocusSync();

	       void    DropEvent(XWindow& w, XEvent *event);
	static void    DropStatusEvent(XEvent *event);
	
	static Index<Txt> drop_formats;
	
	static Txt     Unicode(const WTxt& w);
	static WTxt    Unicode(const Txt& s);
	static bool    ClipHas(i32 type, tukk fmt);
	static Txt     ClipGet(i32 type, tukk fmt);

	XWindow*       AddXWindow(Window &w);
	void           RemoveXWindow(Window &w);
	XWindow*       XWindowFromWindow(Window &w);

public:
	struct Xclipboard {
		Window win;

		VecMap<i32, ClipData> data;

		Txt    Read(i32 fmt, i32 selection, i32 property);
		void   Write(i32 fmt, const ClipData& data);
		bool   IsAvailable(i32 fmt, tukk type);

		void   Clear()                     { data.Clear(); }
		void   Request(XSelectionRequestEvent *se);

		Xclipboard();
		~Xclipboard();
	};

	static Xclipboard& xclipboard();

	static i32  Xeventtime;

	static XIM  xim;

	void DnD(Window src, bool paste);

	virtual void    EventProc(XWindow& w, XEvent *event);
	virtual bool    HookProc(XEvent *event);
	Window          GetWindow() const         { return utop ? utop->window : None; }
	static i32      GetCaretBlinkTime()               { return 500; }
	static  Ctrl   *CtrlFromWindow(Window w);
	static bool     TrapX11Errors();
	static void     UntrapX11Errors(bool b);

	Window GetParentWindow(void) const;
	Ctrl *GetParentWindowCtrl(void) const;
	Rect GetRectInParentWindow(void) const;

	static void SyncNativeWindows(void);
public:
	static void InitX11(tukk display);
	static void ExitX11();
	static void GuiFlush()                              { XFlush(Xdisplay); }
