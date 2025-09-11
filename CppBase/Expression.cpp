#include "CppBase.h"
#include "Internal.h"

#if 0
#define LDUMP(x)     DDUMP(x)
#define LDUMPC(x)    DDUMPC(x)
#define LLOG(x)      DLOG(x)
#else
#define LDUMP(x)
#define LDUMPC(x)
#define LLOG(x)
#endif

#define LTIMING(x) // DTIMING(x)

namespace drx {

const Array<CppItem>& GetTypeItems(const CppBase& codebase, const Txt& type)
{
	static Array<CppItem> sEmpty;
	i32 q = codebase.Find(type);
	if(q < 0)
		return sEmpty;
	return codebase[q];
}

Txt ParseTemplatedType(const Txt& type, Vec<Txt>& tparam)
{
	tukk s = type;
	Txt r;
	while(*s) {
		if(*s == '<') {
			s++;
			i32 lvl = 0;
			Txt t;
			while(*s) {
				i32 c = *s++;
				if(c == ',' && lvl == 0) {
					tparam.Add(t);
					t.Clear();
				}
				else {
					if(c == '>') {
						if(lvl == 0)
							break;
						lvl--;
					}
					if(c == '<')
						lvl++;
					t.Cat(c);
				}
			}
			tparam.Add(t);
		}
		else
			r.Cat(*s++);
	}
	LLOG("ParseTemplatedType " << type << " -> " << r);
	LDUMPC(tparam);
	return r;
}

Txt ResolveTParam(const CppBase& codebase, const Txt& type, const Vec<Txt>& tparam)
{
	LLOG("ResolveTParam " << type << ' ' << tparam);
	Txt r;
	tukk s = type;
	while(*s) {
		if(IsDigit(*s)) {
			i32 i = *s++ - '0';
			if(i >= 0 && i < tparam.GetCount())
				r.Cat(tparam[i]);
		}
		else
		if(iscib(*s))
			while(iscid(*s))
				r.Cat(*s++);
		else
			r.Cat(*s++);
	}
	LLOG("Resolved " << type << " -> " << r);
	const Array<CppItem>& x = GetTypeItems(codebase, r);
	if(x.GetCount() && x[0].kind == TYPEDEF) {
		LLOG("Is typedef " << x[0].qtype << ", unqualified " << x[0].type << ", natural " << x[0].natural);
		Txt h = x[0].qtype;
		if(h != type && h != r)
			return ResolveTParam(codebase, h, tparam);
		return h;
	}
	return r;
}

void ResolveTParam(const CppBase& codebase, Vec<Txt>& type, const Vec<Txt>& tparam)
{
	for(i32 i = 0; i < type.GetCount(); i++)
		type[i] = ResolveTParam(codebase, type[i], tparam);
}

struct ExpressionTyper {
	const CppBase&        codebase;
	i32                   scan_counter; // limit permutations
	Txt                context_type;
	Txt                usings;
	const Vec<Txt>& xp;
	Index<Txt>         typeset;
	const ParserContext&  parser;

	Vec<Txt>        GetTypeBases(const Txt& type);
	Txt                ResolveReturnType(const CppItem& m, const Vec<Txt>& tparam);

	void                  ExpressionType(bool isptr, const Txt& ttype, i32 ii,
	                                     bool variable, bool can_shortcut_operator,
	                                     Index<Txt>& visited_bases, i32 lvl);
	void                  ExpressionType(bool isptr, const Txt& ttype, i32 ii, bool variable, i32 lvl);

	Index<Txt>         ExpressionType();
	
	enum { MAX_COUNT = 1000 };

