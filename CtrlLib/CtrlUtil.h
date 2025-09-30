void Animate(Ctrl& c, const Rect& target, i32 type = -1);
void Animate(Ctrl& c, i32 x, i32 y, i32 cx, i32 cy, i32 type = -1);

bool CtrlLibDisplayError(const Val& ev);

bool EditText(Txt& s, tukk title, tukk label, wchar (*filter)(wchar), i32 maxlen = 0);
bool EditText(Txt& s, tukk title, tukk label, i32 maxlen = 0);
bool EditTextNotNull(Txt& s, tukk title, tukk label, wchar (*filter)(wchar), i32 maxlen = 0);
bool EditTextNotNull(Txt& s, tukk title, tukk label, i32 maxlen = 0);

bool EditText(WTxt& s, tukk title, tukk label, wchar (*filter)(wchar), i32 maxlen = 0);
bool EditText(WTxt& s, tukk title, tukk label, i32 maxlen = 0);
bool EditTextNotNull(WTxt& s, tukk title, tukk label, wchar (*filter)(wchar), i32 maxlen = 0);
bool EditTextNotNull(WTxt& s, tukk title, tukk label, i32 maxlen = 0);

bool EditNumber(i32& n, tukk title, tukk label, i32 min = INT_MIN, i32 max = INT_MAX, bool notnull = false);
bool EditNumber(double& n, tukk title, tukk label, double min = -DBL_MAX, double max = DBL_MAX, bool notnull = false);

bool EditDateDlg(Date& d, tukk title, tukk label, Date min = Date::Low(), Date max = Date::High(), bool notnull = false);

void Show2(Ctrl& ctrl1, Ctrl& ctrl, bool show = true);
void Hide2(Ctrl& ctrl1, Ctrl& ctrl);

#ifndef TARGET_WINCE //СДЕЛАТЬ?
void UpdateFile(tukk filename);
void SelfUpdate();
bool SelfUpdateSelf();
#endif

void WindowsList();
void WindowsMenu(Bar& bar);

class DelayCallback : public Pte<DelayCallback> {
	Event<>  target;
	i32      delay;

public:
	void     Invoke();
	void     operator<<=(Event<> x)   { target = x; }
	void     SetDelay(i32 ms)         { delay = ms; }
	Event<>  Get()                    { return callback(this, &DelayCallback::Invoke); }
	Event<>  operator~()              { return Get(); }
	operator Event<>()                { return Get(); }

	DelayCallback()                   { delay = 2000; }
	~DelayCallback()                  { KillTimeCallback(this); }
};

#ifdef GUI_WIN
struct Win32PrintDlg_;
#endif

#ifndef TARGET_WINCE

#ifndef VIRTUALGUI

class PrinterJob {
#ifdef GUI_WIN
	One<Win32PrintDlg_> pdlg;
	bool Execute0(bool dodlg);
#endif
#ifdef TARGET_POSIX
	Size                pgsz;
	Size                GetDefaultPageSize(Txt *name = NULL);
#endif
	One<Draw>           draw;
	Vec<i32>         page;
	i32                 from, to, current;
	bool                landscape;
	Txt              name;
	Txt              options;
	bool                dlgSuccess;
	bool                Execute0();

public:
	Draw&               GetDraw();
	operator            Draw&()                         { return GetDraw(); }
	const Vec<i32>&  GetPages() const                { return page; }
	i32                 operator[](i32 i) const         { return page[i]; }
	i32                 GetPageCount() const            { return page.GetCount(); }

	bool                Execute();

	PrinterJob& Landscape(bool b = true)                { landscape = b; return *this; }
	PrinterJob& MinMaxPage(i32 minpage, i32 maxpage);
	PrinterJob& PageCount(i32 n)                        { return MinMaxPage(0, n - 1); }
	PrinterJob& CurrentPage(i32 currentpage);
	PrinterJob& Name(tukk _name)                 { name = _name; return *this; }

	PrinterJob(tukk name = NULL);
	~PrinterJob();
};

#endif

#endif

#ifdef GUI_X11

class TrayIcon : Ctrl {
	virtual bool HookProc(XEvent *event);
	virtual void Paint(Draw& draw);
	virtual void LeftDown(Point p, dword keyflags);
	virtual void LeftUp(Point p, dword keyflags);
	virtual void LeftDouble(Point p, dword keyflags);
	virtual void RightDown(Point p, dword keyflags);

private:
	void AddToTray();

	Window traywin;
	Image  icon;
	void DoMenu(Bar& bar);
	void Call(i32 code, u64 d1, u64 d2, u64 d3);
	void Message(i32 type, tukk title, tukk text, i32 timeout);

public:
	virtual void    LeftDown();
	virtual void    LeftUp();
	virtual void    LeftDouble();
	virtual void    Menu(Bar& bar);
	virtual void    BalloonLeftDown();
	virtual void    BalloonShow();
	virtual void    BalloonHide();
	virtual void    BalloonTimeout();

