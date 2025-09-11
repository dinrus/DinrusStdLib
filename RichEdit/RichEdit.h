#ifndef RICHEDIT_H
#define RICHEDIT_H

#include <drx/CtrlLib/CtrlLib.h>

namespace drx {

#define IMAGECLASS RichEditImg
#define IMAGEFILE <drx/RichEdit/RichEdit.iml>
#include <drx/Draw/iml_header.h>

class RichRuler : public FrameCtrl<Ctrl> {
public:
	virtual void FrameLayout(Rect& r);
	virtual void FrameAddSize(Size& sz);
	virtual void Paint(Draw& w);
	virtual void LeftDown(Point p, dword);
	virtual void RightDown(Point p, dword);
	virtual void LeftDouble(Point p, dword);
	virtual void MouseMove(Point p, dword);
	virtual void LeftUp(Point, dword);

public:
	struct Marker {
		i32   pos, minpos, maxpos;
		Image image;
		bool  top;
		bool  deletable;

		bool operator!=(const Marker& m) {
			return pos != m.pos || minpos != m.minpos || maxpos != m.maxpos ||
			       !image.IsSame(m.image) || top != m.top || deletable != m.deletable;
		}
		Marker() { top = false; deletable = false; }
	};

private:
	i32    x0, pgcx, numbers, marks;
	double grid, numbermul;
	Zoom   zoom;

	i32    track, pos;
	i32    trackdx;
	i32    snap;

	Array<Marker> marker;
	i32           tabpos;
	i32           tabsize;
	i32           newtabalign;

public:
	Event<>       WhenLeftDouble;
	Event<>       WhenLeftDown;
	Event<>       WhenRightDown;
	Event<>       WhenBeginTrack;
	Event<>       WhenTrack;
	Event<>       WhenEndTrack;

	void          SetLayout(i32 x, i32 pgcx, Zoom zoom, double grid,
	                        i32 numbers = INT_MAX, double numbermul = 1, i32 marks = INT_MAX,
	                        i32 snap = 32);

	void          Clear();
	void          Set(i32 i, const Marker& m);
	void          SetCount(i32 n);
	i32           GetCount() const        { return marker.GetCount(); }
	const Marker& operator[](i32 i)       { return marker[i]; }

	i32           FindMarker(Point p);
	i32           GetTrack()              { return track; }
	i32           GetPos()                { return pos; }

	void          SetTabs(i32 pos, i32 size);

	i32           GetNewTabAlign()        { return newtabalign; }

	RichRuler();
	virtual ~RichRuler();
};

enum {
	UNIT_DOT,
	UNIT_POINT,
	UNIT_INCH,
	UNIT_MM,
	UNIT_CM,
};

class UnitEdit : public EditField, public Convert {
public:
	virtual Val GetData() const;
	virtual Val Format(const Val& v) const;
	virtual void  SetData(const Val& v);
	virtual bool  Key(dword key, i32 repcnt);
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);

private:
	SpinButtons spin;
	i32         unit;
	bool        sgn;

	static Txt AsText(double d, i32 unit);
	static Txt DotAsText(i32 dot, i32 unit);
	void       Spin(i32 delta);
	void       Read(double& q, i32& u) const;

public:
	UnitEdit& SetUnit(i32 _unit)                        { unit = _unit; return *this; }
	void      Set(i32 _unit, i32 d)                     { unit = _unit; SetData(d); }
	UnitEdit& WithSgn(bool b = true);

	typedef UnitEdit CLASSNAME;

	UnitEdit();
};

struct FontHeight : public WithDropChoice<EditDouble> {
	virtual bool Key(dword key, i32);

	FontHeight()   { MinMax(1, 72); }
};

#define LAYOUTFILE <drx/RichEdit/RichEdit.lay>
#include <drx/CtrlCore/lay.h>

bool EditRichHeaderFooter(Txt& header_qtf, Txt& footer_qtf);

