#ifndef SQLCTRL_H
#define SQLCTRL_H

#include <drx/Sql/Sql.h>
#include <drx/CtrlLib/CtrlLib.h>


namespace drx {

i32  SqlError(tukk text, tukk error, tukk statement, bool retry = false);
i32  SqlError(tukk text, const SqlSession& session, bool retry = false);
i32  SqlError(tukk text, const Sql& sql APPSQLCURSOR, bool retry = false);

#ifdef TARGET_WIN32
i32  SqlError(HWND parent, tukk text, tukk error, tukk statement, bool retry = false);
i32  SqlError(HWND parent, tukk text, const SqlSession& session, bool retry = false);
i32  SqlError(HWND parent, tukk text, const Sql& sql APPSQLCURSOR, bool retry = false);
#endif

bool   DisplayError(const SqlSession& session, tukk msg = NULL);
bool   ShowError(SqlSession& session, tukk msg = NULL);
bool   ErrorRollback(SqlSession& session, tukk emsg = NULL);
bool   OkCommit(SqlSession& session, tukk emsg = NULL);

bool   DisplayError(const Sql& sql, tukk msg = NULL);
bool   ShowError(Sql& sql, tukk msg = NULL);
bool   ErrorRollback(Sql& sql, tukk emsg = NULL);
bool   OkCommit(Sql& sql, tukk emsg = NULL);

#ifndef NOAPPSQL
bool   DisplayError(tukk msg = NULL);
bool   ShowError(tukk msg = NULL);
bool   ErrorRollback(tukk emsg = NULL);
bool   OkCommit(tukk emsg = NULL);
#endif

void SqlLoad(MapConvert& cv, Sql& sql);
void SqlLoad(MapConvert& cv, const SqlSelect& set, SqlSession& ss APPSQLSESSION);
#ifndef NOAPPSQL
void operator*=(MapConvert& cv, const SqlSelect& set);
#endif

void SqlLoad(DropList& dl, Sql& sql);
void SqlLoad(DropList& dl, const SqlSelect& set, SqlSession& ss APPSQLSESSION);
#ifndef NOAPPSQL
void operator*=(DropList& cv, const SqlSelect& set);
#endif

class SqlOption : public Option {
public:
	virtual void   SetData(const Val& data);
	virtual Val  GetData() const;
};

class SqlNOption : public SqlOption {
	virtual Val  GetData() const;
};

class SqlArray : public ArrayCtrl {
public:
	virtual void  SetData(const Val& v);
	virtual Val GetData() const;
	virtual bool  Accept();

protected:
	virtual bool  UpdateRow();
	virtual void  RejectRow();

private:
	SqlSession *ssn;
	SqlId       table;
	Val       fkv;
	SqlId       fk;
	SqlBool     where;
	SqlSet      orderby;
	i32         querytime;
	i32         count;
	z64       offset;
	bool        lateinsert;
	bool        goendpostquery;
	bool        autoinsertid;
	bool        updatekey;

	SqlBool     GetWhere();
#ifndef NOAPPSQL
	SqlSession& Session()                                  { return ssn ? *ssn : SQL.GetSession(); }
#else
	SqlSession& Session()                                  { ASSERT(ssn); return *ssn; }
#endif

	bool        PerformInsert();
	bool        PerformDelete();

	void      Inserting();

public:
	Callback                           WhenPreQuery;
	Callback                           WhenPostQuery;
	Gate<const VecMap<Id, Val>&>  WhenFilter;

	void      StdBar(Bar& menu);
	bool      CanInsert() const;
	void      StartInsert();
	void      StartDuplicate();
	void      DoRemove();

	void      SetSession(SqlSession& _session)             { ssn = &_session; }

	void      Join(SqlId fk, ArrayCtrl& master);
	void      Join(ArrayCtrl& master);

	void      Query();
	void      AppendQuery(SqlBool where);
	void      Query(SqlBool where)                         { SetWhere(where); Query(); }
	void      ReQuery();
	void      ReQuery(SqlBool where)                       { SetWhere(where); ReQuery(); }

	void      Limit(i32 _offset, i32 _count)               { offset = _offset; count = _count; }
	void      Limit(i32 count)                             { Limit(0, count); }

