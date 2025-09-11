class PythonSyntax : public EditorSyntax {
public:
	virtual void Highlight(wtukk start, wtukk end, HighlightOutput& hls,
	                       CodeEditor *editor, i32 line, z64 pos);
	virtual void IndentInsert(CodeEditor& e, i32 chr, i32 count);

private:
	struct Identation {
		enum Type {
			Tab = 0,
			Space,
			Unknown
		};
	};

	bool             LineHasColon(const WTxt& line);
	i32              CalculateLineIndetations(const WTxt& line, Identation::Type type);
	i32              CalculateSpaceIndetationSize(CodeEditor& editor);
	Identation::Type FindIdentationType(CodeEditor& editor, const WTxt& line);
	char             GetIdentationByType(Identation::Type type);
};
