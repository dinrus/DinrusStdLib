class LogSyntax : public EditorSyntax {
public:
	virtual void            Highlight(wtukk s, wtukk end, HighlightOutput& hls,
	                                  CodeEditor *editor, i32 line, z64 pos);
};
