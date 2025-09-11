#include "CodeEditor.h"

namespace drx {

Array<IdentPos> GetLineIdent(tukk line)
{
	Array<IdentPos> out;
	tukk p = line;
	while(*p && *p != '\n')
		if(*p == '\"' || *p == '\'')
		{
			char term = *p++;
			while(*p && *p != term)
				if(*p++ == '\\' && *p)
					p++;
			if(*p == term)
				p++;
		}
		else if(*p == 's' && p[1] == '_' && p[2] == '(' && (IsAlpha(p[3]) || p[3] == '_'))
		{
			IdentPos& pos = out.Add();
			pos.begin = i32(p - line);
			tukk b = (p += 3);
			while(IsAlNum(*++p) || *p == '_')
				;
			pos.ident = Txt(b, p);
			if(*p == ')')
				p++;
			pos.end = i32(p - line);
		}
		else if(IsAlpha(*p) || *p == '_')
		{
			while(IsAlNum(*++p) || *p == '_')
				;
		}
		else
			p++;
	return out;
}

Vec<Point> GetLineTxt(wtukk wline, bool& is_begin, bool& is_end)
{
	Vec<Point> out;
	wtukk p = wline;
	while(*p <= ' ' && *p && *p != '\n')
		p++;
	is_begin = (*p == '\"');
	is_end = false;
	while(*p && *p != '\n')
	{
		wchar term = *p;
		if(term == '\"' || term == '\'')
		{
			i32 begin = i32(p++ - wline);
			while(*p && *p != '\n')
				if(*p == term)
				{
					wtukk lim = ++p;
					while((u8)*p <= ' ' && *p && *p != '\n')
						p++;
					if(*p != term)
					{
						is_end = (*p == '\n' || *p == 0);
						p = lim;
						break;
					}
					p++;
				}
				else if(*p++ == '\\' && *p && *p != '\n')
					p++;
			if(term == '\"')
				out.Add(Point(begin, i32(p - wline)));
		}
		else
			p++;
	}
	return out;
}

bool CodeEditor::GetTxtRange(z64 cursor, z64& b, z64& e) const
{
	i32 cl = GetLine(cursor);
	cursor -= GetPos64(cl);
	bool is_begin, is_end; //@@@@@@
	Vec<Point> list = GetLineTxt(GetWLine(cl), is_begin, is_end);
	i32 i = list.GetCount();
	while(--i >= 0 && (list[i].x > cursor || list[i].y < cursor))
		;
	if(i < 0)
		return false;
	i32 bl = cl, bp = list[i].x;
	i32 el = cl, ep = list[i].y;
	while(is_begin && bl > 0)
	{
		list = GetLineTxt(GetWLine(bl - 1), is_begin, is_end);
		if(list.IsEmpty() || !is_end)
			break;
		bl--;
		bp = list.Top().x;
	}
	while(el + 1 < GetLineCount() && ep >= GetLineLength(el))
	{
		list = GetLineTxt(GetWLine(el + 1), is_begin, is_end);
		if(list.IsEmpty() || !is_begin)
			break;
		el++;
		ep = list[0].y;
	}
	b = GetPos64(bl, bp);
	e = GetPos64(el, ep);
	return b < e;
}

bool CodeEditor::FindTxt(bool back)
{
	z64 ll, hh;
	hh = GetSelection(ll, hh) ? back ? ll : hh
	                          : GetCursor64();
	i32 l = GetLine(hh);
	i32 h = LimitSize(hh - GetPos64(l));

	while(l >= 0 && l < GetLineCount())
	{
		bool is_begin, is_end;
		Vec<Point> list = GetLineTxt(GetWLine(l), is_begin, is_end);
		if(back)
		{
			i32 i = list.GetCount();
			while(--i >= 0 && list[i].x >= h)
				;
			if(i >= 0)
			{
				h = list[i].x;
				break;
			}
			h = 1000000;
			--l;
		}
		else
		{
			i32 i = 0;
			while(i < list.GetCount() && list[i].y <= h)
				i++;
			if(i < list.GetCount())
			{
				h = list[i].x;
				break;
			}
			h = 0;
			++l;
		}
	}
	z64 b, e;
	if(l < 0 || l >= GetLineCount() || !GetTxtRange(GetPos64(l, h), b, e))
		return false;
	SetSelection(b, e);
	return true;
}

bool CodeEditor::FindLangTxt(bool back)
{
	z64 ll, hh;
	hh = GetSelection(ll, hh) ? back ? ll : hh
	                          : GetCursor64();
	i32 l = GetLine(hh);
	i32 h = LimitSize(hh - GetPos64(l));

	for(;;)
	{
		Array<IdentPos> list = GetLineIdent(GetUtf8Line(l));
		z64 b, e;
		if(back)
		{
			i32 i = list.GetCount();
			while(--i >= 0 && list[i].begin >= h)
				;
			if(i < 0)
			{
				h = 1000000;
				if(--l < 0)
					break;
				continue;
			}
			b = GetPos64(l, list[i].begin);
			e = GetPos64(l, list[i].end);
		}
		else
		{
			i32 i = 0;
			while(i < list.GetCount() && list[i].end <= h)
				i++;
			if(i >= list.GetCount())
			{
				h = 0;
				if(++l >= GetLineCount())
					break;
				continue;
			}
			b = GetPos64(l, list[i].begin);
			e = GetPos64(l, list[i].end);
		}
		SetSelection(b, e);
		return true;
	}
	return false;
}

}
