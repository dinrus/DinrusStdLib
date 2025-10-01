#include "CppBase.h"
#include "Internal.h"

namespace drx {

inline bool IsSpc(u8 c)
{
	return c > 0 && c <= 32;
}

Txt CppMacro::Define(tukk s)
{
	CParser p(s);
	Txt id;
	try {
		if(!p.IsId())
			return Null;
		p.NoSkipSpaces().NoSkipComments(); // '#define TEST(x)' is different form '#define TEST (x)' - later is parameterless
		id = p.ReadId();
		param.Clear();
		if(p.Char('(')) {
			p.SkipSpaces();
			p.Spaces();
			while(p.IsId()) {
				if(param.GetCount())
					param << ",";
				param << p.ReadId();
				p.Char(',');
			}
			if(p.Char3('.', '.', '.'))
				param << '.';
			p.Char(')');
			if(param.GetCount() == 0) // #define foo() bar - need to 'eat' parenthesis, cheap way
				param = ".";
		}
		tukk b = p.GetPtr();
		while(!p.IsEof() && !p.IsChar2('/', '/'))
			p.SkipTerm();
		body = Txt(b, p.GetPtr());
		Md5Stream m;
		m.Put(param);
		m.Put(body);
		m.Finish(md5);
	}
	catch(CParser::Error) {
		return Null;
	}
	return id;
}

Txt CppMacro::ToTxt() const
{
	Txt r;
	if(param.GetCount()) {
		Txt h = param;
		h.Replace(".", "...");
		r << "(" << h << ")";
	}
	if(IsUndef())
		r << " #undef";
	else
		r << ' ' << body;
	return r;
}

void CppMacro::Serialize(Stream& s)
{
	s % param % body;
	s.SerializeRaw(md5, 16);
}

Txt CppMacro::Expand(const Vec<Txt>& p, const Vec<Txt>& ep) const
{
	Txt r;
	tukk s = body;
	Txt pp = param;
	if(*pp.Last() == '.')
		pp.Trim(pp.GetCount() - 1);
	Index<Txt> param(Split(pp, ','));
	static Txt VA_ARGS("__VA_ARGS__"); // static - Speed optimization
	while(*s) {
		if(IsAlpha(*s) || *s == '_') {
			tukk b = s;
			s++;
			while(IsAlNum(*s) || *s == '_')
				s++;
			Txt id(b, s);
			tukk ss = b;
			bool cat = false;
			while(ss > ~body && ss[-1] == ' ')
				ss--;
			if(ss >= ~body + 2 && ss[-1] == '#' && ss[-2] == '#')
				cat = true;
			ss = s;
			while(*ss && *ss == ' ')
				ss++;
			if(ss[0] == '#' && ss[1] == '#')
				cat = true;
			if(id == VA_ARGS) {
				bool next = false;
				for(i32 i = param.GetCount(); i < ep.GetCount(); i++) {
					if(next)
						r.Cat(", ");
					r.Cat((cat ? p : ep)[i]);
					next = true;
				}
			}
			else {
				i32 q = param.Find(id);
				if(q >= 0) {
					if(q < ep.GetCount())
						r.Cat((cat ? p : ep)[q]);
				}
				else
					r.Cat(id);
			}
			continue;
		}
		if(s[0] == '#' && s[1] == '#') {
			i32 q = r.GetLength();
			while(q > 0 && IsSpc(r[q - 1]))
				q--;
			r.Trim(q);
			s += 2;
			while((u8)*s <= ' ')
				s++;
			continue;
		}
		if(*s == '#') {
			tukk ss = s + 1;
			while(IsSpc(*ss))
				ss++;
			if(IsAlpha(*ss) || *ss == '_') {
				tukk b = ss;
				ss++;
				while(IsAlNum(*ss) || *ss == '_')
					ss++;
				Txt id(b, ss);
				i32 q = param.Find(id);
				if(q >= 0) {
					if(q <= p.GetCount()) {
						if(q < p.GetCount())
							r.Cat(AsCTxt(p[q]));
						s = ss;
						continue;
					}
				}
				r.Cat(Txt(s, ss));
				s = ss;
				continue;
			}
		}
		r.Cat(*s++);
	}
	return r;
}

}
