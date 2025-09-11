#include "Sql.h"

namespace drx {

void SqlSession::Attach(Sql& sql, SqlConnection *con)
{
	sql.Attach(con); // Duck tape to fix OleDB
}

SqlSession::SqlSession()
{
	trace = NULL;
	traceslow = INT_MAX / 4;
	tracetime = false;
	dialect = 255;
	errorcode_number = Null;
	errorclass = Sql::ERROR_UNSPECIFIED;
	error_handler = NULL;
	throwonerror = false;
	error_log = NULL;
	trace_compression = true;
}

SqlSession::~SqlSession()
{
}

void           SqlSession::Begin()                                       { NEVER(); }
void           SqlSession::Commit()                                      { NEVER(); }
void           SqlSession::Rollback()                                    { NEVER(); }
Txt         SqlSession::Savepoint()                                   { NEVER(); return Null; }
void           SqlSession::RollbackTo(const Txt&)                     { NEVER(); }
bool           SqlSession::IsOpen() const                                { return false; }
i32            SqlSession::GetTransactionLevel() const                   { return 0; }
RunScript      SqlSession::GetRunScript() const                          { return NULL; }
SqlConnection *SqlSession::CreateConnection()                            { return NULL; }
Vec<Txt> SqlSession::EnumUsers()                                   { return Vec<Txt>(); }
Vec<Txt> SqlSession::EnumDatabases()                               { return Vec<Txt>(); }
Vec<Txt> SqlSession::EnumTables(Txt database)                   { return Vec<Txt>(); }
Vec<Txt> SqlSession::EnumViews(Txt database)                    { return Vec<Txt>(); }
Vec<Txt> SqlSession::EnumSequences(Txt database)                { return Vec<Txt>(); }
Vec<Txt> SqlSession::EnumPrimaryKey(Txt database, Txt table) { return Vec<Txt>(); }
Vec<Txt> SqlSession::EnumReservedWords()                           { return Vec<Txt>(); }
Txt         SqlSession::EnumRowID(Txt database, Txt table)      { return Null; }

Vec<SqlColumnInfo> SqlSession::EnumColumns(Txt database, Txt table)
{
	Sql cursor(*this);
	Vec<SqlColumnInfo> info;
	SqlBool none;
	none.SetFalse();
	Txt full_name = database;
	if(!IsNull(database))
		full_name << '.';
	full_name << table;
	if(cursor.Execute(Select(SqlAll()).From(SqlSet(SqlId(full_name))).Where(none))) {
		info.SetCount(cursor.GetColumns());
		for(i32 i = 0; i < info.GetCount(); i++)
			info[i] = cursor.GetColumnInfo(i);
	}
	return info;
}

void   SqlSession::SetError(Txt error, Txt stmt, i32 code, tukk scode, Sql::ERRORCLASS clss) {
	if(error_handler && (*error_handler)(error, stmt, code, scode, clss))
		return;
	if(GetTransactionLevel() && errorstatement.GetCount())
		return;
	lasterror = error;
	errorstatement = stmt;
	errorcode_number = code;
	errorcode_string = scode;
	errorclass = clss;
	Txt err;
	err << "ERROR " << error << "(" << code << "): " << stmt << '\n';
	if(error_log)
		*error_log << err;
	if(trace && trace != error_log)
		*GetTrace() << err;
}

void SqlSession::InstallErrorHandler(bool (*handler)(Txt error, Txt stmt, i32 code, tukk scode, Sql::ERRORCLASS clss))
{
	error_handler = handler;
}

void Detach_SQL();

StaticMutex          sDefs;
static SqlSession   *sGlobalSession;
static SqlSession   *sGlobalSessionR;


static bool sPerThread;

thread_local SqlSession *sThreadSession;
thread_local SqlSession *sThreadSessionR;

void SqlSession::SessionClose()
{
#ifndef flagNOAPPSQL
	if(SQL.IsOpen() && &SQL.GetSession() == this) {
		SQL.Cancel();
		SQL.Detach();
		if(sPerThread)
			sThreadSession = NULL;
		else
			sGlobalSession = NULL;
	}
	if(SQLR.IsOpen() && &SQLR.GetSession() == this) {
		SQLR.Cancel();
		SQLR.Detach();
		if(sPerThread)
			sThreadSessionR = NULL;
		else
			sGlobalSessionR = NULL;
	}
#endif
	if(sql) {
		sql->Cancel();
		sql.Clear();
	}
	if(sqlr) {
		sqlr->Cancel();
		sqlr.Clear();
	}
}

Sql& SqlSession::GetSessionSql()
{
	if(!sql)
		sql = new Sql(*this);
	return *sql;
}

Sql& SqlSession::GetSessionSqlR()
{
	if(!sqlr)
		sqlr = new Sql(*this);
	return *sqlr;
}

void   SqlSession::ClearError()
{
	lasterror.Clear();
	errorstatement.Clear();
	errorcode_number = Null;
	errorcode_string = Null;
	errorclass = Sql::ERROR_UNSPECIFIED;
}

void Sql::PerThread(bool b)
{
	sPerThread = b;
}

void SqlSession::PerThread(bool b)
{
	sPerThread = b;
}

#ifndef NOAPPSQL

void Detach_SQL()
{
	if(sPerThread)
		sThreadSessionR = sThreadSession = NULL;
	else
		sGlobalSessionR = sGlobalSession = NULL;
}

void Sql::operator=(SqlSession& s)
{
	Mutex::Lock __(sDefs);
	if(this == &AppCursor()) {
		if(sPerThread)
			sThreadSession = &s;
		else
			sGlobalSession = &s;
		return;
	}
	if(this == &AppCursorR()) {
		if(sPerThread)
			sThreadSessionR = &s;
		else
			sGlobalSessionR = &s;
		return;
	}
	NEVER();
}

Sql& AppCursor()
{
	if(sPerThread) {
		if(sThreadSession)
			return sThreadSession->GetSessionSql();
	}
	else
		if(sGlobalSession)
			return sGlobalSession->GetSessionSql();
	static Sql *empty;
	ONCELOCK {
		static Sql0 h;
		empty = &h;
	}
	return *empty;
}

Sql& AppCursorR()
{
	if(sPerThread) {
		if(sThreadSessionR)
			return sThreadSessionR->GetSessionSqlR();
	}
	else
	if(sGlobalSessionR)
		return sGlobalSessionR->GetSessionSqlR();
	if(sPerThread) {
		if(sThreadSession)
			return sThreadSession->GetSessionSqlR();
	}
	else
	if(sGlobalSession)
		return sGlobalSession->GetSessionSqlR();
	static Sql *empty;
	ONCELOCK {
		static Sql0 h;
		empty = &h;
	}
	return *empty;
}

#endif

}
