#define FILESELVERSION 2

Image GetDriveImage(char drive_style);
Image GetFileIcon(tukk path, bool dir, bool force = false);

void DrawFName(Draw& w, i32 x, i32 y, i32 wcx, i32 cy, const WTxt& mname, bool isdir, Font font,
                  Color ink, Color extink, const WTxt& desc = Null, Font descfont = Null,
                  bool justname = false, Color underline = Null);

Image NativePathIcon(tukk path, bool folder);
Image NativePathIcon(tukk path);

Image NativePathLargeIcon(tukk path, bool folder);
Image NativePathLargeIcon(tukk path);

class FileList : public ColumnList, private Display {
public:
	virtual void  LeftDown(Point p, dword);
	virtual bool  Key(dword key, i32 count);

	virtual void  Paint(Draw& w, const Rect& r, const Val& q,
		                Color ink, Color paper, dword style) const;
	virtual Size  GetStdSize(const Val& q) const;

public:
	struct File {
		bool   isdir;
		bool   unixexe;
		bool   hidden;
		Image  icon;
		Txt name;
		Font   font;
		Color  ink;
		z64  length;
		Time   time;
		Color  extink;
		Txt desc;
		Font   descfont;
		Val  data;
		Color  underline;

		operator const Txt&() const { return name; }
	};

	class Order {
	public:
		virtual bool operator()(const File& a, const File& b) const = 0;
		virtual ~Order() {}
	};

private:
	i32            iconwidth;
	EditField      edit;

	bool           renaming;
	bool           justname;
	bool           accelkey;
	bool           selectdir;

	void        UpdateSelect();
	void        Update();
	bool        FindChar(i32 from, i32 chr);
	void        KillEdit();

	struct FileOrder;

	using ColumnList::GetStdSize;
	using ColumnList::Paint;

protected:
	enum {
		TIMEID_STARTEDIT = ColumnList::TIMEID_COUNT,
		TIMEID_COUNT
	};

public:
	Event<const Txt&, const Txt&> WhenRename;

	void        StartEdit();
	void        EndEdit();
	bool        IsEdit() const                       { return edit.IsVisible(); }
	void        OkEdit();

	const File& Get(i32 i) const;
	const File& operator[](i32 i) const              { return Get(i); }
	void        Insert(i32 ii,
	                const Txt& name, const Image& icon = Null,
		            Font font = StdFont(), Color ink = SColorText(),
	                bool isdir = false, z64 length = 0, Time time = Null, Color extink = Null,
	                const Txt& desc = Null, Font descfont = Null, Val data = Null,
	                Color underline = Null, bool unixexe = false, bool hidden = false);
	void        Set(i32 ii,
	                const Txt& name, const Image& icon = Null,
		            Font font = StdFont(), Color ink = SColorText(),
	                bool isdir = false, z64 length = 0, Time time = Null, Color extink = Null,
	                const Txt& desc = Null, Font descfont = Null, Val data = Null,
	                Color underline = Null, bool unixexe = false, bool hidden = false);
	void        Set(i32 ii, const File& f);
	void        SetIcon(i32 ii, const Image& icon);
	void        Add(const File& f);
	void        Add(const Txt& name, const Image& icon = Null,
		            Font font = StdFont(), Color ink = SColorText(),
	                bool isdir = false, z64 length = 0, Time time = Null, Color extink = Null,
	                const Txt& desc = Null, Font descfont = Null, Val data = Null,
	                Color underline = Null, bool unixexe = false, bool hidden = false);
	Txt      GetCurrentName() const;

	i32         Find(tukk name);
	bool        FindSetCursor(tukk name);

	void        Sort(const Order& order);

	FileList&   IconWidth(i32 w)                     { iconwidth = w; Refresh(); return *this; }
	i32         GetIconWidth() const                 { return iconwidth; }
	FileList&   Renaming(bool b = true)              { renaming = b; return *this; }
	FileList&   JustName(bool b = true)              { justname = b; Refresh(); return *this; }
	FileList&   AccelKey(bool b = true)              { accelkey = b; return *this; }
	FileList&   SelectDir(bool b = true)             { selectdir = b; return *this; }

	typedef FileList CLASSNAME;

	FileList();
	virtual ~FileList();
};

bool Load(FileList& list, const Txt& dir, tukk patterns, bool dirs = false,
          Event<bool, const Txt&, Image&> WhenIcon = Null,
          FSInfo& filesystem = StdFileSystemInfo(), const Txt& search = Txt(),
          bool hidden = true, bool hiddenfiles = true, bool lazyicons = false);


