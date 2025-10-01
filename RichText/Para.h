struct RichCaret;
struct RichPos;

struct RichPara {
	enum Code {
		NONE,

		INDEXENTRY  = 2,
		FIELD       = 3,
		FACE        = 4,
		HEIGHT      = 5,
		LANGUAGE    = 6,
		INK         = 7,
		PAPER       = 8,

		TAB         = 9, // placeholder

		LINK        = 10,
		SSCRIPT     = 11,

		BOLD0       = 12,
		BOLD1       = 13,
		BOLDS       = 14,
		ITALIC0     = 15,
		ITALIC1     = 16,
		ITALICS     = 17,
		UNDERLINE0  = 18,
		UNDERLINE1  = 19,
		UNDERLINES  = 20,
		CAPITALS0   = 21,
		CAPITALS1   = 22,
		CAPITALSS   = 23,
		STRIKEOUT0  = 24,
		STRIKEOUT1  = 25,
		STRIKEOUTS  = 26,
		DASHED0     = 27,
		DASHED1     = 28,
		DASHEDS     = 29,

		EXT         = 30,

		OBJECT      = 31
	};
	
	enum ExtCode {
		NONAA0      = 1,
		NONAA1      = 2,
		NONAAS      = 3,
	};

	enum BULLET_STYLE {
		BULLET_NONE,
		BULLET_ROUND,
		BULLET_ROUNDWHITE,
		BULLET_BOX,
		BULLET_BOXWHITE,
		BULLET_TEXT
	};

	enum NUMBER_STYLE {
		NUMBER_NONE,
		NUMBER_1,
		NUMBER_0,
		NUMBER_a,
		NUMBER_A,
		NUMBER_i,
		NUMBER_I,
	};

	enum LINESPACING {
		LSP10 = 0,
		LSP15 = -1,
		LSP20 = -2,
		LSP115 = -3,
	};
	
	enum TabSpecial {
		TAB_RIGHTPOS = 0x8000000
	};
	
	enum RulerStyle {
		RULER_SOLID,
		RULER_DOT,
		RULER_DASH,
	};

	struct CharFormat : public Font {
		i32     language;
		Color   ink, paper;
		Txt     link;
		WTxt    indexentry;
		dword   sscript:2;
		bool    capitals;
		bool    dashed;

#ifdef _DEBUG
		Txt  ToTxt() const;
#endif

		CharFormat();
	};

	struct Tab : Moveable<Tab> {
		i32   pos;
		u8  align;
		u8  fillchar;

		Tab() { align = ALIGN_LEFT; pos = 0; fillchar = 0; }
	};

	struct NumberFormat {
		Txt      before_number, after_number;
		bool        reset_number;
		u8        number[8];

		bool        IsNumbered() const;
		i32         GetNumberLevel() const;
	};

	struct Format : NumberFormat, CharFormat {
		Uuid        styleid;
		Txt      label;
		WithDeepCopy< Vec<Tab> > tab;
		i32         align;
		i32         ruler, before, lm, indent, rm, after;
		Color       rulerink;
		i32         rulerstyle;
		i32         tabsize;
		i32         bullet;
		i32         linespacing;
		bool        newpage, firstonpage, keep, keepnext, orphan, newhdrftr;
		Txt      header_qtf, footer_qtf;

		void        SortTabs();

#ifdef _DEBUG
		Txt      ToTxt() const;
#endif

		Format();
	};

	struct Number {
		i32     n[8];

		Txt  AsText(const NumberFormat& format) const;
		void    TestReset(const NumberFormat& format);
		void    Next(const NumberFormat& format);

		Number();
	};

	struct Part {
		RichObj                  object;
		WTxt                     text;
		CharFormat                  format;
		Id                          field;
		Txt                      fieldparam;
		WithDeepCopy< Array<Part> > fieldpart;

		bool        NonText() const    { return object || field; }
		bool        IsText() const     { return !NonText(); }
		i32         GetLength() const  { return object || field ? 1 : text.GetLength(); }
	};

	struct FieldType {
		virtual Array<Part> Evaluate(const Txt& param, VecMap<Txt, Val>& vars,
		                             const CharFormat& fmt) = 0;
		virtual void        Menu(Bar& bar, Txt *param) const {}
		virtual void        DefaultAction(Txt *param) const {}
		virtual ~FieldType() {}
	};

	struct HeightInfo {
		i32                  ascent;
		i32                  descent;
		i32                  external;
		i32                  ydelta;
		const RichObj    *object;

		i32                  Sum() const { return ascent + descent + external; }
	};

	struct Line : public HeightInfo {
		i32  pos;
		i32  len;
		i32  ppos;
		i32  plen;
		i32  xpos;
		i32  cx;
		i32  objecti;
		bool withtabs;
	};

