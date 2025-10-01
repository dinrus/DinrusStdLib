class TextCtrl : public Ctrl, protected TextArrayOps {
public:
	virtual void  SetData(const Val& v);
	virtual Val   GetData() const;
	virtual void  CancelMode();
	virtual Txt   GetSelectionData(const Txt& fmt) const;
	virtual void  MiddleDown(Point p, dword flags);

public:
	struct UndoRec {
		i32    serial;
		i32    pos;
		i32    size;
		Txt    data;
		bool   typing;

		void   SetText(const Txt& text) { data = FastCompress(text); }
		Txt    GetText() const             { return FastDecompress(data); }
	};

	struct UndoData {
		i32              undoserial;
		BiArray<UndoRec> undo;
		BiArray<UndoRec> redo;

		void Clear()                     { undo.Clear(); redo.Clear(); }
	};

	struct EditPos : Moveable<EditPos> {
		i32   sby;
		z64   cursor;

		void Serialize(Stream& s);
		void Clear()                      { sby = 0; cursor = 0; }
		EditPos()                         { Clear(); }
	};

	enum {
		INK_NORMAL,
		INK_DISABLED,
		INK_SELECTED,
		PAPER_NORMAL,
		PAPER_READONLY,
		PAPER_SELECTED,
		WHITESPACE,
		WARN_WHITESPACE,
		COLOR_COUNT,
	};

protected:
	virtual z64     GetTotal() const                  { return total; }
	virtual i32     GetCharAt(z64 i) const          { return GetChar(i); }

	virtual void    DirtyFrom(i32 line);
	virtual void    SelectionChanged();
	virtual void    ClearLines();
	virtual void    InsertLines(i32 line, i32 count);
	virtual void    RemoveLines(i32 line, i32 count);
	virtual void    PreInsert(i32 pos, const WTxt& text);
	virtual void    PostInsert(i32 pos, const WTxt& text);
	virtual void    PreRemove(i32 pos, i32 size);
	virtual void    PostRemove(i32 pos, i32 size);
	virtual void    SetSb();
	virtual void    PlaceCaret(z64 newcursor, bool sel = false);
	virtual void    InvalidateLine(i32 i);
	virtual i32     RemoveRectSelection();
	virtual WTxt    CopyRectSelection();
	virtual i32     PasteRectSelection(const WTxt& s);
	virtual Txt     GetPasteText();

	struct Ln : Moveable<Ln> {
		i32    len;
		Txt text;

		Ln()                             { len = 0; }
	};

	Vec<Ln>       lin;
	z64              total;

	z64              cpos;
	i32              cline;
	z64              cursor, anchor;
	i32              undoserial;
	bool             rectsel;
	bool             incundoserial;
	i32              undosteps;
	BiArray<UndoRec> undo;
	BiArray<UndoRec> redo;
	i32              dirty;
	i32              undo_op;
	u8               charset;
	bool             truncated;

	bool             selclick;
	Point            dropcaret;
	bool             isdrag;

	Color            color[COLOR_COUNT];

	bool             processtab, processenter;
	bool             nobg;
	i32              max_total;
	i32              max_line_len;

	mutable Stream  *view;
	struct ViewCache {
		i32        blk;
		Vec<Ln> line;
	};
	mutable ViewCache view_cache[2];
	mutable i32       viewlines;

	Vec<z64>       offset256;
	Vec<i32>       total256;
	i32               view_loading_lock;
	z64               view_loading_pos;
	bool              view_all;

	void   IncDirty();
	void   DecDirty();
	i32    Insert0(i32 pos, const WTxt& txt);
	void   Remove0(i32 pos, i32 size);
	i32    InsertU(i32 pos, const WTxt& txt, bool typing = false);
	void   RemoveU(i32 pos, i32 size);
	void   Undodo();
	i32    Insert(i32 pos, const WTxt& txt, bool typing);
	void   DoPaste() { Paste(); }
	void   DoRemoveSelection() { RemoveSelection(); }
	void   RefreshLines(i32 l1, i32 l2);
	static bool   IsUnicodeCharset(u8 charset);

	i32    Load0(Stream& in, u8 charset, bool view);
	i32    LoadLines(Vec<Ln>& ls, i32 n, z64& total, Stream& in, u8 charset,
	                 i32 max_line_len, i32 max_total, bool& truncated,
	                 i32 *view_line_count = NULL) const;
	void   ViewLoading();

	void   SetLine(i32 i, const Txt& txt, i32 len) { lin[i].text = txt; lin[i].len = len; }
	void   SetLine(i32 i, const WTxt& w)           { SetLine(i, ToUtf8(w), w.GetCount()); }
	void   LineRemove(i32 i, i32 n)                   { lin.Remove(i, n); }
	void   LineInsert(i32 i, i32 n)                   { lin.InsertN(i, n); }
	const Ln& GetLn(i32 i) const;

	i32    GetLinePos32(i32& pos) const;
	bool   GetSelection32(i32& l, i32& h) const;
	i32    GetPos32(i32 line, i32 column = 0) const   { return (i32)GetPos64(line, column); }
	i32    GetLength32() const;
	i32    GetCursor32() const;

public:
	virtual void   RefreshLine(i32 i);

	Event<Bar&>  WhenBar;
	Event<>      WhenState;
	Event<>      WhenSel;
	Event<z64>   WhenViewMapping;

	void   CachePos(z64 pos);
	void   CacheLinePos(i32 linei);

	enum CS { CHARSET_UTF8_BOM = 250, CHARSET_UTF16_LE, CHARSET_UTF16_BE, CHARSET_UTF16_LE_BOM, CHARSET_UTF16_BE_BOM };
	enum LE { LE_DEFAULT, LE_CRLF, LE_LF };

	i32    Load(Stream& s, u8 charset = CHARSET_DEFAULT)    { return Load0(s, charset, false); }
	bool   IsTruncated() const                                { return truncated; }
	void   Save(Stream& s, u8 charset = CHARSET_DEFAULT, i32 line_endings = LE_DEFAULT) const;

	i32    GetInvalidCharPos(u8 charset = CHARSET_DEFAULT) const;
	bool   CheckCharset(u8 charset = CHARSET_DEFAULT) const { return GetInvalidCharPos(charset) < 0; }

	i32    LimitSize(z64 size) const                        { return i32(view ? min((z64)max_total, size) : size); }

	i32    GetLinePos(i32& pos) const                         { return GetLinePos32(pos); }
	i32    GetPos(i32 line, i32 column = 0) const             { return GetPos32(line, column); }

	void    Set(const WTxt& s);
	void    Set(const Txt& s, u8 charset = CHARSET_DEFAULT);
	Txt     Get(u8 charset = CHARSET_DEFAULT) const;
	Txt     Get(z64 pos, i32 size, u8 charset = CHARSET_DEFAULT) const;
	WTxt    GetW(z64 pos, i32 size) const;
	WTxt    GetW() const                      { return GetW(0, LimitSize(GetLength64())); }

	void   ClearDirty();
	bool   IsDirty() const                    { return dirty; }

	void   Clear();

	i32    GetLine(z64 pos) const           { return GetLinePos64(pos); }

	const Txt& GetUtf8Line(i32 i) const;
	WTxt       GetWLine(i32 i) const       { return ToUtf32(GetUtf8Line(i)); }
	Txt        GetEncodedLine(i32 i, u8 charset = CHARSET_DEFAULT) const;
	i32        GetLineLength(i32 i) const;

	i32     GetLength() const                 { return GetLength32(); }
	i32     GetLineCount() const              { return view ? viewlines : lin.GetCount(); }
	i32     GetChar(z64 pos) const;
	i32     GetChar() const                   { return cursor < GetLength64() ? GetChar(cursor) : 0; }
	i32     operator[](z64 pos) const       { return GetChar(pos); }

	i32     GetCursor() const                 { return GetCursor32(); }
	i32     GetCursorLine() const             { return GetLine(GetCursor64()); }

	void    SetSelection(z64 anchor = 0, z64 cursor = INT_MAX);
	bool    IsSelection() const               { return IsAnySelection() && !rectsel; }
	bool    IsRectSelection() const           { return IsAnySelection() && rectsel; }
	bool    IsAnySelection() const            { return anchor >= 0 && anchor != cursor; }
	bool    GetSelection(i32& l, i32& h) const{ return GetSelection32(l, h); }
	bool    GetSelection(z64& l, z64& h) const;
	Txt     GetSelection(u8 charset = CHARSET_DEFAULT) const;
	WTxt    GetSelectionW() const;
	void    ClearSelection();
	bool    RemoveSelection();
	void    SetCursor(z64 cursor)              { PlaceCaret(cursor); }
	i32     Paste(const WTxt& text);

	i32     Insert(i32 pos, const WTxt& txt)  { return Insert(pos, txt, false); }
	i32     Insert(i32 pos, const Txt& txt, u8 charset = CHARSET_DEFAULT);
	i32     Insert(i32 pos, tukk txt)     { return Insert(pos, WTxt(txt)); }
	void    Remove(i32 pos, i32 size);

	i32     Append(const WTxt& txt)           { return Insert(GetLength(), txt); }
	i32     Append(const Txt& txt, u8 charset = CHARSET_DEFAULT) { return Insert(GetLength(), txt, charset); }
	i32     Append(tukk txt)              { return Append(WTxt(txt)); }

	void      NextUndo();
	void      Undo();
	void      Redo();
	bool      IsUndo() const                  { return undo.GetCount(); }
	bool      IsRedo() const                  { return redo.GetCount(); }
	void      ClearUndo()                     { undo.Clear(); redo.Clear(); }
	void      ClearRedo()                     { redo.Clear(); }
	bool      IsUndoOp() const                { return undo_op; }
	UndoData  PickUndoData();
	void      SetPickUndoData(UndoData&& data);

	void      Cut();
	void      Copy();
	void      Paste();
	void      SelectAll();

	void      StdBar(Bar& menu);

	void      SetCharset(u8 cs)              { charset = ResolveCharset(cs); }
	u8        GetCharset() const               { return charset; }

	void      SetColor(i32 i, Color c)         { color[i] = c; Refresh(); }
	Color     GetColor(i32 i) const            { return color[i]; }

	i32       View(Stream& s, u8 charset = CHARSET_DEFAULT)    { return Load0(s, charset, true); }
	void      WaitView(i32 line = INT_MAX, bool progress = false);
	void      LockViewMapping()                                  { view_loading_lock++; }
	void      UnlockViewMapping();
	void      SerializeViewMap(Stream& s);
	bool      IsView() const                                     { return view; }
	z64       GetViewSize() const                                { return view ? view->GetSize() : 0; }

	i32       GetLinePos64(z64& pos) const;
	z64       GetPos64(i32 line, i32 column = 0) const;
	z64       GetLength64() const                                { return total; }
	z64       GetCursor64() const                                { return cursor; }

	TextCtrl& UndoSteps(i32 n)                 { undosteps = n; Undodo(); return *this; }
	i32       GetUndoSteps() const             { return undosteps; }
	TextCtrl& ProcessTab(bool b = true)        { processtab = b; return *this; }
	TextCtrl& NoProcessTab()                   { return ProcessTab(false); }
	TextCtrl& ProcessEnter(bool b = true)      { processenter = b; return *this; }
	TextCtrl& NoProcessEnter()                 { return ProcessEnter(false); }
	TextCtrl& NoBackground(bool b = true)      { nobg = b; Transparent(); Refresh(); return *this; }
	TextCtrl& MaxLength(i32 len, i32 linelen)  { max_total = len; max_line_len = linelen; return *this; }
	bool      IsNoBackground() const           { return nobg; }
	bool      IsProcessTab() const             { return processtab; }
	bool      IsProcessEnter() const           { return processenter; }

	typedef TextCtrl CLASSNAME;

	TextCtrl();
	virtual ~TextCtrl();
};

