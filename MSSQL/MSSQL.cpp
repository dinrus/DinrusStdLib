#include "MSSQL.h"

namespace drx {

Txt MsSqlTextType(i32 width)
{
	if(width <= 4000)
		return NFormat("varchar(%d)", width);
	return "text";
}

void IdentityInsert(Sql& sql, const SqlInsert& ins)
{
	sql.Execute("SET IDENTITY_INSERT " + ins.GetTable().ToTxt() + " ON;" +
	            ((SqlStatement)ins).Get(MSSQL));
}

void IdentityInsert(const SqlInsert& ins)
{
	IdentityInsert(SQL, ins);
}

Val MsSqlSequence::Get()
{
	ASSERT(seq);
#ifndef NOAPPSQL
	Sql sql(session ? *session : SQL.GetSession());
#else
	ASSERT(session);
	Sql sql(*session);
#endif
	if(!sql.Execute("select next value for " + ~*seq) || !sql.Fetch())
		return ErrorVal();
	return sql[0];
}

MsSqlSequence::MsSqlSequence(tukk seq_name)
{
	INTERLOCKED {
		static ArrayMap<Txt, SqlId> ids;
		seq = &ids.GetAdd(seq_name, SqlId(seq_name));
	};
}

};
