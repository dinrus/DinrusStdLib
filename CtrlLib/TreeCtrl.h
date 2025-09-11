class TreeCtrl : public Ctrl {
public:
	virtual void  CancelMode();
	virtual void  Paint(Draw& w);
	virtual void  Layout();
	virtual void  LeftDown(Point p, dword flags);
	virtual void  LeftUp(Point p, dword flags);
	virtual void  LeftDouble(Point p, dword flags);
	virtual void  LeftDrag(Point p, dword keyflags);
	virtual void  RightDown(Point p, dword flags);
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual void  MouseMove(Point, dword);
	virtual bool  Key(dword key, i32);
	virtual void  GotFocus();
	virtual void  LostFocus();
	virtual void  ChildGotFocus();
	virtual void  ChildLostFocus();
	virtual void  ChildRemoved(Ctrl *);
	virtual void  SetData(const Val& data);
	virtual Val GetData() const;
	virtual void  DragEnter();
	virtual void  DragAndDrop(Point p, PasteClip& d);
	virtual void  DragRepeat(Point p);
	virtual void  DragLeave();

public:
	class Node
	{
		void           Init() { display = NULL; size = Null; margin = 2; canopen = false; canselect = true; }

	public:
		Image          image;
		i32            margin;
		Val          key;
		Val          value;
		const Display *display;
		Size           size;
		Ptr<Ctrl>      ctrl;
		bool           canopen;
		bool           canselect;

		Node& SetImage(const Image& img)          { image = img; return *this; }
		Node& SetMargin(i32 m)                    { margin = m; return *this; }
		Node& Set(const Val& v)                 { key = value = v; return *this; }
		Node& Set(const Val& v, const Val& t) { key = v; value = t; return *this; }
		Node& SetDisplay(const Display& d)        { display = &d; return *this; }
		Node& SetSize(Size sz)                    { size = sz; return *this; }
		Node& SetCtrl(Ctrl& _ctrl)                { ctrl = &_ctrl; return *this; }
		Node& CanOpen(bool b = true)              { canopen = b; return *this; }
		Node& CanSelect(bool b)                   { canselect = b; return *this; }

		Node();
		Node(const Image& img, const Val& v);
		Node(const Image& img, const Val& v, const Val& t);
		Node(const Val& v);
		Node(const Val& v, const Val& t);
		Node(Ctrl& ctrl);
		Node(const Image& img, Ctrl& ctrl, i32 cx = 0, i32 cy = 0);
	};

private:
	struct Item : Node {
		union {
			i32            parent;
			i32            freelink;
		};

		bool           free;
		bool           isopen;
		bool           sel;
		Vec<i32>    child;
		i32            linei;

		Size GetValSize(const Display *treedisplay) const;
		Size GetCtrlSize() const;
		Size GetSize(const Display *treedisplay) const;

		Item() { isopen = false; linei = -1; parent = -1; canselect = true; sel = false; free = false; }
	};
	struct Line : Moveable<Line> {
		i32  level;
		i32  itemi;
		i32  ll;
		i32  y;
	};

	Array<Item>  item;
	Vec<Line> line;
	i32          cursor;
	i32          freelist;
	i32          levelcx;
	i32          anchor;
	i32          selectcount;
	bool         nocursor;
	bool         noroot;
	bool         dirty;
	bool         hasctrls;
	bool         multiselect;
	bool         nobg;
	bool         popupex;
	bool         chldlck;
	bool         mousemove;
	bool         accel;
	bool         highlight_ctrl;
	bool         multiroot;
	Image        imgEmpty;

	bool         selclick;
	i32          dropitem, dropinsert;
	Point        repoint;
	i32          retime;
	Size         treesize;
	Point        itemclickpos;

	ScrollBars   sb;
	StaticRect   sb_box;
	Scroller     scroller;

	DisplayPopup info;

	const Display *display;

	struct LineLess {
		bool   operator()(i32 y, const Line& l) const            { return y < l.y; }
	};
	
	One<EditTxt> edit_string;
	Ctrl           *editor = NULL;
	i32             edit_cursor = -1;

	struct SortOrder;

	const Display *GetStyle(i32 i, Color& fg, Color& bg, dword& st);
	void   Dirty(i32 id = 0);
	void   ReLine(i32, i32, Size&);
	void   RemoveSubtree(i32 id);
	void   Scroll();
	i32    FindLine(i32 y) const;
	void   RefreshLine(i32 i, i32 ex = 0);
	void   RefreshItem(i32 id, i32 ex);
	void   SetCursorLineSync(i32 i);
	void   MoveCursorLine(i32 c, i32 incr);
	void   SetCursorLine(i32 i, bool sc, bool sel, bool cb);
	void   SetCursor(i32 id, bool sc, bool sel, bool cb);
	void   RemoveCtrls(i32 itemi);
	void   SyncCtrls(bool add, Ctrl *restorefocus);
	bool   Tab(i32 d);
	void   Sort0(i32 id, const ValOrder& order, bool byvalue);
	void   SortDeep0(i32 id, const ValOrder& order, bool byvalue);
	void   Sort0(i32 id, const ValPairOrder& order);
	void   SortDeep0(i32 id, const ValPairOrder& order);
	void   GatherOpened(i32 id, Vec<i32>& o);
	void   SelClear(i32 id);
	void   UpdateSelect();
	void   ShiftSelect(i32 l1, i32 l2);
	void   SelectOne0(i32 id, bool sel, bool cb = false);
	void   DnD(i32 itemid, i32 insert);
	bool   DnDInserti(i32 ii, PasteClip& d, bool bottom);
	bool   DnDInsert(i32 ii, i32 py, i32 q, PasteClip& d);
	void   RefreshSel();
	void   GatherSel(i32 id, Vec<i32>& sel) const;
	void   DoClick(Point p, dword flags, bool down, bool canedit);
	void   SyncInfo();
	void   SyncAfterSync(Ptr<Ctrl> restorefocus);
	Rect   GetValRect(const Line& l) const;
	void   StartEdit();
	void   KillEdit();
	bool   IsEdit() const                       { return editor && editor->GetParent() == this; }

	enum {
		TIMEID_STARTEDIT = Ctrl::TIMEID_COUNT,
		TIMEID_COUNT
	};

	using Ctrl::Close;

protected:
	virtual void SetOption(i32 id);
	void SyncTree(bool immediate = false);
	virtual void Select();
	
	friend class PopUpTree;

public:
	Event<i32>  WhenOpen;
	Event<i32>  WhenClose;
	Event<>     WhenLeftClick;
	Event<>     WhenLeftDouble;
	Event<Bar&> WhenBar;
	Event<>     WhenSel;

	Event<>                     WhenDrag;
	Event<i32, PasteClip&>      WhenDropItem;
	Event<i32, i32, PasteClip&> WhenDropInsert;
	Event<PasteClip&>           WhenDrop;
	
	Event<i32>                  WhenStartEdit;
	Event<i32, const Val&>    WhenEdited;

	// deprecated - use WhenSel
	Event<>         WhenCursor;
	Event<>         WhenSelection;

	void   SetRoot(const Node& n);
	void   SetRoot(const Image& img, Val v);
	void   SetRoot(const Image& img, Val key, Val text);
	void   SetRoot(const Image& img, Ctrl& ctrl, i32 cx = 0, i32 cy = 0);
	i32    Insert(i32 parentid, i32 i, const Node& n);
	i32    Insert(i32 parentid, i32 i);
	i32    Insert(i32 parentid, i32 i, const Image& img, Val value, bool withopen = false);
	i32    Insert(i32 parentid, i32 i, const Image& img, Val key, Val value, bool withopen = false);
	i32    Insert(i32 parentid, i32 i, const Image& img, Val key, const Txt& value, bool withopen = false);
	i32    Insert(i32 parentid, i32 i, const Image& img, Val key, tukk value, bool withopen = false);
	i32    Insert(i32 parentid, i32 i, const Image& img, Ctrl& c, i32 cx = 0, i32 cy = 0, bool wo = false);
	i32    Add(i32 parentid, const Node& n);
	i32    Add(i32 parentid);
	i32    Add(i32 parentid, const Image& img, Val value, bool withopen = false);
	i32    Add(i32 parentid, const Image& img, Val key, Val value, bool withopen = false);
	i32    Add(i32 parentid, const Image& img, Val key, const Txt& value, bool withopen = false);
	i32    Add(i32 parentid, const Image& img, Val key, tukk value, bool withopen = false);
	i32    Add(i32 parentid, const Image& img, Ctrl& ctrl, i32 cx = 0, i32 cy = 0, bool withopen = false);
	void   Remove(i32 id);
	void   RemoveChildren(i32 id);

	void   Swap(i32 id1, i32 id2);
	void   SwapChildren(i32 parentid, i32 i1, i32 i2);

	i32    GetChildCount(i32 id) const                         { return item[id].child.GetCount(); }
	i32    GetChild(i32 id, i32 i) const                       { return item[id].child[i]; }
	i32    GetChildIndex(i32 parentid, i32 childid) const;
	i32    GetParent(i32 id) const                             { return item[id].parent; }

	Val  Get(i32 id) const;
	Val  GetVal(i32 id) const;
	Val  operator[](i32 id) const                            { return Get(id); }
	void   Set(i32 id, Val v);
	void   Set(i32 id, Val key, Val value);
	
	void   SetDisplay(i32 id, const Display& display);
	const Display& GetDisplay(i32 id) const;
	void   RefreshItem(i32 id)                                 { RefreshItem(id, 0); }

	i32    GetLineCount();
	i32    GetItemAtLine(i32 i);
	i32    GetLineAtItem(i32 id);

	Node   GetNode(i32 id) const                               { return item[id]; }
	void   SetNode(i32 id, const Node& n);

	bool   IsValid(i32 id) const;

	bool   IsOpen(i32 id) const;
	void   Open(i32 id, bool open = true);
	void   Close(i32 id)                                       { Open(id, false); }
	Vec<i32> GetOpenIds() const;
	void   OpenIds(const Vec<i32>& ids);

	void   OpenDeep(i32 id, bool open = true);
	void   CloseDeep(i32 id)                                   { OpenDeep(id, false); }

	void   MakeVisible(i32 id);

	void   SetCursorLine(i32 i);
	i32    GetCursorLine() const                               { return cursor; }
	void   KillCursor();
	void   SetCursor(i32 id);
	i32    GetCursor() const;
	bool   IsCursor() const                                    { return GetCursor() >= 0; }

	Point  GetItemClickPos() const                             { return itemclickpos; }

	Point  GetScroll() const;
	void   ScrollTo(Point sc);

	Val  Get() const;
	Val  GetVal() const;
	
	void   SetVal(const Val& v);

	i32    Find(Val key);
	bool   FindSetCursor(Val key);

	void   Sort(i32 id, const ValOrder& order, bool byvalue = false);
	void   SortDeep(i32 id, const ValOrder& order, bool byvalue = false);

	void   Sort(i32 id, i32 (*compare)(const Val& v1, const Val& v2) = StdValCompare,
	            bool byvalue = false);
	void   SortDeep(i32 id, i32 (*compare)(const Val& v1, const Val& v2) = StdValCompare,
	                bool byvalue = false);

	void   SortByVal(i32 id, const ValOrder& order);
	void   SortDeepByVal(i32 id, const ValOrder& order);

	void   SortByVal(i32 id, i32 (*compare)(const Val& v1, const Val& v2) = StdValCompare);
	void   SortDeepByVal(i32 id, i32 (*compare)(const Val& v1, const Val& v2) = StdValCompare);

	void   Sort(i32 id, const ValPairOrder& order);
	void   SortDeep(i32 id, const ValPairOrder& order);
	void   Sort(i32 id, i32 (*compare)(const Val& k1, const Val& v1, const Val& k2, const Val& v2));
	void   SortDeep(i32 id, i32 (*compare)(const Val& k1, const Val& v1, const Val& k2, const Val& v2));

	void   Clear();

	void         ClearSelection();
	void         SelectOne(i32 id, bool sel = true);
	bool         IsSelected(i32 id) const       { return item[id].sel; }
	bool         IsSel(i32 id) const;
	i32          GetSelectCount() const         { return selectcount; }
	bool         IsSelDeep(i32 id) const;
	Vec<i32>  GetSel() const;
	void         Remove(const Vec<i32>& id);
	void         RemoveSelection()              { Remove(GetSel()); }

	void         AdjustAction(i32 parent, PasteClip& x);
	Vec<i32>  InsertDrop(i32 parent, i32 ii, const TreeCtrl& src, PasteClip& x);
	Vec<i32>  InsertDrop(i32 parent, i32 ii, PasteClip& d);
	Image        GetDragSample();

	void         StdLeftDouble();

	void         ScrollIntoLine(i32 i);
	void         CenterLine(i32 i);

	void         ScrollIntoCursor();
	void         CenterCursor();

	Size         GetTreeSize() const         { return treesize; }

	void         OkEdit();
	void         EndEdit();

	TreeCtrl& NoCursor(bool b = true)        { nocursor = b; if(b) KillCursor(); return *this; }
	TreeCtrl& NoRoot(bool b = true)          { noroot = b; Dirty(); Refresh(); return *this; }
	TreeCtrl& LevelCx(i32 cx)                { levelcx = cx; Dirty(); return *this; }
	TreeCtrl& MultiSelect(bool b = true)     { multiselect = b; return *this; }
	TreeCtrl& NoBackground(bool b = true)    { nobg = b; Transparent(); Refresh(); return *this; }
	TreeCtrl& PopUpEx(bool b = true)         { popupex = b; return *this; }
	TreeCtrl& NoPopUpEx()                    { return PopUpEx(false); }
	TreeCtrl& MouseMoveCursor(bool m = true) { mousemove = m; return *this; }
	TreeCtrl& Accel(bool a = true)           { accel = a; return *this; }
	TreeCtrl& SetDisplay(const Display& d);
	TreeCtrl& HighlightCtrl(bool a = true)   { highlight_ctrl = a; Refresh(); return *this; }
	TreeCtrl& RenderMultiRoot(bool a = true) { multiroot = a; Refresh(); return *this; }
	TreeCtrl& EmptyNodeIcon(const Image& a)  { imgEmpty = a; Refresh(); return *this; }
	TreeCtrl& Editor(Ctrl& e)                { editor = &e; return *this; }
	
	TreeCtrl& SetScrollBarStyle(const ScrollBar::Style& s) { sb.SetStyle(s); return *this; }

	typedef TreeCtrl CLASSNAME;

	TreeCtrl();
	~TreeCtrl();
};

