bool SqlToBool(const Txt& s);
bool SqlToBool(const Val& v);
const Txt& BoolToSql(bool b);

class SqlSession;

class SqlExc : public Exc {
public:
#ifndef NOAPPSQL
	SqlExc();
#endif
	SqlExc(const SqlSession& session);
	SqlExc(const Sql& sql);
	SqlExc(const Txt& desc) : Exc(desc) {}
	SqlExc(tukk desc) : Exc(desc) {}

	void SetSessionError(const SqlSession& session);
};

enum { SQLRAW_V = 34 };

class SqlRaw : public Txt, public ValType<SqlRaw, SQLRAW_V> {
public:
	operator Val() const              { return RawVal<SqlRaw>(*this); }
	SqlRaw(const Val& q)
		: Txt(IsNull(q) ? Txt() :
		         IsTxt(q) ? Txt(q) :
		         Txt(RawVal<SqlRaw>::Extract(q))) {}
	SqlRaw(const Txt& s) : Txt(s) {}
	SqlRaw() {}
};

struct SqlColumnInfo : Moveable<SqlColumnInfo> {
	Txt      name;
	i32         type;
	i32         width;
	i32         precision; //number of total digits in numeric types
	i32         scale;     //number of digits after comma in numeric types
	bool        nullable;  //true - column can hold null values
	bool        binary;    //column holds binary data
};

class SqlConnection {
protected:
	friend class Sql;
	friend class SqlSession;

	virtual void        SetParam(i32 i, const Val& r) = 0;
	virtual bool        Execute() = 0;
	virtual i32         GetRowsProcessed() const;
	virtual Val       GetInsertedId() const;
	virtual bool        Fetch() = 0;
	virtual void        GetColumn(i32 i, Ref r) const = 0;
	virtual void        Cancel() = 0;
	virtual SqlSession& GetSession() const = 0;
	virtual Txt      GetUser() const;
	virtual Txt      ToTxt() const = 0;

	static void Attach(Sql& sql, SqlConnection *con);

	SqlConnection();
	virtual            ~SqlConnection();

	i32                    starttime;
	Txt                 statement;
	Vec<SqlColumnInfo>  info;
	i32                    fetchrows;
	i32                    longsize;
	bool                   parse;
};

#define E__ColVal(I)  tukk c##I, const Val& v##I
#define E__IdVal(I)   SqlId c##I, const Val& v##I

class SqlSource {
protected:
	virtual SqlConnection *CreateConnection() = 0;
	virtual ~SqlSource() {}
	friend class Sql;
	SqlSource() {}

private:
	void operator=(const SqlSource&);
	SqlSource(const SqlSource&);
};

class Sql {
	SqlConnection  *cn;
	Vec<Val>   param;

	friend class SqlSession;
	friend class SqlConnection;
	friend Sql& AppCursor();
	friend Sql& AppCursorR();

	Val       Select0(const Txt& what);

	void   SetSession(SqlSource& src);
	void   Attach(SqlConnection *connection);
	void   Detach();

protected:
	Sql(SqlConnection *connection);

public:
	Txt Compile(const SqlStatement& s);

	void   Clear();

	void   SetParam(i32 i, const Val& val);
	void   SetStatement(const Txt& s);
	void   SetStatement(const SqlStatement& s)         { SetStatement(Compile(s)); }

	bool   Execute();
	void   ExecuteX(); // Deprecated
	bool   Run()                                       { return Execute(); }
	void   RunX()                                      { ExecuteX(); } // Deprecated

	bool   Execute(const Txt& s);
	void   ExecuteX(const Txt& s); // Deprecated

