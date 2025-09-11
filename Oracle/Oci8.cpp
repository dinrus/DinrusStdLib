#include "Oracle8.h"
#include "OciCommon.h"

namespace drx {

#define LLOG(x) RLOG(x)

//#define DLLFILENAME "ora803.dll"
#ifdef TARGET_WIN32
	#define DLLFILENAME "oci.dll"
#else
	#define DLLFILENAME "libclntsh.so"
#endif

#define DLIMODULE   OCI8
#define DLIHEADER   <drx/Oracle/Oci8.dli>
#include <drx/Core/dli_source.h>

void OCI8SetDllPath(Txt oci8_path, T_OCI8& oci8)
{
	static Txt dflt_name;
	if(IsNull(dflt_name))
		dflt_name = oci8.GetLibName();
	if(oci8_path != oci8.GetLibName())
		oci8.SetLibName(Nvl(oci8_path, dflt_name));
}

static Txt OciError(T_OCI8& oci8, OCIError *errhp, i32 *code, bool utf8_session)
{
	if(code) *code = Null;
	if(!oci8) return t_("Ошибка при выполнении подключения к динамической библиотеке OCI8 Oracle.");
	if(!errhp) return t_("Неизвестная ошибка.");
	OraText errbuf[512];
	strcpy((tuk )errbuf, t_("(неизвестная ошибка)"));
	sb4 errcode;
	oci8.OCIErrorGet(errhp, 1, NULL, &errcode, errbuf, sizeof(errbuf), OCI_HTYPE_ERROR);
	if(code) *code = errcode;
	Txt out = (tukk ) errbuf;
	if(utf8_session)
		out = ToCharset(CHARSET_DEFAULT, out, CHARSET_UTF8);
	return out;
}

bool Oracle8::AllocOciHandle(uk hp, i32 type) {
	LLOG("AllocOciHandle(type " << type << "), envhp = " << FormatIntHex(envhp));
	*(duk *)hp = NULL;
	return oci8 && !oci8.OCIHandleAlloc(envhp, (duk *)hp, type, 0, NULL);
}

void Oracle8::FreeOciHandle(uk hp, i32 type) {
	LLOG("FreeOciHandle(" << FormatIntHex(hp) << ", type " << type << ")");
	if(oci8 && hp) oci8.OCIHandleFree(hp, type);
}

void Oracle8::SetOciError(Txt text, OCIError *from_errhp)
{
	i32 errcode;
	Txt msg = OciError(oci8, from_errhp, &errcode, utf8_session);
	SetError(msg, text, errcode, NULL, OciErrorClass(errcode));
}

class OCI8Connection : public Link<OCI8Connection>, public OciSqlConnection {
protected:
	virtual void        SetParam(i32 i, const Val& r);
	virtual void        SetParam(i32 i, OracleRef r);
	virtual bool        Execute();
	virtual i32         GetRowsProcessed() const;
	virtual bool        Fetch();
	virtual void        GetColumn(i32 i, Ref f) const;
	virtual void        Cancel();
	virtual SqlSession& GetSession() const;
	virtual Txt      GetUser() const;
	virtual Txt      ToTxt() const;
	virtual            ~OCI8Connection();

	bool                BulkExecute(tukk stmt, const Vec< Vec<Val> >& param_rows);

	struct Item {
		T_OCI8&        oci8;
		i32            type;
		i32            total_len;
		Buffer<ub2>    len;
		Buffer<sb2>    ind;
//		ub2            rl;
		ub2            rc;
		Vec<Val>  dynamic;
		bool           is_dynamic;
		bool           dyna_full;
		i32            dyna_vtype;
		ub4            dyna_width;
		ub4            dyna_len;
		i32            array_count;
		OCILobLocator *lob;
		OCIBind       *bind;
		OCIDefine     *define;
		OCI8Connection *refcursor;
		union {
			u8          *ptr;
			u8           buffer[8];
		};

		u8 *Data()                 { return total_len > (i32)sizeof(buffer) ? ptr : buffer; }
		u8k *Data() const     { return total_len > (i32)sizeof(buffer) ? ptr : buffer; }
		bool  IsNull() const         { return ind[0] < 0; }
		bool  Alloc(i32 count, OCIEnv *envhp, i32 type, i32 len, i32 res = 0);
		void  Clear();
		void  DynaFlush();

		static sb4 DynaIn(duk ictxp, OCIBind *bindp,
			ub4 iter, ub4 index, duk *bufpp, ub4 *alenp, ub1 *piecep, duk *indp) {
			Item *self = reinterpret_cast<Item *>(ictxp);
			ASSERT(self -> bind == bindp);
			*bufpp = NULL;
			*alenp = 0;
			*indp = NULL;
			*piecep = OCI_ONE_PIECE;
			return OCI_CONTINUE;
		}

		static sb4 DynaOut(duk octxp, OCIBind *bindp,
			ub4 iter, ub4 index, duk *bufpp, ub4 **alenp, ub1 *piecep, duk *indp, ub2 **rcodep) {
			Item *self = reinterpret_cast<Item *>(octxp);
			ASSERT(self -> bind == bindp);
			return self -> Out(iter, index, bufpp, alenp, piecep, indp, rcodep);
		}

		sb4 Out(ub4 iter, ub4 index, duk *bufpp, ub4 **alenp, ub1 *piecep, duk *indp, ub2 **rcodep);

		Item(T_OCI8& oci8);
		~Item();
	};

	Oracle8           *session;
	T_OCI8&            oci8;
	OCIStmt           *stmthp;
	OCIError          *errhp;
	Array<Item>        param;
	Array<Item>        column;
	Txt             parsed_cmd;
	Vec<i32>        dynamic_param;
	i32                dynamic_pos;
	i32                dynamic_rows;
	i32                fetched;
	i32                fetchtime;
	bool               refcursor;

	OCISvcCtx *SvcCtx() const        { ASSERT(session); return session->svchp; }

	Item&     PrepareParam(i32 i, i32 type, i32 len, i32 reserve, i32 dynamic_vtype);
	void      SetParam(i32 i, const Txt& s);
	void      SetParam(i32 i, const WTxt& s);
	void      SetParam(i32 i, i32 integer);
	void      SetParam(i32 i, z64 integer);
	void      SetParam(i32 i, double d);
	void      SetParam(i32 i, Date d);
	void      SetParam(i32 i, Time d);
	void      SetParam(i32 i, Sql& refcursor);
	void      SetRawParam(i32 i, const Txt& s);

	void      AddColumn(i32 type, i32 len);
	void      GetColumn(i32 i, Txt& s) const;
	void      GetColumn(i32 i, WTxt& s) const;
	void      GetColumn(i32 i, i32& n) const;
	void      GetColumn(i32 i, z64& n) const;
	void      GetColumn(i32 i, double& d) const;
	void      GetColumn(i32 i, Date& d) const;
	void      GetColumn(i32 i, Time& t) const;

	void      SetError();
	bool      GetColumnInfo();

	void      Clear();

	OCI8Connection(Oracle8& s);

