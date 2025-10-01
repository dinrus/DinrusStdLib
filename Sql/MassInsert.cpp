#include "Sql.h"

namespace drx {

SqlMassInsert::~SqlMassInsert()
{
	Flush();
}

SqlMassInsert& SqlMassInsert::operator()(SqlId col, const Val& val)
{
	if(pos == 0) {
		cache.Add().nulls = 0;
		cache.Top();
	}
	if(cache.GetCount() == 1)
		column.Add(~col);
	else
		ASSERT(column[pos] == col.Quoted() || column[pos] == ~col);
	Row& r = cache.Top();
	r.value.Add(val);
	if(IsNull(val))
		r.nulls |= ((zu64)1 << pos);
	pos++;
	ASSERT(pos < 62);
	return *this;
}

SqlMassInsert& SqlMassInsert::operator()(const ValMap& data)
{
	for(i32 i = 0; i < data.GetCount(); i++)
		operator()((Txt)data.GetKey(i), data.GetVal(i));
	return *this;
}

SqlMassInsert& SqlMassInsert::EndRow(SqlBool remove)
{
	cache.Top().remove = remove;
	if(pos == 0)
		return *this;
	if(cache.GetCount() && cache[0].value.GetCount() * cache.GetCount() > 5000 || cache.GetCount() > 990) // MSSQL maximum is 1000
		Flush();
	ASSERT(column.GetCount() == pos);
	pos = 0;
	return *this;
}

void SqlMassInsert::Flush()
{
	zu64k DONE = (zu64)-1;
	if(cache.GetCount() == 0)
		return;
	if(use_transaction)
		sql.GetSession().Begin();
	SqlBool remove;
	bool doremove = false;
	for(i32 ii = 0; ii < cache.GetCount(); ii++) {
		SqlBool rm = cache[ii].remove;
		if(!rm.IsEmpty()) {
			doremove = true;
			remove = remove || rm;
		}
	}
	if(doremove)
		sql * Delete(table).Where(remove);
	Txt insert;
	i32 dialect = sql.GetDialect();
	if(findarg(dialect, MY_SQL, PGSQL, MSSQL) >= 0) {
		insert << "insert into " + ~table + '(';
		for(i32 i = 0; i < column.GetCount(); i++) {
			if(i)
				insert << ", ";
			insert << column[i];
		}
		insert << ") values ";
		for(i32 i = 0; i < cache.GetCount(); i++) {
			Row& r = cache[i];
			if(r.value.GetCount()) {
				if(i)
					insert << ", ";
				insert << "(";
				for(i32 i = 0; i < r.value.GetCount(); i++) {
					if(i)
						insert << ", ";
					insert << SqlCompile(dialect, SqlFormat(r.value[i]));
				}
				insert << ")";
			}
		}
	}
	else
	for(i32 ii = 0; ii < cache.GetCount(); ii++) {
		zu64 nulls = cache[ii].nulls;
		if(nulls != DONE) {
			insert << "insert into " + ~table + '(';
			bool nextcol = false;
			for(i32 i = 0; i < column.GetCount(); i++) {
				if(!(nulls & ((zu64)1 << i))) {
					if(nextcol)
						insert << ", ";
					nextcol = true;
					insert << column[i];
				}
			}
			insert << ')';
			bool nextsel = false;
			for(i32 i = ii; i < cache.GetCount(); i++) {
				Row& r = cache[i];
				if(r.nulls == nulls && r.value.GetCount()) {
					r.nulls = DONE;
					if(nextsel)
						insert << " union all";
					nextsel = true;
					insert << " select ";
					bool nextval = false;
					for(i32 i = 0; i < r.value.GetCount(); i++)
						if(!(nulls & ((zu64)1 << i))) {
							if(nextval)
								insert << ", ";
							nextval = true;
							insert << SqlCompile(dialect, SqlFormat(r.value[i]));
						}
					if(dialect == ORACLE)
						insert << " from dual";
				}
			}
		}
	}
	sql.Execute(insert);
	if(sql.WasError()) {
		error = true;
		if(use_transaction)
			sql.GetSession().Rollback();
	}
	else
		if(use_transaction)
			sql.GetSession().Commit();
	cache.Clear();
	column.Clear();
	pos = 0;
}

}