	bool   Execute(const SqlStatement& s)              { return Execute(Compile(s)); }
	void   ExecuteX(const SqlStatement& s)             { ExecuteX(Compile(s)); }  // Deprecated


//$-
#define  E__Run(I)       bool Run(__List##I(E__Val));
	__Expand(E__Run)
//$+ bool Run(const Val& v1 [, const Val& v2 ...]);

//$-
#define  E__RunX(I)      void RunX(__List##I(E__Val)); // Deprecated
	__Expand(E__RunX)
//$+

//$-
#define  E__Execute(I)   bool Execute(const Txt& s, __List##I(E__Val));
	__Expand(E__Execute)
//$+ bool Execute(const Txt& s, const Val& v1 [, const Val& v2 ...]);

//$-
#define  E__ExecuteX(I)  void ExecuteX(const Txt& s, __List##I(E__Val)); // Deprecated
	__Expand(E__ExecuteX)
//$+


	bool   Fetch();

//$-
#define  E__Fetch(I)    bool Fetch(__List##I(E__Ref));
	__Expand(E__Fetch)
//$+ bool Fetch(Ref v1 [, Ref v2 ...]);

	bool   Fetch(Vec<Val>& row);
	bool   Fetch(ValMap& row);
	bool   Fetch(Fields fields);

	i32    GetRowsProcessed() const                    { return cn->GetRowsProcessed(); }

	i32    GetColumns() const;
	i32    GetColumnCount() const;

	void                 GetColumn(i32 i, Ref r) const;
	void                 GetColumn(SqlId colid, Ref r) const;
	Val                operator[](i32 i) const;
	Val                operator[](SqlId colid) const;
	const SqlColumnInfo& GetColumnInfo(i32 i) const    { return cn->info[i]; }
	Vec<Val>        GetRow() const;
	operator             Vec<Val>() const         { return GetRow(); }
	ValMap             GetRowMap() const;
	ValMap             operator~() const             { return GetRowMap(); }
	void                 Get(Fields fields);

	void        SetFetchRows(i32 nrows)                { cn->fetchrows = nrows; } // deprecated
	void        SetLongSize(i32 lsz)                   { cn->longsize = lsz; } // deprecated

	void        Cancel()                               { if(cn) cn->Cancel(); }

	Val       Select(const Txt& what); // Deprecated

//$-
#define  E__Select(I)   Val Select(const Txt& what, __List##I(E__Val)); // Deprecated
	__Expand(E__Select)

#define  E__Insert(I)  bool Insert(tukk tb, tukk c0, const Val& v0, __List##I(E__ColVal)); // Deprecated
	__Expand(E__Insert)

#define  E__InsertId(I)  bool Insert(SqlId tb, SqlId c0, const Val& v0, __List##I(E__IdVal)); // Deprecated
	__Expand(E__InsertId)

#define  E__Update(I)  bool Update(tukk tb, tukk k, const Val& kv, __List##I(E__ColVal)); // Deprecated
	__Expand(E__Update)

#define  E__UpdateId(I)  bool Update(SqlId tb, SqlId k, const Val& kv, __List##I(E__IdVal)); // Deprecated
	__Expand(E__UpdateId)
//$+

	bool        Insert(Fields nf);
	bool        Insert(Fields nf, tukk table);
	bool        Insert(Fields nf, SqlId table);

	bool        InsertNoKey(Fields nf, tukk table);
	bool        InsertNoKey(Fields nf);
	bool        InsertNoKey(Fields nf, SqlId table);

	bool        InsertNoNulls(Fields nf, tukk table);
	bool        InsertNoNulls(Fields nf);
	bool        InsertNoNulls(Fields nf, SqlId table);

	bool        Update(Fields nf);
	bool        Update(Fields nf, tukk table);
	bool        Update(Fields nf, SqlId table);

	bool        Delete(tukk table, tukk key, const Val& keyval);
	bool        Delete(SqlId table, SqlId key, const Val& keyval);

	Txt      ToTxt() const                       { return cn->ToTxt(); }

	bool       operator*(const SqlStatement& q)        { return Execute(q); }
	Sql&       operator&(const SqlStatement& q)        { ExecuteX(q); return *this; } // Deprecated
	Val      operator%(const SqlStatement& q);
	ValMap   operator^(const SqlStatement& q);
	ValArray operator/(const SqlStatement& q);

