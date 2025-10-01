class RichCell : DeepCopyOption<RichCell> {
public:
	struct Format {
		Rect     border;
		Rect     margin;
		i32      align;
		i32      minheight;
		Color    color;
		Color    bordercolor;
		bool     keep;
		bool     round;

		Format();
	};

	Format   format;
	i32      vspan, hspan;
	RichTxt  text;

private:
	bool     Reduce(RichContext& rc) const;
	PageY    Align(const RichContext& rc, PageY npy) const;
	void     DrawCell(Draw& w, i32 l, i32 r, i32 y, i32 yy, const Rect& border,
	                  const Rect& margin, const PaintInfo& pi) const;

	void        ClearText();
	void        ClearText(const RichPara::Format& format, const RichStyles& style);
	void        Clear();

	PageY       GetTop(RichContext rc) const;
	PageY       GetHeight(RichContext rc) const;
	void        Paint(PageDraw& pw, RichContext rc, PageY npy,
	                  const Rect& xpg, const Rect& nxpg,
	                  i32 y, i32 ny, const PaintInfo& pi,
	                  bool select) const;
	RichCaret   GetCaret(i32 pos, RichContext rc, PageY pyy) const;
	i32         GetPos(i32 x, PageY y, RichContext rc, PageY pyy) const;
	RichHotPos  GetHotPos(i32 x, PageY y, i32 tolerance, RichContext rc, PageY pyy) const;

	void        GatherValPos(Vec<RichValPos>& f, RichContext rc, PageY pyy, i32 pos, i32 type) const;

	friend class RichTable;
	friend class RichText;

public:
	rval_default(RichCell);

	RichCell(const RichCell& src, i32);
	RichCell();
};

class RichTable : DeepCopyOption<RichTable> {
public:
	struct Format {
		i32    before, lm, rm, after;
		i32    frame;
		Color  framecolor;
		i32    grid;
		Color  gridcolor;
		WithDeepCopy< Vec<i32> > column;
		i32    header;
		bool   keep, newpage, newhdrftr;
		Txt header_qtf, footer_qtf;

		Format();
	};

	struct CellInfo {
		bool  valid;
		Point master;

		operator bool() const { return valid; }

		CellInfo()            { valid = true; }
	};

private:
	Format format;
	Array< Array<RichCell> > cell;
	mutable One<RichText> header, footer;

	struct PaintCell : Moveable<PaintCell> {
		i32   left; // left pos with grid
		i32   right; // right pos with grid
		i32   page_left; // left pos without grid (if any)
		i32   page_right; // right pos without grid (if any)
		PageY hy; // end of cell
		bool  top; // this is top cell of vspan (or single cell without vspan)
		bool  bottom; // this is bottom cell of vspan (or single cell without vspan)
		
		RichContext MakeRichContext(RichContext rc) const;

		PaintCell()    { top = true; }
	};

	struct PaintRow : Moveable<PaintRow> {
		PageY             gpy; // position of grid line (if not first)
		PageY             py, pyy; //start, end of line
		Buffer<PaintCell> cell;
		bool              first; // first row on the page

		PaintCell& operator[](i32 i)                { return cell[i]; }
		const PaintCell& operator[](i32 i) const    { return cell[i]; }
	};

	struct Layout {
		Buffer<PaintRow>  row;
		i32               frame;
		i32               grid;
		PageY             pyy;

		PaintRow& operator[](i32 i)                 { return row[i]; }
		const PaintRow& operator[](i32 i) const     { return row[i]; }
		
		rval_default(Layout); // Old CLANG requires this
		Layout() {}
	};

	struct TabLayout : Layout {
		bool              hasheader = false;
		Layout            header;
		Size              sz;
		PageY             py;
		Rect              first_page;
		Rect              next_page;
		Rect              header_page;
		
		rval_default(TabLayout); // Old CLANG requires this
		TabLayout() {}
	};

	mutable TabLayout clayout;

	Buffer< Buffer<CellInfo> > ci;
	i32              r_row, r_column; // r_ - refresh info
	Rect             r_first_page, r_next_page, r_header_page;
	PageY            r_py, r_pyy;

	void             Invalidate();
	void             InvalidateRefresh(i32 i, i32 j);
	void             InvalidateRefresh(Point p)            { InvalidateRefresh(p.y, p.x); }

	bool             Reduce(Rect& r) const;
	bool             Reduce(RichContext rc, Rect& first_page, Rect& next_page) const;
	Rect             GetPageRect(PageY py) const;
	void             NextPage(PageY& py, bool header = false) const;
	RichContext      MakeRichContext(RichContext rc, PageY py, bool header = false) const;
	Layout           Realize(PageY py, RichContext arc, i32 ny, bool header) const;
	bool             RowPaint(PageDraw& pw, RichContext rc, const Layout& tab, bool header,
                              i32 i, i32 ny, Rect pg, Rect npg, VecMap<i32, Rect>& frr,
                              PaintInfo& pi, i32 pd, bool sel) const;

