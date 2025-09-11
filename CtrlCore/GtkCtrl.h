	void Create(Ctrl *owner, bool popup);

	static gboolean GtkProc(GtkWidget *widget, GdkEvent *event, gpointer user_data);
	static void     IMCommit(GtkIMContext *context, gchar *str, gpointer user_data);
	static void     IMPreedit(GtkIMContext *context, gpointer user_data);
	static void     IMPreeditEnd(GtkIMContext *context, gpointer user_data);
	static void     IMLocation(Ctrl *w);

	static i32      DoButtonEvent(GdkEvent *event, bool press);
	static void     AddEvent(gpointer user_data, i32 type, const Val& value, GdkEvent *event);
	static void     FetchEvents(bool may_block);

/*
_DBG_
	static void     AddEvent(gpointer user_data, i32 type, const Val& value);
	static void     DoMouseEvent(i32 state, Point pos);
*/

	bool   DispatchMouseIn(i32 act, i32 zd);
	Image  GtkMouseEvent(i32 action, i32 act, i32 zd);
	void   GtkButtonEvent(i32 action);

	static bool IsWaitingEvent0(bool fetch);
	static bool ProcessEvent0(bool *quit, bool fetch);
	static bool ProcessEvents0(bool *quit, bool fetch);

	static void CaptureSync();

	static gboolean GtkEvent(GtkWidget *widget, GdkEvent *key, gpointer user_data);
	static gboolean GtkDraw(GtkWidget *widget, cairo_t *cr, gpointer data);

	static Point GetMouseInfo(GdkWindow *win, GdkModifierType& mod);
	static GdkDevice *GetMouseDevice();
#if GTK_CHECK_VERSION(3, 20, 0)
	static GdkSeat   *GetSeat();