	ExpressionTyper(const CppBase& codebase, const ParserContext& parser, const Vec<Txt>& xp);
};

ExpressionTyper::ExpressionTyper(const CppBase& codebase, const ParserContext& parser, const Vec<Txt>& xp)
:	codebase(codebase), xp(xp), parser(parser)
{
	scan_counter = 0;
	context_type = parser.current_scope;
	usings = parser.context.namespace_using;
}

Vec<Txt> ExpressionTyper::GetTypeBases(const Txt& type)
{
	const Array<CppItem>& n = GetTypeItems(codebase, type);
	Txt bases;
	for(i32 i = 0; i < n.GetCount(); i++) {
		const CppItem& im = n[i];
		if(im.IsType())
			bases << im.qptype << ';';
	}
	Index<Txt> r;
	Vec<Txt> h = Split(bases, ';');
	for(i32 i = 0; i < h.GetCount(); i++)
		r.FindAdd(h[i]);
	return r.PickKeys();
}

Txt ExpressionTyper::ResolveReturnType(const CppItem& m, const Vec<Txt>& tparam)
{
	if(m.tparam.GetCount()) {
		i32 q = InScListIndex(m.qtype, m.tname);
		if(q >= 0 && q < tparam.GetCount())
			return tparam[q];
	}
	return m.qtype;
}

Vec<Txt> SplitTScope(tukk s)
{
	Vec<Txt> r;
	tukk b = s;
	while(*s) {
		if(*s == '<') {
			s++;
			i32 lvl = 0;
			while(*s) {
				i32 c = *s++;
				if(c == '>') {
					if(lvl == 0)
						break;
					lvl--;
				}
				if(c == '<')
					lvl++;
			}
		}
		else
		if(*s == ':') {
			if(s > b)
				r.Add(Txt(b, s));
			while(*s == ':')
				s++;
			b = s;
		}
		else
			s++;
	}
	if(s > b)
		r.Add(Txt(b, s));
	return r;
}

Txt AddTParams(const Txt& type, const Txt& ttype)
{ // drx::Vec::Foo, drx::Vec<drx::Txt>::Bar -> drx::Vec<drx::Txt>::Foo
	if(ttype.Find('<') < 0)
		return type;
	Vec<Txt> t = SplitTScope(type);
	Vec<Txt> tt = SplitTScope(ttype);
	i32 i = 0;
	Txt r;
	while(i < t.GetCount() && i < tt.GetCount()) {
		i32 q = tt[i].Find('<');
		if(q < 0)
			q = tt[i].GetLength();
		if(tt[i].Mid(0, q) != t[i])
			break;
		if(i)
			r << "::";
		r << tt[i];
		i++;
	}
	while(i < t.GetCount()) {
		if(i)
			r << "::";
		r << t[i];
		i++;
	}
	LLOG("AddTParam " << type << ", " << ttype << " -> " << r);
	return r;
}

void ExpressionTyper::ExpressionType(bool isptr, const Txt& ttype, i32 ii,
                                     bool variable, bool can_shortcut_operator,
                                     Index<Txt>& visited_bases, i32 lvl)
{
	LLOG("--- ExpressionType " << scan_counter << ", lvl " << lvl << ", ttype " << ttype << ", isptr " << isptr
	     << ", ii: " << ii << ", " << (ii < xp.GetCount() ? xp[ii] : "<end>"));
	if(++scan_counter >= MAX_COUNT || lvl > 100) // sort of ugly limitation of parsing permutations
		return;
	LDUMP(can_shortcut_operator);
	Vec<Txt> tparam;
	Txt type = ParseTemplatedType(ttype, tparam);
	i32 c0 = typeset.GetCount();
	const Array<CppItem>& n = GetTypeItems(codebase, type);
	LDUMP(type);
	LDUMP(tparam);
	
	// STL/NTL is too much to parse for now, so until we have better expression resolver, let us play dirty
	static Index<Txt> std_container;
	static Index<Txt> std_pair_container;
	static Index<Txt> std_container_iterator;
	static Index<Txt> std_pair_container_iterator;
	static Index<Txt> upp_container;
	static Index<Txt> upp_container_iterator;
	static Index<Txt> upp_map_container;
	static Index<Txt> upp_map_container_iterator;
	static Index<Txt> upp_map_container_key_iterator;
	ONCELOCK {
		Vec<Txt> a = Split("array;vector;deque;forward_list;list;stack;queue;priority_queue;"
		                         "set;multiset;unordered_set;unordered_multiset", ';');
		for(i32 i = 0; i < a.GetCount(); i++) {
			std_container.Add("std::" + a[i]);
			std_container_iterator.Add("std::" + a[i] + "::iterator");
		}
		a = Split("map;multimap;unordered_map;unordered_multimap", ';');
		for(i32 i = 0; i < a.GetCount(); i++) {
			std_container.Add("std::" + a[i]);
			std_pair_container_iterator.Add("std::" + a[i] + "::iterator");
		}
		a = Split("VecMap;ArrayMap;SortedVecMap;SortedArrayMap", ';');
		for(i32 i = 0; i < a.GetCount(); i++) {
			upp_map_container.Add("drx::" + a[i]);
			upp_map_container_iterator.Add("drx::" + a[i] + "::iterator");
			upp_map_container_iterator.Add("drx::" + a[i] + "::Iterator");
			upp_map_container_key_iterator.Add("drx::" + a[i] + "::KeyIterator");
		}
		a = Split("Index;ArrayIndex;InVector;InArray;SortedIndex", ';');
		for(i32 i = 0; i < a.GetCount(); i++) {
			upp_container.Add("drx::" + a[i]);
			upp_container_iterator.Add("drx::" + a[i] + "::iterator");
			upp_container_iterator.Add("drx::" + a[i] + "::Iterator");
		}
	}
	if(tparam.GetCount() > 0 && std_container_iterator.Find(type) >= 0) {
		LLOG("# nasty iterator");
		typeset.Clear();
		ExpressionType(false, tparam[0], ii, variable, can_shortcut_operator, visited_bases, lvl + 1);
		scan_counter = MAX_COUNT;
		return;
	}
	if(tparam.GetCount() > 1 && std_pair_container_iterator.Find(type) >= 0) {
		LLOG("# nasty pair iterator");
		typeset.Clear();
		ExpressionType(false, "std::pair<" + tparam[0] + "," + tparam[1] + ">",
		               ii, variable, can_shortcut_operator, visited_bases, lvl + 1);
		scan_counter = MAX_COUNT;
		return;
	}
	if(tparam.GetCount() > 0 && upp_container_iterator.Find(type) >= 0) {
		LLOG("# drx nasty iterator");
		typeset.Clear();
		ExpressionType(false, tparam[0], ii, variable, can_shortcut_operator, visited_bases, lvl + 1);
		scan_counter = MAX_COUNT;
		return;
	}
	if(tparam.GetCount() > 1 && upp_map_container_iterator.Find(type) >= 0) {
		LLOG("# drx map nasty iterator");
		typeset.Clear();
		ExpressionType(false, tparam[1], ii, variable, can_shortcut_operator, visited_bases, lvl + 1);
		scan_counter = MAX_COUNT;
		return;
	}
	if(tparam.GetCount() > 1 && upp_map_container_key_iterator.Find(type) >= 0) {
		LLOG("# drx map nasty key iterator");
		typeset.Clear();
		ExpressionType(false, tparam[0], ii, variable, can_shortcut_operator, visited_bases, lvl + 1);
		scan_counter = MAX_COUNT;
		return;
	}

	if(codebase.namespaces.Find(ttype) < 0 && ttype.GetCount()) // do not search for namespace typedefs
		for(i32 i = 0; i < n.GetCount(); i++)
			if(n[i].kind == TYPEDEF) {
				LLOG("typedef -> " << n[i].qtype);
				ExpressionType(false, AddTParams(ResolveTParam(codebase, n[i].qtype, tparam), ttype), ii, variable, can_shortcut_operator, visited_bases, lvl + 1);
				return;
			}
	if(ii >= xp.GetCount()) {
		LLOG("==== Final type: " << ttype);
		typeset.FindAdd(ttype);
		return;
	}
	Txt id = xp[ii];

	// More nasty tricks to make things work with containers (hopefully temporary)
	if((id == "begin" || id == "end") && std_container.Find(type) >= 0) {
		LLOG("# nasty begin/end");
		typeset.Clear();
		ExpressionType(false, ttype + "::iterator", ii + 1, variable, can_shortcut_operator, visited_bases, lvl + 1);
		scan_counter = MAX_COUNT;
		return;
	}
	if(findarg(id, "begin", "end", "Begin", "End") >= 0 &&
	   (upp_map_container.Find(type) >= 0 || upp_container.Find(type) >= 0)) {
		LLOG("# nasty drx begin/end");
		typeset.Clear();
		ExpressionType(false, ttype + "::Iterator", ii + 1, variable, can_shortcut_operator, visited_bases, lvl + 1);
		scan_counter = MAX_COUNT;
		return;
	}
	if(findarg(id, "KeyBegin", "KeyEnd") >= 0 && upp_map_container.Find(type) >= 0) {
		LLOG("# nasty drx KeyBegin/KeyEnd");
		typeset.Clear();
		ExpressionType(false, ttype + "::KeyIterator", ii + 1, variable, can_shortcut_operator, visited_bases, lvl + 1);
		scan_counter = MAX_COUNT;
		return;
	}
	
	i32 q = id.ReverseFind(':');
	if(q > 0 && id[q - 1] == ':') {
		type = ResolveTParam(codebase, Qualify(codebase, ttype, id.Mid(0, q - 1), usings), tparam);
		id = id.Mid(q + 1);
	}
	if(id.Find('<') >= 0) // as in Single<Display>
		id = ParseTemplatedType(id, tparam);
	LLOG("ExpressionType " << type << " ii: " << ii << " id:" << id << " variable:" << variable);
	
	for(i32 i = 0; i < tparam.GetCount(); i++) // need to qualify template parameters
		tparam[i] = Qualify(codebase, context_type, tparam[i], usings);

	bool shortcut_oper = false;
	if(!iscid(*id) && *id != '.' && !isptr) {
		shortcut_oper = can_shortcut_operator;
		id = "operator" + id;
		LLOG("id as: " << id);
	}
	if(*id == '.' || (!variable && !iscid(*id))) {
		LLOG(". " << ttype);
		ExpressionType(isptr, ttype, ii + 1, false, lvl + 1);
		return;
	}
	LDUMP(id);
	for(i32 i = 0; i < n.GetCount(); i++) {
		const CppItem& m = n[i];
		if(m.name == id) {
			Txt t = AddTParams(ResolveReturnType(m, tparam), ttype);
			bool skipfnpars = m.IsCode() && ii + 1 < xp.GetCount() && xp[ii + 1] == "()";
			ExpressionType(m.isptr, ResolveTParam(codebase, t, tparam), ii + skipfnpars + 1,
			               m.IsData() && !m.isptr, lvl + 1);
		}
	}
	
	if(typeset.GetCount() == c0 && type.GetCount()) { // try to find id in type - Class::Method case
		Txt type2 = ParseTemplatedType(type, tparam);
		const Array<CppItem>& n = GetTypeItems(codebase, type2);
		for(i32 i = 0; i < n.GetCount(); i++) {
			const CppItem& m = n[i];
			if(m.name == id) {
				Txt t = AddTParams(ResolveReturnType(m, tparam), type);
				bool skipfnpars = m.IsCode() && ii + 1 < xp.GetCount() && xp[ii + 1] == "()";
				ExpressionType(m.isptr, ResolveTParam(codebase, t, tparam), ii + skipfnpars + 1,
				               m.IsData() && !m.isptr, lvl + 1);
			}
		}
	}

	if(typeset.GetCount() != c0 || IsNull(type))
		return;
	Vec<Txt> base = GetTypeBases(type);
	LDUMPC(base);
	ResolveTParam(codebase, base, tparam);
	LDUMPC(base);
	for(i32 i = 0; i < base.GetCount(); i++)
		if(visited_bases.Find(base[i]) < 0) {
			visited_bases.Add(base[i]);
			ExpressionType(isptr, base[i], ii, variable, false, visited_bases, lvl + 1);
			if(typeset.GetCount() != c0)
				return;
		}

	if(shortcut_oper) {
		LLOG("Shortcut " << xp[ii] << ", ttype " << ttype);
		ExpressionType(false, ttype, ii + 1, false, lvl + 1);
	}
}

void ExpressionTyper::ExpressionType(bool isptr, const Txt& ttype, i32 ii, bool variable, i32 lvl)
{
	Index<Txt> bases;
	ExpressionType(isptr, ttype, ii, false, true, bases, 0);
}

Index<Txt> ExpressionTyper::ExpressionType()
{
	LLOG("------------------------------------------------------");
	LLOG("**** ExpressionType " << xp);
	Txt type;
	if(xp.GetCount() == 0)
		return pick(typeset);
	if(xp[0] == "this") {
		LLOG("this: " << type);
		ExpressionType(false, context_type, 1, false, 0);
		return pick(typeset);
	}
	i32 q = parser.local.FindLast(xp[0]);
	if(q >= 0) {
		Txt type = Qualify(codebase, context_type, parser.local[q].type, parser.context.namespace_using);
		LLOG("Found type local: " << type << " in scope: " << context_type << ", isptr: " << parser.local[q].isptr);
		ExpressionType(parser.local[q].isptr, type, 1, !parser.local[q].isptr, 0);
		return pick(typeset);
	}
	if(context_type.GetCount()) {
		ExpressionType(false, context_type, 0, false, 0);
		if(typeset.GetCount())
			return pick(typeset);
	}
	if(xp.GetCount() >= 2 && xp[1] == "()") {
		Vec<Txt> usings = Split(parser.context.namespace_using, ';');
		usings.Add("");
		for(i32 i = 0; i < usings.GetCount(); i++) {
			Txt qtype = Qualify(codebase, context_type, Merge("::", usings[i], xp[0]), parser.context.namespace_using);
			Vec<Txt> tparam;
			if(codebase.Find(ParseTemplatedType(qtype, tparam)) >= 0) {
				LLOG("Is constructor " << qtype);
				ExpressionType(false, qtype, 2, false, 0);
			}
		}
		if(typeset.GetCount())
			return pick(typeset);
	}
	Vec<Txt> ns = parser.GetNamespaces();
	for(i32 i = 0; i < ns.GetCount(); i++)
		ExpressionType(false, ns[i], 0, false, 0);
	return pick(typeset);
}

Index<Txt> GetExpressionType(const CppBase& codebase, const ParserContext& parser, const Vec<Txt>& xp)
{ // xp is a list of meaningful parts like "foo", "." , "Fn", "()", "->", "m", "[]"
	return ExpressionTyper(codebase, parser, xp).ExpressionType();
}

void SkipPars(CParser& p)
{
	i32 lvl = 1;
	while(lvl && !p.IsEof())
		if(p.Char('(') || p.Char('['))
			lvl++;
		else
		if(p.Char(')') || p.Char(']'))
			lvl--;
		else
			p.SkipTerm();
}

Vec<Txt> MakeXP(tukk s)
{
	Vec<Txt> xp;
	try {
		CParser p(s);
		while(!p.IsChar2(':', ':') && !p.IsId() && !p.IsEof())
			p.SkipTerm();
		while(!p.IsEof()) {
			Txt id;
			if(p.IsChar2(':', ':') || p.IsId()) {
				for(;;)
					if(p.Char2(':', ':'))
						id.Cat("::");
					else
					if(p.IsId())
						id.Cat(p.ReadId());
					else
						break;
				tukk s = p.GetPtr();
				if(p.Char('<')) {
					i32 lvl = 1;
					while(lvl && !p.IsEof()) {
						if(p.Char('<'))
							lvl++;
						else
						if(p.Char('>'))
							lvl--;
						else
							p.SkipTerm();
					}
					while(s < p.GetPtr()) {
						if((u8)*s > ' ')
							id.Cat(*s);
						s++;
					}
				}
				xp.Add(id);
			}
			else
			if(p.Char('(')) {
				xp.Add("()");
				SkipPars(p);
			}
			else
			if(p.Char('[')) {
				xp.Add("[]");
				SkipPars(p);
			}
			else
			if(p.Char2('-', '>'))
				xp.Add("->");
			else
			if(p.Char('.'))
				xp.Add(".");
			else
				break;
		}
	}
	catch(CParser::Error) {}
	return xp;
}

};