class ParaFormatting : public WithParaLayout<StaticRect> {
public:
	DropList n[8];

private:
	UnitEdit tabpos;
	DropList tabtype;
	DropList tabfill;
	bool     keepindent;
	Font     font;
	bool     modified;
	Txt      header_qtf, footer_qtf;

	RichPara::NumberFormat GetNumbering();
	bool                   IsNumbering();
	i32                    ComputeIndent();
	void                   SetMod()                  { modified = true; }

	typedef ParaFormatting CLASSNAME;

public:
	void  Set(i32 unit, const RichText::FormatInfo& formatinfo, bool baselevel = false);
	dword Get(RichText::FormatInfo& formatinfo);
	void  SetFont(Font fnt)                          { font = fnt; }
	bool  IsChanged() const                          { return IsModifiedDeep() || modified; }
	void  EnableNumbering();
	void  SetupIndent();
	void  EditHdrFtr();
	void  NewHdrFtr();
	void  SyncHdrFtr();

	ParaFormatting();
};

class StyleManager : public WithStylesLayout<TopWindow> {
public:
	ParaFormatting            para;
	FontHeight                height;

private:
	i32                       unit;
	ArrayMap<Uuid, RichStyle> style;
	Index<Uuid>               dirty;
	EditTxt                   name;

	void EnterStyle();
	void SaveStyle();
	void SetupFont();
	void SetupFont0();
	void Create();
	void ReloadNextStyles();
	void Remove();

	void GetFont(Font& font);

	void Menu(Bar& bar);

	typedef StyleManager CLASSNAME;

public:
	void     Set(const RichText& text, const Uuid& current = Null);
	void     Set(tukk qtf);
	bool     IsChanged() const;
	void     Get(RichText& text);
	RichText Get();
	Txt      GetQTF();

	void     Setup(const Vec<i32>& faces, i32 aunit = UNIT_DOT);

	StyleManager();
};

void SetupFaceList(DropList& face);

class RichEdit : public Ctrl, private TextArrayOps {
public:
	virtual void  Layout();
	virtual void  Paint(Draw& w);
	virtual bool  Key(dword key, i32 count);
	virtual void  LeftDown(Point p, dword flags);
	virtual void  MiddleDown(Point p, dword flags);
	virtual void  LeftUp(Point p, dword flags);
	virtual void  LeftDrag(Point p, dword flags);
	virtual void  RightDown(Point p, dword flags);
	virtual void  LeftDouble(Point p, dword flags);
	virtual void  LeftTriple(Point p, dword flags);
	virtual void  MouseMove(Point p, dword flags);
	virtual void  LeftRepeat(Point p, dword flags);
	virtual void  MouseWheel(Point p, i32 zdelta, dword keyflags);
	virtual void  CancelMode();
	virtual Image CursorImage(Point p, dword flags);
	virtual Val   GetData() const;
	virtual void  SetData(const Val& v);
	virtual void  Serialize(Stream& s);
	virtual void  DragAndDrop(Point p, PasteClip& d);
	virtual void  DragRepeat(Point p);
	virtual void  DragLeave();
	virtual Txt   GetSelectionData(const Txt& fmt) const;
	virtual Point GetPreedit();
	virtual Font  GetPreeditFont();
	virtual Rect  GetCaret() const;

private:
	virtual i32   GetCharAt(z64 i) const                  { return GetChar((i32)i); }
	virtual z64 GetTotal() const                          { return GetLength(); }