enum {
	FILELISTSORT_NAME,
	FILELISTSORT_EXT,
	FILELISTSORT_TIME,
	FILELISTSORT_SIZE,
	FILELISTSORT_DESCENDING = 0x8000,
};

void SortBy(FileList& list, i32 kind);

// deprecated, use Sort above
inline void SortByName(FileList& list) { SortBy(list, FILELISTSORT_NAME); }
inline void SortByExt(FileList& list) { SortBy(list, FILELISTSORT_EXT); }
inline void SortByTime(FileList& list) { SortBy(list, FILELISTSORT_TIME); }

#ifdef GUI_WIN
// Helper class for lazy (using aux thread) evaluation of .exe icons in Win32
class LazyExeFileIcons {
	TimeCallback tm;
	Txt       dir;
	FileList    *list;
	i32          pos;
	Vec<i32>  ndx;
	Event<bool, const Txt&, Image&> WhenIcon;

	Mutex  mutex;

	void   Do();
	void   Restart(i32 delay)                 { tm.KillSet(delay, callback(this, &LazyExeFileIcons::Do)); }
	WTxt Path();
	void   Done(Image img);

public:
	void ReOrder();
	void Start(FileList& list_, const Txt& dir_, Event<bool, const Txt&, Image&> WhenIcon_);
};
#endif

Txt DirUp(Txt& dir, bool basedir = false);

Txt FormatFileSize(z64 n);

class FileSel : public WithFileSelectorLayout<TopWindow> {
public:
	virtual bool Key(dword key, i32 count);
	virtual void Activate();

private:
	SizeGrip    sizegrip;

	Button      dirup, mkdir, plus, minus, toggle;

protected:
	Vec<Txt> mask;
	Vec<Txt> path;
	Vec<Txt> lru;
	Vec<Txt> fw;
	i32            lastsby;
	FSInfo *filesystem;

	Txt         olddir;
	Txt         basedir;
	Txt         activetype;
	Txt         defext;
	Txt         preselect;
	Txt         default_name;
	Vec<Txt> fn;

#ifdef TARGET_WIN32
	static bool netroot_loaded;
	static Array<NetNode> netroot;
	Array<NetNode> netstack;
	Array<NetNode> netnode;
#endif

#ifdef GUI_WIN
	LazyExeFileIcons  lazyicons;
#endif

	DisplayCtrl    preview_display;
	Ctrl          *preview;
	FileList       list;
	ArrayCtrl      places;

	enum {
		OPEN, SAVEAS, SELECTDIR
	};

	i32         mode;
	bool        asking;
	bool        multi;
	bool        rdonly;
	bool        bidname;
	bool        appmodal;
	bool        loaded;
	bool        noexeicons;

	Ctrl       *file_ctrl = NULL;
	i32         file_ctrl_cx;

	bool          force_ext = true; // -> false if there is wildcard mask allowing set of exts
	Index<Txt> allowed_ext; // allowed extensions typed by user if force_ext

	static StaticMutex li_mutex;
	static void      (*li_current)(const Txt& path, Image& result);
	static Txt      li_path;
	static Image       li_result;
	static bool        li_running;
	static i32         li_pos;
	TimeCallback       li_tm;

	bool        loading_network = false;

	static void LIThread();
	Txt      LIPath();
	void        StartLI();
	void        DoLI();
	void        ScheduleLI()                                 { li_tm.KillSet(0, OTVET(DoLI)); }

	void        LoadNet();
	void        SelectNet();
	bool        PatternMatch(tukk fn);
	bool        OpenItem();
	void        OpenItem2()                                  { OpenItem(); }
	void        Open();
	void        DirUp();
	void        MkDir();
	void        Plus();
	void        Minus();
	void        Toggle();
	void        Reload();
	void        PlusMinus(tukk title, bool sel);
	void        Update();
	void        FileUpdate();
	void        Rename(const Txt& on, const Txt& nn);
	void        Choice();
	void        SearchLoad();
	void        Load();
	Txt      FilePath(const Txt& fn);
	void        SetDir(const Txt& dir);
	Txt      GetDir() const;
	void        AddName(Vec<Txt>& fn, Txt& o);
	Txt      ResolveLnk(const Txt& name) const;
	Txt      ResolveLnkDir(const Txt& name) const;
	Txt      ResolveLnkFile(const Txt& name) const;
	void        Finish();
	bool        Execute(i32 mode);
	bool        IsMulti()                                     { return multi && mode == OPEN; }
	void        SyncSplitter();
	void        InitSplitter();
	Txt      GetMask();
	void        GoToPlace();
	void        AddPlaceRaw(const Txt& path, const Image& m, const Txt& name, tukk group = NULL, i32 row = -1);
	void        AddSystemPlaces(i32 row = -1);
#ifdef TARGET_WIN32
	bool        ScanNetwork(Function<Array<NetNode> ()> fn);
	void        ScanNetworkRoot();
#endif