class LineEdit : public TextCtrl {
public:
	virtual bool   Key(dword key, i32 count);
	virtual void   Paint(Draw& w);
	virtual void   LeftDown(Point p, dword flags);
	virtual void   RightDown(Point p, dword flags);
	virtual void   LeftRepeat(Point p, dword keyflags);
	virtual void   LeftDouble(Point p, dword keyflags);
	virtual void   LeftTriple(Point p, dword keyflags);
	virtual void   LeftUp(Point p, dword flags);
	virtual void   LeftDrag(Point p, dword flags);
	virtual void   MouseMove(Point p, dword flags);
	virtual void   MouseWheel(Point, i32 zdelta, dword);
	virtual Image  CursorImage(Point, dword);
	virtual void   DragAndDrop(Point p, PasteClip& d);
	virtual void   DragRepeat(Point p);
	virtual void   DragLeave();
	virtual void   Layout();
	virtual void   RefreshLine(i32 i);
	virtual Font   GetPreeditFont();
	virtual Rect   GetCaret() const;

protected:
	virtual void    SetSb();
	virtual void    PlaceCaret(z64 newcursor, bool sel = false);
	virtual i32     RemoveRectSelection();
	virtual WTxt    CopyRectSelection();
	virtual i32     PasteRectSelection(const WTxt& s);

public:
	enum Flags {
		SHIFT_L = 1,
		SHIFT_R = 2,
		SPELLERROR = 4,
		NOENDFILL = 8,
	};

