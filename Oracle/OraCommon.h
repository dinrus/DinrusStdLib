#ifndef ORACOMMON_H
#define ORACOMMON_H

class OciSqlConnection;


namespace drx {

Date        OciDecodeDate(u8k data[7]);
bool        OciEncodeDate(u8 data[7], Date d);
Time        OciDecodeTime(u8k data[7]);
bool        OciEncodeTime(u8 data[7], Time t);

tukk OciParseTxt(tukk s);
Txt      OciParseTxtError(tukk s);
Txt      OciParseRefError(tukk s);

i32         OciParse(tukk statement, Txt& out, OciSqlConnection *conn, SqlSession *session);
Sql::ERRORCLASS OciErrorClass(i32 errcode);

class SqlSequence : public ValGen {
	SqlId       ssq;
	SqlId&      seq;
	SqlSession *session;

public:
	virtual Val  Get();

	Val operator++()                                                  { return Get(); }

	void Set(SqlId id, SqlSession& s)                                   { ssq = id; session = &s; }

#ifndef NOAPPSQL
	void Set(SqlId id)                                                  { ssq = id; session = NULL; }
	SqlSequence(tukk name) : ssq(name), seq(ssq)                 { session = NULL; }
	SqlSequence(SqlId& seq) : seq(seq)                                  { session = NULL; }
#endif
	SqlSequence(tukk name, SqlSession& s) : ssq(name), seq(ssq)  { session = &s; }
	SqlSequence(SqlId& seq, SqlSession& s) : seq(seq)                   { session = &s; }
	SqlSequence() : seq(ssq)                                            { session = NULL; }
};

#ifndef NOAPPSQL
bool OraclePerformScript(const Txt& text, StatementExecutor& se = SQLStatementExecutor(), Gate<i32, i32> progress_canceled = false);
#else
bool OraclePerformScript(const Txt& text, StatementExecutor& se, Gate<i32, i32> progress_canceled = false);
#endif

Vec<Txt> OracleSchemaUsers(Sql& cursor);
Vec<Txt> OracleSchemaTables(Sql& cursor, Txt database);
Vec<Txt> OracleSchemaViews(Sql& cursor, Txt database);
Vec<Txt> OracleSchemaSequences(Sql& cursor, Txt database);
Vec<Txt> OracleSchemaPrimaryKey(Sql& cursor, Txt database, Txt table);
Txt         OracleSchemaRowID(Sql& cursor, Txt database, Txt table);
Vec<Txt> OracleSchemaReservedWords();

i32k ORA_BLOB_V = -1;
i32k ORA_CLOB_V = -2;

class OracleRef {
public:
	OracleRef(i32 ora_vtype = VOID_V) : ora_vtype(ora_vtype) {}
	OracleRef(const Val& v) : ora_vtype(IsVal(v) ? ValTo<OracleRef>(v).ora_vtype : VOID_V) {}

	operator    Val () const                { return RawToVal(*this); }
	static bool IsVal(const Val& v)       { return IsTypeRaw<OracleRef>(v); }

	tukk Parse(tukk stmt);
	i32         GetType() const               { return ora_vtype; }

	i32         GetOraType() const;
	i32         GetMaxLen() const;

private:
	i32         ora_vtype;
};

class OciSqlConnection : public SqlConnection {
	friend i32 OciParse(tukk statement, Txt& out, OciSqlConnection *conn, SqlSession *session);

protected:
	virtual void SetParam(i32 i, OracleRef ref) = 0;
};

}

#endif
