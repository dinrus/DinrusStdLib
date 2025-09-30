class TagSyntax : public EditorSyntax { // Tag based languages (XML, HTML)
public:
	virtual void            Clear();
	virtual void            ScanSyntax(wtukk ln, wtukk e, i32 line, i32 tab_size);
	virtual void            Serialize(Stream& s);
	virtual void            Highlight(wtukk s, wtukk end, HighlightOutput& hls,
	                                  CodeEditor *editor, i32 line, z64 pos);
	virtual void            CheckSyntaxRefresh(CodeEditor& e, z64 pos, const WTxt& text);
	virtual void            IndentInsert(CodeEditor& editor, i32 chr, i32 count);
	virtual bool            CheckBrackets(CodeEditor& e, z64& bpos0, z64& bpos);

private:

	enum { TEXT, TAG0, TAG, ENDTAG, ATTR, COMMENT, DECL, PI, SCRIPT };

	bool    html;
	bool    witz;
	i32     status;
	i32     hl_ink;
	i32     hl_paper;
	Txt     tagname;

	CSyntax script;     // for <style> or <script>
	enum { CSS, JS };
	i32     script_type;

	HighlightOutput *hout;

	wtukk Spaces(wtukk s, wtukk e);
	void DoScript(wtukk s, wtukk e, CodeEditor *editor, i32 line, i32 tabsize, z64 pos);
	void Do(wtukk s, wtukk e, CodeEditor *editor, i32 line, i32 tabsize, z64 pos);
	void Put0(i32 ink, i32 n = 1, i32 paper = PAPER_NORMAL);

	void Set(i32 ink, i32 paper = PAPER_NORMAL)                 { hl_ink = ink; hl_paper = paper; }
	void Put(i32 n = 1)                                         { Put0(hl_ink, n, hl_paper); }
	void SetPut(i32 ink, i32 n = 1, i32 paper = PAPER_NORMAL)   { Set(ink, paper); Put(n); }

public:
	TagSyntax&  Html(bool b)                                    { html = b; return *this; }
	TagSyntax&  Witz(bool b)                                    { html = b; return *this; }
	CSyntax    *GetScript()                                        { return status == SCRIPT ? &script : NULL; }

	TagSyntax();
};