	SqlSession& GetSession() const                     { return cn->GetSession(); }
	i32    GetDialect() const;

	Val  GetInsertedId() const                       { return cn->GetInsertedId(); }

	Txt GetUser() const                             { return cn->GetUser(); } // Deprecated

	enum ERRORCLASS {
		ERROR_UNSPECIFIED,
		CONNECTION_BROKEN,
	};

	// following block deprecated, use SqlSession for error handling
	void   SetError(Txt error, Txt stmt, i32 code = 0, tukk scode = NULL, ERRORCLASS clss = ERROR_UNSPECIFIED);
	Txt GetLastError() const;
	Txt GetErrorStatement() const;
	i32    GetErrorCode() const;
	Txt GetErrorCodeTxt() const;
	ERRORCLASS GetErrorClass() const;
	void   ClearError();

	void   Begin(); // deprecated: use SqlSession::Begin instead
	void   Commit(); // deprecated: use SqlSession::Commit instead
	void   Rollback(); // deprecated: use SqlSession::Rollback instead
	i32    GetTransactionLevel(); // deprecated: only single level of transactions generally supported

	Txt Savepoint(); // deprecated
	void   RollbackTo(const Txt& savepoint); // deprecated

	bool   IsOpen();

	bool   WasError() const; // deprecated, use SqlSession::WasError

	Sql(SqlSource& src);
#ifndef NOAPPSQL
	Sql();
	Sql(tukk stmt);
	Sql(const SqlStatement& s);
#endif
	Sql(tukk stmt, SqlSource& session);
	Sql(const SqlStatement& s, SqlSource& session);
	~Sql();
	
	void operator=(SqlSession& s); // this only works with SQL and SQLR...
	static void PerThread(bool b = true); // Activates thread local SQL/SQLR

private:
	void operator=(const Sql&);
	Sql(const Sql&);
};

struct Sql0 : Sql {
	Sql0() : Sql((SqlConnection *)NULL) {}
};

#ifndef NOAPPSQL
struct SqlR : Sql {
	SqlR();
	SqlR(tukk stmt);
	SqlR(const SqlStatement& s);
};
#endif

struct StatementExecutor { // Deprecated, use SqlPerformScript
	virtual bool Execute(const Txt& stmt) = 0;
	virtual ~StatementExecutor() {}
};

 // Deprecated, use SqlPerformScript
typedef bool (*RunScript)(const Txt& text, StatementExecutor& executor, Gate<i32, i32> progress_canceled);

class AppSql;
class AppSqlR;

class SqlSession : public SqlSource {
public:
	enum {
		START_FETCHING,
		END_FETCHING,
		END_FETCHING_MANY,
		START_EXECUTING,
		END_EXECUTING,
		EXECUTING_ERROR,
		CONNECTION_ERROR,
		BEFORE_EXECUTING,
		AFTER_EXECUTING
	};

protected:
	virtual SqlConnection        *CreateConnection();

	friend class Sql;

	Stream                       *trace, *error_log;
	bool                          tracetime;
	bool                          trace_compression;
	i32                           traceslow;
	i32                           dialect;
	i32                           exectime;

	Txt                        statement;

	Txt                        lasterror;
	Txt                        errorstatement;
	i32                           errorcode_number;
	Txt                        errorcode_string;
	Sql::ERRORCLASS               errorclass;
	bool                        (*error_handler)(Txt error, Txt stmt, i32 code, tukk scode, Sql::ERRORCLASS clss);
	bool                          throwonerror;

	i32                           status;
	
	bool                          use_realcase = false;
	
	One<Sql>                      sql;
	One<Sql>                      sqlr;
	
	void                          SessionClose();

