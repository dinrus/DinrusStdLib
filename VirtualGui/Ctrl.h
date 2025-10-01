private:
	static Ptr<Ctrl>      desktop;
	static Vec<Ctrl *> topctrl;
	static bool           invalid;

	static Point fbCursorPos;
	static Image fbCursorImage;
	
	static Rect  fbCaretRect;
	static i32   fbCaretTm;
	
	static bool  fbEndSession;
	static z64 fbEventLoop;
	static z64 fbEndSessionLoop;
	
	static void CursorSync();
	
	i32 FindTopCtrl() const;
	static Rect GetClipBound(const Vec<Rect>& inv, const Rect& r);
	static void DoPaint();
	static void SyncTopWindows();

	void DestroyWnd();

	void NewTop()                       { SetTop(new Top); GetTop()->owner_window = NULL; }
	void PutForeground();
	static void MouseEventFB(Ptr<Ctrl> t, i32 event, Point p, i32 zdelta);

	static void DrawLine(const Vec<Rect>& clip, i32 x, i32 y, i32 cx, i32 cy, bool horz,
	                     u8k *pattern, i32 animation);
	static void DragRectDraw0(const Vec<Rect>& clip, const Rect& rect, i32 n,
	                          u8k *pattern, i32 animation);

	friend class TopWindowFrame;
	friend class SystemDraw;
	friend struct DnDLoop;

	void  SetOpen(bool b)               { isopen = b; }

	static void DeleteDesktopTop();

	static i32    GetCaretBlinkTime()               { return 500; }
	
protected:
	static i32 PaintLock;

public:
	static void DoMouseFB(i32 event, Point p, i32 zdelta = 0);
	static bool DoKeyFB(dword key, i32 cnt);

	static void InitFB();
	static void ExitFB();
	static void EndSession();
	
	static void PaintAll()                     { DoPaint(); }

	static void  SetDesktop(Ctrl& q);
	static Ctrl *GetDesktop()                  { return desktop; }
	static void  SetDesktopSize(Size sz);
	
	static void Invalidate()                   { invalid = true; }

	void DragRectDraw(const Rect& rect1, const Rect& rect2, const Rect& clip, i32 n,
	                  Color color, i32 type, i32 animation);

	static Ctrl *FindMouseTopCtrl();

	static void PaintScene(SystemDraw& draw);
	static void PaintCaretCursor(SystemDraw& draw);
	
	enum { DRAWDRAGRECT_SCREEN = 0x8000 };
	