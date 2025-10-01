enum {
	IDEXIT = 9999
};

class TopWindow : public Ctrl {
public:
	virtual Size     GetMinSize() const;
	virtual Size     GetStdSize() const;
	virtual void     Activate();
	virtual void     Deactivate();
	virtual bool     Accept();
	virtual void     Reject();
	virtual void     Paint(Draw& w);
	virtual bool     IsShowEnabled() const;
	virtual bool     Key(dword key, i32 count);
	virtual void     Close();
	virtual Txt   GetDesc() const;
	virtual void     ChildGotFocus();
	virtual void     SetMinSize(Size sz);

public:
	struct TopStyle : ChStyle<TopStyle> {
		Val background;
	};

protected:
	enum {
		TIMEID_DEFSYNCTITLE = Ctrl::TIMEID_COUNT,
		TIMEID_COUNT,
	};

private:
	struct Abreak : Pte<Abreak> {
		i32         ID;
		TopWindow *dlg;

		void    Accept()      { dlg->AcceptBreak(ID); }
		void    Reject()      { dlg->RejectBreak(ID); }
		void    Break()       { dlg->Break(ID); }
	};

	Array<Abreak> action;
	Ptr<Ctrl>     activefocus;
	Txt        backup;
	PaintRect     background;
	Size          minsize;
	bool          dokeys;
	bool          fullscreen;

	u8          center:2;
	
	i32           exitcode = 0;

	void          PlaceFocus();
	void          ActiveFocus0(Ctrl& ctrl);
	Abreak       *FindAddAction(i32 ID);
	Abreak       *FindAction(i32 ID);

	Rect        overlapped;

	void        SyncSizeHints();
	void        SyncTitle();
	void        SyncCaption();

	void        SetupRect(Ctrl *owner);
	
	void        FixIcons();

	void        DefaultBreak();

	enum { MINIMIZED, MAXIMIZED, OVERLAPPED };

	WTxt     title;
	bool        minimizebox:1;
	bool        maximizebox:1;
	bool        noclosebox:1;
	bool        sizeable:1;
	bool        tool:1;
	bool        frameless:1;
	bool        urgent:1;
	bool        close_rejects:1;
	u8        state;
	Image       icon, largeicon;

	const TopStyle *st;
	
	void        GuiPlatformConstruct();
	void        GuiPlatformDestruct();

#ifdef GUITARGET_TOPWINDOW_DECLS_INCLUDE
	#include GUITARGET_TOPWINDOW_DECLS_INCLUDE
#else
	GUITARGET_TOPWINDOW_DECLS
#endif

	friend class Ctrl;

public:
	virtual     void ShutdownWindow();

	Event<>     WhenClose;

	void        Backup();
	void        Restore();

	void        Break(i32 ID = IDEXIT);
	bool        AcceptBreak(i32 ID);
	void        RejectBreak(i32 ID);

	void        WorkAreaTrim();

	Callback    Breaker(i32 ID = IDEXIT);
	Callback    Acceptor(i32 ID);
	Callback    Rejector(i32 ID);

	TopWindow&  Breaker(Ctrl& m, i32 ID = -1);
	TopWindow&  Acceptor(Ctrl& m, i32 ID);
	TopWindow&  Rejector(Ctrl& m, i32 ID);

	TopWindow&  NoCenter()                          { center = 0; return *this; }
	TopWindow&  CenterOwner()                       { center = 1; return *this; }
	TopWindow&  CenterScreen()                      { center = 2; return *this; }

	void       Open(Ctrl *owner);
	void       Open();
	void       OpenMain();
	i32        Run(bool appmodal = false);
	i32        RunAppModal()                          { return Run(true); }
	i32        Execute();
	bool       ExecuteOK()                            { return Execute() == IDOK;     }
	bool       ExecuteCancel()                        { return Execute() == IDCANCEL; }
	
	i32        GetExitCode() const                    { return exitcode; }

	void       Minimize(bool effect = false);
	void       Maximize(bool effect = false);
	void       Overlap(bool effect = false);

	bool       IsMaximized() const                    { return state == MAXIMIZED; }
	bool       IsMinimized() const                    { return state == MINIMIZED; }
	bool       IsOverlapped() const                   { return state == OVERLAPPED; }

	TopWindow& ActiveFocus(Ctrl& ctrl)                { ActiveFocus0(ctrl); return *this; }

