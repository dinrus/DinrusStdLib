#include "Sql.h"

namespace drx {

static Txt sPutId(tukk type, Index<Txt>& id, Txt n, i32 w = 0)
{
	Txt r = type;
	if(id.Find(n) < 0) {
		id.Add(n);
		r << '_';
	}
	r << Txt(' ', max(0, w - r.GetLength())) << '(' << n;
	return r;
}

Txt ExportSch(SqlSession& session, const Txt& database)
{
	Txt r;
	Vec<Txt> tab = session.EnumTables(database);
	Index<Txt> id;
	for(i32 i = 0; i < tab.GetCount(); i++) {
		r << sPutId("TABLE", id, ToUpper(tab[i])) << ")\r\n";
		Vec<SqlColumnInfo> c = session.EnumColumns(database, tab[i]);
		for(i32 i = 0; i < c.GetCount(); i++) {
			Txt type;
			i32    width = Null;
			switch(c[i].type) {
			case INT_V:
				type = "INT";
				break;
			case DOUBLE_V:
				type = "DOUBLE";
				break;
			case DATE_V:
				type = "DATE";
				break;
			case TIME_V:
				type = "TIME";
				break;
			case TXT_V:
				type = "STRING";
				width = c[i].width;
				if(width < 0 || width > 40000)
					width = 2000;
				break;
			default:
				type = "STRING";
				width = 200;
				break;
			}
			r << '\t' << sPutId(type, id, ToUpper(c[i].name), 8);
			if(width > 0 && width < 40000)
				r << ", " << width;
			r << ")\r\n";
		}
		r << "END_TABLE\r\n\r\n";
	}
	return r;
}

#ifndef NOAPPSQL
Txt ExportSch(const Txt& database)
{
	return ExportSch(SQL.GetSession(), database);
}
#endif

static void sId(Txt& r, const Txt& id, Index<Txt>& done)
{
	Txt u = ToUpper(id);
	if(done.Find(u) >= 0)
		return;
	done.Add(u);
	if(u == id)
		r << "SQLID(" << id << ")\r\n";
	else
		r << "SQL_ID(" << u << ", " << id << ")\r\n";
}

Txt ExportIds(SqlSession& session, const Txt& database)
{
	Txt r;
	Vec<Txt> tab = session.EnumTables(database);
	Index<Txt> done;
	for(i32 i = 0; i < tab.GetCount(); i++) {
		sId(r, tab[i], done);
		Vec<SqlColumnInfo> c = session.EnumColumns(database, tab[i]);
		for(i32 i = 0; i < c.GetCount(); i++)
			sId(r, c[i].name, done);
	}
	return r;
}

#ifndef NOAPPSQL
Txt ExportIds(const Txt& database)
{
	return ExportIds(SQL.GetSession(), database);
}
#endif

}
