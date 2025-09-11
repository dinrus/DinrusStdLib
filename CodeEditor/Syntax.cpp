#include "CodeEditor.h"

namespace drx {

void EditorSyntax::Clear()
{
}

void EditorSyntax::ScanSyntax(wtukk ln, wtukk e, i32 line, i32 tab_size)
{
}

void EditorSyntax::Serialize(Stream& s)
{
}

void EditorSyntax::IndentInsert(CodeEditor& editor, i32 chr, i32 count)
{
	editor.InsertChar(chr, count);
}

bool EditorSyntax::CheckBrackets(CodeEditor& e, z64& bpos0, z64& bpos)
{
	return false;
}

bool EditorSyntax::CanAssist() const
{
	return false;
}

void EditorSyntax::ReformatComment(CodeEditor& e)
{
}

void EditorSyntax::Highlight(wtukk s, wtukk end, HighlightOutput& hls, CodeEditor *editor, i32 line, z64 pos)
{
}

Vec<IfState> EditorSyntax::PickIfStack()
{
	return Vec<IfState>();
}

Color EditorSyntax::IfColor(char c)
{
	switch(c){
	case IfState::IF:          return LtBlue();
	case IfState::ELIF:        return Gray();
	case IfState::ELSE:        return Green();
	case IfState::ELSE_ERROR:  return LtRed();
	case IfState::ENDIF_ERROR: return LtMagenta();
	default:                   return Null;
	}
}

void EditorSyntax::CheckSyntaxRefresh(CodeEditor& e, z64 pos, const WTxt& text)
{
}

EditorSyntax::~EditorSyntax() {}

}
