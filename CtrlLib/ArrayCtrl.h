template <class T>
void DefaultCtrlFactoryFn(One<Ctrl>& ctrl)
{
	ctrl.Create<T>();
}

template <class T>
Callback1<One<Ctrl>&> DefaultCtrlFactory()
{
	return callback(DefaultCtrlFactoryFn<T>);
}

class ArrayCtrl : public Ctrl {
public:
	virtual void  CancelMode();
	virtual bool  Accept();
	virtual void  Reject();
	virtual void  Paint(Draw& w);
	virtual void  Layout();
	virtual bool  Key(dword key, i32);
	virtual void  LeftDown(Point p, dword);
	virtual void  LeftDouble(Point p, dword);
	virtual void  LeftDrag(Point p, dword);
	virtual void  LeftUp(Point p, dword flags);
	virtual void  RightDown(Point p, dword);
	virtual void  MouseMove(Point p, dword);
	virtual void  MouseLeave();
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual Image CursorImage(Point p, dword);
	virtual void  DragAndDrop(Point p, PasteClip& d);
	virtual void  DragRepeat(Point p);
	virtual void  DragLeave();
	virtual void  GotFocus();
	virtual void  LostFocus();
	virtual void  ChildGotFocus();
	virtual void  ChildLostFocus();
	virtual void  Serialize(Stream& s);

public:
	struct IdInfo {
		Val             insertval;
		ValGen         *insertgen;
		wchar           (*accel)(wchar);

		IdInfo& InsertVal(const Val& v)  { insertval = v; return *this; }
		IdInfo& InsertVal(ValGen& g)     { insertgen = &g; return *this; }
		IdInfo& Accel(wchar (*filter)(wchar))    { accel = filter; return *this; }
		IdInfo& Accel()                      { return Accel(CharFilterDefaultToUpperAscii); }

		Val   GetInsertVal()             { return insertgen ? insertgen->Get() : insertval; }

		IdInfo()                             { insertgen = NULL; accel = NULL; }
	};

	class Column : FormatConvert {
		ArrayCtrl            *arrayctrl;
		i32                   index;
		Vec<i32>           pos;
		const Convert        *convert;
		Function<Val(const Val&)> convertby;
		Ptr<Ctrl>              edit;
		const Display         *display;
		Event<i32, One<Ctrl>&> factory;
		Event<One<Ctrl>&>     factory1;
		wchar               (*accel)(wchar);
		i32                   margin;
		bool                  cached;
		bool                  clickedit;
		mutable Any           cache;
		const ValOrder       *order;
		Function<i32 (const Val& a, const Val& b)> cmp;
		Gate<i32, i32>        line_order;


		void   InvalidateCache(i32 i);
		void   InsertCache(i32 i, i32 n);
		void   RemoveCache(i32 i);
		void   ClearCache();
		void   Sorts();

		typedef Column CLASSNAME;

		friend class ArrayCtrl;

	public:
		Column& Add(i32 _pos)                      { pos.Add(_pos); return *this; }
		Column& Add(const Id& id)                  { pos.Add(-arrayctrl->AsNdx(id)); return *this; }
		Column& AddIndex(const Id& id)             { arrayctrl->AddIndex(id); return Add(id); }
		Column& AddIndex()                         { Add(arrayctrl->GetIndexCount()); arrayctrl->AddIndex(); return *this; }

		Column& SetConvert(const Convert& c);
		Column& ConvertBy(Function<Val(const Val&)> cv);
		Column& SetFormat(tukk fmt);
		Column& SetDisplay(const Display& d);
		Column& NoEdit();
		Column& Edit(Ctrl& e);
		template <class T>
		Column& Ctrls()                            { return Ctrls(DefaultCtrlFactory<T>()); }
		Column& WithLined(Event<i32, One<Ctrl>&> factory);
		Column& With(Event<One<Ctrl>&> factory);
		Column& InsertVal(const Val& v);
		Column& InsertVal(ValGen& g);
		Column& NoClickEdit()                      { clickedit = false; return *this; }
		Column& Cache();
		Column& Accel(wchar (*filter)(wchar))          { accel = filter; return *this; }
		Column& Accel()                            { return Accel(CharFilterDefaultToUpperAscii); }

		Column& Sorting();
		Column& Sorting(const ValOrder& o);
		Column& SortingLined(Gate<i32, i32> line_order);
		Column& SortingBy(Function<i32 (const Val& a, const Val& b)> cmp);
		Column& SortDefault(bool desc = false);