	SqlArray& SetTable(SqlId _table)                       { table = _table; return *this; }
	SqlArray& SetTable(SqlId table, SqlId key)             { AddKey(key); return SetTable(table); }
	SqlArray& SetWhere(SqlBool _where)                     { where = _where; return *this;  }
	SqlArray& SetOrderBy(SqlSet _orderby)                  { orderby = _orderby; return *this; }
	SqlArray& SetOrderBy(const SqlVal& a)                  { return SetOrderBy(SqlSet(a)); }
	SqlArray& SetOrderBy(const SqlVal& a, const SqlVal& b) { return SetOrderBy(SqlSet(a, b)); }
	SqlArray& SetOrderBy(const SqlVal& a, const SqlVal& b, const SqlVal& c)
	                                                       { return SetOrderBy(SqlSet(a, b, c)); }
	SqlArray& GoEndPostQuery(bool b = true)                { goendpostquery = b; return *this; }
	SqlArray& AutoInsertId(bool b = true)                  { autoinsertid = b; return *this; }
	SqlArray& AppendingAuto()                              { Appending(); return AutoInsertId(); }
	SqlArray& UpdateKey(bool b =  true)                    { updatekey = b; return *this; }

	void      Clear();
	void      Reset();

	typedef   SqlArray CLASSNAME;

	SqlArray();
};

class SqlCtrls : public IdCtrls {
public:
	SqlCtrls& operator()(SqlId id, Ctrl& ctrl)       { Add(id, ctrl); return *this; }
	void      Table(Ctrl& dlg, SqlId table);
	SqlCtrls& operator()(Ctrl& dlg, SqlId table)     { Table(dlg, table); return *this; }
	SqlSet    Set() const;
	operator  SqlSet() const                         { return Set(); }
	void      Read(Sql& sql);
	bool      Fetch(Sql& sql);
	bool      Load(Sql& sql, SqlSelect select)       { sql * select; return Fetch(sql); }
	bool      Load(Sql& sql, SqlId table, SqlBool where);
#ifndef NOAPPSQL
	bool      Fetch()                                { return Fetch(SQL); }
	bool      Load(SqlSelect select)                 { return Load(SQL, select); }
	bool      Load(SqlId table, SqlBool where);
#endif
	void      Insert(SqlInsert& insert) const;
	void      Update(SqlUpdate& update) const;
	void      UpdateModified(SqlUpdate& update) const;
	SqlInsert Insert(SqlId table) const;
	SqlUpdate Update(SqlId table) const;
	SqlUpdate UpdateModified(SqlId table) const;

	SqlId       operator()(i32 i) const              { return item[i].id; }
	SqlId       GetKey(i32 i) const                  { return item[i].id; }

//deprecated:
	Callback  operator<<=(Callback cb);
};

class SqlDetail : public StaticRect {
public:
	virtual void  SetData(const Val& v);
	virtual Val GetData() const;
	virtual bool  Accept();

private:
	SqlSession *ssn;
	SqlCtrls    ctrls;
	SqlId       table;
	Val       fkval;
	SqlId       fk;
	bool        present;
	bool        autocreate;

#ifndef NOAPPSQL
	SqlSession& Session()                          { return ssn ? *ssn : SQL.GetSession(); }
#else
	SqlSession& Session()                          { ASSERT(ssn); return *ssn; }
#endif

	void        Query();

public:
	Callback    WhenPostQuery;

	bool        IsPresent() const                  { return present; }
	bool        Create();
	bool        Delete();

	void        SetSession(SqlSession& _session)   { ssn = &_session; }

	SqlDetail&  Add(SqlId id, Ctrl& ctrl);
	SqlDetail&  operator()(SqlId id, Ctrl& ctrl)   { return Add(id, ctrl); }

	SqlDetail&  Join(SqlId fk, ArrayCtrl& master);
	SqlDetail&  Join(ArrayCtrl& master);
	SqlDetail&  SetTable(SqlId _table)             { table = _table; return *this; }
	SqlDetail&  AutoCreate(bool b = true)          { autocreate = b; return *this; }

	void        Reset();

	SqlDetail();
};

void        SqlViewVal(const Txt& title, const Txt& value);

void        SQLCommander(SqlSession& session);
#ifndef NOAPPSQL
inline void SQLCommander() { SQLCommander(SQL.GetSession()); }
#endif
void        SQLObjectTree(SqlSession& session APPSQLSESSION);

INITIALIZE(SqlCtrl)

}

#endif