	TopWindow& Title(const WTxt& _title);
	TopWindow& Title(tukk s);
	const WTxt& GetTitle() const                   { return title; }
	TopWindow& Sizeable(bool b = true);
	TopWindow& NoSizeable()                           { return Sizeable(false); }
	bool       IsSizeable() const                     { return sizeable; }
	TopWindow& MinimizeBox(bool b = true);
	TopWindow& MaximizeBox(bool b = true);
	TopWindow& Zoomable(bool b = true)                { MinimizeBox(b); return MaximizeBox(b); }
	TopWindow& NoZoomable()                           { return Zoomable(false); }
	bool       IsZoomable() const                     { return maximizebox; }
	TopWindow& Background(const PaintRect& prect);
	const PaintRect& GetBackground() const            { return background; }
	TopWindow& ToolWindow(bool b = true);
	TopWindow& NoToolWindow()                         { return ToolWindow(false); }
	bool       IsToolWindow() const                   { return tool; }
	TopWindow& TopMost(bool b = true, bool stay_top = true);
	TopWindow& NoTopMost()                            { return TopMost(false); }
	bool       IsTopMost() const;
	TopWindow& FullScreen(bool b = true);
	bool       IsFullScreen() const                   { return fullscreen; }
	TopWindow& FrameLess(bool b = true);
	bool       IsFrameLess() const                    { return frameless; }
	TopWindow& Urgent(bool b = true);
	bool       IsUrgent() const                       { return urgent; }
	TopWindow& NoAccessKeysDistribution()             { dokeys = false; return *this; }
	TopWindow& NoCloseBox(bool b = true)              { noclosebox = b; return *this; }
	TopWindow& CloseBoxRejects(bool b = true)         { close_rejects = b; return *this; }

	TopWindow& Icon(const Image& m);
	TopWindow& LargeIcon(const Image& m);
	TopWindow& Icon(const Image& smallicon, const Image& largeicon);

	static const TopStyle& StyleDefault();
	TopWindow&  SetStyle(const TopStyle& s);
	
	Image      GetIcon() const                        { return icon; }
	Image      GetLargeIcon() const                   { return largeicon; }

	void       SerializePlacement(Stream& s, bool reminimize = false);
	
	static void ShutdownWindows();

	typedef TopWindow CLASSNAME;

	TopWindow();
	~TopWindow();
};

void Maxisize(TopWindow& win, i32 screencxmax);

void ArrangeOKCancel(Ctrl& ok, Ctrl& cancel);

i32  SwapOKCancel();
void SwapOKCancel_Write(i32 b);

void SetLayout_Size(Ctrl& ctrl, Size sz);
void SetLayout_Size(Ctrl& ctrl, i32 cx, i32 cy);

template <class L>
void InitLayout(Ctrl& ctrl, L& layout)
{
	InitLayout(ctrl, layout, layout, layout);
}

template <class T>
void CtrlLayout(T& ctrl) {
	InitLayout(ctrl, ctrl, ctrl, ctrl);
	Size sz = T::GetLayoutSize();
	SetLayout_Size(ctrl, sz);
	ctrl.SetRect(sz);
}

template <class T>
void CtrlLayout(T& ctrl, tukk title) {
	CtrlLayout(ctrl);
	ctrl.Title(title);
}

template <class T>
void CtrlLayoutOK(T& ctrl, tukk title) {
	CtrlLayout(ctrl, title);
	ctrl.Acceptor(ctrl.ok, IDOK);
	ctrl.ok.Ok();
}

template <class T>
void CtrlLayoutCancel(T& ctrl, tukk title) {
	CtrlLayout(ctrl, title);
	ctrl.Rejector(ctrl.cancel, IDCANCEL);
	ctrl.cancel.Cancel();
}

template <class T>
void CtrlLayoutOKCancel(T& ctrl, tukk title) {
	CtrlLayoutOK(ctrl, title);
	ctrl.Rejector(ctrl.cancel, IDCANCEL);
	ctrl.cancel.Cancel();
	ArrangeOKCancel(ctrl.ok, ctrl.cancel);
}

template <class T>
void CtrlLayoutExit(T& ctrl, tukk title) {
	CtrlLayout(ctrl, title);
	ctrl.Acceptor(ctrl.exit, IDEXIT);
	ctrl.exit.Exit();
}
