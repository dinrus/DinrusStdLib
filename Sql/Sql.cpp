#include "Sql.h"

namespace drx {

#define LLOG(x) // DLOG(x)

bool SqlToBool(const Txt& s) {
	return !(IsNull(s) || *s == '0' && s[1] == '\0');
}

bool SqlToBool(const Val& v) {
	if(IsNull(v)) return false;
	if(IsTxt(v)) return SqlToBool(Txt(v));
	if(IsNumber(v)) return (i32) v;
	return true;
}

const Txt& BoolToSql(bool b) {
	static Txt T("1"), F("0");
	return b ? T : F;
}

void FieldOperator::Field(Ref f) {}

void FieldOperator::Field(tukk name, Ref f) { Field(f); }

void FieldOperator::Field(tukk name, Ref f, bool *b) { Field(name, f); }

void FieldOperator::Width(i32 width) {}


FieldOperator& FieldOperator::operator()(tukk name, bool& b) {
	Txt x = BoolToSql(b);
	Field(name, x, &b);
	b = SqlToBool(x);
	return *this;
}

static char sql_error[] = "Database error";

#ifndef NOAPPSQL
SqlExc::SqlExc() : Exc(sql_error) {
	SetSessionError(SQL.GetSession());
}
#endif

SqlExc::SqlExc(const SqlSession& session) : Exc(sql_error) {
	SetSessionError(session);
}

SqlExc::SqlExc(const Sql& sql) : Exc(sql_error) {
	SetSessionError(sql.GetSession());
}

void SqlExc::SetSessionError(const SqlSession& session) {
	if(session.WasError())
		*this = session.GetLastError();
	else
		*this = Txt(sql_error);
	*this << "\nSQL error: " << session.GetErrorStatement();
}

SqlConnection::SqlConnection()  { parse = true; fetchrows = 32; longsize = 16384; }
SqlConnection::~SqlConnection() {}

void SqlConnection::Cancel() {}

i32  SqlConnection::GetRowsProcessed() const {
	NEVER();
	return 0;
}

Txt SqlConnection::GetUser() const {
	NEVER();
	return Null;
}

Val SqlConnection::GetInsertedId() const
{
	NEVER();
	return Null;
}

Txt Sql::Compile(const SqlStatement& s)
{
	u8 dialect = GetDialect();
	ASSERT(dialect);
	return s.Get(dialect);
}

void Sql::Clear() {
	if(cn) {
		cn->Cancel();
		cn->parse = true;
	}
}

void Sql::SetParam(i32 i, const Val& val) {
	cn->SetParam(i, val);
	if(GetSession().GetTrace())
		param.Set(i, val);
}

void Sql::SetStatement(const Txt& s) {
	cn->statement = s;
	cn->parse = true;
}

bool Sql::Execute() {
	SqlSession &session = GetSession();

	session.SetStatement(cn->statement);
	session.SetStatus(SqlSession::BEFORE_EXECUTING);
	cn->starttime = msecs();
	Stream *s = session.GetTrace();
	if(s) {
#ifndef NOAPPSQL
		if(this == &AppCursor())
			*s << "SQL* ";
		else
		if(this == &AppCursorR())
			*s << "SQLR* ";
#endif
		Txt st = cn->statement;
		if(session.IsTraceCompression())
			st = CompressLog(st);
		i32 i = 0;
		for(tukk q = st; *q; q++)
			if(*q == '?' && i < param.GetCount()) {
				Val v = param[i++];
				if(IsTxt(v))
					*s << '\'' << v << '\'';
				else
					*s << v;
			}
			else
				s->Put(*q);
		*s << '\n';
	}
	if(!session.IsOpen())
	{
		session.SetStatus(SqlSession::CONNECTION_ERROR);
		return false;
	}
	session.SetStatus(SqlSession::START_EXECUTING);
	bool b = cn->Execute();
	session.SetTime(msecs() - cn->starttime);
	session.SetStatus(SqlSession::END_EXECUTING);
	if(!b)
		session.SetStatus(SqlSession::EXECUTING_ERROR);
	if(!session.IsUseRealcase())
		for(i32 i = 0; i < cn->info.GetCount(); i++)
			cn->info[i].name = ToUpper(cn->info[i].name);

	session.SetStatus(SqlSession::AFTER_EXECUTING);
	if(!b && session.throwonerror)
		throw SqlExc(GetSession());
	return b;
}

void Sql::ExecuteX() {
	if(!Execute())
		throw SqlExc(GetSession());
}

bool Sql::Execute(const Txt& s) {
	SetStatement(s);
	return Execute();
}

void Sql::ExecuteX(const Txt& s) {
	SetStatement(s);
	ExecuteX();
}

//$-

#define E__SetParam(I)    SetParam(I - 1, p##I)

#define E__RunF(I) \
bool Sql::Run(__List##I(E__Val)) { \
	__List##I(E__SetParam); \
	return Run(); \
}
__Expand(E__RunF)

#define E__RunFX(I) \
void Sql::RunX(__List##I(E__Val)) { \
	__List##I(E__SetParam); \
	RunX(); \
}
__Expand(E__RunFX)

#define E__ExecuteF(I) \
bool Sql::Execute(const Txt& s, __List##I(E__Val)) { \
	SetStatement(s); \
	__List##I(E__SetParam); \
	return Execute(); \
}
__Expand(E__ExecuteF)

#define E__ExecuteFX(I) \
void Sql::ExecuteX(const Txt& s, __List##I(E__Val)) { \
	SetStatement(s); \
	__List##I(E__SetParam); \
	ExecuteX(); \
}
__Expand(E__ExecuteFX)

//$+

bool Sql::Fetch() {
	SqlSession& session = GetSession();
	session.SetStatus(SqlSession::START_FETCHING);

	dword t0 = msecs();
	bool b = cn->Fetch();
	dword t = msecs();

	dword total = cn->starttime == INT_MAX ? 0 : t - cn->starttime;
	dword fetch = t - t0;

	session.SetStatus(SqlSession::END_FETCHING);
	if(!b) {
		session.SetTime(total);
		session.SetStatus(SqlSession::END_FETCHING_MANY);
	}
	Stream *s = session.GetTrace();
	if(s) {
		if((i32)total > session.traceslow)
			*s << "SLOW SQL: " << total << " ms: " << cn->statement << DRX::EOL;
		else
		if((i32)fetch > session.traceslow)
			*s << "SLOW SQL: " << fetch << " ms further fetch: " << cn->statement << DRX::EOL;
	}
	cn->starttime = INT_MAX;
	return b;
}

//$-
#define E__GetColumn(I) cn->GetColumn(I - 1, p##I)

#define E__FetchF(I) \
bool Sql::Fetch(__List##I(E__Ref)) { \
	if(!Fetch()) return false; \
	__List##I(E__GetColumn); \
	return true; \
}
__Expand(E__FetchF)
//$+

Vec<Val> Sql::GetRow() const {
	Vec<Val> row;
	i32 n = GetColumns();
	row.SetCount(n);
	for(i32 i = 0; i < n; i++)
		row[i] = (*this)[i];
	return row;
}

bool Sql::Fetch(Vec<Val>& row) {
	if(!Fetch()) return false;
	row = GetRow();
	return true;
}

ValMap Sql::GetRowMap() const
{
	ValMap m;
	i32 n = GetColumns();
	for(i32 i = 0; i < n; i++)
		m.Add(GetColumnInfo(i).name, (*this)[i]);
	return m;
}

Val Sql::operator%(const SqlStatement& q)
{
	return Select0(Compile(q));
}

ValMap Sql::operator^(const SqlStatement& q)
{
	Execute(q);
	ValMap m;
	Fetch(m);
	return m;
}

ValArray Sql::operator/(const SqlStatement& q)
{
	ValArray va;
	Execute(q);
	ValMap m;
	while(Fetch(m))
		va.Add(m);
	return va;
}

bool Sql::Fetch(ValMap& row) {
	if(!Fetch()) return false;
	row = GetRowMap();
	return true;
}

struct sReadFields : public FieldOperator {
	Sql *sql;