		Column& Margin(i32 m)                      { margin = m; return *this; }

		HeaderCtrl::Column& HeaderTab();
		const HeaderCtrl::Column& HeaderTab() const;
		Column& Tip(tukk tip)               { HeaderTab().Tip(tip); return *this; }

		Column();

// deprecated (due to overloading issues)
		Column& Ctrls(Callback1<One<Ctrl>&> factory);
		Column& Ctrls(void (*factory)(One<Ctrl>&)) { return Ctrls(Event<i32, One<Ctrl>&>([=](i32, One<Ctrl>& h) { factory(h); })); }
		Column& Ctrls(Event<i32, One<Ctrl>&> factory);
		Column& Ctrls(void (*factory)(i32, One<Ctrl>&)) { return Ctrls(Event<i32, One<Ctrl>&>([=](i32 a, One<Ctrl>& b){ factory(a, b); })); }
		Column& Sorting(Gate<i32, i32> order) { return SortingLined(order); }
		Column& Sorting(i32 (*c)(const Val& a, const Val& b)) { return SortingBy(c); }
	};

	struct Order {
		virtual bool operator()(const Vec<Val>& row1, const Vec<Val>& row2) const = 0;
		virtual ~Order() {}
	};

private:
	struct ItemOrder;
	struct RowOrder;
	struct SortPredicate;

	struct Control : Moveable<Control> {
		i32   pos;
		Ctrl *ctrl;
	};

	struct CellCtrl : ParentCtrl {
		virtual void LeftDown(Point, dword);

		bool       owned;
		bool       value;
		Ctrl      *ctrl;
	};

	struct CellInfo : Moveable<CellInfo> {
		BitAndPtr ptr;

		void           Free();
		void           Set(Ctrl *ctrl, bool owned, bool value);
		bool           IsCtrl() const             { return ptr.GetBit(); }
		CellCtrl&      GetCtrl() const            { ASSERT(IsCtrl()); return *(CellCtrl *)ptr.GetPtr(); }

		void           Set(const Display& d)      { ASSERT(!IsCtrl()); ptr.Set0((uk )&d); }
		bool           IsDisplay() const          { return !ptr.GetBit() && ptr.GetPtr(); }
		const Display& GetDisplay() const         { ASSERT(IsDisplay()); return *(const Display *)ptr.GetPtr(); }

		CellInfo(CellInfo&& s);
		CellInfo() {}
		~CellInfo();
	};

	friend class Column;
	friend struct SortPredicate;

	struct Ln : Moveable<Ln> {
		i32 y;
		i32 cy;
		Ln()               { cy = Null; }
	};

	struct Line : Moveable<Line> {
		bool          select:1;
		bool          enabled:1;
		bool          visible:1;
		bool          heading:1;
		Color         paper;
		Vec<Val>   line;

		Line() { select = false; enabled = true; visible = true; heading = false; paper = Null; }
	};

	static i32 StdValCompare(const Val& a, const Val& b) { return drx::StdValCompare(a, b); }


	Vec<Line>               array;
	HeaderCtrl                 header;
	ScrollBar                  sb;
	Scroller                   scroller;
	Array<Column>              column;
	Vec<Control>            control;
	ArrayMap<Id, IdInfo>       idx;
	Vec<Ln>                 ln;
	Vec< Vec<CellInfo> > cellinfo;
	Vec<bool>               modify;
	Vec<i32>                column_width, column_pos;
	DisplayPopup               info;
	const Order               *columnsortsecondary;
	i32                        min_visible_line, max_visible_line;
	i32                        ctrl_low, ctrl_high;
	i32                        sorting_from;
	Index<Txt>                 id_ndx;
	MarginFrame                scrollbox;

	i32   keypos;
	i32   cursor;
	i32   anchor;
	i32   linecy;
	i32   virtualcount;
	Point clickpos;
	i32   dropline, dropcol;
	i32   sortcolumn;
	bool  sortcolumndescending;
	bool  columnsortfindkey;
	i32   acceptingrow;

	mutable i32   selectcount;

	Txt row_name;
	Color  gridcolor;
	Color  evenpaper, evenink, oddpaper, oddink;

	bool  horzgrid:1;
	bool  vertgrid:1;
	bool  nocursor:1;
	bool  mousemove:1;
	bool  editmode:1;
	bool  insertmode:1;