	using       WithFileSelectorLayout<TopWindow>::Title;

	typedef FileSel CLASSNAME;

public:
	static bool IsLnkFile(const Txt& p);

	Event<bool, const Txt&, Image&> WhenIcon;
	void (*WhenIconLazy)(const Txt& path, Image& result);

	void        Serialize(Stream& s);

	bool        ExecuteOpen(tukk title = NULL);
	bool        ExecuteSaveAs(tukk title = NULL);

	bool        ExecuteSelectDir(tukk title = NULL);

	Txt Get() const                           { return GetFile(0); }
	void   Set(const Txt& s);
	void   Set(const Vec<Txt>& s)          { fn <<= s; bidname = true; }

	operator Txt() const                      { return Get(); }
	void operator=(const Txt& s)              { Set(s); }

	Txt operator~() const                     { return Get(); }
	void operator<<=(const Txt& s)            { Set(s); }

	i32    GetCount() const                      { return fn.GetCount(); }
	Txt GetFile(i32 i) const;
	Txt operator[](i32 i) const               { return GetFile(i); }
	void   ClearFiles()                          { fn.Clear(); }

	bool   GetReadOnly() const                   { return readonly; }
	Txt GetActiveDir() const                  { return dir.GetData(); }
	i32    GetActiveType() const                 { return type.GetIndex(); }

	void   Filesystem(FSInfo& fsys)      { filesystem = &fsys;}
	FSInfo& GetFilesystem() const        { return *filesystem; }

	FileSel& Type(tukk name, tukk ext);
	FileSel& Types(tukk d);
	FileSel& AllFilesType();
	FileSel& ClearTypes();
	FileSel& ActiveDir(const Txt& d)          { dir <<= d; return *this; }
	FileSel& ActiveType(i32 i);
	FileSel& PreSelect(const Txt& path);
	FileSel& DefaultExt(tukk ext)         { defext = ext; return *this; }
	FileSel& Sort(i32 kind)						 { sortby.SetData(kind); return *this; }
	FileSel& Columns(i32 n)						 { list.Columns(n); return *this; }
	FileSel& Multi(bool b = true)                { multi = b; return *this; }
	FileSel& ReadOnlyOption(bool b = true)       { rdonly = b; return *this; }
	FileSel& MkDirOption(bool b = true)          { mkdir.Show(b); return *this; }
	FileSel& NoMkDirOption()                     { return MkDirOption(false); }
	FileSel& BaseDir(tukk dir)            { basedir = dir; return *this; }
	FileSel& Asking(bool b = true)               { asking = b; return *this; }
	FileSel& NoAsking()                          { return Asking(false); }
	FileSel& EditFName(bool b)                { file.SetEditable(b); return *this; }
	FileSel& NoEditFName()                    { return EditFName(false); }
	FileSel& AppModal(bool b = true)             { appmodal = b; return *this; }
	FileSel& NoAppModal()                        { return AppModal(false); }
	FileSel& Preview(Ctrl& ctrl);
	FileSel& Preview(const Display& d);
	FileSel& ClearPlaces();
	FileSel& AddPlace(const Txt& path, const Image& m, const Txt& name, tukk group = NULL, i32 row = -1);
	FileSel& AddPlace(const Txt& path, const Txt& name, tukk group = NULL, i32 row = -1);
	FileSel& AddPlace(const Txt& path, tukk group = NULL, i32 row = -1);
	FileSel& AddPlaceSeparator();
	FileSel& AddStandardPlaces();
	FileSel& FileCtrl(Ctrl& ext, i32 cx)         { file_ctrl = &ext; file_ctrl_cx = cx; return *this; }
	FileSel& FileCtrl(Ctrl& ext)                 { return FileCtrl(ext, ext.GetMinSize().cx); }
	FileSel& DefaultName(const Txt& s)        { default_name = s; return *this; }
	FileSel& NoExeIcons(bool b = true)           { noexeicons = b; return *this; }

	FileSel();
	virtual ~FileSel();
};

Txt SelectFileOpen(tukk types);
Txt SelectFileSaveAs(tukk types);
Txt SelectDir();
Txt SelectLoadFile(tukk types);
bool   SelectSaveFile(tukk types, const Txt& data);

struct SelectFileIn : FileIn {
	SelectFileIn(tukk types);
};

struct SelectFileOut : FileOut {
	SelectFileOut(tukk types);
};