	void Field(tukk name, Ref f) {
		sql->GetColumn(SqlId(name), f);
	}
};

void Sql::Get(Fields fo)
{
	sReadFields ff;
	ff.sql = this;
	fo(ff);
}

bool Sql::Fetch(Fields fo) {
	if(!Fetch()) return false;
	Get(fo);
	return true;
}

i32 Sql::GetColumnCount() const
{
	return cn->info.GetCount();
}

i32  Sql::GetColumns() const {
	return GetColumnCount();
}

void Sql::GetColumn(i32 i, Ref r) const {
	cn->GetColumn(i, r);
}

void Sql::GetColumn(SqlId colid, Ref r) const
{
	Txt s = ~colid;
	for(i32 j = 0; j < 2; j++) {
		for(i32 i = 0; i < cn->info.GetCount(); i++)
			if(cn->info[i].name == s) {
				GetColumn(i, r);
				return;
			}
		if(!GetSession().IsUseRealcase())
			s = ToUpper(s);
	}
	r.SetNull();
}

Val Sql::operator[](i32 i) const {
	Val v;
	cn->GetColumn(i, v);
	return v;
}

Val Sql::operator[](SqlId id) const {
	Txt s = ~id;
	for(i32 j = 0; j < 2; j++) {
		for(i32 i = 0; i < cn->info.GetCount(); i++)
			if(cn->info[i].name == s)
				return operator[](i);
		if(!GetSession().IsUseRealcase())
			s = ToUpper(s);
	}
	NEVER_(Txt().Cat() << "SQL [" << ~id << "] not found");
	return Val();
}

Val Sql::Select0(const Txt& s) {
	SetStatement(s);
	if(!Run())
		return ErrorVal(GetLastError());
	if(!Fetch())
		return Null;
	Val v;
	cn->GetColumn(0, v);
	return v;
}

Val Sql::Select(const Txt& s) {
	return Select0("select " + s);
}

//$-
#define E__SelectF(I) \
Val Sql::Select(const Txt& s, __List##I(E__Val)) { \
	__List##I(E__SetParam); \
	return Select(s); \
}
__Expand(E__SelectF)

#define E__Inserter(I)  clist += ", ", clist += c##I, qlist += ", ?", SetParam(I, v##I)

#define E__InsertF(I) \
bool Sql::Insert(tukk table, tukk c0, const Val& v0, __List##I(E__ColVal)) { \
	Txt  clist = c0; \
	Txt  qlist = "?"; \
	SetParam(0, v0); \
	__List##I(E__Inserter); \
	return Execute(Txt("insert into ") + table + '(' + clist + ") values(" + qlist + ')'); \
}
__Expand(E__InsertF)

#define E__InserterId(I)  clist += ", ", clist += c##I.ToTxt(), qlist += ", ?", SetParam(I, v##I)

#define E__InsertIdF(I) \
bool Sql::Insert(SqlId table, SqlId c0, const Val& v0, __List##I(E__IdVal)) { \
	Txt  clist = c0.ToTxt(); \
	Txt  qlist = "?"; \
	SetParam(0, v0); \
	__List##I(E__InserterId); \
	return Execute( \
			Txt("insert into ") + table.ToTxt() + '(' + clist + ") values(" + qlist + ')'); \
}
__Expand(E__InsertIdF)

static inline void sComma(i32 I, Txt& s) {
	if(I > 1) s.Cat(", ");
}

#define E__Updater(I)  sComma(I, list), list.Cat(c##I), list.Cat(" = ?"), SetParam(I - 1, v##I)

#define  E__UpdateF(I) \
bool Sql::Update(tukk table, tukk key, const Val& keyval, __List##I(E__ColVal)) { \
	Txt list; \
	__List##I(E__Updater); \
	SetParam(I, keyval); \
	return Execute(Txt ("update ") + table + " set " + list + " where " + key + " = ?"); \
}
__Expand(E__UpdateF)

#define E__UpdaterId(I)  sComma(I, list), list.Cat(c##I.ToTxt()), list.Cat(" = ?"), SetParam(I - 1, v##I)

#define  E__UpdateIdF(I) \
bool Sql::Update(SqlId table, SqlId key, const Val& keyval, __List##I(E__IdVal)) { \
	Txt list; \
	__List##I(E__UpdaterId); \
	SetParam(I, keyval); \
	return Execute(Txt ("update ") + table.ToTxt() + \
	               " set " + list + " where " + key.ToTxt() + " = ?"); \
}
__Expand(E__UpdateIdF)
//$+

bool Sql::Delete(tukk table, tukk key, const Val& keyval) {
	return Execute("delete from " + Txt(table) + " where " + key + " = ?", keyval);
}

bool Sql::Delete(SqlId table, SqlId key, const Val& keyval) {
	return Delete(~table.ToTxt(), ~key.ToTxt(), keyval);
}

i32 Sql::GetDialect() const {
	return GetSession().GetDialect();
}

struct NfInsert : public FieldOperator {
	i32    i;
	Sql   *sql;
	Txt clist;
	Txt qlist;

