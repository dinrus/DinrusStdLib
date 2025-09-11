class DiffSyntax : public EditorSyntax {
public:
	DiffSyntax();

	virtual void Highlight(wtukk start, wtukk end, HighlightOutput& hls,
	                       CodeEditor *editor, i32 line, z64 pos);
	
protected:
	void Put(i32 ink, i32 n = 1, i32 paper = PAPER_NORMAL);
	void Do(wtukk s, wtukk end, CodeEditor *editor, i32 line, i32 tabsize, z64 pos);
	
	i32  FindTheNumberOfCharsToLineEnd(wtukk current, wtukk end) const;
	bool IsPattern(wtukk current, wtukk end, Txt pattern) const;
	
private:
	HighlightOutput *hout;
};
