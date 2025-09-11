#include "CppBase.h"
#include "Internal.h"

namespace drx {

#define LLOG(x)     // DLOG(x)
#define LTIMING(x)  // DTIMING(x)

bool DoQualify(ScopeInfo& nf, const Txt& type, const Txt& usings, Txt& qt);

bool Qualify0(ScopeInfo& nf, const Txt& type, const Txt& usings, Txt& qt)
{ // Qualify single type based on scoping information
	const Vec<Txt>& nd = nf.GetScopes(usings);
	if(nd.GetCount()) {
		LTIMING("First test");
		qt = nd[0] + type;
		if(nf.base.Find(qt) >= 0)
			return true;
	}
	if(nf.GetScope() >= 0) {
		i32 q = type.ReverseFind(':');
		if(q > 0) {
			LTIMING("Qualifying qualification");
			ScopeInfo hnf(nf);
			hnf.NoBases();
			Txt qn;
			Txt qs = type.Mid(0, q - 1);
			if(IsDigit(*qs)) {
				qt = type;
				return true;
			}
			if(DoQualify(hnf, qs, usings, qn)) {
				Txt tp = type.Mid(q + 1);
				if(nf.base.Find(qn) >= 0) {
					qt = qn + "::" + tp;
					return true;
				}
				i32 scopei = nf.base.Find(qn);
				if(scopei >= 0) {
					ScopeInfo nnf(nf.base, scopei);
					const Vec<Txt>& bs = nnf.GetBases();
					for(i32 i = 0; i < bs.GetCount(); i++) {
						qt = bs[i] + tp;
						if(nf.base.Find(qt) >= 0)
							return true;
					}
				}
			}
			if(nf.base.Find(qs) >= 0) {
				qt = qs;
				return true;
			}
			qt = type;
			if(nf.base.Find(qt) >= 0) // e.g. std::string
				return true;
			qt = type.Mid(q + 1);
			return true;
		}
		else {
			LTIMING("Bases");
			const Vec<Txt>& bs = nf.GetBases();
			for(i32 i = 0; i < bs.GetCount(); i++) {
				qt = bs[i] + type;
				if(nf.base.Find(qt) >= 0)
					return true;
			}
		}
	}
	if(type[0] != ':') {
		LTIMING("Testing scopes");
		for(i32 i = 1; i < nd.GetCount(); i++) {
			qt = nd[i] + type;
			if(nf.base.Find(qt) >= 0)
				return true;
		}
	}
	i32 q = type.Find(':');
	if(q < 0)
		return false;
	return Qualify0(nf, type.Mid(q + 1), usings, qt);
}

bool DoQualify(ScopeInfo& nf, const Txt& type, const Txt& usings, Txt& qt)
{
	LTIMING("Qualify");
	i32 q = nf.cache.Find(type);
	if(q >= 0) {
		qt = nf.cache[q];
		return true;
	}
	LTIMING("Qualify0");
	if(!Qualify0(nf, type, usings, qt))
		return false;
	nf.cache.Add(type, qt);
	return true;
}

Txt DoQualify(ScopeInfo& nf, const Txt& type, const Txt& usings)
{
	Txt qt;
	return DoQualify(nf, type, usings, qt) ? qt : type;
}

static Txt s_int("i32");
static Txt s_void("void");
static Txt s_double("double");
static Txt s_char("char");
static Txt s_float("float");
static Txt s_long("long");
static Txt s_const("const");
static Txt s_struct("struct");
static Txt s_class("class");
static Txt s_unsigned("unsigned");

static Txt s_dint("цел");
static Txt s_dvoid("проц");
static Txt s_ddouble("дво");
static Txt s_dchar("сим");
static Txt s_dfloat("плав");
static Txt s_dlong("дол");
//static Txt s_dconst("const");
//static Txt s_dstruct("struct");
//static Txt s_dclass("class");
//static Txt s_dunsigned("бцел");

inline void Qualify(Txt& r, ScopeInfo& nf, tukk b, tukk s, const Txt& usings)
{
	Txt type(b, s);
	if(type.GetCount() == 0 || type == s_const ||
	   type == s_int || type == s_double || type == s_char ||
	   type == s_long || type == s_unsigned || type == s_struct || type == s_class ||
	   type == s_float || type == s_dint || type == s_dlong || type == s_dvoid || type == s_ddouble
	   || type == s_dchar || type == s_dfloat)
	    {
		r << type;
		return;
	}
	r << DoQualify(nf, type, usings);
}

Txt QualifyIds(ScopeInfo& nf, const Txt& k, const Txt& usings, bool all)
{
	LTIMING("QualifyIds");
	Txt r;
	tukk s = k;
	Vec<Txt> empty;
	while(*s) {
		i32 c = *s;
		if(c == ':') {
			tukk b = s++;
			while(*s == ':' || iscid(*s)) s++;
/*			if(all) {
				if(iscid(*r.Last()))
					r << ' ';
				ScopeInfo nnf(nf.GetScope(), nf.base);
				Qualify(r, nnf, b, s, usings);
			}
			else*/
				r.Cat(b, s);
		}
		else
		if(iscid(c)) {
			if(iscid(*r.Last()))
				r << ' ';
			if(s[0] == 'c' && s[1] == 'o' && s[2] == 'n' && s[3] == 's' && s[4] == 't' && !iscid(s[5])) {
				r << s_const;
				s += 5;
			}
			else
			if(s[0] == 'm' && s[1] == 'u' && s[2] == 't' && s[3] == 'a' && s[4] == 'b' && s[5] == 'l' && s[6] == 'e' && !iscid(s[7])) {
				r << "mutable";
				s += 7;
			}
			else
			if(s[0] == 'v' && s[1] == 'o' && s[2] == 'l' && s[3] == 'a' && s[4] == 't' && s[5] == 'i' && s[6] == 'l' && s[7] == 'e' && !iscid(s[8])) {
				r << "volatile";
				s += 8;
			}
			else {
				tukk b = s++;
				while(*s == ':' || iscid(*s)) s++;
				if(all)
					Qualify(r, nf, b, s, usings);
				else
					r.Cat(b, s);
			}
		}
		else {
			if(c == '(')
				all = true;
			if(c != ' ')
				r.Cat(c);
			s++;
		}
	}
	return r;
}

Txt Qualify(const CppBase& base, const Txt& scope, const Txt& type, const Txt& usings)
{
	if(type.StartsWith("::"))
		return type.Mid(2);
	ScopeInfo nf(base, scope);
	return QualifyIds(nf, type, usings, true);
}

Txt QualifyKey(const CppBase& base, const Txt& scope, const Txt& type, const Txt& usings)
{
	ScopeInfo nf(base, scope);
	return QualifyIds(nf, type, usings, false);
}

void QualifyTypes(CppBase& base, const Txt& scope, CppItem& m)
{
	ScopeInfo nf(base, scope);
	m.qtype = QualifyIds(nf, m.type, m.using_namespaces, true);
	m.qptype = QualifyIds(nf, m.ptype, m.using_namespaces, true);
}

void QualifyPass0(CppBase& base)
{ // Move scopes - solve: namespace X { struct C { void Foo(); }; using namespace X; void C::Foo() {}
	Vec<i32> remove_scope;
	for(i32 ni = 0; ni < base.GetCount(); ni++) {
		Array<CppItem>& n = base[ni];
		ScopeInfo nf(base);
		Txt scope = base.GetKey(ni);
		Txt usings;
		i32    toscopei = -1;
		Vec<i32> remove_item;
		for(i32 i = 0; i < n.GetCount(); i++) {
			CppItem& m = n[i];
			if(m.qualify && m.impl) {
				if(usings != m.using_namespaces) {
					usings = m.using_namespaces;
					toscopei = -1;
					Vec<Txt> h = Split(m.using_namespaces, ';');
					for(i32 i = 0; i < h.GetCount(); i++) {
						Txt ns = h[i] + "::" + scope;
						toscopei = base.Find(ns);
						if(toscopei >= 0)
							break;
					}
				}
				if(toscopei >= 0 && toscopei != ni) {
					base[toscopei].Add(m);
					remove_item.Add(i);
				}
			}
		}
		n.Remove(remove_item);
		if(scope.GetCount() && n.GetCount() == 0)
			remove_scope.Add(ni);
	}
	base.Remove(remove_scope);
}

void QualifyPass1(CppBase& base)
{ // Qualify types
	LTIMING("QualifyPass1");
	for(i32 ni = 0; ni < base.GetCount(); ni++) {
		Array<CppItem>& n = base[ni];
		ScopeInfo nf(base, ni);
		for(i32 i = 0; i < n.GetCount(); i++) {
			CppItem& m = n[i];
			if(m.kind == NAMESPACE)
				base.namespaces.FindAdd(base.GetKey(ni));
			if(m.IsType() && m.qualify) {
				m.qualify = false;
				m.qtype = QualifyIds(nf, m.type, m.using_namespaces, true); // type of item, now qualified (probaly already was)
				m.qptype = QualifyIds(nf, m.ptype, m.using_namespaces, true); // base classes
				m.qitem = m.item; // name of type (struct) item is already qualified
			}
		}
	}
}

void QualifyPass2(CppBase& base)
{ // Qualify function parameters
	LTIMING("QualifyPass2");
	for(i32 ni = 0; ni < base.GetCount(); ni++) {
		Array<CppItem>& n = base[ni];
		ScopeInfo nf(base, ni);
		for(i32 i = 0; i < n.GetCount(); i++) {
			CppItem& m = n[i];
			if(m.uname.GetCount() == 0 && m.name.GetCount())
				m.uname = ToUpper(m.name);
			if(!m.IsType() && m.qualify) {
				m.qualify = false;
				m.qtype = QualifyIds(nf, m.type, m.using_namespaces, true);
				m.qptype = QualifyIds(nf, m.ptype, m.using_namespaces, true);
				m.qitem = m.IsCode() ? QualifyIds(nf, m.item, m.using_namespaces, false)
				                     : m.item;
			}
		}
	}
}

void Qualify(CppBase& base)
{
	Md5Stream md5;
	Vec<i32> no = GetSortOrder(base.GetKeys());
	for(i32 q = 0; q < base.GetCount(); q++) {
		i32 ni = no[q];
		md5 << base.GetKey(ni);
		const Array<CppItem>& n = base[ni];
		for(i32 i = 0; i < n.GetCount(); i++) {
			const CppItem& m = n[i];
			if(m.IsType())
				md5 << ';' << m.ptype;
		}
		md5 << '\n';
	}
	Txt c5 = md5.FinishTxt();
	if(c5 != base.types_md5) { // set of types changed, requalify everything
		base.types_md5 = c5;
		for(i32 ni = 0; ni < base.GetCount(); ni++) {
			Array<CppItem>& n = base[ni];
			for(i32 i = 0; i < n.GetCount(); i++)
				n[i].qualify = true;
		}
	}
	base.namespaces.Clear();
	QualifyPass0(base);
	QualifyPass1(base);
	QualifyPass2(base);
}

}