	virtual void Field(tukk name, Ref f) {
		if(i) {
			clist += ", ";
			qlist += ", ";
		}
		clist += name;
		qlist += "? ";
		sql->SetParam(i++, f);
	}
};

bool Sql::Insert(Fields nf, tukk table) {
	NfInsert w;
	w.i = 0;
	w.sql = this;
	nf(w);
	return Execute(Txt("insert into ") + (table ? Txt(table) : w.table) +
	               '(' + w.clist + ") values(" + w.qlist + ')');
}

bool Sql::Insert(Fields nf) {
	return Insert(nf, NULL);
}

bool Sql::Insert(Fields nf, SqlId table) {
	return Insert(nf, (tukk )~table);
}

struct NfInsertNoKey : public FieldOperator {
	i32    i;
	Sql   *sql;
	Txt clist;
	Txt qlist;

	virtual void Field(tukk name, Ref f) {
		if(clist.GetCount()) {
			clist += ", ";
			qlist += ", ";
		}
		if(i) {
			clist += name;
			qlist += "? ";
			sql->SetParam(i - 1, f);
		}
		i++;
	}
};

bool Sql::InsertNoKey(Fields nf, tukk table) {
	NfInsertNoKey w;
	w.i = 0;
	w.sql = this;
	nf(w);
	return Execute(Txt("insert into ") + (table ? Txt(table) : w.table) +
	               '(' + w.clist + ") values(" + w.qlist + ')');
}

bool Sql::InsertNoKey(Fields nf) {
	return InsertNoKey(nf, NULL);
}

bool Sql::InsertNoKey(Fields nf, SqlId table) {
	return InsertNoKey(nf, (tukk )~table);
}

struct NfInsertNoNulls : public FieldOperator {
	i32    i;
	Sql   *sql;
	Txt clist;
	Txt qlist;