	struct Lines {
		i32                        len;
		i32                        clen;
		i32                        cx;
		Buffer<wchar>              text;
		Buffer<u8>                 spell;
		Buffer<i32>                pos;
		Buffer<i32>                width;
		Buffer<HeightInfo>         height;
		Buffer<const CharFormat *> format;
		Array<Line>                line;
		Array<CharFormat>          hformat;
		Array<RichObj>             object;
		i32                        first_indent;
		i32                        next_indent;


		i32                  GetCount() const        { return line.GetCount(); }
		Line&                operator[](i32 i)       { return line[i]; }
		const Line&          operator[](i32 i) const { return line[i]; }
		void                 Justify(const Format& format);
		i32                  BodyHeight() const;

		rval_default(Lines);
	
		Lines();
		~Lines();

	private:
		static Array<Lines>&       Cache();
		bool                       incache;
		bool                       justified;
		z64                        cacheid;
		
		friend struct RichPara;
	};

	static const VecMap<Id, FieldType *>& fieldtype() { return fieldtype0(); }
	static void  Register(Id id, FieldType& ft);
	template <class T> static void Register(Id id)       { static T x; Register(id, x); }

	z64         cacheid;
	bool        incache;
	Array<Part> part;
	Format      format;

	static void Charformat(Stream& out, const CharFormat& o, const CharFormat& n,
	                       const CharFormat& s);

	static void DrawRuler(Draw& w, i32 x, i32 y, i32 cx, i32 cy, Color ink, i32 style);

	void        Cat(const WTxt& s, const CharFormat& f);
	void        Cat(tukk s, const CharFormat& f);
	void        Cat(const RichObj& o, const CharFormat& f);
	void        Cat(Id fieldtype, const Txt& param, const CharFormat& f);

	i32         GetLength() const;
	i32         GetCount() const                          { return part.GetCount(); }
	bool        IsEmpty() const;
	Txt         Pack(const Format& style, Array<RichObj>& obj) const;
	void        Unpack(const Txt& s, const Array<RichObj>& obj, const Format& style);
	
	void        ApplyStyle(const Format& newstyle);

	Part&       operator[](i32 i)                         { return part[i]; }
	const Part& operator[](i32 i) const                   { return part[i]; }
	i32         FindPart(i32& pos) const;
	void        Trim(i32 pos);
	void        Mid(i32 pos);
	void        Append(const RichPara& p)                 { part.Append(p.part); }

	void        GetRichPos(RichPos& rp, i32 pos) const;

	Lines       FormatLines(i32 cx) const;
	void        Paint(PageDraw& pw, RichContext rc, const PaintInfo& pi, const Number& n, const Bits& spellerror, bool baselevel) const;
	RichCaret   GetCaret(i32 pos, RichContext rc) const;
	i32         GetPos(i32 x, PageY y, RichContext rc) const;
	void        GatherLabels(Vec<RichValPos>& info, RichContext rc, i32 pos) const;
	void        GatherIndexes(Vec<RichValPos>& info, RichContext rc, i32 pos) const;
	i32         GetVertMove(i32 pos, i32 gx, const Rect& page, i32 dir) const;

	WTxt        GetText() const;

	bool        HasPos() const;

	bool        EvaluateFields(VecMap<Txt, Val>& vars);

	void        operator<<=(const RichPara& p)            { format = p.format; part <<= p.part; }

#ifdef _DEBUG
	void        Dump();
#endif

	void        ApplyZoom(Zoom z);
	
	void        CacheId(z64 id);

	rval_default(RichPara);

	RichPara();
	~RichPara();

private:
	Tab         GetNextTab(i32 pos, i32 cx) const;
	void        Smh(Lines& lines, HeightInfo *th, i32 cx) const;
	Lines       Begin(RichContext& rc) const;
	bool        BreaksPage(const RichContext& rc, const Lines& pl, i32 i) const;
	void        PackParts(Stream& out, const CharFormat& chrstyle,
	                      const Array<Part>& part, CharFormat& cf,
	                      Array<RichObj>& obj) const;
	void        UnpackParts(Stream& in, const CharFormat& chrstyle,
	                        Array<Part>& part, const Array<RichObj>& obj, i32& oi);
	static void Flush(Draw& draw, const PaintInfo& pi, wtuk text, const CharFormat **i0,
                      i32 *wd, i32 pos, i32 len, i32 x0, i32 x, i32 y0, i32 y, i32 linecy,
                      i32 lineascent, Zoom z, bool highlight);
	i32         PosInLine(i32 x, const Rect& page, const Lines& pl, i32 lni) const;

	static StaticMutex      cache_lock;
	static Array<RichPara>& Cache();

	static VecMap<Id, FieldType *>& fieldtype0();

	struct StorePart;
};

inline bool operator==(const RichPara::Tab& a, const RichPara::Tab& b) {
	return a.pos == b.pos && a.align == b.align && a.fillchar == b.fillchar;
}

bool NumberingDiffers(const RichPara::Format& fmt1, const RichPara::Format& fmt2);

void Sort(Vec<RichPara::Tab>& tab);

void operator*=(RichPara::Format& fmt, Zoom z);