	struct Highlight : Moveable<Highlight> {
		Color paper;
		Color ink;
		Font  font;
		word  flags;
		wchar chr;

		bool operator==(const Highlight& h) const
		     { return paper == h.paper && ink == h.ink && font == h.font; }

		Highlight() { flags = 0; }
	};

protected:
	virtual void  HighlightLine(i32 line, Vec<Highlight>& h, z64 pos);
	virtual void  NewScrollPos();

	ScrollBars       sb;
	i32              gcolumn;
	z64              mpos;


	Font             font;
	Size             fsz;
	CharFilter       filter;
	i32              tabsize;
	i32              bordercolumn;
	Color            bordercolor;
	Color            hline, vline;
	i32              vlinex;
	Scroller         scroller;
	Point            caretpos;
	bool             nohbar;
	bool             showtabs;
	bool             cutline;
	bool             overwrite;
	bool             showspaces;
	bool             showlines;
	bool             showreadonly;
	bool             warnwhitespace;
	bool             dorectsel; // TODO: Refactor this ugly hack!

	void   Paint0(Draw& w);

	void   AlignChar();

	void   MovePage(i32 dir, bool sel);

	void   PlaceCaret0(Point p);
	i32    PlaceCaretNoG(z64 newcursor, bool sel = false);

	void   Scroll();
	void   SetHBar();
	Rect   DropCaret();
	void   RefreshDropCaret();
	void   DoPasteColumn() { PasteColumn(); }
	void   SyncFont();
	bool   IsDoubleChar(i32 ch) const;