	Event<>         WhenLeftDown;
	Event<>         WhenLeftUp;
	Event<>         WhenLeftDouble;
	Event<Bar&> WhenBar;
	Event<>         WhenBalloonLeftDown;
	Event<>         WhenBalloonShow;
	Event<>         WhenBalloonHide;
	Event<>         WhenBalloonTimeout;

	void            Break()                                { EndLoop(0); }
	void            Run()                                  { EventLoop(this); }

	void            Show(bool b = true);
	void            Hide()                                 { Show(false); }
	bool            IsVisible() const                      { return true; }

	void            Info(tukk title, tukk text, i32 timeout = 10)    { Message(1, title, text, timeout); }
	void            Warning(tukk title, tukk text, i32 timeout = 10) { Message(2, title, text, timeout); }
	void            Error(tukk title, tukk text, i32 timeout = 10)   { Message(3, title, text, timeout); }

	TrayIcon&  Icon(const Image &img)                      { icon = img; Refresh(); return *this; }
	TrayIcon&  Tip(tukk text)                       { Ctrl::Tip(text); return *this; }

	typedef TrayIcon CLASSNAME;

	TrayIcon();
};

#endif

#ifdef GUI_GTK

class TrayIcon {
private:
	GtkStatusIcon *tray_icon;
	Txt         tooltip;
	ImageGdk       image;
	bool           active;

	static gboolean DoButtonPress(GtkStatusIcon *, GdkEventButton *e, gpointer user_data);
	static gboolean DoButtonRelease(GtkStatusIcon *, GdkEventButton *e, gpointer user_data);
	static void     PopupMenu(GtkStatusIcon *, guint, guint32, gpointer user_data);
	static void     DoActivate(GtkStatusIcon *, gpointer user_data);

	void DoMenu(Bar& bar);
	void ExecuteMenu();

	void Sync();

	void Message(i32 type, tukk title, tukk text, i32 timeout);

public:
	virtual void    Menu(Bar& bar);
	virtual void    LeftDown();
	virtual void    LeftUp();
	virtual void    LeftDouble();

	Event<>         WhenLeftDown;
	Event<>         WhenLeftUp;
	Event<>         WhenLeftDouble;
	Event<Bar&> WhenBar;

	void            Break();
	void            Run();

	void            Show(bool b = true);
	void            Hide()                                 { Show(false); }
	bool            IsVisible() const;

	// Not implemented by GTK:
	void            Info(tukk title, tukk text, i32 timeout = 10)    { Message(1, title, text, timeout); }
	void            Warning(tukk title, tukk text, i32 timeout = 10) { Message(2, title, text, timeout); }
	void            Error(tukk title, tukk text, i32 timeout = 10)   { Message(3, title, text, timeout); }

	TrayIcon&  Icon(const Image &img)                      { if(image.Set(img)) Sync(); return *this; }
	TrayIcon&  Tip(tukk text)                       { if(tooltip != text) tooltip = text; Sync(); return *this; }

	typedef TrayIcon CLASSNAME;

	TrayIcon();
	~TrayIcon();
};

#endif

#ifdef GUI_WIN

class TrayIcon : private Ctrl {
	Image           icon;
	bool            visible;
	Txt          tip;
	NOTIFYICONDATAW nid;
	HWND            hwnd;

	void Notify(dword msg);
	void DoMenu(Bar& bar);
	void Message(i32 type, tukk title, tukk text, i32 timeout = 10);

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
	virtual void    Menu(Bar& bar);
	virtual void    LeftDown();
	virtual void    LeftUp();
	virtual void    LeftDouble();
	virtual void    BalloonLeftDown();
	virtual void    BalloonShow();
	virtual void    BalloonHide();
	virtual void    BalloonTimeout();

	Event<>         WhenLeftDown;
	Event<>         WhenLeftUp;
	Event<>         WhenLeftDouble;
	Event<Bar&> WhenBar;
	Event<>         WhenBalloonLeftDown;
	Event<>         WhenBalloonShow;
	Event<>         WhenBalloonHide;
	Event<>         WhenBalloonTimeout;

	void            Show(bool b = true);
	void            Hide()                                 { Show(false); }
	bool            IsVisible() const                      { return visible; }
	void            Break()                                { EndLoop(0); }
	void            Run()                                  { EventLoop(this); }

