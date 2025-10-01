#include "PostgreSQL.h"

#ifdef TARGET_WIN32
#include "Winsock2.h"
#endif

#ifdef TARGET_POSIX
#include <sys/socket.h>
#endif

#define LLOG(x) // DLOG(x)

#ifndef flagNOPOSTGRESQL

namespace drx {

enum PGSQL_StandardOid {
	PGSQL_BOOLOID = 16,
	PGSQL_BYTEAOID = 17,
	PGSQL_CHAROID = 18,
	PGSQL_NAMEOID = 19,
	PGSQL_INT8OID = 20,
	PGSQL_INT2OID = 21,
	PGSQL_INT2VECTOROID = 22,
	PGSQL_INT4OID = 23,
	PGSQL_REGPROCOID = 24,
	PGSQL_TEXTOID = 25,
	PGSQL_OIDOID = 26,
	PGSQL_TIDOID = 27,
	PGSQL_XIDOID = 28,
	PGSQL_CIDOID = 29,
	PGSQL_OIDVECTOROID = 30,
	PGSQL_FLOAT4OID = 700,
	PGSQL_FLOAT8OID = 701,
	PGSQL_DATEOID = 1082,
	PGSQL_TIMEOID = 1083,
	PGSQL_TIMESTAMPOID = 1114,
	PGSQL_TIMESTAMPZOID = 1184,
	PGSQL_NUMERICOID = 1700
};

i32 OidToType(Oid oid)
{
	switch(oid) {
		case PGSQL_BOOLOID:
			return BOOL_V;
		case PGSQL_INT8OID:
			return INT64_V;
		case PGSQL_INT2OID:
		case PGSQL_INT2VECTOROID:
		case PGSQL_INT4OID:
			return INT_V;
		case PGSQL_FLOAT4OID:
		case PGSQL_FLOAT8OID:
		case PGSQL_NUMERICOID:
			return DOUBLE_V;
		case PGSQL_DATEOID:
			return DATE_V;
		case PGSQL_TIMEOID:
		case PGSQL_TIMESTAMPOID:
		case PGSQL_TIMESTAMPZOID:
			return TIME_V;
		case PGSQL_BYTEAOID:
			return SQLRAW_V;
	}
	return TXT_V;
}

class PostgreSQLConnection : public SqlConnection {
protected:
	virtual void        SetParam(i32 i, const Val& r);
	virtual bool        Execute();
	virtual i32         GetRowsProcessed() const;
	virtual Val       GetInsertedId() const;
	virtual bool        Fetch();
	virtual void        GetColumn(i32 i, Ref f) const;
	virtual void        Cancel();
	virtual SqlSession& GetSession() const;
	virtual Txt      GetUser() const;
	virtual Txt      ToTxt() const;

private:
	PostgreSQLSession& session;

	PGconn         *conn;
	Vec<Txt>  param;
	PGresult       *result;
	Vec<Oid>     oid;
	i32             rows;
	i32             fetched_row; //-1, if not fetched yet
	Txt          last_insert_table;

	void            FreeResult();
	Txt          ErrorMessage();
	Txt          ErrorCode();