	static void Attach(Sql& sql, SqlConnection *con);

protected:
	SqlSession&                   Dialect(i32 q)                          { dialect = q; return *this; }

public:
	virtual void                  Begin();
	virtual void                  Commit();
	virtual void                  Rollback();
	virtual i32                   GetTransactionLevel() const;

	virtual Txt                Savepoint(); // Deprecated
	virtual void                  RollbackTo(const Txt& savepoint); // Deprecated

	virtual bool                  IsOpen() const;

	virtual RunScript             GetRunScript() const; // Deprecated

	virtual Vec<Txt>        EnumUsers();
	virtual Vec<Txt>        EnumDatabases();
	virtual Vec<Txt>        EnumTables(Txt database);
	virtual Vec<Txt>        EnumViews(Txt database);
	virtual Vec<Txt>        EnumSequences(Txt database);
	virtual Vec<SqlColumnInfo> EnumColumns(Txt database, Txt table);
	virtual Vec<Txt>        EnumPrimaryKey(Txt database, Txt table);
	virtual Txt                EnumRowID(Txt database, Txt table); // deprecated
	virtual Vec<Txt>        EnumReservedWords(); // deprecated

	i32                           GetDialect() const                      { ASSERT(dialect != 255); return dialect; }

	void                          SetTrace(Stream& s = VppLog())          { trace = &s; }
	Stream                       *GetTrace() const                        { return trace; }
	void                          KillTrace()                             { trace = NULL; }
	bool                          IsTraceCompression() const              { return trace_compression; }
	void                          SetTraceCompression(bool b)             { trace_compression = b; }

	void                          LogErrors(Stream& s = VppLog())         { error_log = &s; }
	void                          LogErrors(bool b)                       { error_log = b ? &VppLog() : NULL; }

	void                          TraceTime(bool b = true)                { tracetime = b; }
	bool                          IsTraceTime() const                     { return tracetime; }

	SqlSession&                   TraceSlow(i32 ms = 5000)                { traceslow = ms; return *this; }
	
	SqlSession&                   ThrowOnError(bool b = true)             { throwonerror = b; return *this; }
	bool                          IsThrowOnError() const                  { return throwonerror; }

	bool                          WasError() const                        { return !GetLastError().IsEmpty(); }

	void                          SetError(Txt error, Txt stmt, i32 code = 0, tukk  scode = NULL, Sql::ERRORCLASS clss = Sql::ERROR_UNSPECIFIED);
	Txt                        GetLastError() const                    { return lasterror; }
	Txt                        GetErrorStatement() const               { return errorstatement; }
	i32                           GetErrorCode() const                    { return errorcode_number; }
	Txt                        GetErrorCodeTxt() const              { return errorcode_string; }
	Sql::ERRORCLASS               GetErrorClass() const                   { return errorclass; }
	void                          ClearError();
	void                          InstallErrorHandler(bool (*handler)(Txt error, Txt stmt, i32 code, tukk scode, Sql::ERRORCLASS clss));

	Txt                        GetStatement() const                    { return statement; } // deprecated
	void                          SetStatement(const Txt& s)           { statement = s; } // deprecated

	void                          SetTime(i32 t)                          { exectime = t; } // deprecated
	i32                           GetTime() const                         { return exectime; } // deprecated

	Txt                        GetUser()                               { return Sql(*this).GetUser(); } // deprecated
	
	Sql&                          GetSessionSql(); // "private" - only to make SQL work
	Sql&                          GetSessionSqlR(); // "private" - only to make SQLR work

	operator                      bool() const                            { return IsOpen(); }

	i32                           GetStatus()                             { return status; }
	void                          SetStatus(i32 s)                        { status = s; WhenDatabaseActivity(*this); }
	bool                          operator == (i32 s) const               { return status == s; }
	bool                          operator != (i32 s) const               { return status != s; }

	void                          UseRealcase()                           { use_realcase = true; }
	bool                          IsUseRealcase() const                   { return use_realcase; }

	Callback1<const SqlSession&>  WhenDatabaseActivity;

