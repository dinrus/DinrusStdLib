#ifndef _PostgreSQL_PostgeSQL_h_
#define _PostgreSQL_PostgeSQL_h_

#ifndef flagNOPOSTGRESQL

#include <drx/Sql/Sql.h>
#include <libpq-fe.h>

// debian: sudo apt-get install libpq-dev

namespace drx {

// Postgre -> Val types
// Bytea_v values are stored as bytea data, but recived as string type
tukk PostgreSQLReadTxt(tukk s, Txt& stmt);


// Deprecated, use SqlPerformScript instead
bool PostgreSQLPerformScript(const Txt& text,
#ifdef NOAPPSQL
	StatementExecutor& se,
#else
	StatementExecutor& se = SQLStatementExecutor(),
#endif
	Gate<i32, i32> progress_canceled = Null
);


Txt PostgreSQLTextType(i32 n);

class PostgreSQLConnection;

class PostgreSQLSession : public SqlSession {
public:
	virtual bool                  IsOpen() const                   { return conn; }
	virtual RunScript             GetRunScript() const             { return &PostgreSQLPerformScript; }

	virtual Vec<Txt>        EnumUsers();
	virtual Vec<Txt>        EnumDatabases();
	virtual Vec<Txt>        EnumTables(Txt database);
	virtual Vec<Txt>        EnumViews(Txt database);
	virtual Vec<Txt>        EnumSequences(Txt database);
	virtual Vec<SqlColumnInfo> EnumColumns(Txt database, Txt table);
	virtual Vec<Txt>        EnumPrimaryKey(Txt database, Txt table);
	virtual Txt                EnumRowID(Txt database, Txt table);
	virtual Vec<Txt>        EnumReservedWords();

protected:
	virtual SqlConnection *CreateConnection();

private:
	PGconn               *conn;
	PGresult             *result;
	
	Txt                conns;
	bool                  keepalive;
	bool                  hex_blobs;
	bool                  noquestionparams = false;
	
	VecMap<Txt, Txt> pkache;

	void                  ExecTrans(tukk  statement);
	Vec<Txt>        EnumData(char type, tukk schema = NULL);
	Txt                ErrorMessage();
	Txt                ErrorCode();
	i32                   level;
	u8                  charset;
	
	Txt                FromCharset(const Txt& s) const;
	Txt                ToCharset(const Txt& s) const;
	
	void                  DoKeepAlive();

	friend class PostgreSQLConnection;

public:
	Gate1<i32>            WhenReconnect;

	bool                  Open(tukk connect);
	bool                  ConnectionOK();
	bool                  ReOpen();
	void                  Close();

	void                  SetCharset(u8 chrset)         { charset = chrset; }
	void                  KeepAlive(bool b = true)        { keepalive = b; DoKeepAlive(); }
	void                  NoQuestionParams(bool b = true) { noquestionparams = b; }

	Txt                GetUser()                       { return PQuser(conn); }
	operator PGconn *     ()                              { return conn; }

	virtual void          Begin();
	virtual void          Commit();
	virtual void          Rollback();
	virtual i32           GetTransactionLevel() const;

	PostgreSQLSession()                                   { conn = NULL; Dialect(PGSQL); level = 0; keepalive = hex_blobs = false; }
	~PostgreSQLSession()                                  { Close(); }
	PGconn * GetPGConn()                                  { return conn; }
};

class PgSequence : public ValGen {
	SqlId       ssq;
	SqlId&      seq;
	SqlSession *session;

public:
	virtual Val  Get();

	Val operator++()                                                  { return Get(); }

	void Set(SqlId id, SqlSession& s)                                   { ssq = id; session = &s; }

#ifndef NOAPPSQL
	void Set(SqlId id)                                                  { ssq = id; session = NULL; }
	PgSequence(tukk name) : ssq(name), seq(ssq)                  { session = NULL; }
	PgSequence(SqlId& seq) : seq(seq)                                   { session = NULL; }
#endif
	PgSequence(tukk name, SqlSession& s) : ssq(name), seq(ssq)   { session = &s; }
	PgSequence(SqlId& seq, SqlSession& s) : seq(seq)                    { session = &s; }
	PgSequence() : seq(ssq)                                             { session = NULL; }
};

}

#endif

#endif
