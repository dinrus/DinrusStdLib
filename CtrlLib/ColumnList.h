class ColumnList : public Ctrl, private CtrlFrame {
public:
	virtual void  Paint(Draw& w);
	virtual void  Layout();
	virtual Image CursorImage(Point p, dword);
	virtual void  LeftDown(Point p, dword);
	virtual void  LeftUp(Point p, dword);
	virtual void  LeftDouble(Point p, dword);
	virtual void  RightDown(Point p, dword);
	virtual void  LeftDrag(Point p, dword keyflags);
	virtual void  MouseMove(Point p, dword);
	virtual void  MouseLeave();
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual bool  Key(dword key, i32 count);
	virtual void  GotFocus();
	virtual void  LostFocus();
	virtual void  CancelMode();
	virtual void  DragEnter();
	virtual void  DragAndDrop(Point p, PasteClip& d);
	virtual void  DragRepeat(Point p);
	virtual void  DragLeave();
	virtual Val GetData() const;
	virtual void  SetData(const Val& key);

private:
	virtual void  FrameLayout(Rect& r);
	virtual void  FrameAddSize(Size& sz);
	virtual void  FramePaint(Draw& draw, const Rect& r);

private:
	i32        ncl;
	i32        cx, cy;
	i32        cursor, anchor;
	i32        dx;
	i32        ci;
	i32        mpos;
	ScrollBar  sb;
	Scroller   scroller;
	CtrlFrame *frame;
	i32        dropitem;
	bool       insert;
	bool       clickkill;
	bool       nobg;
	bool       popupex;
	bool       selclick;
	i32        mode;

	DisplayPopup info;

	const Display *display;

	struct Item {
		bool           sel;
		bool           canselect;
		Val          key;
		Val          value;
		const Display *display;
	};

	Array<Item> item;
	i32         selcount;
	bool        multi;

	struct  ItemOrder;
	friend struct ItemOrder;

	void    SetSb();
	void    Scroll();
	i32     GetDragColumn(i32 x) const;
	i32     RoundedCy();
	void    Page(bool down);
	void    PointDown(Point p);
	void    DoClick(Point p, dword flags);
	void    ShiftSelect();
	void    RefreshItem(i32 i, i32 ex = 0);
	void    RefreshCursor()                    { RefreshItem(cursor); }
	void    GetItemStyle(i32 i, Color& ink, Color& paper, dword& style);
	dword   PaintItem(Draw& w, i32 i, const Rect& r);
	void    SyncInfo();
	void    SetCursor0(i32 c, bool sel);
	void    UpdateSelect();
	void    RefreshSel();
	void    DoLeftDown(Point p, dword);
	dword   SwapKey(dword key);
	void    PaintRows(Draw &w, Size &sz);
	i32     GetSbPos(const Size &sz) const;
	void    ScrollInto(i32 pos);

	bool    DnDInsert(i32 i, i32 py, PasteClip& d, i32 q);
	void    DnD(i32 _drop, bool _insert);
	
	friend class FileList;

public:
	enum {
		MODE_LIST,
		MODE_COLUMN,
		MODE_ROWS
	};

	Event<>      WhenLeftClick;
	Event<Point> WhenLeftClickPos;
	Event<>      WhenLeftDouble;
	Event<Bar&>  WhenBar;
	Event<>      WhenSel;

	Event<>                  WhenDrag;
	Event<i32, PasteClip&>   WhenDropItem;
	Event<i32, PasteClip&>   WhenDropInsert;
	Event<PasteClip&>        WhenDrop;

	// deprecated - use WhenSel
	Event<>          WhenSelection;
	Event<>          WhenEnterItem;
	Event<>          WhenKillCursor;

	i32     GetColumnItems() const;
	i32     GetColumnCx(i32 i = 0) const;
	i32     GetPageItems() const;

	i32     GetItem(Point p);
	Rect    GetItemRect(i32 i) const;

	i32     GetCursor() const                          { return cursor; }
	void    SetCursor(i32 c);
	void    KillCursor();
	bool    IsCursor() const                           { return cursor >= 0; }

	i32     GetSbPos() const                           { return GetSbPos(GetSize()); }
	void    SetSbPos(i32 y);
	
	i32     GetScroll() const                          { return sb; }
	void    ScrollTo(i32 a)                            { sb.Set(a); }

	void    SetFrame(CtrlFrame& frame);

	void         Clear();
	void         Add(const Val& val, bool canselect = true);
	void         Add(const Val& val, const Display& display, bool canselect = true);
	void         Add(const Val& key, const Val& val, bool canselect = true);
	void         Add(const Val& key, const Val& val, const Display& display, bool canselect = true);

	i32          GetCount() const                     { return item.GetCount(); }
	const Val& Get(i32 i) const                     { return item[i].key; }
	const Val& GetVal(i32 i) const                { return item[i].value; }
	const Val& operator[](i32 i) const              { return item[i].key; }

	void         Set(i32 ii, const Val& key, const Val& val, bool canselect = true);
	void         Set(i32 ii, const Val& key, const Val& val, const Display& display, bool canselect = true);
	void         Set(i32 ii, const Val& val, bool canselect = true);
	void         Set(i32 ii, const Val& val, const Display& display, bool canselect = true);

	void         Set(const Val& key, const Val& val, const Display& display, bool canselect = true);
	void         Set(const Val& key, const Val& val, bool canselect = true);

	void         Insert(i32 ii, const Val& val, bool canselect = true);
	void         Insert(i32 ii, const Val& val, const Display& display, bool canselect = true);
	void         Insert(i32 ii, const Val& key, const Val& val, bool canselect = true);
	void         Insert(i32 ii, const Val& key, const Val& val, const Display& display, bool canselect = true);
	void         Remove(i32 ii);
	void         Remove(const Val & key)			  { i32 ii = Find(key); if (ii >= 0) Remove(ii); }

	void         RemoveSelection();

	i32          GetSelectCount() const               { return selcount; }
	bool         IsSelection() const                  { return selcount > 0; }
	void         ClearSelection();
	void         SelectOne(i32 i, bool sel);
	bool         IsSelected(i32 i) const;
	bool         IsSel(i32 i) const;

	i32          Find(const Val& key) const;

	void         Sort(const ValOrder& order);

	Image        GetDragSample();

	void         InsertDrop(i32 ii, const Vec<Val>& data, PasteClip& d, bool self);
	void         InsertDrop(i32 ii, const Vec<Val>& keys, const Vec<Val>& data, PasteClip& d, bool self);
	void         InsertDrop(i32 ii, const ColumnList& src, PasteClip& d);
	void         InsertDrop(i32 ii, PasteClip& d);

	void         SerializeSettings(Stream& s);

	ColumnList&  Mode(i32 m);
	ColumnList&  ListMode()                           { return Mode(MODE_LIST); }
	ColumnList&  RowMode()                            { return Mode(MODE_ROWS); }
	ColumnList&  ColumnMode()                         { return Mode(MODE_COLUMN); }
	ColumnList&  Columns(i32 _n)                      { ncl = _n; Refresh(); return *this; }
	i32          GetColumns() const                   { return ncl; }
	ColumnList&  ItemHeight(i32 _cy)                  { cy = _cy; RefreshLayout(); SetSb(); Refresh(); return *this; }
	i32          GetItemHeight() const                { return cy; }
	ColumnList&  ItemWidth(i32 _cx)                   { cx = _cx; RefreshLayout(); SetSb(); Refresh(); return *this; }
	i32          GetItemWidth() const                 { return cx; }
	ColumnList&  RoundSize(bool b = true);
	ColumnList&  NoRoundSize()                        { return RoundSize(false); }
	ColumnList&  ClickKill(bool b = true)             { clickkill = b; return *this; }
	ColumnList&  NoClickKill()                        { return ClickKill(false); }
	ColumnList&  SetDisplay(const Display& d)         { display = &d; return *this; }
	ColumnList&  NoBackground(bool b = true)          { nobg = b; Transparent(); Refresh(); return *this; }
	ColumnList&  Multi(bool b = true)                 { multi = b; return *this; }
	bool         IsMulti() const                      { return multi; }
	ColumnList&  MultiSelect(bool b = true)           { multi = b; return *this; }
	bool         IsMultiSelect() const                { return multi; }
	ColumnList&  PopUpEx(bool b = true)               { popupex = b; return *this; }
	ColumnList&  NoPopUpEx()                          { return PopUpEx(false); }
	ColumnList&  AutoHideSb(bool b = true)            { sb.AutoHide(b); return *this; }
	ColumnList&  NoAutoHideSb()                       { return AutoHideSb(false); }

	ColumnList&  SetScrollBarStyle(const ScrollBar::Style& s) { sb.SetStyle(s); return *this; }

	typedef ColumnList CLASSNAME;

	ColumnList();
	virtual ~ColumnList();
};
