class RichTxt : DeepCopyOption<RichTxt> {
public:
	enum {
		BOLD       = 0x00000001,
		ITALIC     = 0x00000002,
		UNDERLINE  = 0x00000004,
		FACE       = 0x00000008,
		HEIGHT     = 0x00000010,
		LANGUAGE   = 0x00000020,
		INK        = 0x00000040,
		PAPER      = 0x00000080,
		LINK       = 0x00000100,
		SSCRIPT    = 0x00000200,
		CAPITALS   = 0x00000400,
		STRIKEOUT  = 0x00000800,
		LANG       = 0x00001000,
		INDEXENTRY = 0x00002000,
		DASHED     = 0x00004000,
		NOAA       = 0x00008000,
	};

	enum {
		ALIGN     = 0x80000000,
		BEFORE    = 0x40000000,
		LM        = 0x20000000,
		INDENT    = 0x10000000,
		RM        = 0x08000000,
		AFTER     = 0x04000000,
		TABSIZE   = 0x02000000,
		BULLET    = 0x01000000,
		NEWPAGE   = 0x00800000,
		KEEP      = 0x00400000,
		TABS      = 0x00200000,
		STYLE     = 0x00100000,
		LABEL     = 0x00080000,
		KEEPNEXT  = 0x00040000,
		ORPHAN    = 0x00020000,
		NUMBERING = 0x00010000,
		SPACING   = 0x00008000,
		RULER     = 0x00004000,
		RULERINK  = 0x00002000,
		RULERSTYLE= 0x00001000,
		NEWHDRFTR = 0x00000800,
		FIRSTONPAGE = 0x00000400,
	};

	struct FormatInfo : RichPara::Format {
		dword charvalid;
		dword paravalid;

		void  Set(const RichPara::Format& fmt);
		void  Set(const RichPara::CharFormat& fmt);
		void  Combine(const RichPara::CharFormat& fmt);
		void  Combine(const RichPara::Format& fmt);
		void  Combine(const FormatInfo& fmt);
		void  ApplyTo(RichPara::CharFormat& fmt) const;
		void  ApplyTo(RichPara::Format& fmt) const;
	};

	class Formating {
		Vec<Uuid>    styleid;
		Vec<Txt>  format;
		friend class RichText;
		friend class RichTxt;
	};

protected:
	struct Para : DeepCopyOption<Para> {
		Uuid                  styleid;
		i32                   length;
		Txt                content;
		Array<RichObj>     object;
		z64                 updateserial;
		mutable i32           ccx;
		mutable i32           cy;
		mutable i32           ruler;
		mutable i32           before;
		mutable Vec<i32>   linecy;
		mutable i32           after;
		mutable bool          newpage;
		mutable bool          firstonpage;
		mutable bool          keep;
		mutable bool          keepnext;
		mutable bool          orphan;
		mutable i32           numbering;
		mutable Bits          spellerrors;
		mutable bool          checked;
		mutable bool          haspos;
		mutable bool          newhdrftr;
		mutable Txt        header_qtf, footer_qtf;
		mutable One<RichText> header, footer;
		One<RichPara::NumberFormat> number;

		void Invalidate();

		Para(const Para& src, i32);
		Para() { length = 0; Invalidate(); numbering = -1; checked = false; haspos = false; }
	};

	struct Part : MoveableAndDeepCopyOption<Part>, Any {
		Part(const Part& src, i32);
		Part();
	};

	Vec<Part>           part;
	Txt                 header_qtf, footer_qtf;
	mutable One<RichText>  header, footer;
	mutable i32            length;
	mutable i32            tabcount;

	enum {
		NONE, SPARA, PARA, FROM, ALL
	};

	i32  r_type;
	i32  r_parti;
	i32  r_paraocx;
	i32  r_paraocy;
	bool r_keep = false;
	bool r_keepnext = false;
	bool r_newpage = false;
	bool r_firstonpage = false;

	void       Init();

	void       Invalidate();

	void       SetRefresh(i32 parai);
	void       SetRefreshFrom(i32 parai);
	void       RefreshAll()                   { SetRefreshFrom(0); }

	void       ParaRemove(i32 parai, i32 pos, i32 count);
	void       ParaInsert(i32 parai, i32 pos, const RichPara& p);
	RichPara   ParaCopy(i32 parai, i32 pos, i32 count) const;

	void       Put(i32 i, const RichPara& p, const RichStyle& s);
	void       Put(i32 i, const RichPara& p, const RichStyles& s);

	void        Sync0(const Para& pp, i32 parti, const RichContext& rc) const;
	void        Sync(i32 parti, const RichContext& rc) const;
	bool        BreaksPage(PageY py, const Para& pp, i32 i, const Rect& page) const;
	void        Advance(i32 parti, RichContext& rc, RichContext& begin) const;
	RichContext GetAdvanced(i32 parti, const RichContext& rc, RichContext& begin) const;
	RichContext GetPartContext(i32 parti, const RichContext& rc0) const;
	PageY       GetPartPageY(i32 parti, const RichContext& rc) const     { return GetPartContext(parti, rc).py; }

	struct ParaOp {
		virtual bool operator()(RichTxt::Para& p) = 0;
		virtual ~ParaOp() {}
	};
	bool             Update(ParaOp& op);
	RichTxt&         GetText0(i32& pos, bool update);
	RichTxt&         GetUpdateText(i32& pos);
	const RichTxt&   GetConstText(i32& pos) const;
	RichTable&       GetTable0(i32 table, bool update);
	RichTable&       GetUpdateTable(i32 table);
	RichTxt&         GetTableUpdateText(i32 table, const RichStyles& style, i32& pi);

