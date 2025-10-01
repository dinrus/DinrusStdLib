class RichText : public RichTxt, public DeepCopyOption<RichText> {
	mutable Mutex mutex; // To cover all those laze evaluation scenarios
	RichStyles    style;
	Txt        footer_hack; // ugly hack
	bool          nolinks = false; // another ugly hack

	void       Init();

	void       PaintHeaderFooter(PageDraw& pw, const Rect& page, PageY py, const PaintInfo& pi,
	                             i32 from_page, i32 to_page) const;

	struct StyleChangeOp;
	struct SetStylesOp;
	struct RemoveStyleOp;
	struct MergeStylesOp;
	friend struct MergeStylesOp;
	struct OverrideStylesOp;

public:
	void                  Clear();

	i32                   GetStyleCount() const                { return style.GetCount(); }
	Uuid                  GetStyleId(i32 i) const              { return style.GetKey(i); }
	Uuid                  GetStyleId(const Txt& name) const;
	const RichStyle&      GetStyle(i32 i) const                { return style[i]; }
	const RichStyle&      GetStyle(const Uuid& id) const       { return DRX::GetStyle(style, id); }
	void                  SetStyle(const Uuid& id, const RichStyle& _style);
	bool                  HasStyle(const Uuid& id)             { return style.Find(id) >= 0; }
	void                  RemoveStyle(const Uuid& id);

	const RichStyles&     GetStyles() const                    { return style; }
	void                  SetStyles(const RichStyles& styles);
	void                  OverrideStyles(const RichStyles& masterstyles,
	                                     bool matchname = true, bool addmissing = true);
	void                  MergeStyles(const RichText& text);

	RichPara              Get(i32 i) const                     { return RichTxt::Get(i, style); }
	void                  Cat(const RichPara& p)               { RichTxt::Cat(p, style); }
	void                  CatPick(RichText&& p);
	using                 RichTxt::CatPick;

	RichContext           Context(const Rect& page, PageY py, RichText *header, RichText *footer) const;
	RichContext           Context(const Rect& page, PageY py) const { return Context(page, py, ~header, ~footer); }
//	RichContext           Context(const Rect& page) const { return Context(page, PageY(0, 0)); }

	RichPos               GetRichPos(i32 pos, i32 maxlevel = INT_MAX) const;
	i32                   operator[](i32 pos) const            { return GetRichPos(pos).chr; }

	void                  ApplyZoom(Zoom z);

	PageY                 GetHeight(PageY py, const Rect& page) const;
	PageY                 GetHeight(const Rect& page) const;
	void                  Paint(PageDraw& w, PageY py, const Rect& page, const PaintInfo& pi) const;
	void                  Paint(PageDraw& w, const Rect& page, const PaintInfo& pi) const;
	RichCaret             GetCaret(i32 pos, const Rect& page) const;
	i32                   GetPos(i32 x, PageY y, const Rect& page) const;
	RichHotPos            GetHotPos(i32 x, PageY y, i32 tolerance, const Rect& page) const;
	i32                   GetVertMove(i32 pos, i32 gx, const Rect& page, i32 dir) const;
	i32                   GetWidth() const;

	i32                   GetHeight(i32 cx) const;
	i32                   GetHeight(Zoom zoom, i32 cx) const;
	void                  Paint(Draw& w, i32 x, i32 y, i32 cx, const PaintInfo& pi) const;
	void                  Paint(Zoom zoom, Draw& w, i32 x, i32 y, i32 cx) const;
	void                  Paint(Draw& w, i32 x, i32 y, i32 cx) const;

	Vec<RichValPos>    GetValPos(const Rect& page, i32 type) const;

	i32                   AdjustCursor(i32 anchor, i32 cursor) const;

	void                  Remove(i32 pos, i32 count);
	void                  Insert(i32 pos, const RichText& p);
	RichText              Copy(i32 pos, i32 count) const;

	FormatInfo            GetFormatInfo(i32 pos, i32 count) const;
	void                  ApplyFormatInfo(i32 pos, const FormatInfo& fi, i32 count);
	void                  ReplaceStyle(i32 pos, const Uuid& id);

	void                  ReStyle(i32 pos, const Uuid& id);

	Formating             SaveFormat(i32 pos, i32 count) const;
	void                  RestoreFormat(i32 pos, const Formating& info);


	RichTable::Format     GetTableFormat(i32 table) const;
	void                  SetTableFormat(i32 table, const RichTable::Format& fmt);
	i32                   SetTable(i32 pos, const RichTable& table);
	void                  DestroyTable(i32 table);
	RichTable             CopyTable(i32 table) const;
	void                  ReplaceTable(i32 table, const RichTable& tab);
	void                  RemoveParaSpecial(i32 table, bool before);
	bool                  CanRemoveParaSpecial(i32 table, bool before);
	void                  InsertParaSpecial(i32 table, bool before, const RichPara::Format& fmt);
	bool                  ShouldInsertParaSpecial(i32 table, bool before);
	RichTable             CopyTable(i32 table, const Rect& sel) const;
	void                  ClearTable(i32 table, const Rect& sel);
	void                  PasteTable(i32 table, Point pos, const RichTable& tab);
	void                  InsertTableRow(i32 table, i32 row);
	void                  RemoveTableRow(i32 table, i32 row);
	void                  InsertTableColumn(i32 table, i32 column);
	void                  RemoveTableColumn(i32 table, i32 column);
	Point                 GetMasterCell(i32 table, i32 row, i32 column);
	Point                 GetMasterCell(i32 table, Point p)                 { return GetMasterCell(table, p.y, p.x); }
	void                  SplitCell(i32 table, Point cell, Size sz);
	void                  JoinCell(i32 table, const Rect& sel);
	RichCell::Format      GetCellFormat(i32 table, const Rect& sel) const;
	void                  SetCellFormat(i32 table, const Rect& sel, const RichCell::Format& fmt,
	                                    bool setkeep, bool setround);
	FormatInfo            GetTableFormatInfo(i32 table, const Rect& sel) const;
	void                  ApplyTableFormatInfo(i32 table, const Rect& sel, const RichText::FormatInfo& fi);
	void                  AdjustTableSel(i32 table, Rect& sel) const        { return GetConstTable(table).AdjustSel(sel); }

	bool                  Iterate(Iterator& r) const                        { return RichTxt::Iterate(r, 0, style); }
	bool                  Iterate(UpdateIterator& r)                        { return RichTxt::Iterate(r, 0, style); }

	bool                  EvaluateFields(VecMap<Txt, Val>& vars)    { return RichTxt::EvaluateFields(style, vars); }

	void                  InvalidateAll()                                   { r_type = ALL; }
	void                  Validate();
	bool                  GetInvalid(PageY& top, PageY& bottom, const Rect& page,
	                                 i32 sell, i32 selh, i32 osell, i32 oselh) const;

	struct ClipboardType {
		ClipboardType();
		virtual ~ClipboardType();

		virtual i32      Level();
		virtual RichText ReadClipboard(const RichPara::Format& f) = 0;
		virtual void     WriteClipboard(const RichText& text) = 0;
	};

	static void           Register(ClipboardType& type);
	
	//Ugly hacks
	void                  SetFooter(const Txt& s)                          { footer_hack = s; }
	Txt                GetFooter() const                                   { return footer_hack; }
	void                  PrintNoLinks(bool b = true)                         { nolinks = b; }
	bool                  IsPrintNoLinks() const                              { return nolinks; }

	RichText()            { Init(); }
	RichText(const RichText& x, i32);
	RichText(RichText&& x);
	RichText(RichTxt&& x, RichStyles&& st);
	
	RichText& operator=(RichText&& x);
};
