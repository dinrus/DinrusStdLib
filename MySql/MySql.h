#ifndef __mysql_h__
#define __mysql_h__

// debian: sudo apt-get install libmysqlclient-dev

#ifndef flagNOMYSQL

#include <drx/Sql/Sql.h>
#include <mariadb/mysql.h>

#ifndef MYSQL_PORT // missing in mariadb client
#define MYSQL_PORT 3306
#endif

namespace drx {

tukk MySqlReadTxt(tukk s, Txt& stmt);

bool MySqlPerformScript(const Txt& text, StatementExecutor& se, Gate<i32, i32> progress_canceled = Null);
#ifdef NOAPPSQL
bool MySqlUpdateSchema(const SqlSchema& sch, i32 i, StatementExecutor& se);
#else
bool MySqlUpdateSchema(const SqlSchema& sch, i32 i, StatementExecutor& se = SQLStatementExecutor());
#endif

Txt MySqlTextType(i32 n);

class MySqlSession : public SqlSession {
public:
	virtual bool           IsOpen() const;
	virtual RunScript      GetRunScript() const             { return &MySqlPerformScript; }
	virtual Vec<Txt> EnumUsers();
	virtual Vec<Txt> EnumDatabases();
	virtual Vec<Txt> EnumTables(Txt database);

protected:
	virtual SqlConnection *CreateConnection();

private:
	MYSQL *mysql;
	Txt username;
	double lastid;
	i32    level;

	Txt connect_user;
	Txt connect_password;
	Txt connect_database;
	Txt connect_host;
	i32    connect_port;
	Txt connect_socket;

	bool MysqlQuery(tukk query);
	bool DoConnect();
	
	friend class MySqlConnection;
	typedef MySqlSession CLASSNAME;

public:
	Gate<>               WhenReconnect;

	bool Connect(tukk user = NULL, tukk password = NULL, tukk database = NULL,
		         tukk host = NULL, i32 port = MYSQL_PORT, tukk socket = NULL);
	bool Open(tukk connect);
	bool Reconnect();
	void Close();

	Txt   GetUser()   { return username; }

	operator MYSQL *()   { return mysql; }

	virtual void   Begin();
	virtual void   Commit();
	virtual void   Rollback();
	virtual i32    GetTransactionLevel() const;
	
	void    AutoReconnect()   { WhenReconnect = OTVET(Reconnect); }

	MySqlSession()       { mysql = NULL; Dialect(MY_SQL); }
	~MySqlSession()      { Close(); }
};

}

#endif

#endif