	struct RefreshDraw;
	friend class TextCtrl;

	i32    GetMousePos32(Point p) const       { return (i32)GetMousePos(p); }

public:
	Event<> WhenScroll;

	Size   GetFontSize() const;
	z64    GetGPos(i32 ln, i32 cl) const;
	z64    GetMousePos(Point p) const;
	Point  GetColumnLine(z64 pos) const;
	z64    GetColumnLinePos(Point pos) const  { return GetGPos(pos.y, pos.x); }
	Point  GetIndexLine(z64 pos) const;
	z64    GetIndexLinePos(Point pos) const;

	void   SetRectSelection(z64 l, z64 h);
	void   SetRectSelection(const Rect& rect);
	Rect   GetRectSelection() const;
	bool   GetRectSelection(const Rect& rect, i32 line, z64& l, z64& h);

	void   ScrollUp()                         { sb.LineUp(); }
	void   ScrollDown()                       { sb.LineDown(); }
	void   ScrollPageUp()                     { sb.PageUp(); }
	void   ScrollPageDown()                   { sb.PageDown(); }
	void   ScrollBegin()                      { sb.VertBegin(); }
	void   ScrollEnd()                        { sb.VertEnd(); }

	Rect   GetLineScreenRect(i32 line) const;

	void   TopCursor(i32 lines = 0);
	void   CenterCursor();