	RichPara         Get(i32 parti, const RichStyles& s, bool usecache) const;
	RichPara         Get(i32 parai, const Uuid& styleid, const RichStyles& s, bool usecache) const;
	RichPara         Get(i32 parai, const RichStyle& style, bool usecache) const;

	void        CombineFormat(FormatInfo& f, i32 pi, i32 pi2, bool& first, const RichStyles& style) const;
	static void ApplyStyle(const FormatInfo& fi, RichPara& pa, const RichStyles& style);
	static void Apply(const FormatInfo& fi, RichPara& pa, const RichStyles& style);
	void        ApplyFormat(const FormatInfo& f, i32 pi, i32 pi2, const RichStyles& style);
	void        SaveFormat(Formating& f, i32 p1, i32 p2, const RichStyles& style) const;
	void        RestoreFormat(i32 pi, const Formating& info, i32& ii, const RichStyles& style);

	void        GetAllLanguages(Index<i32>& all) const;

	friend class RichTable;
	friend class RichText;
	friend class RichCell;

	i32                   ComputeLength() const;

	void                  GetRichPos(i32 pos, RichPos& rp, i32 ti, i32 maxlevel, const RichStyles& st) const;
	RichPara::Format      GetFirstFormat(const RichStyles& st) const;

	PageY                 GetTop(RichContext rc) const;
	PageY                 GetHeight(RichContext rc) const;
	i32                   GetWidth(const RichStyles& st) const;
	void                  Paint(PageDraw& w, RichContext& rc, const PaintInfo& pi) const;
	RichCaret             GetCaret(i32 pos, RichContext rc) const;
	i32                   GetPos(i32 x, PageY y, RichContext rc) const;
	i32                   GetVertMove(i32 pos, i32 gx, RichContext rc, i32 dir) const;
	RichHotPos            GetHotPos(i32 x, PageY y, i32 tolerance, RichContext rc) const;

	void                  GatherValPos(Vec<RichValPos>& f, RichContext rc, i32 pos, i32 type) const;

	bool                  EvaluateFields(const RichStyles& s, VecMap<Txt, Val>& vars);

	void                  ApplyZoom(Zoom z, const RichStyles& ostyle, const RichStyles& zstyle);

public:
	enum ValPosType { LABELS, INDEXENTRIES };

	i32                   GetPartCount() const                 { return part.GetCount(); }
	bool                  IsPara(i32 i) const                  { return part[i].Is<Para>(); }
	bool                  IsTable(i32 i) const;
	i32                   GetPartLength(i32 pi) const;
	i32                   FindPart(i32& pos) const;
	i32                   GetPartPos(i32 pi) const;

	RichPara              Get(i32 i, const RichStyles& s) const;
	RichPara              Get(i32 i, const Uuid& styleid, const RichStyles& s) const;
	RichPara              Get(i32 i, const RichStyle& style) const;
	Uuid                  GetParaStyle(i32 i) const            { return part[i].Get<Para>().styleid; }
	void                  SetParaStyle(i32 i, const Uuid& si);

	const RichTable&      GetTable(i32 i) const;

	i32                   GetLength() const;

	i32                   GetTableCount() const;
	const RichTable&      GetConstTable(i32 table) const;

	bool                  IsEmpty() const;

	RichCellPos           GetCellPos(i32 table, i32 row, i32 column) const;
	RichCellPos           GetCellPos(i32 table, Point p) const;

	void                  Clear();

	void                  ClearSpelling();

	void                  SetPick(i32 parti, RichTable&& table);
	void                  CatPick(RichTable&& table)                  { SetPick(GetPartCount(), pick(table)); }
	void                  Set(i32 parai, const RichPara& p, const RichStyles& s);
	void                  Insert(i32 parai, const RichPara& p, const RichStyles& s);
	void                  Cat(const RichPara& p, const RichStyles& s) { Set(GetPartCount(), p, s); }

	void                  RemovePart(i32 parti);

	void                  Normalize();

	Vec<i32>           GetAllLanguages() const;
	WTxt               GetPlainText(bool withcr = true, bool allow_tabs = false) const;

	void                  SetHeaderQtf(tukk qtf);
	void                  SetFooterQtf(tukk qtf);
	Txt                GetHeaderQtf() const                        { return header_qtf; }
	Txt                GetFooterQtf() const                        { return footer_qtf; }
	void                  ClearHeader()                               { SetHeaderQtf(NULL); }
	void                  ClearFooter()                               { SetFooterQtf(NULL); }

	struct UpdateIterator {
		enum { CONTINUE = 0, STOP = 1, UPDATE = 2 };
		virtual i32 operator()(i32 pos, RichPara& para) = 0;
		virtual ~UpdateIterator() {}
	};
	bool                  Iterate(UpdateIterator& r, i32 gpos, const RichStyles& s);

	struct Iterator {
		virtual bool operator()(i32 pos, const RichPara& para) = 0;
		virtual ~Iterator() {}
	};
	bool                  Iterate(Iterator& r, i32 gpos, const RichStyles& s) const;

	RichTxt(const RichTxt& src, i32);
	RichTxt();

	rval_default(RichTxt);

#ifdef _DEBUG
	void                  Dump();
#endif
};