	friend class Oracle8;
};

void OCI8Connection::Item::Clear() {
	ind.Clear();
	len.Clear();
	if(type == SQLT_BLOB || type == SQLT_CLOB)
		oci8.OCIDescriptorFree((duk )lob, OCI_DTYPE_LOB);
	else
	if(total_len > (i32)sizeof(buffer))
		delete[] ptr;
	total_len = 0;
	lob = NULL;
	dynamic.Clear();
	dyna_full = false;
	dyna_vtype = VOID_V;
	dyna_width = 0;
}

OCI8Connection::Item::Item(T_OCI8& oci8_)
: oci8(oci8_)
{
	lob = NULL;
	bind = NULL;
	define = NULL;
	total_len = 0;
	refcursor = 0;
	dyna_full = false;
	is_dynamic = false;
}

OCI8Connection::Item::~Item() {
	Clear();
}

bool OCI8Connection::Item::Alloc(i32 _count, OCIEnv *envhp, i32 _type, i32 _len, i32 res) {
	if(_type == type && total_len >= _len && array_count >= _count) return false;
	Clear();
	type = _type;
	total_len = _len + res;
	array_count = _count;
	if(type == SQLT_BLOB || type == SQLT_CLOB)
		oci8.OCIDescriptorAlloc(envhp, (duk *) &lob, OCI_DTYPE_LOB, 0, NULL);
	if(total_len > (i32)sizeof(buffer))
		ptr = new u8[total_len];
	ind.Alloc(_count);
	ind[0] = -1;
	len.Alloc(_count);
	len[0] = _len;
	return true;
}

void OCI8Connection::Item::DynaFlush() {
	if(dyna_full) {
		dyna_full = false;
		Val v;
		if(ind[0] == 0) {
			if(dyna_len > dyna_width)
				dyna_width = dyna_len;
			u8k *p = Data();
			switch(type) {
			case SQLT_INT:
				if(dyna_len == sizeof(i32))
					v = *(i32k *)p;
				else if(dyna_len == sizeof(z64))
					v = *(const z64 *)p;
				break;

			case SQLT_FLT:
				v = *(const double *)p;
				break;

			case SQLT_STR:
				v = Txt((tukk )p, dyna_len);
				break;

			case SQLT_DAT:
				v = OciDecodeTime(p);
				break;

			case SQLT_CLOB:
			case SQLT_BLOB:
			default:
				NEVER();
				break;
			}
		}
		dynamic.Add(v);
	}
}

sb4 OCI8Connection::Item::Out(ub4 iter, ub4 index, duk *bufpp, ub4 **alenp, ub1 *piecep, duk *indp, ub2 **rcodep) {
	DynaFlush();
	*bufpp = Data();
	*alenp = &dyna_len;
	dyna_len = total_len;
	*piecep = OCI_ONE_PIECE;
	ind.Alloc(1);
	ind[0] = 0;
	*indp = ind;
	rc = 0;
	*rcodep = &rc;
	dyna_full = true;
	return OCI_CONTINUE;
}

OCI8Connection::Item& OCI8Connection::PrepareParam(i32 i, i32 type, i32 len, i32 res, i32 dynamic_vtype) {
	while(param.GetCount() <= i)
		param.Add(new Item(oci8));
	Item& p = param[i];
	if(p.Alloc(1, session -> envhp, type, len, res))
		parse = true;
	p.dyna_vtype = dynamic_vtype;
	p.is_dynamic = (dynamic_vtype != VOID_V);
	return p;
}

void OCI8Connection::SetParam(i32 i, const Txt& s) {
	Txt rs = (session->utf8_session ? ToCharset(CHARSET_UTF8, s) : s);
	i32 l = rs.GetLength();
	Item& p = PrepareParam(i, SQLT_STR, l + 1, 100, VOID_V);
	memcpy(p.Data(), rs, l + 1);
	p.ind[0] = l ? 0 : -1;
}

void OCI8Connection::SetParam(i32 i, const WTxt& s) {
	Txt rs = (session->utf8_session ? ToUtf8(s) : s.ToTxt());
	i32 l = rs.GetLength();
	Item& p = PrepareParam(i, SQLT_STR, l + 1, 100, VOID_V);
	memcpy(p.Data(), rs, l + 1);
	p.ind[0] = l ? 0 : -1;
}

void OCI8Connection::SetParam(i32 i, i32 integer) {
	Item& p = PrepareParam(i, SQLT_INT, sizeof(i32), 0, VOID_V);
	*(i32 *) p.Data() = integer;
	p.ind[0] = IsNull(integer) ? -1 : 0;
}

void OCI8Connection::SetParam(i32 i, z64 integer) {
	Item& p = PrepareParam(i, SQLT_INT, sizeof(z64), 0, VOID_V);
	*(z64 *) p.Data() = integer;
	p.ind[0] = IsNull(integer) ? -1 : 0;
}

void OCI8Connection::SetParam(i32 i, double d) {
	Item& p = PrepareParam(i, SQLT_FLT, sizeof(double), 0, VOID_V);
	*(double *) p.Data() = d;
	p.ind[0] = IsNull(d) ? -1 : 0;
}

void OCI8Connection::SetParam(i32 i, Date d) {
	Item& w = PrepareParam(i, SQLT_DAT, 7, 0, VOID_V);
	w.ind[0] = (OciEncodeDate(w.Data(), d) ? 0 : -1);
}

void OCI8Connection::SetParam(i32 i, Time t) {
	Item& w = PrepareParam(i, SQLT_DAT, 7, 0, VOID_V);
	w.ind[0] = (OciEncodeTime(w.Data(), t) ? 0 : -1);
}

void OCI8Connection::SetParam(i32 i, OracleRef r) {
	PrepareParam(i, r.GetOraType(), r.GetMaxLen(), 0, r.GetType());
}

void OCI8Connection::SetRawParam(i32 i, const Txt& s) {
	i32 l = s.GetLength();
	Item& p = PrepareParam(i, SQLT_LBI, l, 0, VOID_V);
	memcpy(p.Data(), s, l);
	p.ind[0] = l ? 0 : -1;
}

/*
class Oracle8RefCursorStub : public SqlSource {
public:
	Oracle8RefCursorStub(SqlConnection *cn) : cn(cn) {}
	virtual SqlConnection *CreateConnection() { return cn; }

private:
	SqlConnection *cn;
};
*/

void OCI8Connection::SetParam(i32 i, Sql& rc) {
	Item& w = PrepareParam(i, SQLT_RSET, -1, 0, VOID_V);
	w.refcursor = new OCI8Connection(*session);
	w.refcursor -> refcursor = true;
	*(OCIStmt **)w.Data() = w.refcursor -> stmthp;
	w.ind[0] = 0;
//	Oracle8RefCursorStub stub(w.refcursor);
	Attach(rc, w.refcursor);
//	rc = Sql(stub);
}

void  OCI8Connection::SetParam(i32 i, const Val& q) {
	if(q.IsNull())
		SetParam(i, Txt());
	else
		switch(q.GetType()) {
			case SQLRAW_V:
				SetRawParam(i, SqlRaw(q));
				break;
			case TXT_V:
				SetParam(i, Txt(q));
				break;
			case WTXT_V:
				SetParam(i, WTxt(q));
				break;
			case BOOL_V:
			case INT_V:
				SetParam(i, i32(q));
				break;
			case INT64_V:
			case DOUBLE_V:
				SetParam(i, double(q));
				break;
			case DATE_V:
				SetParam(i, Date(q));
				break;
			case TIME_V:
				SetParam(i, (Time)q);
				break;
			default:
				if(IsTypeRaw<Sql *>(q)) {
					SetParam(i, *ValTo<Sql *>(q));
					break;
				}
				if(OracleRef::IsVal(q)) {
					SetParam(i, OracleRef(q));
					break;
				}
				NEVER();
		}
}

void OCI8Connection::AddColumn(i32 type, i32 len) {
	column.Add(new Item(oci8)).Alloc(1, session -> envhp, type, len);
}

bool OCI8Connection::BulkExecute(tukk stmt, const Vec< Vec<Val> >& param_rows)
{
	ASSERT(session);

	i32 time = msecs();
	i32 args = 0;
	Txt cvt_stmt = ToCharset(session->utf8_session
		? CHARSET_UTF8 : CHARSET_DEFAULT, stmt, CHARSET_DEFAULT);
	if((args = OciParse(cvt_stmt, parsed_cmd, this, session)) < 0)
		return false;

	session->statement = parsed_cmd;
	i32 nrows = param_rows.GetCount();

	if(Stream *s = session->GetTrace()) {
		*s << "BulkExecute(#" << nrows << " rows)\n";
		for(i32 r = 0; r < nrows; r++) {
			const Vec<Val>& row = param_rows[r];
			*s << "[row #" << r << "] ";
			bool quotes = false;
			i32 argn = 0;
			for(tukk q = cvt_stmt; *q; q++) {
				if(*q== '\'' && q[1] != '\'')
					quotes = !quotes;
				if(!quotes && *q == '?') {
					if(argn < row.GetCount())
						*s << SqlCompile(ORACLE, SqlFormat(row[argn++]));
					else
						*s << t_("<не прилагается>");
				}
				else
					*s << *q;
			}
			*s << "\n";
		}
		*s << "//BulkExecute\n";
	}

	if(oci8.OCIStmtPrepare(stmthp, errhp, (u8 *)~parsed_cmd, parsed_cmd.GetLength(), OCI_NTV_SYNTAX,
		OCI_DEFAULT)) {
		SetError();
		return false;
	}

	for(i32 a = 0; a < args; a++) {
		i32 max_row_len = 1;
		i32 sql_type = 0;
		for(i32 r = 0; r < nrows; r++) {
			Val v = (a < param_rows[r].GetCount() ? param_rows[r][a] : Val());
			i32 len = 0;
			i32 vt = v.GetType();
			if(!IsNull(v)) {
				if(IsNumberValTypeNo(vt)) {
					if((vt == INT_V || vt == INT64_V || vt == BOOL_V)
					&& (!sql_type || sql_type == SQLT_INT)) {
						sql_type = SQLT_INT;
						len = (vt == INT64_V ? sizeof(z64) : sizeof(i32));
					}
					else if(!sql_type || sql_type == SQLT_INT || sql_type == SQLT_FLT) {
						sql_type = SQLT_FLT;
						len = sizeof(double);
					}
					else {
						RLOG("invalid type combination in BulkExecute: " << sql_type << " <- number");
					}
				}
				else if(IsDateTime(v)) {
					if(!sql_type || sql_type == SQLT_DAT) {
						sql_type = SQLT_DAT;
						len = 7;
					}
					else {
						RLOG("invalid type combination in BulkExecute: " << sql_type << " <- date/time");
					}
				}
				else if(IsTxt(v)) {
					if(!sql_type || sql_type == SQLT_STR) {
						sql_type = SQLT_STR;
						if(session->utf8_session) {
							WTxt wstr(v);
							len = 1 + Utf8Len(wstr, wstr.GetLength());
						}
						else
							len = 1 + Txt(v).GetLength();
					}
					else {
						RLOG("invalid type combination in BulkExecute: " << sql_type << " <- string");
					}
				}
				else {
					RLOG("invalid data type: " << v.GetType());
				}
			}
			if(len > max_row_len)
				max_row_len = len;
		}

		if(sql_type == 0)
			sql_type = SQLT_STR;
		i32 sum_len = nrows * max_row_len;
		Item& p = param.Add(new Item(oci8));
		p.Alloc(nrows, session->envhp, sql_type, sum_len, 0);
		p.dyna_vtype = VOID_V;
		p.is_dynamic = false;
		sb2 *indp = p.ind;
		ub2 *lenp = p.len;

		switch(sql_type) {
			case SQLT_INT: {
				ASSERT(sum_len >= nrows * (i32)sizeof(max_row_len));
				if(max_row_len == sizeof(i32)) {
					i32 *datp = (i32 *)p.Data();
					for(i32 r = 0; r < nrows; r++) {
						i32 i = (param_rows[r].GetCount() > a ? (i32)param_rows[r][a] : (i32)Null);
						*datp++ = i;
						*indp++ = IsNull(i) ? -1 : 0;
						*lenp++ = sizeof(i32);
					}
				}
				else if(max_row_len == sizeof(z64)) {
					z64 *datp = (z64 *)p.Data();
					for(i32 r = 0; r < nrows; r++) {
						z64 i = (param_rows[r].GetCount() > a ? (z64)param_rows[r][a] : (z64)Null);
						*datp++ = i;
						*indp++ = IsNull(i) ? -1 : 0;
						*lenp++ = sizeof(z64);
					}
				}
				break;
			}
			case SQLT_FLT: {
				ASSERT(sum_len >= nrows * (i32)sizeof(double));
				double *datp = (double *)p.Data();
				for(i32 r = 0; r < nrows; r++) {
					double d = (param_rows[r].GetCount() > a ? (double)param_rows[r][a] : (double)Null);
					*datp++ = d;
					*indp++ = IsNull(d) ? -1 : 0;
					*lenp++ = sizeof(double);
				}
				break;
			}
			case SQLT_DAT: {
				ASSERT(sum_len >= nrows * 7);
				u8 *datp = p.Data();
				for(i32 r = 0; r < nrows; r++) {
					Time d = (param_rows[r].GetCount() > a ? (Time)param_rows[r][a] : (Time)Null);
					*indp++ = OciEncodeTime(datp, d) ? 0 : -1;
					datp += 7;
					*lenp++ = 7;
				}
				break;
			}
			case SQLT_STR: {
				u8 *datp = p.Data();
				for(i32 r = 0; r < nrows; r++) {
					Txt s;
					if(session->utf8_session)
						s = ToUtf8(param_rows[r].GetCount() > a ? (WTxt)param_rows[r][a] : WTxt());
					else
						s = (param_rows[r].GetCount() > a ? (Txt)param_rows[r][a] : Txt());
					*indp++ = (IsNull(s) ? -1 : 0);
					i32 rawlen = s.GetLength() + 1;
					*lenp++ = rawlen;
					memcpy(datp, s, rawlen);
					datp += max_row_len;
				}
				ASSERT((i32)(datp - (u8 *)p.Data()) <= sum_len);
				break;
			}
			default: {
				RLOG("unsupported SQL type: " << sql_type);
			}
		}

		if(oci8.OCIBindByPos(stmthp, &p.bind, errhp, a + 1, p.Data(), max_row_len, p.type,
			p.ind, p.len, NULL, nrows, NULL, OCI_DEFAULT)) {
			SetError();
			return false;
		}
	}

	if(oci8.OCIStmtExecute(SvcCtx(), stmthp, errhp, nrows, 0, NULL, NULL,
		session->StdMode() && session->level == 0 ? OCI_COMMIT_ON_SUCCESS : OCI_DEFAULT)) {
		SetError();
		session->PostError();
		return false;
	}

	if(Stream *s = session->GetTrace()) {
		if(session->IsTraceTime())
			*s << Format("----- exec %d ms\n", msecs(time));
	}
	return true;
}

bool Oracle8::BulkExecute(const SqlStatement& stmt, const Vec< Vec<Val> >& param_rows)
{
	if(param_rows.IsEmpty())
		return true;
	return BulkExecute(stmt.Get(ORACLE), param_rows);
}

bool OCI8Connection::Execute() {
	ASSERT(session);
	i32 time = msecs();
	i32 args = 0;
	if(parse) {
//		Cancel();
		Txt cvt_stmt = ToCharset(session->utf8_session
			? CHARSET_UTF8 : CHARSET_DEFAULT, statement, CHARSET_DEFAULT);
		if((args = OciParse(cvt_stmt, parsed_cmd, this, session)) < 0)
			return false;
		ub4 fr = fetchrows;

		oci8.OCIAttrSet(stmthp, OCI_HTYPE_STMT, &fr, 0, OCI_ATTR_PREFETCH_ROWS, errhp);
		if(oci8.OCIStmtPrepare(stmthp, errhp, (u8 *)~parsed_cmd, parsed_cmd.GetLength(), OCI_NTV_SYNTAX,
			              OCI_DEFAULT)) {
			SetError();
			return false;
		}

		while(param.GetCount() < args)
			SetParam(param.GetCount(), Txt());
		param.Trim(args);
		dynamic_param.Clear();
		for(i32 i = 0; i < args; i++) {
			Item& p = param[i];
			if(oci8.OCIBindByPos(stmthp, &p.bind, errhp, i + 1, p.Data(), p.total_len, p.type,
			p.ind, NULL, NULL, 0, NULL, p.is_dynamic ? OCI_DATA_AT_EXEC : OCI_DEFAULT)) {
				SetError();
				return false;
			}
			if(p.is_dynamic) {
				dynamic_param.Add(i);
				if(oci8.OCIBindDynamic(p.bind, errhp, &p, &Item::DynaIn, &p, &Item::DynaOut)) {
					SetError();
					return false;
				}
			}
		}
	}
	ub2 type;
	if(oci8.OCIAttrGet(stmthp, OCI_HTYPE_STMT, &type, NULL, OCI_ATTR_STMT_TYPE, errhp)) {
		SetError();
		return false;
	}

	if(oci8.OCIStmtExecute(SvcCtx(), stmthp, errhp, type != OCI_STMT_SELECT, 0, NULL, NULL,
		              session->StdMode() && session->level == 0 ? OCI_COMMIT_ON_SUCCESS : OCI_DEFAULT)) {
		SetError();
		session->PostError();
		return false;
	}

	if(!dynamic_param.IsEmpty()) {
		dynamic_pos = -1;
		for(i32 i = 0; i < dynamic_param.GetCount(); i++)
			param[dynamic_param[i]].DynaFlush();
		dynamic_rows = param[dynamic_param[0]].dynamic.GetCount();
	}

	if(parse) {
		if(!GetColumnInfo())
			return false;
		for(i32 i = 0; i < param.GetCount(); i++)
			if(param[i].refcursor && !param[i].refcursor -> GetColumnInfo())
				return false;
	}
	fetched = 0;
	fetchtime = 0;
	if(Stream *s = session->GetTrace()) {
		*s << ToTxt() << '\n';
		if(session->IsTraceTime())
			*s << Format("----- exec %d ms\n", msecs(time));
	}
	return true;
}

bool OCI8Connection::GetColumnInfo() {
	info.Clear();
	column.Clear();
	ub4 argcount;
	if(oci8.OCIAttrGet(stmthp, OCI_HTYPE_STMT, &argcount, 0, OCI_ATTR_PARAM_COUNT, errhp) != OCI_SUCCESS) {
		SetError();
		return false;
	}
	if(!dynamic_param.IsEmpty()) {
		for(i32 i = 0; i < dynamic_param.GetCount(); i++) {
			const Item& p = param[dynamic_param[i]];
			SqlColumnInfo& ci = info.Add();
			ci.name = Format("#%d", i + 1);
			ci.type = p.dyna_vtype;
			ci.width = p.dyna_width;
			ci.precision = Null;
			ci.scale = Null;
			ci.binary = (p.type == SQLT_LBI || p.type == SQLT_BLOB);
		}
		parse = false;
		return true;
	}
	for(ub4 i = 1; i <= argcount; i++) {
		OCIParam *pd;
		if(oci8.OCIParamGet(stmthp, OCI_HTYPE_STMT, errhp, (duk *)&pd, i) != OCI_SUCCESS) {
			SetError();
			return false;
		}
		tuk name;
		ub4 name_len;
		ub2 type, width;
		ub2 prec;
		sb1 scale;
		oci8.OCIAttrGet(pd, OCI_DTYPE_PARAM, &type, NULL,  OCI_ATTR_DATA_TYPE, errhp);
		oci8.OCIAttrGet(pd, OCI_DTYPE_PARAM, &width, NULL, OCI_ATTR_DATA_SIZE, errhp);
		oci8.OCIAttrGet(pd, OCI_DTYPE_PARAM, &name, &name_len, OCI_ATTR_NAME, errhp);
		oci8.OCIAttrGet(pd, OCI_DTYPE_PARAM, &prec, NULL,  OCI_ATTR_PRECISION, errhp);
		oci8.OCIAttrGet(pd, OCI_DTYPE_PARAM, &scale, NULL,  OCI_ATTR_SCALE, errhp);
		SqlColumnInfo& ii = info.Add();
		ii.width = width;
		ii.precision = prec;
		ii.scale = scale;
		ii.name = ToUpper(TrimRight(Txt(name, name_len)));
		ii.binary = false;
		bool blob = false;
		switch(type) {
		case SQLT_NUM:
			ii.type = DOUBLE_V;
			AddColumn(SQLT_FLT, sizeof(double));
			break;
		case SQLT_DAT:
			ii.type = TIME_V;
			AddColumn(SQLT_DAT, 7);
			break;
		case SQLT_BLOB:
			ii.type = ORA_BLOB_V;
			AddColumn(SQLT_BLOB, sizeof(OCILobLocator *));
			blob = true;
			ii.binary = true;
			break;
		case SQLT_CLOB:
			ii.type = ORA_CLOB_V;
			AddColumn(SQLT_CLOB, sizeof(OCILobLocator *));
			blob = true;
			break;
		case SQLT_RDD:
			ii.type = TXT_V;
			AddColumn(SQLT_STR, 64);
			break;
		case SQLT_TIMESTAMP: // type 187
			ii.type = TXT_V;
			AddColumn(SQLT_STR, 30); 
			break;
		default:
			ii.type = TXT_V;
			AddColumn(SQLT_STR, ii.width ? ii.width + 1 : longsize);
			break;
		}
		Item& c = column.Top();
		oci8.OCIDefineByPos(stmthp, &c.define, errhp, i,
			blob ? (uk )&c.lob : (uk )c.Data(), blob ? -1 : c.total_len,
			c.type, c.ind, c.len, NULL, OCI_DEFAULT);
	}
	parse = false;
	return true;
}

i32 OCI8Connection::GetRowsProcessed() const {
	if(!dynamic_param.IsEmpty())
		return dynamic_pos + 1;
	ub4 rp = 0;
	oci8.OCIAttrGet(stmthp, OCI_HTYPE_STMT, &rp, NULL, OCI_ATTR_ROW_COUNT, errhp);
	return rp;
}

bool OCI8Connection::Fetch() {
	ASSERT(!parse);
	if(parse) return false;
	if(!dynamic_param.IsEmpty()) // dynamic pseudo-fetch
		return (dynamic_pos < dynamic_rows && ++dynamic_pos < dynamic_rows);
	i32 fstart = msecs();
	sword status = oci8.OCIStmtFetch(stmthp, errhp, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
	bool ok = false;
	if(status == OCI_SUCCESS || status == OCI_SUCCESS_WITH_INFO) {
		fetchtime += msecs(fstart);
		++fetched;
		ok = true;
	}
	else if(status != OCI_NO_DATA)
		SetError();
	if(Stream *s = session->GetTrace()) {
		if(!ok || fetched % 100 == 0 && fetchtime)
			*s << NFormat("----- fetch(%d) in %d ms, %8n ms/rec, %2n rec/s\n",
				fetched, fetchtime,
				fetchtime / max<double>(fetched, 1),
				fetched * 1000.0 / max<double>(fetchtime, 1));
	}
	return ok;
}

void OCI8Connection::GetColumn(i32 i, Txt& s) const {
	if(!dynamic_param.IsEmpty()) {
		s = param[dynamic_param[i]].dynamic[dynamic_pos];
		return;
	}
	const Item& c = column[i];
	if(c.type == SQLT_BLOB || c.type == SQLT_CLOB) {
		if (sizeof(i32) < sizeof(uintptr_t)){ // CPU_64
			z64 handle;
			GetColumn(i, handle);
			if(!IsNull(handle)) {
				if(c.type == SQLT_CLOB && session->utf8_session) {
					OracleClob clob(*session, handle);
					s = FromUnicode(clob.Read());
				}
				else {
					OracleBlob blob(*session, handle);
					s = LoadStream(blob);
				}
			}
			else
				s = Null;
			return;
		}
		i32 handle;
		GetColumn(i, handle);
		if(!IsNull(handle)) {
			if(c.type == SQLT_CLOB && session->utf8_session) {
				OracleClob clob(*session, handle);
				s = FromUnicode(clob.Read());
			}
			else {
				OracleBlob blob(*session, handle);
				s = LoadStream(blob);
			}
		}
		else
			s = Null;
		return;
	}
	if(c.ind[0] < 0)
		s = Null;
	else {
		ASSERT(c.type == SQLT_STR);
		s = (tuk ) c.Data();
		if(session->utf8_session)
			s = ToCharset(CHARSET_DEFAULT, s, CHARSET_UTF8);
	}
}

void OCI8Connection::GetColumn(i32 i, WTxt& ws) const {
	if(!dynamic_param.IsEmpty()) {
		ws = param[dynamic_param[i]].dynamic[dynamic_pos];
		return;
	}
	const Item& c = column[i];
	if(c.type == SQLT_BLOB || c.type == SQLT_CLOB) {
		if (sizeof(i32) < sizeof(uintptr_t)){ // CPU_64
			z64 handle;
			GetColumn(i, handle);
			if(!IsNull(handle)) {
				if(session->utf8_session && c.type == SQLT_CLOB) {
					OracleClob clob(*session, handle);
					ws = clob.Read();
				}
				else {
					OracleBlob blob(*session, handle);
					Txt s = LoadStream(blob);
					ws = s.ToWTxt();
				}
			}
			else
				ws = Null;
			return;
		}
		i32 handle;
		GetColumn(i, handle);
		if(!IsNull(handle)) {
			if(session->utf8_session && c.type == SQLT_CLOB) {
				OracleClob clob(*session, handle);
				ws = clob.Read();
			}
			else {
				OracleBlob blob(*session, handle);
				Txt s = LoadStream(blob);
				ws = s.ToWTxt();
			}
		}
		else
			ws = Null;
		return;
	}
	Txt s;
	if(c.ind[0] < 0)
		s = Null;
	else {
		ASSERT(c.type == SQLT_STR);
		s = (tuk ) c.Data();
	}
	if(session->utf8_session)
		ws = ToUtf32(s);
	else
		ws = s.ToWTxt();
}

void OCI8Connection::GetColumn(i32 i, double& n) const {
	if(!dynamic_param.IsEmpty()) {
		n = param[dynamic_param[i]].dynamic[dynamic_pos];
		return;
	}
	const Item& c = column[i];
	if(c.ind[0] < 0)
		n = DOUBLE_NULL;
	else {
		ASSERT(c.type == SQLT_FLT || c.type == SQLT_BLOB || c.type == SQLT_CLOB);
		n = c.type == SQLT_BLOB || c.type == SQLT_CLOB ? (i32)(uintptr_t)c.lob : *(double *) c.Data();
	}
}

void OCI8Connection::GetColumn(i32 i, i32& n) const {
	if(!dynamic_param.IsEmpty()) {
		n = param[dynamic_param[i]].dynamic[dynamic_pos];
		return;
	}
	const Item& c = column[i];
	if(c.ind[0] < 0)
		n = Null;
	else if(c.type == SQLT_BLOB || c.type == SQLT_CLOB) {
		ASSERT(sizeof(i32) >= sizeof(uintptr_t)); // won't work in 64-bit mode
		n = (i32)(uintptr_t)c.lob;
	}
	else if(c.type == SQLT_INT) {
		if(c.len[0] == sizeof(i32))
			n = *(i32 *)c.Data();
		else if(c.len[0] == sizeof(z64))
			n = (i32)*(z64 *)c.Data();
		else {
			NEVER();
			n = Null;
		}
	}
	else if(c.type == SQLT_FLT) {
		n = (i32)*(double *)c.Data();
	}
	else {
		NEVER();
		n = Null;
	}
}

void OCI8Connection::GetColumn(i32 i, z64& n) const {
	if(!dynamic_param.IsEmpty()) {
		n = param[dynamic_param[i]].dynamic[dynamic_pos];
		return;
	}
	const Item& c = column[i];
	if(c.ind[0] < 0)
		n = Null;
	else if(c.type == SQLT_BLOB || c.type == SQLT_CLOB)
		n = (z64)(uintptr_t)c.lob;
	else if(c.type == SQLT_INT) {
		if(c.len[0] == sizeof(i32))
			n = *(i32 *)c.Data();
		else if(c.len[0] == sizeof(z64))
			n = *(z64 *)c.Data();
		else {
			NEVER();
			n = Null;
		}
	}
	else if(c.type == SQLT_FLT) {
		n = (z64)*(double *)c.Data();
	}
	else {
		NEVER();
		n = Null;
	}
}

void OCI8Connection::GetColumn(i32 i, Date& d) const {
	if(!dynamic_param.IsEmpty()) {
		d = param[dynamic_param[i]].dynamic[dynamic_pos];
		return;
	}
	const Item& c = column[i];
	if(c.ind[0] < 0)
		d = Null; // d.year = d.month = d.day = 0;
	else {
		ASSERT(c.type == SQLT_DAT);
		u8k *data = c.Data();
		d = OciDecodeDate(data);
	}
}

void OCI8Connection::GetColumn(i32 i, Time& t) const {
	if(!dynamic_param.IsEmpty()) {
		t = param[dynamic_param[i]].dynamic[dynamic_pos];
		return;
	}
	const Item& c = column[i];
	if(c.ind[0] < 0)
		t = Null; // t.year = t.month = t.day = 0;
	else {
		ASSERT(c.type == SQLT_DAT);
		u8k *data = c.Data();
		t = OciDecodeTime(data);
	}
}

void  OCI8Connection::GetColumn(i32 i, Ref f) const {
	if(!dynamic_param.IsEmpty()) {
		f.SetVal(param[dynamic_param[i]].dynamic[dynamic_pos]);
		return;
	}
	switch(f.GetType()) {
		case WTXT_V: {
			WTxt ws;
			GetColumn(i, ws);
			f.SetVal(ws);
			break;
		}
		case TXT_V: {
			Txt s;
			GetColumn(i, s);
			f.SetVal(s);
			break;
		}
		case BOOL_V:
		case INT_V: {
			i32 d;
			GetColumn(i, d);
			f.SetVal(d);
			break;
		}
		case INT64_V: {
			z64 d;
			GetColumn(i, d);
			f.SetVal(d);
			break;
		}
		case DOUBLE_V: {
			double d;
			GetColumn(i, d);
			f.SetVal(d);
			break;
		}
		case DATE_V: {
			Date d;
			GetColumn(i, d);
			f.SetVal(d);
			break;
		}
		case TIME_V: {
			Time t;
			GetColumn(i, t);
			f.SetVal(t);
			break;
		}
		case VALUE_V: {
			switch(column[i].type) {
				case SQLT_STR:
				/*case SQLT_RDD:*/ {
					Txt s;
					GetColumn(i, s);
					f.SetVal(s);
					break;
				}
				case SQLT_BLOB:
				case SQLT_CLOB:
				case SQLT_INT: {
					z64 d;
					GetColumn(i, d);
					f.SetVal(d);
					break;
				}
				case SQLT_FLT: {
					double d;
					GetColumn(i, d);
					f.SetVal(d);
					break;
				}
				case SQLT_DAT: {
					Time m;
					GetColumn(i, m);
					if(m.hour || m.minute || m.second)
						f = Val(m);
					else
						f = Val(Date(m));
					break;
				}
				default: {
					NEVER();
				}
			}
			break;
		}
		default: {
			NEVER();
		}
	}
}

void OCI8Connection::Cancel() {
	oci8.OCIBreak(SvcCtx(), errhp);
	parse = true;
}

void OCI8Connection::SetError() {
	if(session)
		session->SetOciError(statement, errhp);
	parse = true;
}

SqlSession& OCI8Connection::GetSession() const {
	ASSERT(session);
	return *session;
}

Txt OCI8Connection::GetUser() const {
	ASSERT(session);
	return session->user;
}

Txt OCI8Connection::ToTxt() const {
	Txt lg;
	bool quotes = false;
	i32 argn = 0;
	for(tukk q = statement; *q; q++) {
		if(*q== '\'' && q[1] != '\'')
			quotes = !quotes;
		if(!quotes && *q == '?') {
			if(argn < param.GetCount()) {
				const Item& m = param[argn++];
				if(m.IsNull())
					lg << "Null";
				else
				switch(m.type) {
				case SQLT_STR:
					lg.Cat('\'');
					lg += (tukk ) m.Data();
					lg.Cat('\'');
					break;
				case SQLT_FLT:
					lg << *(const double *) m.Data();
					break;
				case SQLT_DAT:
					lg << OciDecodeTime(m.Data());
//					u8k *data = m.Data();
//					lg << (i32)data[3] << '.' << (i32)data[2] << '.' <<
//						  i32(data[0] - 100) * 100 + data[1] - 100;
					break;
				}
			}
			else
				lg += t_("<не прилагается>");
		}
		else
			lg += *q;
	}
	return lg;
}

static i32 conn_count = 0;

OCI8Connection::OCI8Connection(Oracle8& s)
: session(&s)
, oci8(s.oci8)
{
	LLOG("OCI8Connection construct, #" << ++conn_count << " total");
	refcursor = false;
	if(!session->AllocOciHandle(&stmthp, OCI_HTYPE_STMT)
	|| !session->AllocOciHandle(&errhp, OCI_HTYPE_ERROR))
		session->SetError(t_("Ошибка при инициализации подключения"), t_("Подключение к OCI8"));
	LinkAfter(&s.clink);
}

void OCI8Connection::Clear() {
	if(session) {
		if(refcursor) {
			OCIStmt *aux = 0;
			if(!session -> AllocOciHandle(&aux, OCI_HTYPE_STMT)) {
				i32 errcode;
				Txt err = OciError(oci8, errhp, &errcode, session->utf8_session);
				session->SetError(err, t_("Закрытие курсора ссылки"), errcode, NULL, OciErrorClass(errcode));
			}
			static char close[] = "begin close :1; end;";
			bool err = false;
			OCIBind *bind = 0;
			err = oci8.OCIStmtPrepare(aux, errhp, (OraText *)close, (ub4)strlen(close), OCI_NTV_SYNTAX, OCI_DEFAULT)
				|| oci8.OCIBindByPos(aux, &bind, errhp, 1, &stmthp, 0, SQLT_RSET, 0, 0, 0, 0, 0, OCI_DEFAULT)
				|| oci8.OCIStmtExecute(SvcCtx(), aux, errhp, 1, 0, 0, 0, OCI_DEFAULT);
			if(err)
				SetError();
			session -> FreeOciHandle(aux, OCI_HTYPE_STMT);
		}
		session -> FreeOciHandle(stmthp, OCI_HTYPE_STMT);
		session -> FreeOciHandle(errhp, OCI_HTYPE_ERROR);
		session = NULL;
	}
}

OCI8Connection::~OCI8Connection() {
	Clear();
	LLOG("OCI8Connection destruct, #" << --conn_count << " left");
}

SqlConnection *Oracle8::CreateConnection() {
	return new OCI8Connection(*this);
}

bool   Oracle8::IsOpen() const {
	return svchp;
}

bool Oracle8::Open(const Txt& connect_string, bool use_objects, Txt *warn) {
	Txt name, pwd, server;
	tukk b = connect_string, *p = b;
	while(*p && *p != '/' && *p != '@')
		p++;
	name = Txt(b, p);
	if(*p == '/') {
		b = ++p;
		while(*p && *p != '@')
			p++;
		pwd = Txt(b, p);
	}
	if(*p == '@')
		server = ++p;
	return Login(name, pwd, server, use_objects, warn);
}

static void OCIInitError(Oracle8& ora, Txt infn)
{
	ora.Logoff();
	ora.SetError(NFormat(t_("Ошибка при инициализации библиотеки OCI8 (%s)"), infn),
		t_("Подключение к базе данных Oracle."), 0, NULL, Sql::CONNECTION_BROKEN);
}

bool Oracle8::Login(tukk name, tukk pwd, tukk db, bool use_objects, Txt *warn) {
	LLOG("Oracle8::Login");
	level = 0;
	Logoff();
	ClearError();
	user = ToUpper(Txt(name));
	LLOG("Loading OCI8 library");
	if(!oci8.Load()) {
		SetError(t_("Ошибка при загрузке динамической библиотеки подключения OCI8 Oracle."),
			t_("Подключение к базе данных Oracle."), 0, NULL, Sql::CONNECTION_BROKEN);
		return false;
	}
	LLOG("OCI8 loaded -> OCIInitialize, OCIEnvInit");
	i32 accessmode = (use_objects ? OCI_OBJECT : 0)
#if defined(_MULTITHREADED) || defined(TARGET_POSIX)
		| OCI_THREADED
#endif
	;
	
	utf8_session = false;
	if(!disable_utf8_mode
	&& oci8.OCIEnvNlsCreate) {
		if(oci8.OCIEnvNlsCreate(&envhp, accessmode, 0, 0, 0, 0, 0, 0,
		OCI_NLS_NCHARSET_ID_AL32UT8, OCI_NLS_NCHARSET_ID_AL32UT8) != OCI_SUCCESS
		&& oci8.OCIEnvNlsCreate(&envhp, accessmode, 0, 0, 0, 0, 0, 0,
		OCI_NLS_NCHARSET_ID_UT8, OCI_NLS_NCHARSET_ID_UT8) != OCI_SUCCESS)
			LLOG("OCI8: error on initialization utf8 NLS");
		else
			utf8_session = true;
	}
	if(!utf8_session) {
		if(oci8.OCIEnvCreate) {
			if(oci8.OCIEnvCreate(&envhp, accessmode, 0, 0, 0, 0, 0, 0)) {
				OCIInitError(*this, "OCIEnvCreate");
				return false;
			}
		} else {
			if(oci8.OCIInitialize(accessmode, 0, 0, 0, 0)) {
				OCIInitError(*this, "OCIInitialize");
				return false;
			}
			if(oci8.OCIEnvInit(&envhp, OCI_DEFAULT, 0, 0)) {
				OCIInitError(*this, "OCIEnvInit");
				return false;
			}
		}
	}
	if(!AllocOciHandle(&errhp, OCI_HTYPE_ERROR)) {
		OCIInitError(*this, "OCI_HTYPE_ERROR");
		return false;
	}
	if(!AllocOciHandle(&svchp, OCI_HTYPE_SVCCTX)) {
		OCIInitError(*this, "OCI_HTYPE_SVCCTX");
		return false;
	}
	if(!AllocOciHandle(&srvhp, OCI_HTYPE_SERVER)) {
		OCIInitError(*this, "OCI_HTYPE_SERVER");
		return false;
	}
	if(!AllocOciHandle(&seshp, OCI_HTYPE_SESSION)) {
		OCIInitError(*this, "OCI_HTYPE_SESSION");
		return false;
	}
	LLOG("Attributes allocated -> OCIServerAttach");
	if(oci8.OCIServerAttach(srvhp, errhp, (u8 *)db, (sb4)strlen(db), 0)) {
		SetOciError(NFormat(t_("Подключение к серверу '%s'"), db), errhp);
		Logoff();
		return false;
	}
	LLOG("Server attached -> OCIAttrSet, OCISessionBegin");
	in_server = true;
	sword retcode;
	if(oci8.OCIAttrSet(svchp, OCI_HTYPE_SVCCTX, srvhp, 0, OCI_ATTR_SERVER, errhp)
	|| oci8.OCIAttrSet(seshp, OCI_HTYPE_SESSION, (u8 *)name, (ub4)strlen(name), OCI_ATTR_USERNAME, errhp)
	|| oci8.OCIAttrSet(seshp, OCI_HTYPE_SESSION, (u8 *)pwd, (ub4)strlen(pwd), OCI_ATTR_PASSWORD, errhp)
	|| oci8.OCIAttrSet(svchp, OCI_HTYPE_SVCCTX, seshp, 0, OCI_ATTR_SESSION, errhp)
	|| (retcode = oci8.OCISessionBegin(svchp, errhp, seshp, OCI_CRED_RDBMS, OCI_DEFAULT)) != OCI_SUCCESS
	&& retcode != OCI_SUCCESS_WITH_INFO) {
		SetOciError(t_("Подключение к базе данных Oracle."), errhp);
		Logoff();
		return false;
	}
	if(retcode == OCI_SUCCESS_WITH_INFO && warn) {
		i32 errcode;
		*warn = OciError(oci8, errhp, &errcode, utf8_session);
	}
	LLOG("Session attached, user = " + GetUser());
	in_session = true;
	return true;
}

void Oracle8::Logoff() {
	SessionClose();
	LOG("Oracle8::Logoff, #" << conn_count << " connections pending");
	while(!clink.IsEmpty()) {
		clink.GetNext()->Clear();
		clink.GetNext()->Unlink();
		LOG("-> #" << conn_count << " connections left");
	}
	if(in_session)
	{
		in_session = false;
		LLOG("OCISessionEnd");
		oci8.OCISessionEnd(svchp, errhp, seshp, OCI_DEFAULT);
	}
	if(in_server)
	{
		in_server = false;
		LLOG("OCIServerDetach");
		oci8.OCIServerDetach(srvhp, errhp, OCI_DEFAULT);
	}
	FreeOciHandle(seshp, OCI_HTYPE_SESSION);
	seshp = NULL;
	FreeOciHandle(srvhp, OCI_HTYPE_SERVER);
	srvhp = NULL;
	FreeOciHandle(svchp, OCI_HTYPE_SVCCTX);
	svchp = NULL;
	FreeOciHandle(errhp, OCI_HTYPE_ERROR);
	errhp = NULL;
	if(envhp) {
		FreeOciHandle(envhp, OCI_HTYPE_ENV);
		envhp = NULL;
		if(!oci8.OCIEnvCreate)
			oci8.OCITerminate(OCI_DEFAULT);
	}
}

void   Oracle8::PostError() {
}

Txt Oracle8::Spn() {
	return Format("TRANSACTION_LEVEL_%d", level);
}

void   Oracle8::Begin() {
	if(Stream *s = GetTrace())
		*s << user << "(OCI8) -> StartTransaction(level " << level << ")\n";
	level++;
//	ClearError();
	if(level > 1)
		Sql(*this).Execute("savepoint " + Spn());
}

void   Oracle8::Commit() {
	i32 time = msecs();
	ASSERT(tmode == ORACLE || level > 0);
	if(level)
		level--;
//	else
//		ClearError();
	if(level == 0) {
		oci8.OCITransCommit(svchp, errhp, OCI_DEFAULT);
//		if(Stream *s = GetTrace())
//			*s << "%commit;\n";
	}
	if(Stream *s = GetTrace())
		*s << NFormat("----- %s (OCI8) -> Commit(level %d) %d ms\n", user, level, msecs(time));
}

void   Oracle8::Rollback() {
	ASSERT(tmode == ORACLE || level > 0);
	if(level > 1)
		Sql(*this).Execute("rollback to savepoint " + Spn());
	else {
		oci8.OCITransRollback(svchp, errhp, OCI_DEFAULT);
//		if(Stream *s = GetTrace())
//			*s << "%rollback;\n";
	}
	if(level)
		level--;
//	else
//		ClearError();
	if(Stream *s = GetTrace())
		*s << user << "(OCI8) -> Rollback(level " << level << ")\n";
}

Txt Oracle8::Savepoint() {
	static dword i;
	i = (i + 1) & 0x8fffffff;
	Txt s = Sprintf("SESSION_SAVEPOINT_%d", i);
	Sql(*this).Execute("savepoint " + s);
	return s;
}

void   Oracle8::RollbackTo(const Txt& savepoint) {
	Sql(*this).Execute("rollback to savepoint " + savepoint);
}

Oracle8::Oracle8(T_OCI8& oci8_)
: oci8(oci8_)
{
	level = 0;
	envhp = NULL;
	errhp = NULL;
	srvhp = NULL;
	seshp = NULL;
	svchp = NULL;
	tmode = NORMAL;
	disable_utf8_mode = false;
	in_session = in_server = utf8_session = false;
	Dialect(ORACLE);
}

Oracle8::~Oracle8() {
	Logoff();
}

Vec<Txt> Oracle8::EnumUsers()
{
	Sql cursor(*this);
	return OracleSchemaUsers(cursor);
}

Vec<Txt> Oracle8::EnumDatabases()
{
	Sql cursor(*this);
	return OracleSchemaUsers(cursor);
}

Vec<Txt> Oracle8::EnumTables(Txt database)
{
	Sql cursor(*this);
	return OracleSchemaTables(cursor, database);
}

Vec<Txt> Oracle8::EnumViews(Txt database)
{
	Sql cursor(*this);
	return OracleSchemaViews(cursor, database);
}

Vec<Txt> Oracle8::EnumSequences(Txt database)
{
	Sql cursor(*this);
	return OracleSchemaSequences(cursor, database);
}

Vec<Txt> Oracle8::EnumPrimaryKey(Txt database, Txt table)
{
	Sql cursor(*this);
	return OracleSchemaPrimaryKey(cursor, database, table);
}

Txt Oracle8::EnumRowID(Txt database, Txt table)
{
	Sql cursor(*this);
	return OracleSchemaRowID(cursor, database, table);
}

Vec<Txt> Oracle8::EnumReservedWords()
{
	return OracleSchemaReservedWords();
}

bool Oracle8::BulkExecute(tukk stmt, const Vec< Vec<Val> >& param_rows)
{
	if(!*stmt || param_rows.IsEmpty())
		return true;
	return OCI8Connection(*this).BulkExecute(stmt, param_rows);
}

void OracleBlob::SetStreamSize(z64 pos) {
	ASSERT(pos <= GetStreamSize());
	if(pos < (i32)GetStreamSize())
		session->oci8.OCILobTrim(session->svchp, session->errhp, locp, (i32)pos);
}

dword OracleBlob::Read(z64 at, uk ptr, dword size) {
	ASSERT(IsOpen() && (style & STRM_READ) && session);
	ASSERT(at == (dword)at);
	ub4 n = size;
	if(session->oci8.OCILobRead(session->svchp, session->errhp, locp, &n, (dword)at + 1, ptr, size,
		NULL, NULL, 0, SQLCS_IMPLICIT) != OCI_SUCCESS) return 0;
	return n;
}

void OracleBlob::Write(z64 at, ukk ptr, dword size) {
	ASSERT(IsOpen() && (style & STRM_WRITE) && session);
	ASSERT(at == (dword)at);
	if(!size)
		return;
	ub4 n = size;
	i32 res = session->oci8.OCILobWrite(session->svchp, session->errhp, locp, &n, (dword)at + 1, (uk )ptr, size,
		OCI_ONE_PIECE, NULL, NULL, 0, SQLCS_IMPLICIT);
	if(res != OCI_SUCCESS || n != size)
	{
		RLOG("OracleBlob::Write(" << at << ", " << size << "): res = " << res << ", n = " << n);
		SetError();
	}
}

void OracleBlob::Assign(Oracle8& s, z64 blob) {
	session = &s;
	locp = (OCILobLocator *)(ptrdiff_t)blob;
	ub4 n;
	OpenInit(READWRITE,
		session->oci8.OCILobGetLength(session->svchp, session->errhp, locp, &n) == OCI_SUCCESS ? n : 0);
}


void OracleBlob::Assign(const Sql& sql, z64 blob) {
	Oracle8 *session = dynamic_cast<Oracle8 *>(&sql.GetSession());
	ASSERT(session);
	Assign(*session, blob);
}

bool OracleBlob::IsOpen() const {
	return locp;
}

void OracleBlob::Close() {
	if(locp) Flush();
	locp = NULL;
}

OracleBlob::OracleBlob(const Sql& sql, z64 blob) {
	Assign(sql, blob);
}

OracleBlob::OracleBlob(Oracle8& session, z64 blob) {
	Assign(session, blob);
}

OracleBlob::OracleBlob() {
	locp = NULL;
	session = NULL;
}

OracleBlob::~OracleBlob() {
	Close();
}

void OracleClob::SetLength(i32 sz)
{
	SetStreamSize(sz << 1);
}

void OracleClob::SetStreamSize(z64 pos) {
	ASSERT(pos <= GetStreamSize());
	if(pos < (i32)GetStreamSize())
		session->oci8.OCILobTrim(session->svchp, session->errhp, locp, (i32)(pos >> 1));
}

dword OracleClob::Read(z64 at, uk ptr, dword size) {
	ASSERT(IsOpen() && (style & STRM_READ) && session);
	ASSERT(at == (dword)at);
	i32 full_bytes = 0;
	bool utf8 = session->IsUtf8Session();
	ub4 readpos = (ub4)(at >> 1);
	ub4 readsize = (ub4)(((at + size + 1) & ~1) - (readpos << 1));
	ub4 read16 = (readsize + 1) >> 1;
	ub4 bufsize = min(utf8 ? 4 * (i32)read16 : (i32)read16, 16384);
	Buffer<char> charbuf(bufsize);
	while(read16 > 0) {
		ub4 nchars = read16;
		sword res = session->oci8.OCILobRead(session->svchp, session->errhp, locp,
			&nchars, readpos + 1, charbuf, bufsize,
			NULL, NULL, 0, SQLCS_IMPLICIT);
		if(res != OCI_SUCCESS && res != OCI_NEED_DATA)
			return full_bytes;
		WTxt unibuf;
		if(utf8)
			unibuf = ToUtf32(charbuf, nchars);
		else
			unibuf = ToUnicode(charbuf, nchars, CHARSET_DEFAULT);
		i32 ulen = unibuf.GetLength();
		i32 uoff = (i32)(at & 1);
		i32 upart = min((i32)size, 2 * ulen - uoff);
		memcpy(ptr, (u8k *)~unibuf + uoff, upart);
		ptr = (u8 *)ptr + upart;
		at += upart;
		size -= upart;
		read16 -= ulen;
		full_bytes += upart;
		if(res != OCI_NEED_DATA)
			break;
	}
	return full_bytes;
}

void OracleClob::Write(z64 at, ukk ptr, dword size) {
	ASSERT(IsOpen() && (style & STRM_WRITE) && session);
	ASSERT(at == (dword)at);
	if(!size)
		return;
	bool utf8 = session->IsUtf8Session();
	if(at & 1) {
		char auxbuf[2];
		Read(at - 1, auxbuf, 1);
		auxbuf[1] = *(u8 *)ptr;
		word wch = Peek16le(auxbuf);
		Txt chrbuf = (utf8 ? ToUtf8(wch) : Txt(FromUnicode(wch, CHARSET_DEFAULT), 1));
		ub4 n = chrbuf.GetLength();
		sword res = session->oci8.OCILobWrite(session->svchp, session->errhp, locp,
			&n, (dword)(at >> 1) + 1, (duk )chrbuf.Begin(), chrbuf.GetLength(),
			OCI_ONE_PIECE, NULL, NULL, 0, SQLCS_IMPLICIT);
		if(res != OCI_SUCCESS || n != 1) {
			SetError();
			return;
		}
		
		ptr = (u8 *)ptr + 1;
		size--;
		at++;
	}
	
	if(size >> 1) {
		ub4 nchars = size >> 1;
		Txt chrbuf;
		if(utf8)
			chrbuf = ToUtf8((wtukk )ptr, nchars);
		else
			chrbuf = FromUnicode((wtukk )ptr, nchars, CHARSET_DEFAULT);
		ub4 n = chrbuf.GetLength();
		sword res = session->oci8.OCILobWrite(session->svchp, session->errhp, locp,
			&n, (dword)(at >> 1) + 1, (duk )chrbuf.Begin(), chrbuf.GetLength(),
			OCI_ONE_PIECE, NULL, NULL, 0, SQLCS_IMPLICIT);
		if(res != OCI_SUCCESS || n != nchars) {
			SetError();
			return;
		}
		ptr = (u8 *)ptr + (nchars << 1);
		at += (nchars << 1);
		size -= (nchars << 1);
	}
	
	if(size & 1) {
		char auxbuf[2];
		ub4 pos16 = (ub4)(at >> 1);
		Read(at, auxbuf + 1, 1);
		auxbuf[0] = *((u8 *)ptr + size - 1);
		word wch = Peek16le(auxbuf);
		Txt chrbuf = (utf8 ? ToUtf8(wch) : Txt(FromUnicode(wch, CHARSET_DEFAULT), 1));
		ub4 n = chrbuf.GetLength();
		sword res = session->oci8.OCILobWrite(session->svchp, session->errhp, locp,
			&n, pos16 + 1, (duk )chrbuf.Begin(), chrbuf.GetLength(),
			OCI_ONE_PIECE, NULL, NULL, 0, SQLCS_IMPLICIT);
		if(res != OCI_SUCCESS || n != 1) {
			SetError();
			return;
		}
	}
}

WTxt OracleClob::Read()
{
	WTxtBuf out;
	i32 nchars = (i32)(GetLeft() >> 1);
	out.SetCount(nchars);
	Seek(0);
	Stream::Get(out, 2 * nchars);
	return WTxt(out);
}

void OracleClob::Write(const WTxt& w)
{
	Put(w, 2 * w.GetLength());
}

void OracleClob::Assign(Oracle8& s, z64 blob) {
	session = &s;
	locp = (OCILobLocator *)(ptrdiff_t)blob;
	ub4 n;
	OpenInit(READWRITE,
		session->oci8.OCILobGetLength(session->svchp, session->errhp, locp, &n) == OCI_SUCCESS
		? 2 * n : 0);
}


void OracleClob::Assign(const Sql& sql, z64 blob) {
	Oracle8 *session = dynamic_cast<Oracle8 *>(&sql.GetSession());
	ASSERT(session);
	Assign(*session, blob);
}

bool OracleClob::IsOpen() const {
	return locp;
}

void OracleClob::Close() {
	if(locp) Flush();
	locp = NULL;
}

OracleClob::OracleClob(const Sql& sql, z64 blob) {
	Assign(sql, blob);
}

OracleClob::OracleClob(Oracle8& session, z64 blob) {
	Assign(session, blob);
}

OracleClob::OracleClob() {
	locp = NULL;
	session = NULL;
}

OracleClob::~OracleClob() {
	Close();
}

}