#endif
	static bool MouseIsGrabbed();
	bool GrabMouse();
	static void UngrabMouse();
	
	static i32 scale; // in case GUI is scaling (e.g. in UHD mode)

	enum {
		EVENT_NONE = -15321,
		EVENT_TEXT,
		EVENT_FOCUS_CHANGE,
	};
	
	struct GEvent0 {
		i32        time;
		i32        windowid;
		i32        type;
		Val        value;
		Point      mousepos;
		guint      state;
		i32        count;

		bool       pen;
		bool       pen_barrel;
		bool       pen_inverted;
		bool       pen_eraser;
		double     pen_pressure;
		double     pen_rotation;
		Pointf     pen_tilt;
	};
	
	struct GEvent : Moveable<GEvent, GEvent0> {
		GdkEvent  *event;

		void Free();
		void Set(const GEvent& e);
		GEvent(const GEvent& e);
		void operator=(const GEvent& e);

		GEvent();
		~GEvent();
	};
	
	struct Win : Moveable<Win> {
		i32          id;
		GtkWidget   *gtk;
		GdkWindow   *gdk;
		Ptr<Ctrl>    ctrl;
		Vec<Rect> invalid; // areas invalidated to be processed at next opportunity
	};

	void   Proc();
	bool   SweepConfigure(bool wait);
	bool   SweepFocus(bool wait);
	void   SyncWndRect(const Rect& rect);

	static BiVec<GEvent>     Events;
	static Vec<Ptr<Ctrl>>    activePopup; // created with 'activate' flag - usually menu
	static Vec<Ptr<Ctrl>>    visiblePopup; // any popup visible on screen
	static Vec<Win>          wins;
	static Ptr<Ctrl>         grabwindow;
	static Ptr<Ctrl>         grabpopup;
	static Ptr<Ctrl>         sel_ctrl;
	static Ptr<Ctrl>         activeCtrl;
	static bool              invalids; // there are active invalid areas

	static i32 FindGtkId(i32 id);
	static i32 FindCtrl(Ctrl *ctrl);
	static i32 FindGtkWindow(GtkWidget *gtk);
	static i32 FindGdkWindow(GdkWindow *gdk);
	
	static Ctrl *GetTopCtrlFromId(i32 id);
	static Ctrl *GetTopCtrlFromId(gpointer user_data) { return GetTopCtrlFromId((u32)(uintptr_t)user_data); }

	static void SyncPopupCapture();
	void ReleasePopupCapture();
	
	static void FocusSync();
	static gboolean TimeHandler(GtkWidget *);
	static void InvalidateMousePos();
	static void StopGrabPopup();
	static void StartGrabPopup();
	static bool ReleaseWndCapture0();
	static void DoCancelPreedit();
	
	static Rect frameMargins;
	static Rect GetFrameMargins();

	static Index<Txt>      dnd_targets;
	static Txt             dnd_text_target;
	static Txt             dnd_image_target;
	static Txt             dnd_files_target;
	static Txt             dnd_data;
	static Txt             dnd_data_fmt;
	static bool            dnd_data_wait;
	static bool            dnd_events;
	static GtkWidget      *dnd_widget;
	static GdkDragContext *dnd_context;
	static guint           dnd_time;

	static Ptr<Ctrl>                          dnd_source;
	static const VecMap<Txt, ClipData> *dnd_source_data;
	static Vec<Txt>                     dnd_fmts;
	static i32                                dnd_result;
	static Image                              dnd_icon;
	
	static void            GtkSelectionDataSet(GtkSelectionData *selection_data, const Txt& fmt, const Txt& data);
	static void            GtkGetClipData(GtkClipboard *clipboard, GtkSelectionData *selection_data, guint info, gpointer user_data);
	static void            AddFmt(GtkTargetList *list, const Txt& fmt, i32 info);
	static GtkTargetList*  CreateTargetList(const VecMap<Txt, ClipData>& target);
	static Txt             GtkDataGet(GtkSelectionData *s);

	       void DndInit();
	       void DndExit();

	static void GtkDragBegin(GtkWidget *widget, GdkDragContext *context, gpointer user_data);
	static void GtkDragDelete(GtkWidget *widget, GdkDragContext *context, gpointer user_data);
	static void GtkDragGetData(GtkWidget *widget, GdkDragContext *context, GtkSelectionData *data, guint info, guint time, gpointer user_data);
	static void GtkDragDataReceived(GtkWidget *widget, GdkDragContext *context, gint x, gint y, GtkSelectionData *data, guint info, guint time, gpointer user_data);
	static gboolean GtkDragDrop(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data);
	static void GtkDragEnd(GtkWidget *widget, GdkDragContext *context, gpointer user_data);
	static gboolean GtkDragFailed(GtkWidget *widget, GdkDragContext *context, GtkDragResult result, gpointer user_data);
	static void GtkDragLeave(GtkWidget *widget, GdkDragContext *context, guint time, gpointer user_data);
	static gboolean GtkDragMotion(GtkWidget *widget, GdkDragContext *context, gint x, gint y, guint time, gpointer user_data);

	static Ctrl  *DragWnd(gpointer user_data);
	static void   DndTargets(GdkDragContext *context);
	static bool   IsDragAvailable(tukk fmt);
	static Txt    DragGet(tukk fmt);
	static PasteClip GtkDnd(GtkWidget *widget, GdkDragContext *context, gint x, gint y,
	                        guint time, gpointer user_data, bool paste);
	static bool   ProcessInvalids();

	friend void InitGtkApp(i32 argc, tuk *argv, tukk *envptr);
	friend void GuiPlatformGripResize(TopWindow *q);

public: // really private:
	struct Gclipboard {
		VecMap<Txt, ClipData> target;
		GtkClipboard *clipboard;

		Txt Get(const Txt& fmt);
		void   Put(const Txt& fmt, const ClipData& data);
		bool   IsAvailable(const Txt& fmt);
		void   Clear();

		Gclipboard(GdkAtom type);
	};

	static Gclipboard& gclipboard();
	static Gclipboard& gselection();
	static Txt         RenderPrimarySelection(const Val& fmt);

	static Vec<Event<>> hotkey;
	static Vec<dword>   keyhot;
	static Vec<dword>   modhot;
	static guint           MouseState;

	static i32    SCL(i32 x)                        { return scale * x; }
	static Rect   SCL(i32 x, i32 y, i32 cx, i32 cy) { return RectC(SCL(x), SCL(y), SCL(cx), SCL(cy)); }
	static double LSC(i32 x)                        { return (double)x / scale; }
	
	static i32    GetCaretBlinkTime()               { return 500; }
            
public:
	static void      EndSession()              {}
	static bool      IsEndSession()            { return false; }
	static void      PanicMsgBox(tukk title, tukk text);
	
	static Point     CurrentMousePos;
	static guint     CurrentState;
	static guint32   CurrentTime;
	static GEvent    CurrentEvent;

	GdkWindow *gdk() const;
	GtkWindow *gtk() const;

	static GdkFilterReturn RootKeyFilter(GdkXEvent *xevent, GdkEvent *event, gpointer data);