	void            Info(tukk title, tukk text, i32 timeout = 10)    { Message(1, title, text, timeout); }
	void            Warning(tukk title, tukk text, i32 timeout = 10) { Message(2, title, text, timeout); }
	void            Error(tukk title, tukk text, i32 timeout = 10)   { Message(3, title, text, timeout); }

	TrayIcon&  Icon(const Image &img);
	TrayIcon&  Tip(tukk text);

	typedef TrayIcon CLASSNAME;

	TrayIcon();
	~TrayIcon();
};

class FileSelNative {
protected:
	struct FileType : Moveable<FileType> {
		Txt name;
		Txt ext;
	};

	Vec<FileType> type;

	i32    activetype;
	Txt activedir;
	Txt defext;
	bool   rdonly;
	bool   multi;
	bool   readonly;
	bool   asking;

	Vec<Txt> filename;

public:
	void Serialize(Stream& s);

	void New()                                   { filename.Clear(); }
	bool IsNew() const                           { return filename.IsEmpty(); }
	bool Execute(bool open, tukk title = NULL);
	bool ExecuteOpen(tukk title = NULL)   { return Execute(true, title); }
	bool ExecuteSaveAs(tukk title = NULL) { return Execute(false, title); }
	bool ExecuteSelectDir(tukk title = NULL);

	Txt Get() const;
	void  Set(const Txt& s)                   { filename.At(0) = s; }

	operator Txt() const                      { return Get(); }
	void operator=(const Txt& s)              { Set(s); }

	Txt operator~() const                     { return Get(); }
	void operator<<=(const Txt& s)            { Set(s); }

	i32   GetCount() const                       { return filename.GetCount(); }
	const Txt& operator[](i32 i) const        { return filename[i]; }

	bool   GetReadOnly() const                   { return readonly; }
	Txt GetActiveDir() const                  { return activedir; }

	FileSelNative& Type(tukk name, tukk ext);
	FileSelNative& AllFilesType();
	FileSelNative& ActiveDir(const Txt& dir)   { activedir = dir; return *this; }
	FileSelNative& ActiveType(i32 i)              { activetype = i; return *this;  }
	FileSelNative& DefaultExt(tukk ext)    { defext = ext; return *this; }
	FileSelNative& Multi(bool b = true)           { multi = b; return *this; }
	FileSelNative& ReadOnlyOption(bool b = true)  { rdonly = b; return *this; }
	FileSelNative& Asking(bool b = true)          { asking = b; return *this; }
	FileSelNative& NoAsking()                     { return Asking(false); }

	FileSelNative();
};

typedef FileSelNative FileSelector;

#endif

#ifdef GUI_X11
typedef FileSel FileSelNative;
#endif

#if defined(GUI_GTK) || defined(TARGET_COCOA)
class FileSelNative {
	Vec<Txt> path;
	Vec< Tuple2<Txt, Txt> > type;

	Txt ipath;
	bool   confirm;
	bool   multi;
	bool   hidden;
	i32    activetype;
	bool   Execute0(i32 mode, tukk title);

public:
	void Serialize(Stream& s);

	void New()                                            { path.Clear(); }
	bool IsNew() const                                    { return path.IsEmpty(); }

	bool   Execute(bool open, tukk title = NULL)   { return Execute0(open, title); }
	bool   ExecuteOpen(tukk title = NULL)          { return Execute(true, title); }
	bool   ExecuteSaveAs(tukk title = NULL)        { return Execute(false, title); }
	bool   ExecuteSelectDir(tukk title = NULL)     { return Execute0(2, title); }

	Txt Get() const                                    { return path.GetCount() ? path[0] : Txt::GetVoid(); }
	operator Txt() const                               { return Get(); }
	Txt operator~() const                              { return Get(); }

	void   Set(const Txt& s)                           { ipath = s; }
	void   operator=(const Txt& s)                     { Set(s); }
	void   operator<<=(const Txt& s)                   { Set(s); }

	i32    GetCount() const                               { return path.GetCount(); }
	const  Txt& operator[](i32 i) const                { return path[i]; }

	Txt GetActiveDir() const                           { return ipath; }

	FileSelNative& Type(tukk name, tukk ext) { type.Add(MakeTuple(Txt(name), Txt(ext))); return *this; }
	FileSelNative& AllFilesType();
	FileSelNative& Asking(bool b = true)                   { confirm = b; return *this; }
	FileSelNative& NoAsking()                              { return Asking(false); }
	FileSelNative& Multi(bool b = true)                    { multi = b; return *this; }
	FileSelNative& ShowHidden(bool b = true)               { hidden = b; return *this; }
	FileSelNative& ActiveDir(const Txt& dir)            { ipath = dir; return *this; }
	FileSelNative& ActiveType(i32 i)                       { activetype = i; return *this; }

	FileSelNative();
};