	bool  inserting:1;
	bool  appending:1;
	bool  appendline:1;
	bool  noinsertappend:1;
	bool  autoappend:1;
	bool  removing:1;
	bool  moving:1;
	bool  askremove:1;
	bool  duplicating:1;
	bool  multiselect:1;
	bool  hasctrls:1;
	bool  headerctrls:1;
	bool  popupex:1;
	bool  nobg:1;
	bool  focussetcursor:1;
	bool  allsorting:1;
	bool  spanwidecells:1;
	bool  accept_edits:1;

	mutable bool  selectiondirty:1;

	unsigned  bains:2;

	bool  isdrag:1;
	bool  selclick:1;

	Image cursor_override;

	i32    Pos(i32 np) const;

	void   InsertCache(i32 i);
	void   RemoveCache(i32 i);

	void   SetSb();
	void   MinMaxLine();
	void   SyncColumnsPos();
	void   HeaderLayout();
	void   HeaderScroll();
	void   Scroll();
	i32    FindEnabled(i32 i, i32 dir);
	void   Page(i32 q);

	void   DoPoint(Point p, bool dosel = true);
	void   DoClick(Point p, dword flags);
	i32    GetClickColumn(i32 ii, Point p);
	void   ClickColumn(Point p);
	void   ClickSel(dword flags);

	Point           FindCellCtrl(Ctrl *c);
	Ctrl           *SyncLineCtrls(i32 i, Ctrl *p = NULL);
	void            SyncPageCtrls();
	void            SyncCtrls(i32 i0 = 0);
	bool            IsCtrl(i32 i, i32 j) const;
	const CellCtrl& GetCellCtrl(i32 i, i32 j) const;
	CellCtrl&       GetCellCtrl(i32 i, i32 j);
	void            SetCtrlVal(i32 i, i32 ii, const Val& v);

	void   PlaceEdits();
	void   EndEdit();
	void   ColEditSetData(i32 col);
	void   CtrlSetData(i32 col);
	Val    Get0(i32 i, i32 ii) const;
	void   Set0(i32 i, i32 ii, const Val& v);
	void   AfterSet(i32 i, bool sync_ctrls = true);

	void   Reline(i32 i, i32 y);
	void   Remove0(i32 i);
	void   DisableCtrls();
	void   SetCtrls();
	void   DoRemovem();
	bool   KillCursor0();

	const Display& GetCellInfo(i32 i, i32 j, bool f0, Val& v, Color& fg, Color& bg, dword& st);
	Ctrl&  SetCtrl(i32 i, i32 j, Ctrl *newctrl, bool owned, bool value);
	Size   DoPaint(Draw& w, bool sample);
	void   SpanWideCell(const Display& d, const Val& q, i32 cm, i32& cw, Rect& r, i32 i, i32& j);

	bool   TestKey(i32 i, wchar key);

	bool   SetCursor0(i32 i, bool dosel = true);

	void   SyncSelection() const;
	void   KeyMultiSelect(i32 aanchor, dword key);

	void   HeaderScrollVisibility();

	void   RefreshSel();
	bool   DnDInsert(i32 line, i32 py, PasteClip& d, i32 q);
	void   DnD(i32 line, i32 col);
	enum { DND_INSERTLINE = -1, DND_LINE = -2 };

	bool   ColumnSortPred(i32 i1, i32 i2, i32 column, const ValOrder *o);
	bool   OrderPred(i32 i1, i32 i2, const ArrayCtrl::Order *o);
	bool   DescendingPred(i32 i1, i32 i2, const Gate<i32, i32> *pred);
	void   SyncInfo();
	void   SortA();
	void   SortB(const Vec<i32>& o);

	void   SelectOne(i32 i, bool sel = true, bool raise = true);

	i32    AsNdx(const Txt& id)              { return id_ndx.FindAdd(id); }

	using Ctrl::IsModified;

	// These are listed here as private because name has changed to SetMap/AddMap
	void       Set(i32 i, const ValMap& m);
	void       Add(const ValMap& m);

	bool       IsLineVisible0(i32 i) const { return i < 0 ? false : i < array.GetCount() ? array[i].visible : true; }

public: // temporary (TRC 06/07/28) // will be removed!
	Ctrl&  SetCtrl(i32 i, i32 j, Ctrl *newctrl) { return SetCtrl(i, j, newctrl, true, true); }

protected:
	virtual bool UpdateRow();
	virtual void RejectRow();

	void   ClearModify();

public:
	Event<>           WhenSel; // the most usual ArrayCtrl event