	virtual void Field(tukk name, Ref f) {
		if(!f.IsNull()) {
			if(clist.GetCount()) {
				clist += ", ";
				qlist += ", ";
			}
			clist << name;
			qlist << "? ";
			sql->SetParam(i++, f);
		}
	}
};

bool Sql::InsertNoNulls(Fields nf, tukk table)
{
	NfInsertNoNulls w;
	w.i = 0;
	w.sql = this;
	nf(w);
	return Execute(Txt("insert into ") + (table ? Txt(table) : w.table) +
	               '(' + w.clist + ") values(" + w.qlist + ')');
}

bool Sql::InsertNoNulls(Fields nf)
{
	return InsertNoNulls(nf, NULL);
}

bool Sql::InsertNoNulls(Fields nf, SqlId table)
{
	return InsertNoNulls(nf, (tukk )~table);
}

#define E__Updater(I)  sComma(I, list), list.Cat(c##I), list.Cat(" = ?"), SetParam(I - 1, v##I)

struct NfUpdate : public FieldOperator {
	i32    i;
	Sql   *sql;
	Txt list;
	Txt key;
	Val  keyval;

	virtual void Field(tukk name, Ref f) {
		if(i == 0) {
			key = name;
			keyval = f;
		}
		else {
			if(i > 1)
				list += ", ";
			list << name << " = ?";
			sql->SetParam(i - 1, f);
		}
		i++;
	}
};

bool Sql::Update(Fields nf, tukk table) {
	NfUpdate w;
	w.i = 0;
	w.sql = this;
	nf(w);
	SetParam(w.i - 1, w.keyval);
	return Execute(Txt ("update ") + (table ? Txt(table) : w.table) +
	               " set " + w.list + " where " + w.key + " = ?");
}

bool Sql::Update(Fields nf) {
	return Update(nf, NULL);
}

bool Sql::Update(Fields nf, SqlId table) {
	return Update(nf, (tukk )~table);
}

void Sql::SetSession(SqlSource& s) {
	Detach();
	cn = s.CreateConnection();
}

void Sql::SetError(Txt err, Txt stmt, i32 code, tukk scode, ERRORCLASS clss)
{
	GetSession().SetError(err, stmt, code, scode, clss);
}

void   Sql::ClearError()                          { GetSession().ClearError(); }

Txt Sql::GetLastError() const                  { return GetSession().GetLastError(); }
Txt Sql::GetErrorStatement() const             { return GetSession().GetErrorStatement(); }
i32    Sql::GetErrorCode() const                  { return GetSession().GetErrorCode(); }
Txt Sql::GetErrorCodeTxt() const            { return GetSession().GetErrorCodeTxt(); }
Sql::ERRORCLASS Sql::GetErrorClass() const        { return GetSession().GetErrorClass(); }
bool   Sql::WasError() const                      { return GetSession().WasError(); }

void   Sql::Begin()                               { ClearError(); GetSession().Begin(); }
void   Sql::Commit()                              { GetSession().Commit(); }
void   Sql::Rollback()                            { GetSession().Rollback(); }
i32    Sql::GetTransactionLevel()                 { return GetSession().GetTransactionLevel(); }

Txt Sql::Savepoint()                           { return GetSession().Savepoint(); }
void   Sql::RollbackTo(const Txt& savepoint)   { GetSession().RollbackTo(savepoint); }

bool   Sql::IsOpen()                              { return cn && GetSession().IsOpen(); }

void SqlConnection::Attach(Sql& sql, SqlConnection *con)
{
	sql.Attach(con); // Duck tape to fix Oci8
}

#ifndef NOAPPSQL
Sql::Sql() {
	cn = NULL;
	if(SQL.cn)
		cn = SQL.GetSession().CreateConnection();
}
#endif

Sql::Sql(SqlSource& s) {
	cn = s.CreateConnection();
}

#ifndef NOAPPSQL
Sql::Sql(tukk stmt) {
	cn = SQL.GetSession().CreateConnection();
	SetStatement(stmt);
}
#endif

Sql::Sql(tukk stmt, SqlSource& s) {
	cn = s.CreateConnection();
	SetStatement(stmt);
}

#ifndef NOAPPSQL
Sql::Sql(const SqlStatement& stmt) {
	cn = SQL.GetSession().CreateConnection();
	SetStatement(stmt);
}
#endif

Sql::Sql(const SqlStatement& stmt, SqlSource& s) {
	cn = s.CreateConnection();
	SetStatement(stmt);
}

Sql::Sql(SqlConnection *connection)
: cn(connection)
{}

void Sql::Detach()
{
	if(cn) delete cn;
	cn = NULL;
	param.Clear();
}

void Sql::Attach(SqlConnection *connection)
{
	Detach();
	cn = connection;
}

Sql::~Sql() {
	Detach();
}

#ifndef NOAPPSQL

SqlR::SqlR()
:	Sql(SQLR.GetSession()) {}

SqlR::SqlR(tukk stmt)
:	Sql(stmt, SQLR.GetSession()) {}

SqlR::SqlR(const SqlStatement& s)
:	Sql(s, SQLR.GetSession()) {}

#endif

#ifndef NOAPPSQL
void operator*=(ValMap& map, SqlSelect select)
{
	map.Clear();
	Sql sql;
	sql * select;
	while(sql.Fetch())
		map.Add(sql[0], sql[1]);
}
#endif

}