	const TabLayout& Realize(RichContext rc) const;

	mutable i32  length, tabcount;

	static void  ExpandFrr(VecMap<i32, Rect>& frr, i32 pi, i32 l, i32 r, i32 t, i32 b);

	friend class RichTxt;
	friend class RichText;

public:
	Array<RichCell>&       operator[](i32 i)             { return cell[i]; }
	const Array<RichCell>& operator[](i32 i) const       { return cell[i]; }
	RichCell&       operator[](Point p)                  { return cell[p.y][p.x]; }
	const RichCell& operator[](Point p) const            { return cell[p.y][p.x]; }
	i32             GetCellPos(i32 i, i32 j) const;
	i32             GetCellPos(Point p) const            { return GetCellPos(p.y, p.x); }
	i32             GetTableCount(i32 i, i32 j) const;
	i32             GetTableCount(Point p) const         { return GetTableCount(p.y, p.x); }
	Point           GetMasterCell(i32 i, i32 j) const;
	Point           GetMasterCell(Point p) const         { return GetMasterCell(p.y, p.x); }
	const RichCell& GetMaster(i32 i, i32 j) const;

	i32           GetLength() const;
	i32           GetTableCount() const;

	void          RemoveRow(i32 rowi);
	void          InsertRow(i32 rowi, const RichStyles& style);

private:
	void          Normalize0();

	RichTable     Copy(const Rect& sel) const;
	void          Paste(Point pos, const RichTable& tab);
	void          RemoveRow0(i32 rowi);
	void          RemoveColumn0(i32 column);
	void          RemoveColumn(i32 column);
	void          InsertColumn(i32 column, const RichStyles& style);
	bool          IsRowEmpty(i32 row);
	bool          IsColumnEmpty(i32 column);
	void          SplitCell(Point cl, Size sz, const RichStyles& style);
	RichCell::Format GetCellFormat(const Rect& sel) const;
	void             SetCellFormat(const Rect& sel, const RichCell::Format& fmt,
	                               bool setkeep, bool setround);

	PageY         GetHeight(RichContext rc) const;
	PageY         GetTop(RichContext rc) const;
	void          Paint(PageDraw& pw, RichContext rc, const PaintInfo& pi, bool baselevel) const;
	RichCaret     GetCaret(i32 pos, RichContext rc) const;
	i32           GetPos(i32 x, PageY y, RichContext rc) const;
	i32           GetVertMove(i32 pos, i32 gx, RichContext rc, i32 dir) const;
	RichHotPos    GetHotPos(i32 x, PageY y, i32 tolerance, RichContext rc) const;
	void          AdjustSel(Rect& sel) const;
	
	i32           GetWidth(const RichStyles& st) const;

	void          GatherValPos(Vec<RichValPos>& f, RichContext rc, i32 pos, i32 type) const;
	void          ClearSpelling();

	Point         FindCell(i32& pos) const;
	RichPos       GetRichPos(i32 pos, const RichStyles& st) const;

	CellInfo      GetCellInfo(i32 i, i32 j)              { return ci[i][j]; }

	i32           GetInvalid(PageY& top, PageY& bottom, RichContext rc) const;
	void          Validate();

	void          ApplyZoom(Zoom z, const RichStyles& ostyle, const RichStyles& zstyle);

public:
	void                    AddColumn(i32 cx);
	void                    SetPick(i32 i, i32 j, RichTxt&& text);
	RichTxt                 GetPick(i32 i, i32 j);
	const RichTxt&          Get(i32 i, i32 j) const                { return cell[i][j].text; }
	void                    SetQTF(i32 i, i32 j, tukk qtf);
	void                    SetFormat(i32 i, i32 j, const RichCell::Format& fmt);
	const RichCell::Format& GetFormat(i32 i, i32 j) const          { return cell[i][j].format; }
	bool                    operator()(i32 i, i32 j) const         { return ci[i][j].valid; }
	void                    SetSpan(i32 i, i32 j, i32 vspan, i32 hspan);
	Size                    GetSpan(i32 i, i32 j) const;
	void                    SetFormat(const Format& fmt);
	const Format&           GetFormat() const                      { return format; }

	i32                     GetColumns() const                     { return format.column.GetCount(); }
	i32                     GetRows() const                        { return cell.GetCount(); }
	Size                    GetSize() const                        { return Size(GetColumns(), GetRows()); }

	void          Normalize();

	rval_default(RichTable);

	RichTable(const RichTable& src, i32);
	RichTable();
};