	void   MoveUpDown(i32 n, bool sel = false);

	void   MoveLeft(bool sel = false);
	void   MoveRight(bool sel = false);
	void   MoveUp(bool sel = false);
	void   MoveDown(bool sel = false);
	void   MovePageUp(bool sel = false);
	void   MovePageDown(bool sel = false);
	void   MoveHome(bool sel = false);
	void   MoveEnd(bool sel = false);
	void   MoveTextBegin(bool sel = false);
	void   MoveTextEnd(bool sel = false);

	bool   InsertChar(dword key, i32 count = 1, bool canoverwrite = false);
	void   DeleteChar();
	void   Backspace();
	void   DeleteLine();
	void   CutLine();

	void   PasteColumn(const WTxt& text);
	void   PasteColumn();

	void   Sort();

	Point   GetScrollPos() const              { return sb; }
	Size    GetPageSize()                     { return sb.GetPage(); }
	void    SetScrollPos(Point p)             { sb.Set(p); }

	EditPos GetEditPos() const;
	void    SetEditPos(const EditPos& pos);
	void    SetEditPosSb(const LineEdit::EditPos& pos);
	void    SetEditPosSbOnly(const LineEdit::EditPos& pos);

	void    ScrollIntoCursor();

	Point   GetCaretPoint() const             { return caretpos; }

	void    Clear();

	void    OverWriteMode(bool o = true)      { overwrite = o; PlaceCaret(cursor, false); }
	bool    IsOverWriteMode() const           { return overwrite; }

	void    RefreshChars(bool (*predicate)(i32 c));

	LineEdit& TabSize(i32 n);
	i32       GetTabSize() const              { return tabsize; }
	LineEdit& BorderColumn(i32 col, Color c = SColorFace());
	i32       GetBorderColumn() const         { return bordercolumn; }
	LineEdit& SetFont(Font f);
	Font      GetFont() const                 { return font; }
	LineEdit& NoHorzScrollbar(bool b = true)  { nohbar = b; ScrollIntoCursor(); return *this; }
	bool      IsNoHorzScrollbar() const       { return nohbar; }
	LineEdit& ShowTabs(bool st = true)        { showtabs = st; Refresh(); return *this; }
	bool      IsShowTabs() const              { return showtabs; }
	LineEdit& ShowSpaces(bool ss = true)      { showspaces = ss; Refresh(); return *this; }
	bool      IsShowSpaces() const            { return showspaces; }
	LineEdit& ShowLineEndings(bool sl = true) { showlines = sl; Refresh(); return *this; }
	bool      IsShowLineEndings() const       { return showlines; }
	LineEdit& WarnWhiteSpace(bool b = true)   { warnwhitespace = b; Refresh(); return *this; }
	bool      IsWantWhiteSpace() const        { return warnwhitespace; }
	LineEdit& WithCutLine(bool b)             { cutline = b; return *this; }
	LineEdit& NoCutLine()                     { return WithCutLine(false); }
	bool      IsWithCutLine() const           { return cutline; }
	LineEdit& SetFilter(wchar (*f)(wchar c))      { filter = f; return *this; }
	LineEdit& ShowReadOnly(bool b)            { showreadonly = b; Refresh(); return *this; }
	LineEdit& NoShowReadOnly()                { return ShowReadOnly(false); }
	bool      IsShowReadOnly()                { return showreadonly; }
	LineEdit& ShowCurrentLine(Color color)    { hline = color; Refresh(); return *this; }
	LineEdit& ShowCurrentColumn(Color color)  { vline = color; Refresh(); return *this; }