	Event<>           WhenLeftDouble;
	Event<Point>      WhenMouseMove;
	Event<>           WhenEnterKey;
	Event<>           WhenLeftClick;
	Event<Bar&>       WhenBar;
	Gate<>            WhenAcceptRow;
	Event<>           WhenUpdateRow;
	Event<>           WhenArrayAction;
	Event<>           WhenStartEdit;
	Event<>           WhenAcceptEdit;
	Event<>           WhenCtrlsAction;
	Event<>           WhenScroll;
	Event<>           WhenHeaderLayout;
	Event<>           WhenColumnSorted;

	Event<i32, bool&> WhenLineEnabled;
	Event<i32, bool&> WhenLineVisible;

	Event<>                     WhenDrag;
	Event<i32, i32, PasteClip&> WhenDropCell;
	Event<i32, PasteClip&>      WhenDropInsert;
	Event<i32, PasteClip&>      WhenDropLine;
	Event<PasteClip&>           WhenDrop;

	//Deprecated - use WhenSel
	Event<>           WhenEnterRow;
	Event<>           WhenKillCursor;
	Event<>           WhenCursor;
	Event<>           WhenSelection;

	IdInfo&    IndexInfo(i32 ii);
	IdInfo&    IndexInfo(const Id& id);
	IdInfo&    AddIndex(const Id& id);
	IdInfo&    AddIndex();
	i32        GetIndexCount() const        { return idx.GetCount(); }
	Id         GetId(i32 ii) const          { return idx.GetKey(ii); }
	i32        GetPos(const Id& id) const   { return idx.Find(id); }
	IdInfo&    SetId(i32 ii, const Id& id);
	IdInfo&    AddKey(const Id& id)         { ASSERT(idx.GetCount() == 0); return AddIndex(id); }
	IdInfo&    AddKey()                     { ASSERT(idx.GetCount() == 0); return AddIndex(); }
	Id         GetKeyId() const             { return idx.GetKey(0); }

	Column&    AddColumn(tukk text = NULL, i32 w = 0);
	Column&    AddColumn(const Id& id, tukk text, i32 w = 0);
	Column&    AddColumnAt(i32 ii, tukk text, i32 w = 0);
	Column&    AddColumnAt(const Id& id, tukk text, i32 w = 0);
	Column&    AddRowNumColumn(tukk text = NULL, i32 w = 0);

	i32                       GetColumnCount() const   { return column.GetCount(); }
	i32                       FindColumnWithPos(i32 pos) const;
	i32                       FindColumnWithId(const Id& id) const;
	Vec<i32>               FindColumnsWithPos(i32 pos) const;
	Vec<i32>               FindColumnsWithId(const Id& id) const;
	Column&                   ColumnAt(i32 i)          { return column[i]; }
	Column&                   ColumnAt(const Id& id)   { return column[FindColumnWithId(id)]; }
	HeaderCtrl::Column&       HeaderTab(i32 i)         { return header.Tab(i); }
	HeaderCtrl::Column&       HeaderTab(const Id& id)  { return header.Tab(FindColumnWithId(id)); }
	const Column&             ColumnAt(i32 i) const    { return column[i]; }
	const Column&             ColumnAt(const Id& id) const   { return column[FindColumnWithId(id)]; }
	const HeaderCtrl::Column& HeaderTab(i32 i) const   { return header.Tab(i); }
	const HeaderCtrl::Column& HeaderTab(const Id& id) const   { return header.Tab(FindColumnWithId(id)); }

	const HeaderCtrl&         HeaderObject() const     { return header; }
	HeaderCtrl&               HeaderObject()           { return header; }

	void       SerializeHeader(Stream& s)    { header.Serialize(s); } // deprecated
	void       SerializeSettings(Stream& s);

	IdInfo&    AddCtrl(Ctrl& ctrl);
	IdInfo&    AddCtrl(const Id& id, Ctrl& ctrl);
	IdInfo&    AddIdCtrl(Ctrl& ctrl)         { return AddCtrl(ctrl.GetLayoutId(), ctrl); }
	void       AddCtrlAt(i32 ii, Ctrl& ctrl);
	void       AddCtrlAt(const Id& id, Ctrl& ctrl);
	void       AddRowNumCtrl(Ctrl& ctrl);

	void       SetCount(i32 c);
	void       SetVirtualCount(i32 c);
	i32        GetCount() const;
	void       Clear();
	void       Shrink();
	Val      Get(i32 i, i32 ii) const;
	Val      Get(i32 i, const Id& id) const;
	void       Set(i32 i, i32 ii, const Val& v);
	void       Set(i32 i, const Id& id, const Val& v);

