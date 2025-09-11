#include "Sql.h"

namespace drx {

struct SchTableInfo {
	Vec<Txt> column;
	Vec<Txt> ref_table;
	Vec<Txt> ref_column;
	Txt         primary_key;
	Txt         prefix;
	bool           is_table; // It can also be TYPE, which is not in DB
	
	SchTableInfo() { is_table = false; }
};

ArrayMap<Txt, SchTableInfo>& sSchTableInfo()
{
	static ArrayMap<Txt, SchTableInfo> x;
	return x;
}

static bool sColumn;

void SchDbInfoTable(tukk table)
{
	sSchTableInfo().Add(table).is_table = true;
	sColumn = false;
}

void SchDbInfoType(tukk table)
{
	sSchTableInfo().Add(table).is_table = false;
	sColumn = false;
}

void SchDbInfoColumn(tukk name)
{
	SchTableInfo& f = sSchTableInfo().Top();
	f.column.Add(ToUpper(f.prefix + name));
	f.ref_table.Add();
	f.ref_column.Add();
	if(IsNull(f.primary_key))
		f.primary_key = name;
	sColumn = true;
}

void SchDbInfoVar(void (*fn)(), tukk name)
{
	SchTableInfo& f = sSchTableInfo().Top();
	Txt h = f.prefix;
	f.prefix << name << '$';
	(*fn)();
	f.prefix = h;
}

void SchDbInfoReferences(tukk table)
{
	sSchTableInfo().Top().ref_table.Top() = table;
}

void SchDbInfoReferences(tukk table, tukk column)
{
	SchDbInfoReferences(table);
	sSchTableInfo().Top().ref_column.Top() = column;
}

void SchDbInfoPrimaryKey()
{
	SchTableInfo& f = sSchTableInfo().Top();
	f.primary_key = f.column.Top();
}

void SchDbInfoColumnArray(tukk name, i32 items)
{
	for(i32 i = 0; i < items; i++)
		SchDbInfoColumn(Txt().Cat() << name << i);
}

const SchTableInfo& GetSchTableInfo(const Txt& table)
{
	static SchTableInfo sSchTableInfoZero;
	return sSchTableInfo().Get(~table, sSchTableInfoZero);
}

bool MatchRefName(const Txt& fk, const Txt& pk, const Txt& tab, i32 phase)
{
	return phase || fk == pk || tab + '_' + pk == fk;
}

SqlBool Join(const Txt& tab1, const Txt& as1, const Txt& tab2, const Txt& as2, i32 phase)
{
	const SchTableInfo& t1 = GetSchTableInfo(tab1);
	const SchTableInfo& t2 = GetSchTableInfo(tab2);
	for(i32 i = 0; i < t1.ref_table.GetCount(); i++)
		if(t1.ref_table[i] == tab2 && MatchRefName(t1.column[i], t2.primary_key, tab2, phase))
			return SqlId(t1.column[i]).Of(SqlId(as1)) == SqlId(t2.primary_key).Of(SqlId(as2));
	for(i32 i = 0; i < t2.ref_table.GetCount(); i++)
		if(t2.ref_table[i] == tab1 && MatchRefName(t2.column[i], t1.primary_key, tab1, phase))
			return SqlId(t2.column[i]).Of(SqlId(as2)) == SqlId(t1.primary_key).Of(SqlId(as1));
	return SqlBool::False();
}

i32 sChrf(i32 c)
{
	return c < 32 ? ' ' : c;
}

StaticMutex sM;

ArrayMap<Txt, Array<SchColumnInfo>> GetSchAll()
{
	ArrayMap<Txt, Array<SchColumnInfo>> r;
	Mutex::Lock __(sM);
	for(i32 i = 0; i < sSchTableInfo().GetCount(); i++)
		if(sSchTableInfo()[i].is_table) {
			Array<SchColumnInfo>& cf = r.Add(sSchTableInfo().GetKey(i));
			const SchTableInfo& h = sSchTableInfo()[i];
			for(i32 j = 0; j < h.column.GetCount(); j++) {
				SchColumnInfo& f = cf.Add();
				f.name = h.column[j];
				f.references = h.ref_table[j];
				f.foreign_key = Nvl(h.ref_column[j], f.name);
			}
		}
	return r;
}

Vec<Txt> GetSchTables()
{
	Mutex::Lock __(sM);
	Vec<Txt> r;
	for(i32 i = 0; i < sSchTableInfo().GetCount(); i++)
		if(sSchTableInfo()[i].is_table)
			r.Add(sSchTableInfo().GetKey(i));
	return r;
}

Vec<Txt> GetSchColumns(const Txt& table)
{
	Mutex::Lock __(sM);
	return clone(GetSchTableInfo(table).column);
}

SqlBool FindSchJoin(const Txt& tables)
{
	Mutex::Lock __(sM);
	static VecMap<Txt, SqlBool> cache;
	if(cache.GetCount() > 20000) // Defend against unlikely dynamic Join permutations
		cache.Clear();
	i32 q = cache.Find(tables);
	if(q >= 0)
		return cache[q];
	Vec<Txt> s = Split(Filter(tables, sChrf), ',');
	Vec<Txt> as;
	Vec<Txt> table;
	for(i32 i = 0; i < s.GetCount(); i++) {
		Vec<Txt> ss = Split(s[i], ' ');
		if(ss.GetCount()) {
			table.Add(ss[0]);
			as.Add(ss.Top());
		}
	}
	if(table.GetCount() >= 2) {
		Txt tab1 = table.Top();
		Txt as1 = as.Top();
		for(i32 phase = 0; phase < 2; phase++)
			for(i32 i = 0; i < table.GetCount() - 1; i++) {
				SqlBool b = Join(tab1, as1, table[i], as[i], phase);
				if(!b.IsFalse()) {
					cache.Add(tables, b);
					return b;
				}
			}
	}
	NEVER_("Schema join not found " + tables);
	return SqlBool::False();
}

}