typedef FileSelNative FileSelector;
#endif

class CtrlMapper {
	bool toctrls = true;

public:
	template <class T>
	CtrlMapper& operator()(Ctrl& ctrl, T& val) { if(toctrls) ctrl <<= val; else val = ~ctrl; return *this; }

	CtrlMapper& ToCtrls()                      { toctrls = true; return *this; }
	CtrlMapper& ToVals()                     { toctrls = false; return *this; }
};

class CtrlRetriever {
public:
	struct Item {
		virtual void Set() {}
		virtual void Retrieve() = 0;
		virtual ~Item() {}
	};

private:
	struct CtrlItem0 : Item {
		Ctrl  *ctrl;
	};

	template <class T>
	struct CtrlItem : CtrlItem0 {
		T     *value;

		virtual void Set()       { *ctrl <<= *value; }
		virtual void Retrieve()  { *value = ~*ctrl; }
		virtual ~CtrlItem() {}
	};

	Array<Item> item;

public:
	void Put(Item *newitem)                       { item.Add(newitem); }

	void Put(One<Item>&& newitem)                 { item.Add(newitem.Detach()); }

	template <class T>
	void Put(Ctrl& ctrl, T& val);

	template <class T>
	CtrlRetriever& operator()(Ctrl& ctrl, T& val) { Put(ctrl, val); return *this; }

	void Set();
	void Retrieve();

	Event<>  operator^=(Event<> cb);
	Event<>  operator<<(Event<> cb);

	void Reset()                                  { item.Clear(); }

// Backward compatibility
	Event<>  operator<<=(Event<> cb)              { return *this ^= cb; }
};

template <class T>
void CtrlRetriever::Put(Ctrl& ctrl, T& val)
{
	CtrlItem<T> *m = new CtrlItem<T>();
	m->ctrl = &ctrl;
	m->value = &val;
	m->Set();
	Put(m);
}

class IdCtrls {
protected:
	struct Item {
		Id    id;
		Ctrl *ctrl;
	};
	Array<Item> item;

public:
	void        Reset()                              { item.Clear(); }

	void            Add(Id id, Ctrl& ctrl);
	IdCtrls&        operator()(Id id, Ctrl& ctrl)    { Add(id, ctrl); return *this; }
	i32             GetCount() const                 { return item.GetCount(); }
	Ctrl&           operator[](i32 i)                { return *item[i].ctrl; }
	const Ctrl&     operator[](i32 i) const          { return *item[i].ctrl; }
	Id              GetKey(i32 i) const              { return item[i].id; }
	Id              operator()(i32 i) const          { return item[i].id; }

	bool            Accept();
	void            ClearModify();
	bool            IsModified();
	void            Enable(bool b = true);
	void            Disable()                        { Enable(false); }
	void            SetNull();

	Event<>         operator<<(Event<> action);
	Event<>         operator^=(Event<> action);

	ValMap        Get() const;
	void            Set(const ValMap& m);
	ValMap        operator~() const                { return Get(); }
	const ValMap& operator<<=(const ValMap& m)   { Set(m); return m; }
};

class FileSelButton : public FileSel
{
public:
	enum MODE { MODE_OPEN, MODE_SAVE, MODE_DIR };
	FileSelButton(MODE mode = MODE_OPEN, tukk title = NULL);

	void               Attach(Ctrl& parent);
	void               Detach();
	void               Title(Txt t)      { title = t; }
	Txt             GetTitle() const     { return title; }

	Event<>            WhenSelected;

	FileSelButton&     Tip(tukk txt) { button.Tip(txt); return *this; }

private:
	void               OnAction();

private:
	FrameRight<Button> button;
	Txt             title;
	MODE               mode;
};

struct OpenFileButton  : FileSelButton { OpenFileButton(tukk title = NULL) : FileSelButton(MODE_OPEN, title) {} };
struct SaveFileButton  : FileSelButton { SaveFileButton(tukk title = NULL) : FileSelButton(MODE_SAVE, title) {} };
struct SelectDirButton : FileSelButton { SelectDirButton(tukk title = NULL) : FileSelButton(MODE_DIR,  title) {} };

Image MakeZoomIcon(double scale);

void Set(ArrayCtrl& array, i32 ii, IdCtrls& m);
void Get(ArrayCtrl& array, i32 ii, IdCtrls& m);

void   UpdateSetDir(tukk path);
void   UpdateSetUpdater(tukk exename);
Txt UpdateGetDir();

void UpdateFile(Txt dst, Txt src);

void MemoryProfileInfo();

struct sPaintRedirectCtrl : Ctrl {
	Ctrl *ctrl;

	virtual void Paint(Draw& w) {
		ctrl->Paint(w);
	}
};