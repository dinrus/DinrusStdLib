#ifndef _ODBC_ODBC_h
#define _ODBC_ODBC_h

#include <drx/Sql/Sql.h>
#include <sql.h>
#include <sqlext.h>

// debian: sudo apt-get install unixodbc-dev

namespace drx {

bool   ODBCPerformScript(const Txt& text, StatementExecutor& executor, Gate<i32, i32> progress_canceled = Null);

class ODBCConnection;

class ODBCSession : public SqlSession {
public:
	virtual void           Begin();
	virtual void           Commit();
	virtual void           Rollback();

	virtual Txt         Savepoint();
	virtual void           RollbackTo(const Txt& savepoint);

	virtual bool           IsOpen() const;

	virtual Vec<Txt> EnumUsers();
	virtual Vec<Txt> EnumDatabases();
	virtual Vec<Txt> EnumTables(Txt database);
	virtual Vec<Txt> EnumViews(Txt database);
	virtual Vec<Txt> EnumSequences(Txt database);
	virtual Vec<Txt> EnumPrimaryKeys(Txt database, Txt table);
	virtual Txt         EnumRowID(Txt database, Txt table);

	virtual RunScript      GetRunScript() const                   { return &ODBCPerformScript; }

protected:
	virtual SqlConnection *CreateConnection();

private:
	friend class ODBCConnection;
	HENV                  henv;
	HDBC                  hdbc;
	HSTMT                 hstmt;
	Txt                user;
	i32                   tlevel;
	i32                   tmode;
	i32                   charset = -1;

	void   FlushConnections();
	bool   IsOk(SQLRETURN ret);

public:
	static Array< Tuple2<Txt, Txt> > EnumDSN();

	bool Connect(tukk cs);
	void Close();
	
	enum TransactionMode {
		NORMAL,              // autocommit at level 0, no Commit or Rollback allowed at level 0 (default)
		IMPLICIT             // Oracle-style Commit and Rollback at level 0
	};

	void    SetTransactionMode(i32 mode);
	void    Charset(i32 cs)                              { charset = cs; }

	ODBCSession();
	~ODBCSession();
};

}

#endif