	i32                      viewborder;
	void                    *context;
	Size                     p_size;
	bool                     sizetracking;
	ScrollBar                sb;
	Scroller                 scroller;
	RichRuler                ruler;
	RichText                 text;
	RichText::FormatInfo     formatinfo;
	i32                      cursor, anchor;
	bool                     begtabsel;
	RichCaret                cursorc;
	RichPos                  cursorp;
	RichPos                  anchorp;
	i32                      tablesel;
	Rect                     cells;
	i32                      gx;
	i32                      oselh, osell;
	i32                      objectpos;
	i32                      zsc;
	Rect                     objectrect;
	RichHotPos               tabmove;
	i32                      mpos;
	i32                      undosteps;
	Rect                     dropcaret;
	bool                     selclick;
	DropList                 face;
	FontHeight               height;
	DataPusher               hyperlink;
	DataPusher               label;
	DataPusher               indexentry;
	FrameRight<Button>       gotolabel, gotoentry;
	MultiButton::SubButton  *setstyle;
	PopUpTable               gototable;
	ColorButton              ink, paper;
	ToolButton               adjustunits;
	DropList                 style;
	DropList                 language;
	Size                     pagesz;
	Txt                      footer;
	bool                     nolinks;
	bool                     overwrite;
	bool                     useraction, modified;
	bool                     singleline;
	static i32               fixedlang;
	RichObj                  bar_object;
	Txt                      bar_fieldparam;
	Rect                     caret;

	WithRichFindReplaceLayout<TopWindow> findreplace;

	bool                     found, notfoundfw;
	bool                     persistent_findreplace;

	VecMap<Txt, Val>         vars;

	i32                      unit;
	Color                    showcodes;
	i32                      zoom;
	bool                     spellcheck;
	bool                     paintcarect;

	i32                      undoserial;
	bool                     incundoserial;

	Vec<i32>                 ffs;

	i32                      bullet_indent;

	PaintInfo                paint_info;
	bool                     ignore_physical_size;

	static i32 fh[];

	struct UndoRec {
		i32     serial;
		i32     cursor;

		virtual void         Apply(RichText& txt) = 0;
		virtual One<UndoRec> GetRedo(const RichText& txt) = 0;

		One<UndoRec> Serial(i32 s) { serial = s; return this; }

		virtual ~UndoRec() {}
	};

	struct UndoInsert : UndoRec {
		i32                  pos;
		i32                  length;
		bool                 typing;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoInsert(i32 pos, i32 length, bool typing = false);
	};

	struct UndoRemove : UndoRec {
		i32                  pos;
		RichText             text;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoRemove(const RichText& txt, i32 pos, i32 length);
	};

	struct UndoFormat : UndoRec {
		i32                 pos;
		i32                 length;
		RichText::Formating format;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoFormat(const RichText& txt, i32 pos, i32 length);
	};

	struct UndoStyle : UndoRec {
		Uuid                 id;
		RichStyle            style;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoStyle(const RichText& txt, const Uuid& id);
	};

	struct UndoStyles : UndoRec {
		RichStyles           styles;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoStyles(const RichText& txt);
	};

	struct UndoTableFormat : UndoRec {
		i32                 table;
		RichTable::Format   format;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoTableFormat(const RichText& txt, i32 table);
	};

	struct UndoCreateTable : UndoRec {
		i32              table;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoCreateTable(i32 table) : table(table) {}
	};

	struct UndoDestroyTable : UndoRec {
		i32              pos;
		RichTable        table;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoDestroyTable(const RichText& txt, i32 pos);
	};

	struct UndoRemoveParaSpecial : UndoRec {
		i32              table;
		bool             before;
		RichPara::Format format;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoRemoveParaSpecial(const RichText& txt, i32 table, bool before);
	};

	struct UndoInsertParaSpecial : UndoRec {
		i32              table;
		bool             before;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoInsertParaSpecial(i32 table, bool before) : table(table), before(before) {}
	};

	struct UndoTable : UndoRec {
		i32             table;
		RichTable       copy;

		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);

