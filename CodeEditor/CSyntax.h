class CSyntax : public EditorSyntax { // Curly braces languages (C++, Java, C#, Javascript...) common support
public:
	virtual void            Clear();
	virtual void            ScanSyntax(wtukk ln, wtukk e, i32 line, i32 tab_size);
	virtual void            Serialize(Stream& s);
	virtual void            IndentInsert(CodeEditor& editor, i32 chr, i32 count);
	virtual bool            CheckBrackets(CodeEditor& e, z64& bpos0, z64& bpos);
	virtual bool            CanAssist() const;
	virtual void            Highlight(wtukk s, wtukk end, HighlightOutput& hls,
	                                  CodeEditor *editor, i32 line, z64 pos);
	virtual void            CheckSyntaxRefresh(CodeEditor& e, z64 pos, const WTxt& text);
	virtual Vec<IfState>    PickIfStack(); // СДЕЛАТЬ: Refactor?
	virtual void            ReformatComment(CodeEditor& e);

protected:
	bool        comment;       // we are in /* */ block comment
	bool        linecomment;   // we are in // line comment (because it can be continued by '\')
	bool        string;        // we are in string (becase it can be continued by '\')
	bool        linecont;      // line ended with '\'
	bool        was_namespace; // true if there was 'namespace', until '{' or ';' (not in ( [ brackets)
	WTxt        raw_string;    // we are in C++11 raw string literal, this is end delimiter, e.g. )"
	char        macro;         // can be one of:
	enum        {
		MACRO_OFF = 0,  // last line was not #define
	    MACRO_CONT, // last line was #define and ended with '\'
	    MACRO_END   // last line was a macro, but ended
	};

	i32         cl, bl, pl; // levels of { [ (

	Vec<i32>     brk; // { ( [ stack (contains '{', ')', ']')
	Vec<i32>     blk; // { line stack //СДЕЛАТЬ:SYNTAX: Join blk and bid
	Vec<i32>     bid; // { indentation stack
	Vec<Isx>     par; // ( [ position stack
	Vec<IfState> ifstack;

	i32         stmtline;     // line of latest "if", "else", "while", "do", "for" or -1
	i32         endstmtline;  // line of latest ';' (not in ( [ brackets)
	i32         seline;       // stmtline stored here on ';' (not in ( [ brackets)
	i32         spar;         // ( [ level, reset on "if", "else", "while", "do", "for"
	
	i32         highlight;    // subtype (temporary) СДЕЛАТЬ

	static i32  InitUpp(tukk *q);
	static void InitKeywords();
	wtukk DoComment(HighlightOutput& hls, wtukk p, wtukk e);

	static Vec< Index<Txt> > keyword;
	static Vec< Index<Txt> > name;
	static Index<Txt>        kw_upp;
	static i32 kw_macros, kw_logs, kw_sql_base, kw_sql_func;

	
	static Color BlockColor(i32 level);

	i32     GetCommentPos(CodeEditor& e, i32 l, WTxt& ch) const;
	WTxt    GetCommentHdr(CodeEditor& e, i32 l) const { WTxt h; GetCommentPos(e, l, h); return h; }
	void    IndentInsert0(CodeEditor& e, i32 chr, i32 count, bool reformat);

	void Bracket(z64 pos, HighlightOutput& hls, CodeEditor *editor);

	void  ClearBraces();

	void  Grounding(wtukk ln, wtukk e);

	bool CheckBracket(CodeEditor& e, i32 li, z64 pos, z64 ppos, z64 pos0, WTxt ln, i32 d, i32 limit, z64& bpos0, z64& bpos);
	bool CheckLeftBracket(CodeEditor& e, z64 pos, z64& bpos0, z64& bpos);
	bool CheckRightBracket(CodeEditor& e, z64 pos, z64& bpos0, z64& bpos);

	bool RawTxt(wtukk p, i32& n);
	
public:
	static i32  LoadSyntax(tukk keywords[], tukk names[]);

	enum HighlightType {
		HIGHLIGHT_NONE = -1, HIGHLIGHT_CPP = 0, HIGHLIGHT_USC, HIGHLIGHT_JAVA, HIGHLIGHT_T,
		HIGHLIGHT_CALC, HIGHLIGHT_LAY, HIGHLIGHT_SCH, HIGHLIGHT_SQL, HIGHLIGHT_CS,
		HIGHLIGHT_JAVASCRIPT, HIGHLIGHT_CSS, HIGHLIGHT_JSON, HIGHLIGHT_PHP,
		HIGHLIGHT_COUNT
	};
	
	void    SetHighlight(i32 h)           { highlight = h; }

	CSyntax()                             { Clear(); }
};

wtukk HighlightNumber(HighlightOutput& hls, wtukk p, bool ts, bool octal, bool css);
wtukk HighlightHexBin(HighlightOutput& hls, wtukk p, i32 plen, bool thousands_separator);