	Val      Get(i32 ii) const;
	Val      Get(const Id& id) const;
	Val      GetOriginal(i32 ii) const;
	Val      GetOriginal(const Id& id) const;
	bool       IsModified(i32 ii) const;
	bool       IsModified(const Id& id) const;
	Val      GetKey() const;
	Val      GetOriginalKey() const;
	void       Set(i32 ii, const Val& v);
	void       Set(const Id& id, const Val& v);

	Val      GetColumn(i32 row, i32 col) const;
	Val      GetConvertedColumn(i32 row, i32 col) const;

	i32        GetSelectCount() const;
	bool       IsSelection() const                              { return GetSelectCount(); }
	void       Select(i32 i, bool sel = true)                   { SelectOne(i, sel); }
	void       Select(i32 i, i32 count, bool sel = true);
	bool       IsSelected(i32 i) const                          { return i < array.GetCount() && array[i].select; }
	void       ClearSelection(bool raise = true);
	bool       IsSel(i32 i) const;
	Vec<i32> GetSelKeys() const;

	void       EnableLine(i32 i, bool e);
	void       DisableLine(i32 i)                               { EnableLine(i, false); }
	bool       IsLineEnabled(i32 i) const;
	bool       IsLineDisabled(i32 i) const                      { return !IsLineEnabled(i); }

	void       ShowLine(i32 i, bool e);
	void       HideLine(i32 i)                                  { ShowLine(i, false); }
	bool       IsLineVisible(i32 i) const;

	Vec<Val> ReadRow(i32 i) const; // deprecated name
	Vec<Val> GetLine(i32 i) const                          { return ReadRow(i); }

	void       Set(i32 i, const Vec<Val>& v);
	void       Set(i32 i, Vec<Val>&& v);
	void       Set(i32 i, const VecMap<Txt, Val>& m);

	void       Add();

#define  E__Add(I)      void Add(__List##I(E__Val));
	__Expand(E__Add)
#undef   E__Add

	void       Add(const Vec<Val>& v)                      { Set(array.GetCount(), v); }
	void       Add(Vec<Val>&& v)                           { Set(array.GetCount(), pick(v)); }
	void       Add(const Nuller& null)                          { Add((Val)Null); }
	void       Add(const VecMap<Txt, Val>& m);
	template <typename... Args>
	void       Add(const Args& ...args)                         { Add(gather<Vec<Val>>(args...)); }

	void       SetMap(i32 i, const ValMap& m);
	void       AddMap(const ValMap& m);
	ValMap   GetMap(i32 i) const;

	void       SetArray(i32 i, const ValArray& va);
	void       AddArray(const ValArray& va);
	ValArray GetArray(i32 i) const;

	void       AddSeparator();
	void       AddHeading(const Val& v);
	bool       IsHeading(i32 i) const                           { return i < array.GetCount() && array[i].heading; }

	void       Insert(i32 i);
	void       Insert(i32 i, i32 count);
	void       Insert(i32 i, const Vec<Val>& v);
	void       Insert(i32 i, const Vec< Vec<Val> >& v);

	void       Remove(i32 i);

	void       RemoveSelection();

	Image      GetDragSample();

	void       InsertDrop(i32 line, const Vec<Vec<Val>>& data, PasteClip& d, bool self);
	void       InsertDrop(i32 line, const Vec<Val>& data, PasteClip& d, bool self);
	void       InsertDrop(i32 line, const Val& data, PasteClip& d, bool self);
	void       InsertDrop(i32 line, const ArrayCtrl& src, PasteClip& d);
	void       InsertDrop(i32 line, PasteClip& d);

	bool       IsCursor() const                        { return cursor >= 0; }
	bool       SetCursor(i32 i);
	bool       KillCursor();
	void       CancelCursor();
	i32        GetCursor() const                       { return cursor; }
	void       GoBegin();
	void       GoEnd();
	i32        GetCursorSc() const;
	void       ScCursor(i32 a);
	void       CenterCursor();
	void       ScrollInto(i32 line);
	void       ScrollIntoCursor();
	void       SetCursorEditFocus();
	i32        GetScroll() const;
	void       ScrollTo(i32 sc);
	void       ShowAppendLine();
	bool       AcceptRow(bool endedit = false);

	void       Move(i32 d);
	void       SwapUp();
	void       SwapDown();