		UndoTable(const RichText& txt, i32 table);
	};

	struct UndoBegSelFix : UndoRec {
		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);
	};

	struct UndoBegSelUnFix : UndoRec {
		virtual void         Apply(RichText& txt);
		virtual One<UndoRec> GetRedo(const RichText& txt);
	};

	BiArray<UndoRec> undo;
	Array<UndoRec>   redo;

	FileSel          imagefs;

	struct StyleKey {
		Uuid   styleid;
		Txt    stylename;
		Txt    face;
		i32    height;
		Color  ink;
		Color  paper;

		StyleKey();
	};

	StyleKey   stylekey[20];

	Zoom       clipzoom;

	double     floating_zoom;

	Rect       GetTextRect() const;
	Size       GetZoomedPage() const;
	i32        GetPosY(PageY py) const;
	void       GetPagePoint(Point p, PageY& py, i32& x);
	i32        GetX(i32 x);
	i32        GetSnapX(i32 x);
	PageY      GetPageY(i32 y) const;
	i32        GetNearestPos(i32 x, PageY py);
	void       SetSb();
	void       Scroll();
	void       SetZsc();
	Rect       PlaceCaret();
	void       FinishNF();
	void       Finish();
	void       ReadFormat();
	void       ShowFormat();
	i32        GetMousePos(Point p);
	RichHotPos GetHotPos(Point p);
	i32        GetHotSpot(Point p) const;
	Rect       GetObjectRect(i32 pos) const;
	void       FixObjectRect();
	bool       RemoveBullet(bool backspace);

	void       SetObjectPos(i32 pos);
	void       AdjustObjectSize();
	void       SetObjectPercent(i32 p);
	void       SetObjectYDelta(i32 pt);
	void       SetFace();
	void       SetHeight();

	bool       SelBeg(bool select);
	bool       SelEnd(bool select);
	void       SelCell(i32 dx, i32 dy);

	void       Limit(i32& pos, i32& count);
	bool       InvalidRange(i32 c1, i32 c2);
	void       NextUndo()                 { undoserial += incundoserial; incundoserial = false; }
	void       AddUndo(One<UndoRec>&& ur);

	void       BeginRulerTrack();
	void       RulerTrack();
	void       IndentMark();
	void       HighLightTab(i32 r);
	void       TabAdd(i32 align);
	void       AddTab();
	void       TabMenu();

	void       Hyperlink();
	void       Label();
	void       IndexEntry();
	void       GotoLbl();
	void       GotoEntry();
	void       GotoType(i32 type, Ctrl& l);
	void       Goto();

	void       SetInk();
	void       SetPaper();
	void       SetLanguage();
	void       Language();
	void       SetupLanguage(Vec<i32>&& lng);

	void       SetBullet(i32 bullet);

	void       SetupRuler();

	void       ReadStyles();

	void       SetStyle();

	void       EndSizeTracking();

	RichObj    GetObject() const;
	void       ReplaceObject(const RichObj& obj);

	static bool   IsW(i32 c);

	void Insert(i32 pos, const RichText& text, bool typing = false);
	void Remove(i32 pos, i32 len, bool forward = false);
	void SaveFormat(i32 pos, i32 count);
	void SaveFormat();
	void ModifyFormat(i32 pos, const RichText::FormatInfo& fi, i32 count);
	void SetParaFormat(dword paravalid);

	void SaveStylesUndo();
	void SaveStyleUndo(const Uuid& id);

	bool CursorKey(dword key, i32 count);

	void ApplyFormat(dword charvalid, dword paravalid = 0);

	void MoveNG(i32 newpos, bool select);
	void MoveUpDown(i32 dir, bool select, i32 firststep = 0);
	void MovePageUpDown(i32 dir, bool select);
	void MoveWordRight(bool select);
	void MoveWordLeft(bool select);
	void MoveHomeEnd(i32 dir, bool select);

	void InsertLine();

	void Bold();
	void Italic();
	void Underline();
	void Strikeout();
	void Capitals();
	void SetScript(i32 script);

	void AlignLeft();
	void AlignRight();
	void AlignCenter();
	void AlignJustify();

	void Style();

	void ParaFormat();
	void ToPara();

	void ZoomView(i32 d);
	void SetupUnits();
	void SpellCheck();

	void    AddUserDict();
	WTxt    GetWordAtCursor();
	void    GetWordAtCursorPos(i32& pos, i32& count);

	Rect     GetCaretRect(const RichCaret& pr) const;
	Rect     GetCaretRect() const;

	void     SaveTableFormat(i32 table);
	void     SaveTable(i32 table);
	void     InsertTable();
	void     DestroyTable();
	void     TableProps();
	bool     RemoveSpecial(i32 ll, i32 hh, bool back);
	bool     InsertLineSpecial();
	void     TableInsertRow();
	void     TableRemoveRow();
	void     TableInsertColumn();
	void     TableRemoveColumn();
	void     SplitCell();
	void     JoinCell();
	void     CellProperties();

	void     OpenFindReplace();
	void     CloseFindReplace();
	i32      FindPos();
	RichText ReplaceText();
	void     Find();
	void     Replace();
	void     FindReplaceAddHistory();

	void     Reset();

	void     DoRefreshBar();
	void     RefreshBar();

	bool     Accept(PasteClip& d, RichText& clip, Txt& fmt);
	void     ClipPaste(RichText& clip, const Txt& fmt);
	bool     InSelection(i32& c) const;
	void     RefreshDropCaret();
	void     ZoomClip(RichText& text) const;

	void     InsertImage();

	void     StyleKeys();
	void     ApplyStyleKey(i32 i);

	void     HeaderFooter();
	bool     EditHeaderFooter(Txt& header_qtf, Txt& footer_qtf);

	bool     BegSelTabFix(i32& count);
	bool     BegSelTabFix()                        { i32 dummy; return BegSelTabFix(dummy); }
	void     BegSelTabFixEnd(bool fix);

	Size     GetPhysicalSize(const RichObj& obj);

	struct DisplayDefault : public Display {
		virtual void Paint(Draw& w, const Rect& r, const Val& q,
		                   Color ink, Color paper, dword style) const;
	};

	void UserAction();
	Event<>  User(Event<>  cb);

	static void   SpellerAdd(const WTxt& w, i32 lang);
	static i32    CompareStyle(const Val& a, const Val& b);

	friend class StyleKeysDlg;
	friend class StyleManager;

	using Ctrl::Accept;