	LineEdit& SetScrollBarStyle(const ScrollBar::Style& s)   { sb.SetStyle(s); return *this; }

	typedef LineEdit CLASSNAME;

	LineEdit();
	virtual ~LineEdit();
};

class DocEdit : public TextCtrl {
public:
	virtual void  Paint(Draw& w);
	virtual void  Layout();
	virtual bool  Key(dword key, i32 count);
	virtual void  LeftDown(Point p, dword flags);
	virtual void  LeftDouble(Point p, dword keyflags);
	virtual void  LeftTriple(Point p, dword keyflags);
	virtual void  LeftUp(Point p, dword flags);
	virtual void  RightDown(Point p, dword w);
	virtual void  MouseMove(Point p, dword flags);
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual Image CursorImage(Point p, dword keyflags);
	virtual void  GotFocus();
	virtual void  LostFocus();
	virtual void  RefreshLine(i32 i);
	virtual void  SetSb();

	virtual void  DragAndDrop(Point p, PasteClip& d);
	virtual void  DragRepeat(Point p);
	virtual void  DragLeave();
	virtual void  LeftDrag(Point p, dword flags);

	virtual Rect  GetCaret() const;

protected:
	virtual void  ClearLines();
	virtual void  InsertLines(i32 line, i32 count);
	virtual void  RemoveLines(i32 line, i32 count);
	virtual void  PlaceCaret(z64 pos, bool select = false);
	virtual void  InvalidateLine(i32 i);

	struct Para : Moveable<Para> {
		i32    cx, cy;

		Para()         { cx = -1; cy = 0; }
	};

	Vec<Para> para;
	Font         font;
	i32          after;
	CharFilter   filter;
	ScrollBar    sb;
	i32          cx;
	bool         updownleave, eofline;
	Rect         caret;

	struct Fmt {
		FontInfo      fi;
		i32           len;
		Buffer<wchar> text;
		Buffer<i32>   width;
		Vec<i32>   line;
		i32           LineEnd(i32 i)  {
			return i < line.GetCount() - 1 ? line[i + 1] : len;
		}
	};
	Fmt    Format(const WTxt& text) const;

	void   Invalidate();
	i32    GetHeight(i32 i);
	void   Scroll();
	void   PlaceCaret(bool scroll);
	i32    GetY(i32 parai);
	i32    GetCursorPos(Point p);
	Point  GetCaret(i32 pos);
	void   VertMove(i32 delta, bool select, bool scs);
	void   HomeEnd(i32 x, bool select);
	void   RefreshStyle();
	Rect   DropCaret();
	void   RefreshDropCaret();
	i32    GetMousePos(Point p);

public:
	DocEdit&  After(i32 a)                                   { after = a; RefreshStyle(); return *this; }
	DocEdit&  SetFont(Font f)                                { font = f; RefreshStyle(); return *this; }
	DocEdit&  SetFilter(wchar (*f)(wchar c))                 { filter = f; return *this; }
	DocEdit&  AutoHideSb(bool b = true)                      { sb.AutoHide(b); return *this; }
	bool      IsAutoHideSb() const                           { return sb.IsAutoHide(); }
	DocEdit&  UpDownLeave(bool u = true)                     { updownleave = u; return *this; }
	DocEdit&  NoUpDownLeave()                                { return UpDownLeave(false); }
	bool      IsUpDownLeave() const                          { return updownleave; }
	DocEdit&  SetScrollBarStyle(const ScrollBar::Style& s)   { sb.SetStyle(s); return *this; }
	DocEdit&  EofLine(bool b = true)                         { eofline = b; return *this; }
	DocEdit&  NoEofLine()                                    { return EofLine(false); }
	bool      IsEofLine() const                              { return eofline; }

	EditPos   GetEditPos() const;
	void      SetEditPos(const TextCtrl::EditPos& pos);

	typedef DocEdit CLASSNAME;

	DocEdit();
	virtual ~DocEdit();
};