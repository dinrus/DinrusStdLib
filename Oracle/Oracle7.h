#ifndef ORACLE7_H
#define ORACLE7_H

#include <drx/Sql/Sql.h>
#include "OraCommon.h"
#include "OciCommon.h"

namespace drx {

#define DLLFILENAME "ociw32.dll"
#define DLIMODULE   OCI7
#define DLIHEADER   <drx/Oracle/Oci7.dli>
#include <drx/Core/dli_header.h>

class OCI7Connection;

void OCI7SetDllPath(Txt oci7_path, T_OCI7& oci7 = OCI7_());

class Oracle7 : public SqlSession {
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

public:
	T_OCI7&                       oci7;

protected:
	virtual SqlConnection        *CreateConnection();

protected:
	friend class OCI7Connection;

	Link<OCI7Connection> clink;
	bool                 connected;
	u8                 lda[256];
	u8                 hda[512];
	Txt               user;
	i32                  level;
	bool                 autocommit;
	i32                  tmode;

	Txt         Spn();
	void           SetSp();
	Txt         GetErrorMsg(i32 code) const;
	void           PreExec();
	void           PostExec();

public:
	bool Open(const Txt& s);
	void Close();

	enum TransactionMode {
		NORMAL,              // autocommit at level 0, no Commit or Rollback allowed at level 0
		ORACLE               // Oracle-style Commit and Rollback at level 0
	};

	void    SetTransactionMode(i32 mode)            { tmode = mode; }

	Oracle7(T_OCI7& oci7 = OCI7_());
	~Oracle7();
};

typedef Oracle7 OracleSession;

}

#endif