i32 Copy(TreeCtrl& dst, i32 dstid, i32 i, const TreeCtrl& src, i32 srcid);

class OptionTree : public TreeCtrl {
	Vec<Option *> option;
	Array<Option>    aux;
	bool             manualmode;

protected:
	virtual void SetOption(i32 id);
	virtual void SetChildren(i32 id, bool b);

public:
	Event<>  WhenOption;

	void SetRoot(const Image& img, Option& option, tukk text = NULL);
	void SetRoot(Option& option, tukk text = NULL);
	void SetRoot(const Image& img, tukk text);
	void SetRoot(tukk text);
	i32  Insert(i32 parent, i32 i, const Image& img, Option& option, tukk text = NULL);
	i32  Insert(i32 parent, i32 i, Option& option, tukk text = NULL);
	i32  Insert(i32 parent, i32 i, const Image& img, tukk text);
	i32  Insert(i32 parent, i32 i, tukk text);
	i32  Add(i32 parent, const Image& img, Option& option, tukk text = NULL);
	i32  Add(i32 parent, Option& option, tukk text = NULL);
	i32  Add(i32 parent, const Image& img, tukk text);
	i32  Add(i32 parent, tukk text);
	
	void   SetLabel(i32 id, tukk text);
	Txt GetLabel(i32 id) const                   { return option[id]->GetLabel(); }