	i32        Find(const Val& v, i32 ii = 0, i32 from = 0) const;
	i32        Find(const Val& v, const Id& id, i32 from = 0) const;

	bool       FindSetCursor(const Val& val, i32 ii = 0, i32 from = 0);
	bool       FindSetCursor(const Val& val, const Id& id, i32 from = 0);

	void       ReArrange(const Vec<i32>& order);

	void       Sort(i32 from, i32 count, Gate<i32, i32> order);
	void       Sort(Gate<i32, i32> order);
	void       Sort(const ArrayCtrl::Order& order);
	void       Sort(i32 from, i32 count, const ArrayCtrl::Order& order);
	void       Sort(i32 (*compare)(const Vec<Val>& v1, const Vec<Val>& v2));
	void       Sort(i32 from, i32 count,
	                i32 (*compare)(const Vec<Val>& v1, const Vec<Val>& v2));
	void       Sort(i32 ii, i32 (*compare)(const Val& v1, const Val& v2)
	                = StdValCompare);
	void       Sort(const Id& id, i32 (*compare)(const Val& v1, const Val& v2)
	                = StdValCompare);
	void       Sort()                                  { Sort(0); }

	void       ColumnSort(i32 column, Gate<i32, i32> order);
	void       ColumnSort(i32 column, const ValOrder& order);
	void       ColumnSort(i32 column, i32 (*compare)(const Val& a, const Val& b) = StdValCompare);

	void       SetSortColumn(i32 ii, bool descending = false);
	void       ToggleSortColumn(i32 ii);
	void       DoColumnSort();
	i32        GetSortColumn() const                   { return sortcolumn; }
	bool       IsSortDescending() const                { return sortcolumndescending; }

	bool       IsInsert() const                        { return insertmode; }

	void            SetDisplay(i32 i, i32 col, const Display& d);
	void            SetRowDisplay(i32 i, const Display& d);
	void            SetColumnDisplay(i32 j, const Display& d);
	const Display&  GetDisplay(i32 row, i32 col);
	const Display&  GetDisplay(i32 col);

	void       RefreshRow(i32 i);

	void       SetCtrl(i32 i, i32 col, Ctrl& ctrl, bool value = true);
	Ctrl      *GetCtrl(i32 i, i32 col);
	template <class T>
	T&         CreateCtrl(i32 i, i32 col, bool value = true) { T *c = new T; SetCtrl(i, col, c, true, value); SyncLineCtrls(i); return *c; }

	ArrayCtrl& SetLineCy(i32 cy);
	ArrayCtrl& SetEditLineCy()                         { return SetLineCy(EditField::GetStdHeight() + DPI(4)); }
	void       SetLineCy(i32 i, i32 cy);
	i32        GetLineCy() const                       { return linecy; }
	i32        GetLineY(i32 i) const;
	i32        GetLineCy(i32 i) const;
	i32        GetTotalCy() const;
	i32        GetLineAt(i32 y) const;

	void       SetLineColor(i32 i, Color c);

	Rect       GetCellRect(i32 i, i32 col) const;
	Rect       GetCellRectM(i32 i, i32 col) const;
	Rect       GetScreenCellRect(i32 i, i32 col) const;
	Rect       GetScreenCellRectM(i32 i, i32 col) const;

	Point      GetClickPos() const                     { return clickpos; }
	i32        GetClickColumn() const                  { return clickpos.x; }
	i32        GetClickRow() const                     { return clickpos.y; }

	bool       StartEdit(i32 d = 0);
	i32        GetEditColumn() const;
	bool       IsEdit() const                          { return editmode; }

	void       DoEdit();
	void       DoInsert(i32 cursor);
	void       DoInsertBefore();
	void       DoInsertAfter();
	void       DoAppend();
	bool       DoRemove();
	void       DoDuplicate();
	void       DoSelectAll();
	void       StdBar(Bar& menu);

	bool       IsEditing() const;
	bool       AcceptEnter();

	void       ClearCache();
	void       InvalidateCache(i32 i);

	void       ScrollUp()                              { sb.PrevLine(); }
	void       ScrollDown()                            { sb.NextLine(); }
	void       ScrollPageUp()                          { sb.PrevPage(); }
	void       ScrollPageDown()                        { sb.NextPage(); }
	void       ScrollEnd()                             { sb.End(); }
	void       ScrollBegin()                           { sb.Begin(); }

