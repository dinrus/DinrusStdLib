#include "SqlCtrl.h"
#include "SqlDlg.h"

namespace drx {

i32 SqlError(
#ifdef TARGET_WIN32
	         HWND parent,
#endif
             tukk text, tukk error, tukk statement, bool retry) {
	WithSqlErrorLayout<TopWindow> r;
	CtrlLayoutCancel(r, t_("Ошибка базы данных"));
	r.text = text;
	r.error.AutoHideSb().SetReadOnly() <<= error;
	r.statement.AutoHideSb().SetReadOnly() <<= statement;
	r.retry.Show(retry);
	r.Acceptor(r.retry, IDRETRY);
	r.ActiveFocus(r);
#ifdef TARGET_WIN32
	if(parent) r.Open(parent);
#endif
	return r.Run();
}

i32 SqlError(
#ifdef TARGET_WIN32
	HWND parent,
#endif
	tukk text, const SqlSession& session, bool retry) {
	return SqlError(
#ifdef TARGET_WIN32
		parent,
#endif
		text, session.GetLastError(), session.GetErrorStatement(), retry);
}

i32 SqlError(
#ifdef TARGET_WIN32
	HWND parent,
#endif
	tukk text, const Sql& sql, bool retry) {
	return SqlError(
#ifdef TARGET_WIN32
		parent,
#endif
		text, sql.GetSession(), retry);
}

#ifdef TARGET_WIN32
i32  SqlError(tukk text, tukk error, tukk statement, bool retry) {
	return SqlError(HWND(NULL), text, error, statement, retry);
}

i32  SqlError(tukk text, const SqlSession& session, bool retry) {
	return SqlError(HWND(NULL), text, session, retry);
}

i32  SqlError(tukk text, const Sql& sql, bool retry) {
	return SqlError(HWND(NULL), text, sql, retry);
}
#endif

bool   DisplayError(const SqlSession& session, tukk msg) {
	if(!session.WasError()) return false;
	SqlError(msg ? msg : t_("Неудачная операция"),
			 session.GetLastError(), session.GetErrorStatement());
	return true;
}

bool   ShowError(SqlSession& session, tukk msg) {
	if(DisplayError(session, msg)) {
		session.ClearError();
		return true;
	}
	return false;
}

bool ErrorRollback(SqlSession& session, tukk msg) {
	if(!ShowError(session, msg ? msg : t_("Операция не удалась.\nОперация была отменена")))
		return false;
	session.Rollback();
	return true;
}

bool OkCommit(SqlSession& session, tukk msg) {
	if(ErrorRollback(session, msg))
		return false;
	session.Commit();
	if(ShowError(session, msg ? msg : t_("Ошибка SQL!")))
		return false;
	return true;
}

bool DisplayError(const Sql& sql, tukk msg) {
	return DisplayError(sql.GetSession(), msg);
}

bool ShowError(Sql& sql, tukk msg) {
	return ShowError(sql.GetSession(), msg);
}

bool ErrorRollback(Sql& sql, tukk emsg) {
	return ErrorRollback(sql.GetSession(), emsg);
}

bool OkCommit(Sql& sql, tukk emsg) {
	return OkCommit(sql.GetSession(), emsg);
}

#ifndef NOAPPSQL
bool   DisplayError(tukk msg)    { return DisplayError(SQL, msg); }
bool   ShowError(tukk msg)       { return ShowError(SQL, msg); }
bool   ErrorRollback(tukk emsg)  { return ErrorRollback(SQL, emsg); }
bool   OkCommit(tukk emsg)       { return OkCommit(SQL, emsg); }
#endif

}
