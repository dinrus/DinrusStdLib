#include "CodeEditor.h"

namespace drx {

void PythonSyntax::Highlight(wtukk s, wtukk end, HighlightOutput& hls, CodeEditor *editor, i32 line, z64 pos)
{
	const HlStyle& ink = hl_style[INK_NORMAL];
	while(s < end) {
		i32 c = *s;
		dword pair = MAKELONG(s[0], s[1]);
		if(c == '#') {
			hls.Put(i32(end - s), hl_style[INK_COMMENT]);
			return;
		}
		else
		if(findarg(pair, MAKELONG('0', 'x'), MAKELONG('0', 'X'), MAKELONG('0', 'b'), MAKELONG('0', 'B'),
		                 MAKELONG('0', 'o'), MAKELONG('0', 'O')) >= 0)
			s = HighlightHexBin(hls, s, 2, thousands_separator);
		else
		if(IsDigit(c))
			s = HighlightNumber(hls, s, thousands_separator, false, false);
		else
		if(c == '\'' || c == '\"') {
			wtukk s0 = s;
			s++;
			for(;;) {
				i32 c1 = *s;
				if(s >= end || c1 == c) {
					s++;
					hls.Put((i32)(s - s0), hl_style[INK_CONST_STRING]);
					break;
				}
				s += 1 + (c1 == '\\' && s[1] == c);
			}
		}
		else
		if(IsAlpha(c) || c == '_') {
			static Index<Txt> kws = { "False", "await", "else", "import", "pass", "None", "break",
			                             "except", "in", "raise", "True", "class", "finally", "is",
			                             "return", "and", "continue", "for", "lambda", "try", "as",
			                             "def", "from", "nonlocal", "while", "assert", "del", "global",
			                             "not", "with", "async", "elif", "if", "or", "yield" };
			static Index<Txt> sws = { "self", "NotImplemented", "Ellipsis", "__debug__", "__file__", "__name__" };
			Txt w;
			while(s < end && IsAlNum(*s) || *s == '_')
				w.Cat(*s++);
			hls.Put(w.GetCount(), kws.Find(w) >= 0 ? hl_style[INK_KEYWORD] :
			                      sws.Find(w) >= 0 ? hl_style[INK_UPP] :
			                      ink);
		}
		else
		if(c == '\\' && s[1]) {
			hls.Put(2, ink);
			s += 2;
		}
		else {
			bool hl = findarg(c, '[', ']', '(', ')', ':', '-', '=', '{', '}', '/', '<', '>', '*',
			                     '#', '@', '\\', '.') >= 0;
			hls.Put(1, hl ? hl_style[INK_OPERATOR] : ink);
			s++;
		}
	}
}

void PythonSyntax::IndentInsert(CodeEditor& editor, i32 chr, i32 count)
{
	if(chr == '\n') {
		while(count--) {
			WTxt cursorLine = editor.GetWLine(editor.GetCursorLine());
			editor.InsertChar('\n', 1);
			
			Identation::Type idType = FindIdentationType(editor, cursorLine);
			char idChar = GetIdentationByType(idType);
			i32 mult = 1;
			if(idType == Identation::Space)
				mult = CalculateSpaceIndetationSize(editor);
			if(LineHasColon(cursorLine))
				editor.InsertChar(idChar, mult);
			editor.InsertChar(idChar, CalculateLineIndetations(cursorLine, idType));
		}
	}
	if(count > 0)
		editor.InsertChar(chr, count);
}

bool PythonSyntax::LineHasColon(const WTxt& line)
{
	for(i32 i = line.GetLength() - 1; i >= 0; i--) {
		if(line[i] == ':')
			return true;
	}
	return false;
}

i32 PythonSyntax::CalculateLineIndetations(const WTxt& line, Identation::Type type)
{
	i32 count = 0;
	for(i32 i = 0; i < line.GetLength(); i++) {
		if(type == Identation::Tab && line[i] == '\t')
			count++;
		else
		if(type == Identation::Space && line[i] == ' ')
			count++;
		else
			break;
	}
	return count;
}

PythonSyntax::Identation::Type PythonSyntax::FindIdentationType(CodeEditor& editor, const WTxt& line)
{
	Identation::Type type = Identation::Unknown;
	if(line.StartsWith("\t"))
		type = Identation::Tab;
	else
	if(line.StartsWith(" "))
		type = Identation::Space;
	else {
		for(i32 i = 0; i < editor.GetLineCount(); i++) {
			WTxt cLine = editor.GetWLine(i);
			if(cLine.StartsWith("\t")) {
				type = Identation::Tab;
				break;
			}
			else
			if(cLine.StartsWith(" ")) {
				type = Identation::Space;
				break;
			}
		}
	}
	return type;
}

i32 PythonSyntax::CalculateSpaceIndetationSize(CodeEditor& editor)
{
	i32 current = 0;
	for(i32 i = 0; i < editor.GetLineCount(); i++) {
		WTxt line = editor.GetWLine(i);
		for(i32 j = 0; j < line.GetLength(); j++) {
			if(line[j] == ' ')
				current++;
			else
				break;
		}
		
		if(current > 0)
			break;
	}
	
	// СДЕЛАТЬ: 4 is magic number - try to find the way to get this number from ide constants
	return current > 0 ? current : 4;
}

char PythonSyntax::GetIdentationByType(Identation::Type type)
{
	if(type == Identation::Space)
		return ' ';
	return '\t';
}

}
