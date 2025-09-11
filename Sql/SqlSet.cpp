#include "Sql.h"

namespace drx {

SqlSet operator|(const SqlSet& s1, const SqlSet& s2) {
	if(s1.IsEmpty()) return s2;
	if(s2.IsEmpty()) return s1;
	return SqlSet(s1(SqlSet::SET, ~SQLITE3) + " union " + s2(SqlSet::SET, ~SQLITE3), SqlSet::SETOP);
}

SqlSet operator+(const SqlSet& s1, const SqlSet& s2)
{
	if(s1.IsEmpty()) return s2;
	if(s2.IsEmpty()) return s1;
	return SqlSet(s1(SqlSet::SET, ~SQLITE3) + " union all " + s2(SqlSet::SET, ~SQLITE3), SqlSet::SETOP);
}

SqlSet operator&(const SqlSet& s1, const SqlSet& s2) {
	if(s1.IsEmpty()) return s2;
	if(s2.IsEmpty()) return s1;
	return SqlSet(s1(SqlSet::SET, ~SQLITE3) + " intersect " + s2(SqlSet::SET, ~SQLITE3), SqlSet::SETOP);
}

SqlSet operator-(const SqlSet& s1, const SqlSet& s2) {
	if(s1.IsEmpty() || s2.IsEmpty())
		return s1;
	return SqlSet(s1(SqlSet::SET) + SqlCode(MSSQL|PGSQL|SQLITE3, " except ")(" minus ") + s2(SqlSet::SET), SqlSet::SETOP);
}

Txt SqlSet::operator~() const {
	if(IsEmpty()) return "null";
	return text;
}

Txt SqlSet::operator()() const {
	if(IsEmpty()) return "null";
	return '(' + text + ')';
}

Txt SqlSet::operator()(i32 at) const {
	if(IsEmpty()) return "null";
	return at > priority ? '(' + text + ')' : text;
}

Txt SqlSet::operator()(i32 at, u8 cond) const {
	if(IsEmpty()) return "null";
	if(at <= priority)
		return text;
	TxtBuf out;
	out << SqlCode(cond, "(")() << text << SqlCode(cond, ")")();
	return Txt(out);
}

SqlSet& SqlSet::Cat(const SqlVal& val) {
	if(!IsEmpty()) text.Cat(", ");
	text.Cat(~val);
	priority = SET;
	return *this;
}

SqlSet& SqlSet::Cat(const SqlSet& set) {
	if(set.IsEmpty()) return *this;
	if(!IsEmpty()) text.Cat(", ");
	text.Cat(set.text);
	priority = SET;
	return *this;
}

SqlSet::SqlSet(const SqlVal& p0) {
	Cat(p0);
	priority = SET;
}

SqlSet SqlSetFrom(const ValArray& va, i32 pos, i32 count)
{
	SqlSet set;
	for(i32 i = 0; i < count; i++)
		set << va[pos + i];
	return set;
}

SqlSet SqlSetFrom(const ValArray& va)
{
	return SqlSetFrom(va, 0, va.GetCount());
}

static inline void sCat(SqlSet& s, SqlVal v) { s.Cat(v); }

//$-
#define E__Cat(I)       sCat(*this, p##I)

#define E__SqlSetF(I) \
SqlSet::SqlSet(const SqlVal& p0, __List##I(E__SqlVal)) { \
	Cat(p0); \
	__List##I(E__Cat); \
	priority = SET; \
}
__Expand(E__SqlSetF);
//$+

FieldOperator::FieldOperator() {}
FieldOperator::~FieldOperator() {}

struct FieldSqlSet : FieldOperator {
	SqlSet *set;

	virtual void Field(tukk name, Ref) {
		set->Cat(SqlId(name));
	}
};

SqlSet::SqlSet(Fields nfields) {
	FieldSqlSet fo;
	fo.set = this;
	nfields(fo);
	priority = SET;
}

void FieldDumper::Field(tukk name, Ref f)
{
	if(!s.IsEmpty())
		s.Cat(", ");
	s << name << " = " << Val(f);
}

}