protected:
	enum {
		TIMEID_ENDSIZETRACKING = Ctrl::TIMEID_COUNT,
		TIMEID_REFRESHBAR,
		TIMEID_COUNT
	};

public:
	virtual void  PasteFilter(RichText& txt, const Txt& fmt);
	virtual void  Filter(RichText& txt);

	static double DotToPt(i32 dot);
	static i32    PtToDot(double pt);
	static Bits   SpellParagraph(const RichPara& p);
	static void   FixedLang(i32 lang)              { fixedlang = lang; }

	Event<>                  WhenRefreshBar;
	Event<>                  WhenStartEvaluating;
	Event<Txt&, WTxt&>       WhenHyperlink;
	Event<Txt&>              WhenLabel;
	Event<Txt&>              WhenIndexEntry;
	Event<Bar&>              WhenBar;

	void   StdBar(Bar& menu);

	void   SerializeSettings(Stream& s);

	Zoom   GetZoom() const;

	i32    GetCursor() const                       { return cursor; }
	i32    GetAnchor() const                       { return anchor; }

	void     Select(i32 pos, i32 count);
	bool     IsSelection() const;
	bool     GetSelection(i32& l, i32& h) const;
	RichText GetSelection(i32 maxlen = INT_MAX) const;
	void     SetSelection(i32 l, i32 h)            { Select(l, h - l); }
	bool     RemoveSelection(bool back = false);
	void     CancelSelection();

	void   BeginOp()                               { NextUndo(); }

	i32                  GetLength() const         { return text.GetLength(); }
	void                 PasteText(const RichText& text);
	void                 RemoveText(i32 count);
	RichText             CopyText(i32 pos, i32 count) const;
	RichText::FormatInfo GetFormatInfo() const     { return formatinfo; }
	void                 ApplyFormatInfo(const RichText::FormatInfo& fi);
	i32                  GetChar(i32 pos) const    { return text[pos]; }
	i32                  operator[](i32 pos) const { return text[pos]; }

	void   Undo();
	void   Redo();

	void   ScrollToCursor()                     { Finish(); }

	void   Move(i32 newpos, bool select = false);

	void   Copy();
	void   Cut();
	void   Paste();
	void   InsertObject(i32 type);

	void   Styles();

	bool   Print();
	void   DoPrint()                             { Print(); }

	void   StyleTool(Bar& bar, i32 width = Zx(120));
	void   FaceTool(Bar& bar, i32 width = Zx(130));
	void   HeightTool(Bar& bar, i32 width = Zx(50));
	void   BoldTool(Bar& bar, dword key = K_CTRL_B);
	void   ItalicTool(Bar& bar, dword key = K_CTRL_I);
	void   UnderlineTool(Bar& bar, dword key = K_CTRL_U);
	void   StrikeoutTool(Bar& bar, dword key = 0);
	void   CapitalsTool(Bar& bar, dword key = 0);
	void   SuperscriptTool(Bar& bar, dword key = 0);
	void   SubscriptTool(Bar& bar, dword key = 0);
	void   InkTool(Bar& bar);
	void   PaperTool(Bar& bar);
	void   LanguageTool(Bar& bar, i32 width = Zx(60));
	void   HyperlinkTool(Bar& bar, i32 width = Zx(180), dword key = 0, tukk tip = NULL);
	void   SpellCheckTool(Bar& bar);
	void   IndexEntryTool(Bar& bar, i32 width = Zx(80), dword key = 0, tukk tip = NULL);

	void   LeftTool(Bar& bar, dword key = K_CTRL_L);
	void   RightTool(Bar& bar, dword key = K_CTRL_R);
	void   CenterTool(Bar& bar, dword key = K_CTRL_E);
	void   JustifyTool(Bar& bar, dword key = K_CTRL_J);
	void   RoundBulletTool(Bar& bar, dword key = 0);
	void   RoundWhiteBulletTool(Bar& bar, dword key = 0);
	void   BoxBulletTool(Bar& bar, dword key = 0);
	void   BoxWhiteBulletTool(Bar& bar, dword key = 0);
	void   TextBulletTool(Bar& bar, dword key = 0);
	void   ParaFormatTool(Bar& bar, dword key = 0);
	void   LabelTool(Bar& bar, i32 width = Zx(80), dword key = 0, tukk tip = NULL);
	void   ToParaTool(Bar& bar, dword key = K_CTRL_K);

	void   UndoTool(Bar& bar, dword key = K_CTRL_Z);
	void   RedoTool(Bar& bar, dword key = K_SHIFT_CTRL_Z);
	void   CutTool(Bar& bar, dword key = K_CTRL_X);
	void   CopyTool(Bar& bar, dword key = K_CTRL_C);
	void   PasteTool(Bar& bar, dword key = K_CTRL_V);
	void   PastePlainTextTool(Bar& bar, dword key = K_CTRL_V|K_SHIFT);
	void   ObjectTool(Bar& bar, dword key = 0);
	void   LoadImageTool(Bar& bar, dword key = 0);
	void   FindReplaceTool(Bar& bar, dword key = K_CTRL_F);

	void   InsertTableTool(Bar& bar, dword key = K_CTRL_F12);
	void   TablePropertiesTool(Bar& bar, dword key = K_SHIFT_F12);
	void   InsertTableRowTool(Bar& bar, dword key = 0);
	void   RemoveTableRowTool(Bar& bar, dword key = 0);
	void   InsertTableColumnTool(Bar& bar, dword key = 0);
	void   RemoveTableColumnTool(Bar& bar, dword key = 0);
	void   SplitJoinCellTool(Bar& bar, dword key = 0);
	void   CellPropertiesTool(Bar& bar, dword key = 0);

	void   PrintTool(Bar& bar, dword key = K_CTRL_P);

	void   FontTools(Bar& bar);
	void   ParaTools(Bar& bar);
	void   EditTools(Bar& bar);
	void   TableTools(Bar& bar);

	void   InsertImageTool(Bar& bar);
	void   StyleKeysTool(Bar& bar);

	void   HeaderFooterTool(Bar& bar);

	void   DefaultBar(Bar& bar, bool extended = true);

	void            SetVar(const Txt& id, const Val& v) { vars.GetAdd(id) = v; }
	Val             GetVar(const Txt& id) const           { return vars.Get(id, Val()); }
	void            EvaluateFields();

	bool            GotoLabel(Gate<const WTxt&> match);
	bool            GotoLabel(const Txt& lbl);
	void            BeginPara();
	void            NextPara();
	void            PrevPara();

	void            Clear();
	void            Pick(RichText pick_ t);
	void            SetQTF(tukk qtf)                { Pick(ParseQTF(qtf, 0, context)); }
	const RichText& Get() const                            { return text; }
	Txt             GetQTF(u8 cs = CHARSET_UTF8) const   { return AsQTF(text, cs); }
	void            ApplyStylesheet(const RichText& r);
	void            SetPage(const Size& sz)                { pagesz = sz; Finish(); }
	Size            GetPage()                              { return pagesz; }

	RichEdit&       NoRuler()                              { RemoveFrame(ruler); return *this; }
	RichEdit&       SingleLine(bool b = true)              { singleline = b; return *this; }
	RichEdit&       FontFaces(const Vec<i32>& face);
	RichEdit&       ViewBorder(i32 cx)                     { viewborder = cx; Refresh(); return *this; }
	RichEdit&       ShowCodes(Color c)                     { showcodes = c; Refresh(); return *this; }
	RichEdit&       Unit(i32 u)                            { unit = u; Refresh(); return *this; }
	RichEdit&       SpellCheck(bool b)                     { spellcheck = b; Refresh(); return *this; }
	RichEdit&       SetZoom(i32 z)                         { zoom = z; Refresh(); return *this; }
	RichEdit&       SetContext(uk ctx)                  { context = ctx; Refresh(); return *this; }
	void           *GetContext() const                     { return context; }
	RichEdit&       ClipZoom(Zoom z)                       { clipzoom = z; return *this; }
	RichEdit&       ClipZoom(i32 m, i32 d)                 { clipzoom = Zoom(m, d); return *this; }
	Zoom            GetClipZoom() const                    { return clipzoom; }
	RichEdit&       BulletIndent(i32 i)                    { bullet_indent = i; return *this; }
	RichEdit&       PersistentFindReplace(bool b = true)   { persistent_findreplace = b; return *this; }
	RichEdit&       Floating(double zoomlevel_ = 1);
	RichEdit&       NoFloating(double zoomlevel_ = 1)      { return Floating(Null); }
	RichEdit&       SetPaintInfo(const PaintInfo& pi)      { paint_info = pi; return *this; }
	RichEdit&       IgnorePhysicalObjectSize(bool b = true){ ignore_physical_size = b; return *this; }

	struct UndoInfo {
		i32              undoserial;
		BiArray<UndoRec> undo;
		Array<UndoRec>   redo;

		void Clear()     { undo.Clear(); redo.Clear(); undoserial = 0; }
	};

	struct PosInfo {
		i32  cursor;
		i32  anchor;
		bool begtabsel;
		i32  zsc;

		void Serialize(Stream& s);
	};

	UndoInfo PickUndoInfo();
	void     SetPickUndoInfo(UndoInfo pick_ f);

	PosInfo  GetPosInfo() const;
	void     SetPosInfo(const PosInfo& pos);

	void     SetFooter(const Txt& s)                   { footer = s; }
	void     PrintNoLinks(bool b = true)                  { nolinks = b; }

	typedef RichEdit CLASSNAME;

	RichEdit();
	virtual ~RichEdit();
};

class RichEditWithToolBar : public RichEdit {
	ToolBar  toolbar;
	bool     extended;
	void    RefreshBar();
	void    TheBar(Bar& bar);

public:
	RichEditWithToolBar& Extended(bool b = true) { extended = b; return *this; }
	RichEditWithToolBar& NoExtended()            { return Extended(false); }

	typedef RichEditWithToolBar CLASSNAME;

	RichEditWithToolBar();
};

void AppendClipboard(RichText&& txt);

}

#endif