	i32  Get(i32 id) const                          { return option[id]->Get(); }

	void Clear();

	typedef OptionTree CLASSNAME;

	OptionTree& ManualMode()						{ manualmode = true; return *this; }

	OptionTree();
	~OptionTree();
};

class PopUpTree : public TreeCtrl {
public:
	virtual void Deactivate();
	virtual void Select();
	virtual bool Key(dword key, i32);
	virtual void CancelMode();

private:
	i32          maxheight;
	Point        showpos;
	bool         autosize;
	i32          showwidth;
	bool         up;
	bool         open;

	void         DoClose();
	void         OpenClose(i32 i);

public:
	typedef PopUpTree CLASSNAME;
	PopUpTree();
	virtual ~PopUpTree();

	void         PopUp(Ctrl *owner, i32 x, i32 top, i32 bottom, i32 width);
	void         PopUp(Ctrl *owner, i32 width);
	void         PopUp(Ctrl *owner);

	Event<>      WhenCancel;
	Event<>      WhenSelect;

	PopUpTree&   MaxHeight(i32 maxheight_)          { maxheight = maxheight_; return *this; }
	i32          GetMaxHeight() const               { return maxheight; }
	PopUpTree&   AutoResize(bool b = true)          { autosize = b; return *this; }
	PopUpTree&   SetDropLines(i32 n)                { MaxHeight(n * (GetStdFontCy() + 4)); return *this; }
};

