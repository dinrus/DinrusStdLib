#include "CppBase.h"
#include "Internal.h"

namespace drx {

bool IsCPPFile(const Txt& path)
{
	return findarg(ToLower(GetFileExt(path)) , ".c", ".cpp", ".cc" , ".cxx", ".icpp") >= 0;
}

bool IsHFile(const Txt& path)
{
	return findarg(ToLower(GetFileExt(path)) , ".h", ".hpp", ".hxx" , ".hh") >= 0;
}

void SetSpaces(Txt& l, i32 pos, i32 count)
{
	TxtBuf s(l);
	memset(~s + pos, ' ', count);
	l = s;
}

tukk SkipTxt(tukk s)
{
	CParser p(s);
	try {
		p.ReadOneTxt(*s);
	}
	catch(CParser::Error) {}
	s = p.GetPtr();
	while((u8)*(s - 1) <= ' ')
		s--;
	return s;
}

void RemoveComments(Txt& l, bool& incomment)
{
	i32 q = -1;
	i32 w = -1;
	if(incomment)
		q = w = 0;
	else {
		tukk s = l;
		while(*s) {
			if(*s == '\"')
				s = SkipTxt(s);
			else
			if(s[0] == '/' && s[1] == '/') {
				q = i32(s - ~l);
				SetSpaces(l, q, l.GetCount() - q);
				return;
			}
			else
			if(s[0] == '/' && s[1] == '*') {
				q = i32(s - ~l);
				break;
			}
			else
				s++;
		}
		if(q >= 0)
			w = q + 2;
	}
	while(q >= 0) {
		i32 eq = l.Find("*/", w);
		if(eq < 0) {
			incomment = true;
			SetSpaces(l, q, l.GetCount() - q);
			return;
		}
		SetSpaces(l, q, eq + 2 - q);
		incomment = false;
		q = l.Find("/*");
		w = q + 2;
	}
}

};