	Txt     AsText(Txt (*format)(const Val&), bool sel = false,
	                  tukk tab = "\t", tukk row = "\r\n",
	                  tukk hdrtab = "\t", tukk hdrrow = "\r\n") const;
	void       SetClipboard(bool sel = false, bool hdr = true) const;
	Txt     AsQtf(bool sel = false, bool hdr = true);
	Txt     AsCsv(bool sel = false, i32 sep = ';', bool hdr = true);

	Txt     RowFormat(tukk s);

	ArrayCtrl& RowName(tukk s)                  { row_name = s; return *this; }
	ArrayCtrl& AppendLine(bool b = true)               { appendline = b; return *this; }
	ArrayCtrl& NoAppendLine()                          { return AppendLine(false); }
	bool       IsAppendLine() const                    { return appendline; }
	ArrayCtrl& Inserting(bool b = true)                { inserting = b; return AppendLine(b); }
	ArrayCtrl& NoInserting()                           { return Inserting(false); }
	bool       IsInserting() const                     { return inserting; }
	ArrayCtrl& Removing(bool b = true)                 { removing = b; return *this; }
	ArrayCtrl& NoRemoving()                            { return Removing(false); }
	bool       IsRemoving() const                      { return removing; }
	ArrayCtrl& Appending(bool b = true)                { appending = b; return *this; }
	bool       IsAppending() const                     { return appending || autoappend; }
	ArrayCtrl& AutoAppending(bool b = true)            { autoappend = b; return *this; }
	bool       IsAutoAppending() const                 { return autoappend; }
	ArrayCtrl& BeforeAfterInserting(i32 q = 1)         { bains = 1; return *this; }
	ArrayCtrl& AfterBeforeInserting(i32 q = 2)         { bains = 2; return *this; }
	ArrayCtrl& Duplicating(bool b = true)              { duplicating = b; return *this; }
	ArrayCtrl& NoInsertAppend(bool b = true)           { noinsertappend = b; return *this; }
	bool       IsDuplicating() const                   { return duplicating; }
	ArrayCtrl& Moving(bool b = true)                   { moving = b; return *this; }
	bool       IsMoving() const                        { return moving; }
	ArrayCtrl& NoDuplicating()                         { return Duplicating(false); }
	ArrayCtrl& AskRemove(bool b = true)                { askremove = b; return *this; }
	ArrayCtrl& NoAskRemove()                           { return AskRemove(false); }
	bool       IsAskRemove() const                     { return askremove; }

	ArrayCtrl& Header(bool b = true)                   { header.Invisible(!b); return *this; }
	ArrayCtrl& NoHeader()                              { return Header(false); }
	ArrayCtrl& Track(bool b = true)                    { header.Track(b); return *this; }
	ArrayCtrl& NoTrack()                               { return Track(false); }
	ArrayCtrl& VertGrid(bool b = true)                 { vertgrid = b; Refresh(); return *this; }
	ArrayCtrl& NoVertGrid()                            { return VertGrid(false); }
	ArrayCtrl& HorzGrid(bool b = true)                 { horzgrid = b; Refresh(); return *this; }
	ArrayCtrl& NoHorzGrid()                            { return HorzGrid(false); }
	ArrayCtrl& Grid(bool b = true)                     { return VertGrid(b).HorzGrid(b); }
	ArrayCtrl& NoGrid()                                { return Grid(false); }
	ArrayCtrl& GridColor(Color c)                      { gridcolor = c; return *this; }
	ArrayCtrl& EvenRowColor(Color paper = Blend(SColorMark, SColorPaper, 220), Color ink = SColorText);
	ArrayCtrl& OddRowColor(Color paper = SColorInfo, Color ink = SColorText);
	ArrayCtrl& NoCursor(bool b = true)                 { nocursor = b; return *this; }
	ArrayCtrl& MouseMoveCursor(bool b = true)          { mousemove = b; return *this; }
	ArrayCtrl& NoMouseMoveCursor()                     { return MouseMoveCursor(false); }
	ArrayCtrl& AutoHideSb(bool b = true)               { sb.AutoHide(b); return *this; }
	ArrayCtrl& NoAutoHideSb()                          { return AutoHideSb(false); }
	ArrayCtrl& HideSb(bool b = true)                   { sb.Show(!b); return *this; }
	ArrayCtrl& AutoHideHorzSb(bool b = true)           { header.AutoHideSb(b); return *this; }
	ArrayCtrl& NoAutoHideHorzSb()                      { return AutoHideHorzSb(false); }
	ArrayCtrl& HideHorzSb(bool b = true)               { header.HideSb(b); return *this; }

