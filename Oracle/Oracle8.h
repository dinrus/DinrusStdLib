#ifndef ORACLE8_H
#define ORACLE8_H

#include <drx/Sql/Sql.h>
#include "OraCommon.h"
#include "OciCommon.h"

namespace drx {

#if defined(TARGET_WIN32) && defined(COMPILER_MSC)
#define __STDC__ 1
#endif

// selected definitions from OCI 8 headers
struct OCIEnv;
struct OCIServer;
struct OCISession;
struct OCISvcCtx;
struct OCIError;
struct OCILobLocator;

#define DLLFILENAME "oci.dll"
#define DLIMODULE   OCI8
#define DLIHEADER   <drx/Oracle/Oci8.dli>
#include <drx/Core/dli_header.h>

void OCI8SetDllPath(Txt oci8_path, T_OCI8& oci8 = OCI8_());

//#define dword _dword
//#include <oci.h>
//#undef dword

class OCI8Connection;

class Oracle8 : public SqlSession {
public:
	virtual void                  Begin();
	virtual void                  Commit();
	virtual void                  Rollback();

	virtual Txt                Savepoint();
	virtual void                  RollbackTo(const Txt& savepoint);

	virtual bool                  IsOpen() const;

	virtual RunScript             GetRunScript() const          { return &OraclePerformScript; }

	virtual Vec<Txt>        EnumUsers();
	virtual Vec<Txt>        EnumDatabases();
	virtual Vec<Txt>        EnumTables(Txt database);
	virtual Vec<Txt>        EnumViews(Txt database);
	virtual Vec<Txt>        EnumSequences(Txt database);
	virtual Vec<Txt>        EnumPrimaryKey(Txt database, Txt table);
	virtual Txt                EnumRowID(Txt database, Txt table);
	virtual Vec<Txt>        EnumReservedWords();

	bool                          BulkExecute(tukk stmt, const Vec< Vec<Val> >& param_rows);
	bool                          BulkExecute(const SqlStatement& stmt, const Vec< Vec<Val> >& param_rows);

public:
	T_OCI8&              oci8;

protected:
	virtual SqlConnection *CreateConnection();

private:
	bool                   AllocOciHandle(uk hp, i32 type);
	void                   FreeOciHandle(uk hp, i32 type);
	void                   SetOciError(Txt text, OCIError *errhp);

private:
	friend class OCI8Connection;
	friend class OracleBlob;
	friend class OracleClob;

	OCIEnv              *envhp;
	OCIError            *errhp;
	OCIServer           *srvhp;
	OCISession          *seshp;
	OCISvcCtx           *svchp;
	Link<OCI8Connection> clink;
	bool                 in_server;
	bool                 in_session;
	bool                 utf8_session;
	bool                 disable_utf8_mode;
	Txt               user;
	i32                  level;
	i32                  tmode;

	Txt         Spn();
	void           PostError();
	bool           StdMode()   { return  tmode == NORMAL; }

public:
	bool Login(tukk name, tukk password, tukk connect, bool use_objects, Txt *warn = NULL);
	bool Open(const Txt& connect_string, bool use_objects, Txt *warn = NULL);
	void Logoff();

	enum TransactionMode {
		NORMAL,              // autocommit at level 0, no Commit or Rollback allowed at level 0
		ORACLE               // Oracle-style Commit and Rollback at level 0
	};

	void    SetTransactionMode(i32 mode)            { tmode = mode; }
	void    DisableUtf8Mode(bool dutf8 = true)      { disable_utf8_mode = dutf8; }
	
	bool    IsUtf8Session() const                   { return utf8_session; }
	
	Oracle8(T_OCI8& oci8 = OCI8_());
	~Oracle8();
};

class OracleBlob : public BlockStream {
protected:
	virtual  void  SetStreamSize(z64 size);
	virtual  dword Read(z64 at, uk ptr, dword size);
	virtual  void  Write(z64 at, ukk data, dword size);

public:
	virtual  bool  IsOpen() const;
	virtual  void  Close();

protected:
	OCILobLocator   *locp;
	Oracle8         *session;

	void      Init(dword size);

public:
	operator  bool() const                 { return IsOpen(); }

	void      Assign(const Sql& sql, z64 blob);
	void      Assign(Oracle8& session, z64 blob);

	OracleBlob();
	OracleBlob(const Sql& sql, z64 blob);
	OracleBlob(Oracle8& session, z64 blob);
	~OracleBlob();
};

class OracleClob : public BlockStream {
protected:
	virtual  void  SetStreamSize(z64 size);
	virtual  dword Read(z64 at, uk ptr, dword size);
	virtual  void  Write(z64 at, ukk data, dword size);

public:
	virtual  bool  IsOpen() const;
	virtual  void  Close();

protected:
	OCILobLocator   *locp;
	Oracle8         *session;

	void      Init(dword size);

public:
	operator  bool() const                 { return IsOpen(); }

	void      Assign(const Sql& sql, z64 blob);
	void      Assign(Oracle8& session, z64 blob);

	WTxt   Read();
	void      Write(const WTxt& w);
	void      SetLength(i32 sz);

	OracleClob();
	OracleClob(const Sql& sql, z64 blob);
	OracleClob(Oracle8& session, z64 blob);
	~OracleClob();
};

#ifdef text
#undef text
#endif

}

#endif