class DropTree : public MultiButton {
public:
	virtual bool  Key(dword k, i32 cnt);
	virtual void  SetData(const Val& data);
	virtual Val GetData() const;

private:
	PopUpTree          tree;
	const Display     *valuedisplay;
	DisplayWithIcon    icond;
	i32                dropwidth;
	bool               dropfocus;
	bool               notnull;
	
	void Sync();
	void Drop();
	void Select();
	void Cancel();
	
	typedef DropTree CLASSNAME;

public:
	Event<>       WhenDrop;

	void Clear();

	PopUpTree&   TreeObject()                       { return tree; }

	TreeCtrl *operator->()                          { return &tree; }
	const TreeCtrl *operator->() const              { return &tree; }

	DropTree& MaxHeight(i32 maxheight_)             { tree.MaxHeight(maxheight_); return *this; }
	DropTree& AutoResize(bool b = true)             { tree.AutoResize(b); return *this; }

	DropTree& ValDisplay(const Display& d);
	DropTree& DropFocus(bool b = true)              { dropfocus = b; return *this; }
	DropTree& NoDropFocus()                         { return DropFocus(false); }
	DropTree& SetDropLines(i32 d)                   { tree.SetDropLines(d); return *this; }
	DropTree& DropWidth(i32 w)                      { dropwidth = w; return *this; }
	DropTree& DropWidthZ(i32 w)                     { return DropWidth(Zx(w)); }

	DropTree();
};
