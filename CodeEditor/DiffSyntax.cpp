#include "CodeEditor.h"

namespace drx {

DiffSyntax::DiffSyntax()
{
	hout = NULL;
}

void DiffSyntax::Highlight(wtukk start, wtukk end, HighlightOutput& hls,
                                                      CodeEditor *editor, i32 line, z64 pos)
{
	hout = &hls;
	Do(start, end, editor, line, editor ? editor->GetTabSize() : 4, pos);
	hout = NULL;
}

void DiffSyntax::Put(i32 ink, i32 n, i32 paper)
{
	if(hout)
		hout->Put(n, hl_style[ink], hl_style[paper]);
}

void DiffSyntax::Do(wtukk ln, wtukk end, CodeEditor *editor, i32 line,
                                                                         i32 tabsize, z64 pos)
{
	i32k lineLength = FindTheNumberOfCharsToLineEnd(ln, end);
	
	if(IsPattern(ln, end, "---") || IsPattern(ln, end, "***")) {
		wtukk lnPrefix = end - 4;
		
		if(lnPrefix >= ln) {
			if (IsPattern(lnPrefix, end, "----") || IsPattern(lnPrefix, end, "****")) {
				Put(INK_DIFF_HEADER, lineLength);
				return;
			}
		}
		
		Put(INK_DIFF_FILE_INFO, lineLength);
	}
	else
	if(IsPattern(ln, end, "====") || IsPattern(ln, end, "index"))
		Put(INK_DIFF_FILE_INFO, lineLength);
	else
	if(IsPattern(ln, end, "+++"))
		Put(INK_DIFF_HEADER, lineLength);
	else
	if(IsPattern(ln, end, "@@"))
		Put(INK_DIFF_HEADER, lineLength);
	else
	if(IsDigit(*ln))
		Put(INK_DIFF_HEADER, lineLength);
	else
	if(*ln == '+' || *ln == '>')
		Put(INK_DIFF_ADDED, lineLength);
	else
	if(*ln == '-' || *ln == '<' || *ln == '!')
		Put(INK_DIFF_REMOVED, lineLength);
	else
	if(*ln == ' ')
		Put(INK_NORMAL, lineLength);
	else
		Put(INK_DIFF_COMMENT, lineLength);
}

i32 DiffSyntax::FindTheNumberOfCharsToLineEnd(wtukk current, wtukk end) const
{
	i32 counter = 0;
	
	while(current < end) {
		current++;
		counter++;
	}
	
	return counter;
}

bool DiffSyntax::IsPattern(wtukk current, wtukk end, Txt pattern) const
{
	bool containing = true;
	
	i32 i = 0;
	while((current < end) && (i < pattern.GetCount())) {
		if(ToLower(*current) != ToLower((wchar) pattern[i])) {
			containing = false;
			break;
		}
		
		current++;
		i++;
	}
	
	return (containing && (i == pattern.GetCount()));
}

}