	Txt          FromCharset(const Txt& s) const { return session.FromCharset(s); }
	Txt          ToCharset(const Txt& s) const   { return session.ToCharset(s); }

public:
	PostgreSQLConnection(PostgreSQLSession& a_session, PGconn *a_conn);
	virtual ~PostgreSQLConnection() { Cancel(); }
};

tukk PostgreSQLReadTxt(tukk s, Txt& stmt)
{
	//СДЕЛАТЬ: to clear this, currently this is based on sqlite
	stmt.Cat(*s);
	i32 c = *s++;
	for(;;) {
		if(*s == '\0') break;
		else
		if(*s == '\'' && s[1] == '\'') {
			stmt.Cat("\'\'");
			s += 2;
		}
		else
		if(*s == c) {
			stmt.Cat(c);
			s++;
			break;
		}
		else
		if(*s == '\\') {
			stmt.Cat('\\');
			if(*++s)
				stmt.Cat(*s++);
		}
		else
			stmt.Cat(*s++);
	}
	return s;
}

bool PostgreSQLPerformScript(const Txt& txt, StatementExecutor& se, Gate<i32, i32> progress_canceled)
{
	tukk text = txt;
	for(;;) {
		Txt stmt;
		while(*text <= 32 && *text > 0) text++;
		if(*text == '\0') break;
		for(;;) {
			if(*text == '\0')
				break;
			if(*text == ';')
				break;
			else
			if(*text == '\'')
				text = PostgreSQLReadTxt(text, stmt);
			else
			if(*text == '\"')
				text = PostgreSQLReadTxt(text, stmt);
			else
				stmt.Cat(*text++);
		}
		if(progress_canceled(i32(text - txt.Begin()), txt.GetLength()))
			return false;
		if(!se.Execute(stmt))
			return false;
		if(*text) text++;
	}
	return true;
}

Txt PostgreSQLConnection::ErrorMessage()
{
	return FromCharset(PQerrorMessage(conn));
}

Txt PostgreSQLConnection::ErrorCode()
{
	return PQresultErrorField(result, PG_DIAG_SQLSTATE);
}

Txt PostgreSQLSession::ErrorMessage()
{
	return FromCharset(PQerrorMessage(conn));
}

Txt PostgreSQLSession::ErrorCode()
{
	return PQresultErrorField(result, PG_DIAG_SQLSTATE);
}

Vec<Txt> PostgreSQLSession::EnumUsers()
{
	Vec<Txt> vec;
	Sql sql(*this);
	sql.Execute("select rolname from pg_authid where rolcanlogin");
	while(sql.Fetch())
		vec.Add(sql[0]);
	return vec;
}

Vec<Txt> PostgreSQLSession::EnumDatabases()
{// For now, we really enumerate namespaces rather than databases here
	Vec<Txt> vec;
	Sql sql(*this);
	sql.Execute("select nspname from pg_namespace where nspacl is not null");
	while(sql.Fetch())
		vec.Add(sql[0]);
	return vec;
}

Vec<Txt> PostgreSQLSession::EnumData(char type, tukk schema)
{
	Vec<Txt> vec;
	Sql sql(Format("select n.nspname || '.' || c.relname from pg_catalog.pg_class c "
		             "left join pg_catalog.pg_namespace n "
		               "on n.oid = c.relnamespace "
		            "where c.relkind = '%c' "
		              "and n.nspname like '%s' "
		              "and pg_catalog.pg_table_is_visible(c.oid)",
		              type, schema ? schema : "%"), *this);
	sql.Execute();
	while(sql.Fetch())
		vec.Add(sql[0]);
	return vec;
}

Vec<Txt> PostgreSQLSession::EnumTables(Txt database)
{
	return EnumData('r', database);
}

Vec<Txt> PostgreSQLSession::EnumViews(Txt database)
{
	return EnumData('v', database);
}

Vec<Txt> PostgreSQLSession::EnumSequences(Txt database)
{
	return EnumData('S', database);
}

Vec<SqlColumnInfo> PostgreSQLSession::EnumColumns(Txt database, Txt table)
{
	/* database means schema here - support for schemas is a something to fix in sql interface */

	i32 q = table.Find('.');
	if(q) table = table.Mid(q + 1);
	Vec<SqlColumnInfo> vec;
	Sql sql(Format("select a.attname, a.atttypid, a.attlen, a.atttypmod, a.attnotnull "
	                 "from pg_catalog.pg_attribute a "
	                "inner join pg_catalog.pg_class c "
	                   "on a.attrelid = c.oid "
	                "inner join pg_catalog.pg_namespace n "
	                   "on c.relnamespace = n.oid "
	                "where c.relname = '%s' "
	                  "and n.nspname = '%s' "
	                  "and a.attnum > 0 "
	                  "and a.attisdropped = '0' "
	                "order by a.attnum", table, database), *this);
	sql.Execute();
	while(sql.Fetch())
	{
		SqlColumnInfo &ci = vec.Add();
		i32 type_mod = i32(sql[3]) - sizeof(i32);
		ci.name = sql[0];
		ci.type = OidToType(IsTxt(sql[1]) ? atoi(Txt(sql[1])) : (i32)sql[1]);
		ci.width = sql[2];
		if(ci.width < 0)
			ci.width = type_mod;
		ci.precision = (type_mod >> 16) & 0xffff;
		ci.scale = type_mod & 0xffff;
		ci.nullable = AsTxt(sql[4]) == "0";
		ci.binary = false;
	}
	return vec;
}

Vec<Txt> PostgreSQLSession::EnumPrimaryKey(Txt database, Txt table)
{
//	SELECT cc.conname, a.attname
//	 FROM pg_constraint cc
//	 INNER JOIN pg_class c
//	    ON c.oid=conrelid
//	 INNER JOIN pg_attribute a
//	    ON a.attnum = ANY(conkey)
//	   AND a.attrelid = c.oid
//	WHERE contype='p'
//	AND relname = '?'
	return Vec<Txt>(); //СДЕЛАТЬ
}

Txt PostgreSQLSession::EnumRowID(Txt database, Txt table)
{
	return ""; //СДЕЛАТЬ
}

Vec<Txt> PostgreSQLSession::EnumReservedWords()
{
	return Vec<Txt>(); //СДЕЛАТЬ
}

SqlConnection * PostgreSQLSession::CreateConnection()
{
	return new PostgreSQLConnection(*this, conn);
}

void PostgreSQLSession::ExecTrans(tukk  statement)
{
	if(trace)
		*trace << statement << DRX::EOL;

	i32 itry = 0;

	do {
		result = PQexec(conn, statement);
		if(PQresultStatus(result) == PGRES_COMMAND_OK) {
			PQclear(result);
			return;
		}
	}
	while(level == 0 && (!ConnectionOK() || ErrorMessage().Find("connection") >= 0 && itry == 0)
	      && WhenReconnect(itry++));

	if(trace)
		*trace << statement << " failed: " << ErrorMessage() << " (level " << level << ")\n";
	SetError(ErrorMessage(), statement, 0, ErrorCode());
	PQclear(result);
}

Txt PostgreSQLSession::FromCharset(const Txt& s) const
{
	if(!charset)
		return s;
	Txt r = DRX::ToCharset(GetDefaultCharset(), s, charset);
	return r;
}

Txt PostgreSQLSession::ToCharset(const Txt& s) const
{
	if(!charset)
		return s;
	Txt r = DRX::ToCharset(charset, s);
	return r;
}

void PostgreSQLSession::DoKeepAlive()
{
	if(keepalive && conn) {
		i32 optval = 1;
		setsockopt(PQsocket(conn), SOL_SOCKET, SO_KEEPALIVE, (tuk ) &optval, sizeof(optval));
	}
}

bool PostgreSQLSession::Open(tukk connect)
{
	Close();
	conns = connect;

	{
		MemoryIgnoreLeaksBlock __;
		// PGSQL, when sharing .dll SSL, does not free SSL data
		conn = PQconnectdb(connect);
	}

	if(PQstatus(conn) != CONNECTION_OK)
	{
		SetError(FromSystemCharset(PQerrorMessage(conn)), "При открытии базы данных");
		Close();
		return false;
	}
	level = 0;

	if(PQclientEncoding(conn)) {
		if(PQsetClientEncoding(conn, "UTF8")) {
			SetError("Cannot set UTF8 charset", "Opening database");
			return false;
		}
		charset = CHARSET_UTF8;
	}
	else
		charset = CHARSET_DEFAULT;

	DoKeepAlive();

	LLOG( Txt("Postgresql client encoding: ") + pg_encoding_to_char( PQclientEncoding(conn) ) );

	Sql sql(*this);
	if(sql.Execute("select setting from pg_settings where name = 'bytea_output'") && sql.Fetch() && sql[0] == "hex")
		hex_blobs = true;

	return true;
}

bool PostgreSQLSession::ConnectionOK()
{
	return conn && PQstatus(conn) == CONNECTION_OK;
}

bool PostgreSQLSession::ReOpen()
{
	PQreset(conn);
	if(PQstatus(conn) != CONNECTION_OK)
	{
		SetError(ErrorMessage(), "Opening database");
		return false;
	}
	DoKeepAlive();
	level = 0;
	return true;
}

void PostgreSQLSession::Close()
{
	if(!conn)
		return;
	SessionClose();
	PQfinish(conn);
	conn = NULL;
	level = 0;
}

void PostgreSQLSession::Begin()
{
	ExecTrans("begin");
	level++;
}

void PostgreSQLSession::Commit()
{
	ExecTrans("commit");
	level--;
}

void PostgreSQLSession::Rollback()
{
	ExecTrans("rollback");
	if(level > 0) level--;
}

i32 PostgreSQLSession::GetTransactionLevel() const
{
	return level;
}

void PostgreSQLConnection::SetParam(i32 i, const Val& r)
{
	Txt p;
	if(IsNull(r))
		p = "NULL";
	else
		switch(r.GetType()) {
		case SQLRAW_V: {
			Txt raw = SqlRaw(r);
			size_t rl;
			u8* s = PQescapeByteaConn(conn, (u8k *)~raw, raw.GetLength(), &rl);
			p.Reserve(i32(rl + 16));
			p = "\'" + Txt(s, i32(rl - 1)) + "\'::bytea";
			PQfreemem(s);
			break;
		}
		case WTXT_V:
		case TXT_V: {
				Txt v = r;
				v = ToCharset(v);
				TxtBuf b(v.GetLength() * 2 + 3);
				tuk q = b;
				*q = '\'';
				i32 *err = NULL;
				i32 n = (i32)PQescapeTxtConn(conn, q + 1, v, v.GetLength(), err);
				q[1 + n] = '\'';
				b.SetCount(2 + n);
				p = b;
			}
			break;
		case BOOL_V:
		case INT_V:
			p << i32(r);
			break;
		case INT64_V:
			p << z64(r);
			break;
		case DOUBLE_V:
			p = FormatDouble(double(r), 20);
			break;
		case DATE_V: {
				Date d = r;
				p = Format("\'%04d-%02d-%02d\'", d.year, d.month, d.day);
			}
			break;
		case TIME_V: {
				Time t = r;
				p = Format("\'%04d-%02d-%02d %02d:%02d:%02d\'",
						   t.year, t.month, t.day, t.hour, t.minute, t.second);
			}
			break;
		default:
			NEVER();
		}
	param.At(i, p);
}

bool PostgreSQLConnection::Execute()
{
	Cancel();
	if(statement.GetLength() == 0) {
		session.SetError("Пустая инструкция", statement);
		return false;
	}

	CParser p(statement);
	if((p.Id("insert") || p.Id("INSERT")) && (p.Id("into") || p.Id("INTO")) && p.IsId())
		last_insert_table = p.ReadId();

	Txt query;
	i32 pi = 0;
	tukk s = statement;
	while(s < statement.End())
		if(*s == '\'' || *s == '\"')
			s = PostgreSQLReadTxt(s, query);
		else {
			if(*s == '?' && !session.noquestionparams) {
				if(s[1] == '?') {
					query.Cat('?');
					s++;
				}
				else {
					if(pi >= param.GetCount()) {
						session.SetError("Неверное число параметров", statement);
						return false;
					}
					query.Cat(param[pi++]);
				}
			}
			else
				query.Cat(*s);
			s++;
		}
	param.Clear();

	Stream *trace = session.GetTrace();
	dword time;
	if(session.IsTraceTime())
		time = msecs();

	i32 itry = 0;
	i32 stat;
	do {
		result = PQexecParams(conn, query, 0, NULL, NULL, NULL, NULL, 0);
		stat = PQresultStatus(result);
	}
	while(stat != PGRES_TUPLES_OK && stat != PGRES_COMMAND_OK && session.level == 0 &&
	      (!session.ConnectionOK() || ErrorMessage().Find("connection") >= 0 && itry == 0) && session.WhenReconnect(itry++));

	if(trace) {
		if(session.IsTraceTime())
			*trace << Format("--------------\nexec %d ms:\n", msecs(time));
	}
	if(stat == PGRES_TUPLES_OK) //result set
	{
		rows = PQntuples(result);
		i32 fields = PQnfields(result);
		info.SetCount(fields);
		oid.SetCount(fields);
		for(i32 i = 0; i < fields; i++)
		{
			SqlColumnInfo& f = info[i];
			f.name = ToUpper(PQfname(result, i));
			f.width = PQfsize(result, i);
			i32 type_mod = PQfmod(result, i) - sizeof(i32);
			if(f.width < 0)
				f.width = type_mod;
			f.precision = (type_mod >> 16) & 0xffff;
			f.scale = type_mod & 0xffff;
			f.nullable = true;
			Oid type_oid = PQftype(result, i);
			f.type = OidToType(type_oid);
			oid[i] = type_oid;
		}
		return true;
	}
	if(stat == PGRES_COMMAND_OK) //command executed OK
	{
		rows = atoi(PQcmdTuples(result));
		return true;
	}

	session.SetError(ErrorMessage(), query, 0, ErrorCode());
	FreeResult();
	return false;
}

i32 PostgreSQLConnection::GetRowsProcessed() const
{
	return rows;
}

Val PostgreSQLConnection::GetInsertedId() const
{
	Txt pk = session.pkache.Get(last_insert_table, Null);
	if(IsNull(pk)) {
		Txt sqlc_expr;
		sqlc_expr <<
		"SELECT " <<
		  "pg_attribute.attname " <<
		"FROM pg_index, pg_class, pg_attribute " <<
		"WHERE " <<
		  "pg_class.oid = '" << last_insert_table << "'::regclass AND "
		  "indrelid = pg_class.oid AND "
		  "pg_attribute.attrelid = pg_class.oid AND "
		  "pg_attribute.attnum = any(pg_index.indkey) "
		  "AND indisprimary";
		Sql sqlc(sqlc_expr, session);
		pk = sqlc.Execute() && sqlc.Fetch() ? sqlc[0] : "ID";
		session.pkache.Add(last_insert_table, pk);
	}
	Sql sql("select currval('" + last_insert_table + "_" + pk +"_seq')", session);
	if(sql.Execute() && sql.Fetch())
		return sql[0];
	else
		return Null;
}

bool PostgreSQLConnection::Fetch()
{
	fetched_row++;
	if(result && rows > 0 && fetched_row < rows)
		return true;
	Cancel();
	return false;
}

static Date sDate(tukk s)
{
	// 0123456789012345678
	// YYYY-MM-DD HH-MM-SS
	return Date(atoi(s), atoi(s + 5), atoi(s + 8));
}

void PostgreSQLConnection::GetColumn(i32 i, Ref f) const
{
	if(PQgetisnull(result, fetched_row, i))
	{
		f = Null;
		return;
	}
	tuk s = PQgetvalue(result, fetched_row, i);
	switch(info[i].type)
	{
		case INT64_V:
			f.SetVal(ScanInt64(s));
			break;
		case INT_V:
			f.SetVal(ScanInt(s));
			break;
		case DOUBLE_V: {
				double d = ScanDouble(s);
				f.SetVal(IsNull(d) ? NAN : d);
			}
			break;
		case BOOL_V:
			f.SetVal(*s == 't' ? "1" : "0");
			break;
		case DATE_V:
			f.SetVal(sDate(s));
			break;
		case TIME_V: {
				Time t = ToTime(sDate(s));
				t.hour = atoi(s + 11);
				t.minute = atoi(s + 14);
				t.second = atoi(s + 17);
				f.SetVal(t);
			}
			break;
		default: {
			if(oid[i] == PGSQL_BYTEAOID) {
				if(session.hex_blobs)
					f.SetVal(ScanHexTxt(s, (i32)strlen(s)));
				else {
					size_t len;
					u8* q = PQunescapeBytea((u8k*)s, &len);
					f.SetVal(Txt(q, (i32)len));
					PQfreemem(q);
				}
			}
			else
				f.SetVal(FromCharset(Txt(s)));
		}
	}
}

void PostgreSQLConnection::Cancel()
{
	info.Clear();
	rows = 0;
	fetched_row = -1;
	FreeResult();
}

SqlSession& PostgreSQLConnection::GetSession() const
{
	return session;
}

Txt PostgreSQLConnection::GetUser() const
{
	return PQuser(conn);
}

Txt PostgreSQLConnection::ToTxt() const
{
	return statement;
}

void PostgreSQLConnection::FreeResult()
{
	if(result)
	{
		PQclear(result);
		result = NULL;
	}
}

PostgreSQLConnection::PostgreSQLConnection(PostgreSQLSession& a_session, PGconn *a_conn)
  : session(a_session), conn(a_conn)
{
	result = NULL;
}

Val PgSequence::Get()
{
#ifndef NOAPPSQL
	Sql sql(session ? *session : SQL.GetSession());
#else
	ASSERT(session);
	Sql sql(*session);
#endif
	if(!sql.Execute(Select(NextVal(seq)).Get()) || !sql.Fetch())
		return ErrorVal();
	return sql[0];
}

}

#endif