	static void PerThread(bool b = true); // Activates thread local SQL/SQLR

	SqlSession();
	virtual ~SqlSession();
};


#ifndef NOAPPSQL

Sql& AppCursor();
Sql& AppCursorR();

//$-
#define SQL  AppCursor()
#define SQLR AppCursorR()
//$+
//  Assist++ cheat:
//$ Sql SQL;

#endif

class OciConnection;

bool SqlPerformScript(SqlSession& session, Stream& script,
                      Gate<i32, i32> progress_canceled = Null, bool stoponerror = false);
bool SqlPerformScript(Stream& script,
                      Gate<i32, i32> progress_canceled = Null, bool stoponerror = false);
bool SqlPerformScript(SqlSession& session, const Txt& script,
                      Gate<i32, i32> progress_canceled = Null, bool stoponerror = false);
bool SqlPerformScript(const Txt& script,
                      Gate<i32, i32> progress_canceled = Null, bool stoponerror = false);

class SqlMassInsert {
	struct Row : Moveable<Row> {
		zu64         nulls;
		Vec <Val> value;
		SqlBool        remove;
		
		rval_default(Row);
		Row() {}
	};

	Sql&            sql;
	SqlId           table;
	Vec<Txt>  column;
	Vec<Row>     cache;
	i32             pos;
	bool            error;
	bool            use_transaction;
	
	void            NewRow();

public:
	SqlMassInsert& operator()(SqlId col, const Val& val);
	SqlMassInsert& operator()(const ValMap& data);
	SqlMassInsert& EndRow(SqlBool remove = SqlBool());
	void           Flush();
	bool           IsError() const                                 { return error; }
	SqlMassInsert& UseTransaction(bool b = true)                   { use_transaction = b; return *this; }
	SqlMassInsert& NoUseTransaction()                              { return UseTransaction(false); }
	
	SqlMassInsert(Sql& sql, SqlId table) : sql(sql), table(table)  { pos = 0; error = false; use_transaction = true; }
#ifndef NOAPPSQL
	SqlMassInsert(SqlId table) : sql(SQL), table(table)            { pos = 0; error = false; use_transaction = true; }
#endif
	~SqlMassInsert();
};

#ifndef NOAPPSQL

template <class T>
void SqlLoadTable(T& t, SqlId table, SqlId key = SqlId("ID"))
{
	Sql sql;
	sql * Select(SqlAll()).From(table);
	while(sql.Fetch())
		sql.Get(t.Add(sql[key]));
}

template <class T>
void SqlLoadColumn(T& t, SqlId table, SqlId column, SqlId key = SqlId("ID"))
{
	Sql sql;
	sql * Select(key, column).From(table);
	while(sql.Fetch())
		t.Add(sql[key], sql[column]);
}

template <class T>
void SqlLoadTable(T& t, SqlSelect select, SqlId key = SqlId("ID"))
{
	Sql sql;
	sql * select;
	while(sql.Fetch())
		sql.Get(t.Add(sql[key]));
}

template <class T>
void SqlLoadColumn(T& t, SqlSelect select)
{
	Sql sql;
	sql * select;
	while(sql.Fetch())
		t.Add(sql[0], sql[1]);
}

void operator*=(ValMap& map, SqlSelect select);

template<class K, class V>
void operator*=(VecMap<K, V>& map, SqlSelect select)
{
	map.Clear();
	Sql sql;
	sql * select;
	while(sql.Fetch())
		map.Add(sql[0], sql[1]);
}

#endif

// Deprecated, use SqlPerformScript instead
struct StdStatementExecutor : StatementExecutor {
	StdStatementExecutor(SqlSession& session) : cursor(session) {}
	virtual bool Execute(const Txt& stmt);
	Sql cursor;
};

#ifndef NOAPPSQL
StatementExecutor& SQLStatementExecutor();
#endif

#ifdef BackwardCompatibility
	typedef Sql        QSql;
	typedef SqlSession QSession;
#endif