	ArrayCtrl& MultiSelect(bool b = true)              { multiselect = b; return *this; }
	bool       IsMultiSelect() const                   { return multiselect; }
	ArrayCtrl& NoBackground(bool b = true)             { nobg = b; Transparent(); Refresh(); return *this; }
	ArrayCtrl& PopUpEx(bool b = true)                  { popupex = b; SyncInfo(); return *this; }
	ArrayCtrl& NoPopUpEx()                             { return PopUpEx(false); }
	ArrayCtrl& NoFocusSetCursor()                      { focussetcursor = false; return *this; }
	ArrayCtrl& MovingHeader(bool b)                    { header.Moving(b); return *this; }
	ArrayCtrl& NoMovingHeader()                        { return MovingHeader(false); }
	ArrayCtrl& ColumnSortFindKey(bool b = true)        { columnsortfindkey = b; return *this; }
	ArrayCtrl& AllSorting();
	ArrayCtrl& ColumnSortSecondary(const Order& order) { columnsortsecondary = &order; return *this; }
	ArrayCtrl& NoColumnSortSecondary()                 { columnsortsecondary = NULL; return *this; }
	ArrayCtrl& SortingFrom(i32 from)                   { sorting_from = from; return *this; }

	ArrayCtrl& ColumnWidths(tukk s);
	Txt     GetColumnWidths();

	ArrayCtrl& SetScrollBarStyle(const ScrollBar::Style& s)   { sb.SetStyle(s); header.SetScrollBarStyle(s); return *this; }
	ArrayCtrl& SetHeaderCtrlStyle(const HeaderCtrl::Style& s) { header.SetStyle(s); return *this; }

	ArrayCtrl& CursorOverride(const Image& arrow)             { cursor_override = arrow; return *this; }
	ArrayCtrl& NoCursorOverride()                             { return CursorOverride(Null); }

	ArrayCtrl& SpanWideCells(bool b = true)                   { spanwidecells = b; Refresh(); return *this; }

	ArrayCtrl& AcceptEdits(bool b = true)                     { accept_edits = b; return *this; }

	void Reset();

	typedef ArrayCtrl CLASSNAME;

	ArrayCtrl();
	virtual ~ArrayCtrl();
};

class ArrayOption : public Display, public Pte<ArrayOption> {
public:
	typedef ArrayOption CLASSNAME;
	ArrayOption();
	virtual ~ArrayOption();

	virtual void       Paint(Draw& w, const Rect& r, const Val& q, Color ink, Color paper, dword style) const;

	void               Connect(ArrayCtrl& ac, i32 ii = 0);
	void               Connect(ArrayCtrl& ac, const Id& id)        { Connect(ac, ac.GetPos(id)); }

	void               Disconnect();

	ArrayCtrl::Column& AddColumn(ArrayCtrl& ac, tukk text = NULL, i32 w = 0);
	ArrayCtrl::Column& AddColumn(ArrayCtrl& ac, const Id& id, tukk text, i32 w = 0);

	ArrayOption&       TrueFalse(Val _t, Val _f)               { t = _t; f = _f; return *this; }
	Val              GetFalse() const                            { return f; }
	Val              GetTrue() const                             { return t; }
	ArrayOption&       ThreeState(bool b = true)                   { threestate = b; return *this; }
	ArrayOption&       NoThreeState()                              { return ThreeState(false); }
	bool               IsThreeState() const                        { return threestate; }

	ArrayOption&       HSwitch(bool hs = true)                     { hswitch = hs; return *this; }
	ArrayOption&       NoHSwitch()                                 { return HSwitch(false); }
	bool               IsHSwitch() const                           { return hswitch; }

	ArrayOption&       VSwitch(bool vs = true)                     { vswitch = vs; return *this; }
	ArrayOption&       NoVSwitch()                                 { return VSwitch(false); }
	bool               IsVSwitch() const                           { return vswitch; }

	ArrayOption&       Frame(bool frm = true)                      { frame = frm; return *this; }
	ArrayOption&       NoFrame()                                   { return Frame(false); }
	bool               IsFrame() const                             { return frame; }

	void               Click();

public:
	Event<>            WhenAction;

	Event<>            operator <<= (Event<>  cb)                  { return WhenAction = cb; }

private:
	Vec<i32>        index;
	ArrayCtrl         *array;
	Val              t, f;
	bool               hswitch, vswitch;
	bool               threestate;
	bool               frame;
};