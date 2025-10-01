#include "Oracle7.h"
#include "OciCommon.h"

/*
extern "C" {
#define dword _dword
typedef u8 text;
#include <oratypes.h>
#include <ocidfn.h>
#include <ocidem.h>
#include <ociapr.h>
#undef dword
};
*/

namespace drx {

#ifdef TARGET_WIN32
	#define DLLFILENAME "ociw32.dll"
#else
	#define DLLFILENAME "libclntsh.so"
#endif

#define DLIMODULE   OCI7
#define DLIHEADER   <drx/Oracle/Oci7.dli>
#include <drx/Core/dli_source.h>

void OCI7SetDllPath(Txt oci7_path, T_OCI7& oci7)
{
	static Txt dflt_name;
	if(IsNull(dflt_name))
		dflt_name = oci7.GetLibName();
	if(oci7_path != oci7.GetLibName())
		oci7.SetLibName(Nvl(oci7_path, dflt_name));
}

class OCI7Connection : public Link<OCI7Connection>, public OciSqlConnection {
protected:
	virtual void        SetParam(i32 i, OracleRef r);
	virtual void        SetParam(i32 i, const Val& r);
	virtual bool        Execute();
	virtual i32         GetRowsProcessed() const;
	virtual bool        Fetch();
	virtual void        GetColumn(i32 i, Ref f) const;
	virtual void        Cancel();
	virtual SqlSession& GetSession() const;
	virtual Txt      GetUser() const;
	virtual Txt      ToTxt() const;
	virtual            ~OCI7Connection();

protected:
/*
	enum {
		O_VARCHAR2 =   1,
		O_NUMBER   =   2,
		O_LONG     =   8,
		O_ROWID    =  11,
		O_DATE     =  12,
		O_RAW      =  23,
		O_LONGRAW  =  24,
		O_CHAR     =  96,
		O_MLSLABEL = 105,
	};
*/

	struct Param {
		i16          type;
		word           len;
		sb2            ind;
//		Vec<Val>  dynamic;
//		bool           is_dynamic;
///		bool           dyna_full;
//		i32            dyna_vtype;
//		i32            dyna_width;
		OCI7Connection *refcursor;
		union {
			u8      *ptr;
			u8       buffer[8];
		};

		u8 *Data()                 { return len > 8u ? ptr : buffer; }
		u8k *Data() const     { return len > 8u ? ptr : buffer; }
		bool  IsNull() const         { return ind < 0; }
		void  Free();
//		void  DynaFlush();

		Param()                      { len = 0; refcursor = 0; }
		~Param()                     { Free(); }
	};

	struct Column {
		i32     type;
		i32     len;
		u8   *data;
		sb2    *ind;
		ub2    *rl;
		ub2    *rc;
		bool    lob;

		Column()          { data = NULL; }
		~Column();
	};

 	mutable cda_def cda;

	Array<Param>   param;
	Array<Column>  column;

	Txt         longparam;
	i32            frows;
	unsigned       fetched;
	i32            fetchtime;
	i32            fi;
	bool           eof;
//	Vec<i32>    dynamic_param;
//	i32            dynamic_pos;
//	i32            dynamic_rows;

	T_OCI7&        oci7;
	Oracle7       *session;

	Param&    PrepareParam(i32 i, i32 type, i32 len, i32 reserve /*, i32 dynamic_vtype*/);
	void      SetParam(i32 i, const Txt& s);
	void      SetRawParam(i32 i, const Txt& s);
	void      SetParam(i32 i, i32 integer);
	void      SetParam(i32 i, double d);
	void      SetParam(i32 i, Date d);
	void      SetParam(i32 i, Time d);
	void      SetParam(i32 i, Sql& cursor);

	void      AddColumn(i32 type, i32 len, bool lob = false);
	void      GetColumn(i32 i, Txt& s) const;
	void      GetColumn(i32 i, i32& n) const;
	void      GetColumn(i32 i, double& d) const;
	void      GetColumn(i32 i, Date& d) const;
	void      GetColumn(i32 i, Time& t) const;

	void      SetError();
	bool      GetColumnInfo();

	void      Clear();

    OCI7Connection(Oracle7& s);

	friend class Oracle7;
};

//template MoveType<OCI7Connection::Param>;
//template MoveType<OCI7Connection::Column>;

void OCI7Connection::Param::Free() {
	if(len != (word)-1 && len > sizeof(buffer))
		delete[] ptr;
	len = 0;
}

/*
void OCI7Connection::Param::DynaFlush() {
	Val v;
	if(ind == 0) {
		u8k *p = Data();
		switch(type) {
		case SQLT_INT:
			v = *(i32k *)p;
			break;

		case SQLT_FLT:
			v = *(const double *)p;
			break;

		case SQLT_STR:
			v = Txt((tukk )p);
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
*/

OCI7Connection::Column::~Column() {
	if(data) {
		delete[] data;
		delete[] ind;
		delete[] rl;
		delete[] rc;
	}
}

OCI7Connection::Param& OCI7Connection::PrepareParam(i32 i, i32 type, i32 len, i32 reserve /*, i32 dvtype*/) {
	Param& p = param.At(i);
//	p.dyna_vtype = dvtype;
//	p.is_dynamic = (dvtype != VOID_V);
///	p.dyna_full = false;
//	p.dyna_width = len;
	p.ind = 0;
	if(p.type != type || p.len < len || reserve == 0 && p.len != len) {
		p.Free();
		parse = true;
	}
	if(!p.len) {
		p.len = len + reserve;
		if(p.len > 8)
			p.ptr = new u8[p.len];
		p.type = -1;
		parse = true;
	}
	if(p.type != type) {
		p.type = type;
		parse = true;
	}
	return p;
}

void OCI7Connection::SetParam(i32 i, const Txt& s) {
	i32 l = s.GetLength();
	Param& p = PrepareParam(i, SQLT_STR, l + 1, 256 /*, VOID_V*/);
	memcpy(p.Data(), s, l + 1);
	p.ind = l ? 0 : -1;
}

void OCI7Connection::SetRawParam(i32 i, const Txt& s) {
	PrepareParam(i, SQLT_LBI, 0, 0 /*, VOID_V*/);
	longparam = s;
}

void OCI7Connection::SetParam(i32 i, i32 integer) {
	Param& p = PrepareParam(i, SQLT_INT, sizeof(i32), 0 /*, VOID_V*/);
	*(i32 *) p.Data() = integer;
	p.ind = IsNull(integer) ? -1 : 0;
}

void OCI7Connection::SetParam(i32 i, double d) {
	Param& p = PrepareParam(i, SQLT_FLT, sizeof(double), 0 /*, VOID_V*/);
	*(double *) p.Data() = d;
	p.ind = IsNull(d) ? -1 : 0;
}

void OCI7Connection::SetParam(i32 i, Date d) {
	Param& w = PrepareParam(i, SQLT_DAT, 7, 0 /*, VOID_V*/);
	w.ind = OciEncodeDate(w.Data(), d) ? 0 : -1;
}

void OCI7Connection::SetParam(i32 i, Time t) {
	Param& w = PrepareParam(i, SQLT_DAT, 7, 0 /*, VOID_V*/);
	w.ind = OciEncodeTime(w.Data(), t) ? 0 : -1;
}

void OCI7Connection::SetParam(i32 i, OracleRef r) {
	NEVER(); // OCI7 dynamic's are currently not allowed
//	PrepareParam(i, r.GetOraType(), r.GetMaxLen(), 0, r.GetType());
}

class Oracle7RefCursorStub : public SqlSource {
public:
	Oracle7RefCursorStub(SqlConnection *cn) : cn(cn) {}

	virtual SqlConnection *CreateConnection() { return cn; }

private:
	SqlConnection         *cn;
};

void OCI7Connection::SetParam(i32 i, Sql& cursor) {
	Param& w = PrepareParam(i, SQLT_CUR, -1, 0 /*, VOID_V*/);
	w.refcursor = new OCI7Connection(*session);
	Oracle7RefCursorStub stub(w.refcursor);
	w.ptr = (u8 *)&w.refcursor -> cda;
	w.ind = 0;
}

void  OCI7Connection::SetParam(i32 i, const Val& q) {
	if(q.IsNull())
		SetParam(i, Txt(""));
	else
		switch(q.GetType()) {
		case 34:
			SetRawParam(i, SqlRaw(q));
			break;
		case TXT_V:
		case WTXT_V:
			SetParam(i, Txt(q));
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
			NEVER();
		}
}

void  OCI7Connection::AddColumn(i32 type, i32 len, bool lob) {
	Column& q = column.Add();
	q.type = type;
	q.len = len;
	q.data = new u8[frows * len];
	q.ind = new sb2[frows];
	q.rl = new ub2[frows];
	q.rc = new ub2[frows];
	q.lob = lob;
}

bool OCI7Connection::Execute() {
	i32 time = msecs();
	session->PreExec();
	i32 args = 0;
	if(parse) {
		Txt cmd;
		args = OciParse(statement, cmd, this, session);
		Cancel();
		if(oci7.oparse(&cda, (OraText *)(tukk )cmd, -1, 0, 2) != 0) {
			SetError();
			return false;
		}
		while(param.GetCount() < args)
			SetParam(param.GetCount(), Txt());
		param.Trim(args);
//		dynamic_param.Clear();
  		for(i32 i = 0; i < args; i++) {
			Param& p = param[i];
			if(p.type == SQLT_LBI) {
				Txt h = Format(":%d", i + 1);
				if(oci7.obindps(&cda, 0, (u8 *)~h, h.GetLength(), NULL,
					            max(1, longparam.GetLength()), p.type,
								0, NULL, NULL, NULL, 0, 0, 0, 0, 0, NULL, NULL, 0, 0)) {
					SetError();
					return false;
				}
			}
    		else
			if(oci7.obndrn(&cda, i + 1, (OraText *)p.Data(), p.len, p.type,
					       -1, &p.ind, NULL, -1, -1) != 0) {
				SetError();
				return false;
			}
//			if(p.is_dynamic)
//				dynamic_param.Add(i);
		}
	}
	for(;;) {
//		dword time;
//		if(session->IsTraceTime())
//			time = GetTickCount();
		oci7.oexec(&cda);
//		if(session->IsTraceTime() && session->GetTrace())
//			*session->GetTrace() << Sprintf("--------------\nexec %d ms:\n", GetTickCount() - time);
		if(cda.rc == 0)
			break;
		if(cda.rc != 3129) {
			SetError();
			longparam.Clear();
			return false;
		}
		ub4 l = longparam.GetLength();
		oci7.osetpi(&cda, OCI_LAST_PIECE, (uk )~longparam, &l);
	}

//	if(!dynamic_param.IsEmpty()) {
//		dynamic_pos = -1;
//		for(i32 i = 0; i < dynamic_param.GetCount(); i++)
//			param[dynamic_param[i]].DynaFlush();
//		dynamic_rows = param[dynamic_param[0]].dynamic.GetCount();
//	}

	if(parse) {
		if(!GetColumnInfo())
			return false;
		for(i32 i = 0; i < args; i++)
			if(param[i].refcursor && !param[i].refcursor -> GetColumnInfo())
				return false;
	}
	else {
		fetched = 0;
		fetchtime = 0;
		fi = -1;
		eof = false;
	}
	longparam.Clear();
	session->PostExec();
	if(Stream *s = session->GetTrace()) {
		if(session->IsTraceTime())
			*s << Format("----- exec %d ms\n", msecs(time));
	}
	return true;
}

bool OCI7Connection::GetColumnInfo()
{
	fetched = 0;
	fetchtime = 0;
	fi = -1;
	eof = false;
	info.Clear();
	column.Clear();
	frows = fetchrows;
	Vec<i32> tp;
	i32 i;
	for(i = 0;; i++) {
		char h[256];
		sb4 width;
		sb2 type, prec, scale, null;
		sb4 hlen = 255;
		if(oci7.odescr((cda_def *)&cda, i + 1, &width, &type, (sb1 *)h, &hlen, NULL, &prec,
				       &scale, &null))
			break;
		SqlColumnInfo& ii = info.Add();
		ii.width = width;
		ii.precision = prec;
		ii.scale = scale;
		h[hlen] = '\0';
		ii.name = h;
		ii.name = ToUpper(TrimRight(ii.name));
		ii.binary = (type == SQLT_LBI || type == SQLT_BLOB);
		tp.Add(type);
		if(type == SQLT_LBI || type == SQLT_BLOB || type == SQLT_CLOB)
			frows = 1;
	}
	for(i = 0; i < info.GetCount(); i++) {
		SqlColumnInfo& ii = info[i];
		switch(tp[i]) {
		case SQLT_NUM:
			ii.type = DOUBLE_V;
			AddColumn(SQLT_FLT, sizeof(double));
			break;
		case SQLT_DAT:
			ii.type = TIME_V;
			AddColumn(SQLT_DAT, 7);
			break;
		case SQLT_LBI:
			ii.type = TXT_V;
			AddColumn(SQLT_LBI, 4096);
			break;
		case SQLT_CLOB:
			ii.type = TXT_V;
			AddColumn(SQLT_LNG, 65520, true);
			break;
		case SQLT_BLOB:
			ii.type = TXT_V;
			AddColumn(SQLT_LBI, 65520, true);
			break;
		default:
			ii.type = TXT_V;
			AddColumn(SQLT_STR, ii.width ? ii.width + 1 : longsize);
			break;
		}
	}
	for(i = 0; i < column.GetCount(); i++) {
		Column& q = column[i];
		if(oci7.odefin(&cda, i + 1, (OraText *)q.data, q.len, q.type, -1, q.ind, NULL, -1, -1, q.rl, q.rc)) {
			SetError();
			return false;
		}
	}
	parse = false;
	return true;
}

i32 OCI7Connection::GetRowsProcessed() const {
	return cda.rpc;
}

bool OCI7Connection::Fetch() {
	ASSERT(!parse);
	if(parse || eof) return false;
//	if(!dynamic_param.IsEmpty()) // dynamic pseudo-fetch
//		return (dynamic_pos < dynamic_rows && ++dynamic_pos < dynamic_rows);
	if(frows == 1) {
		fi = 0;
		if(oci7.ofetch(&cda)) {
			if(cda.rc != 1403)
				SetError();
			return false;
		}
		fetched++;
		return true;
	}
	if(fi < 0 || fetched >= cda.rpc) {
		bool tt = session->IsTraceTime();
		i32 fstart = tt ? msecs() : 0;
		if(oci7.ofen(&cda, frows) && cda.rc != 1403) {
			eof = true;
			SetError();
			return false;
		}
		if(tt) {
			fetchtime += msecs(fstart);
			i32 added = fetched + cda.rpc;
			if(Stream *s = session->GetTrace())
				*s << NFormat("----- fetch(%d) in %d ms, %8n ms/rec, %2n rec/s\n",
					added, fetchtime,
					fetchtime / max<double>(added, 1),
					added * 1000.0 / max<double>(fetchtime, 1));
		}
		if(fetched >= cda.rpc) {
			eof = true;
			return false;
		}
		fi = 0;
	}
	else
		fi++;

	fetched++;
	return true;
}

void OCI7Connection::GetColumn(i32 i, Txt& s) const
{
//	if(!dynamic_param.IsEmpty()) {
//		s = param[dynamic_param[i]].dynamic[dynamic_pos];
//		return;
//	}
	const Column& c = column[i];
	u8k *data = c.data + fi * c.len;
	i32 ind = c.ind[fi];
	if(c.type == SQLT_STR)
		if(ind < 0)
			s = Null;
		else
			s = (tukk ) data;
	else if(c.type == SQLT_LBI || c.type == SQLT_LNG) {
		if(ind == -1)
			s = Null;
		else {
			s = Txt(data, *c.rl);
			if(!c.lob) {
				Buffer<u8> buffer(32768);
				ub4 len;
				while(!oci7.oflng(&cda, i + 1, buffer, 32768, c.type, &len, s.GetLength()) && len)
					s.Cat(buffer, len);
				if(s.GetAlloc() - s.GetLength() > 32768)
					s.Shrink();
			}
		}
	}
	else
		NEVER();
}

void OCI7Connection::GetColumn(i32 i, i32& n) const {
//	if(!dynamic_param.IsEmpty()) {
//		n = param[dynamic_param[i]].dynamic[dynamic_pos];
//		return;
//	}
	const Column& c = column[i];
	u8 *data = c.data + fi * c.len;
	if(c.ind[fi] < 0)
		n = INT_NULL;
	else
		switch(c.type) {
		case SQLT_INT:
			n = *(i32 *) data;
			break;
		case SQLT_FLT:
			n = i32(*(double *) data);
			break;
		default:
			ASSERT(0);
		}
}

void OCI7Connection::GetColumn(i32 i, double& d) const {
//	if(!dynamic_param.IsEmpty()) {
//		d = param[dynamic_param[i]].dynamic[dynamic_pos];
//		return;
//	}
	const Column& c = column[i];
	u8 *data = c.data + fi * c.len;
	if(c.ind[fi] < 0)
		d = Null;
	else
		switch(c.type) {
		case SQLT_INT:
			d = *(i32 *) data;
			break;
		case SQLT_FLT:
			d = *(double *) data;
			break;
		default:
			ASSERT(0);
		}
}

void OCI7Connection::GetColumn(i32 i, Date& d) const {
//	if(!dynamic_param.IsEmpty()) {
//		d = param[dynamic_param[i]].dynamic[dynamic_pos];
//		return;
//	}
	const Column& c = column[i];
	u8k *data = c.data + fi * c.len;
	ASSERT(c.type == SQLT_DAT);
	d = (c.ind[fi] < 0 ? Date(Null) : OciDecodeDate(data));
}

void OCI7Connection::GetColumn(i32 i, Time& t) const {
//	if(!dynamic_param.IsEmpty()) {
//		t = param[dynamic_param[i]].dynamic[dynamic_pos];
//		return;
//	}
	const Column& c = column[i];
	u8k *data = c.data + fi * c.len;
	ASSERT(c.type == SQLT_DAT);
	t = (c.ind[fi] < 0 ? Time(Null) : OciDecodeTime(data));
}

void  OCI7Connection::GetColumn(i32 i, Ref f) const {
//	if(!dynamic_param.IsEmpty()) {
//		f.SetVal(param[dynamic_param[i]].dynamic[dynamic_pos]);
//		return;
//	}
	switch(f.GetType()) {
		case TXT_V: {
			Txt s;
			GetColumn(i, s);
			f.SetVal(s);
			break;
		}
		case INT_V: {
			i32 d;
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
			Time d;
			GetColumn(i, d);
			f.SetVal(d);
			break;
		}
		case VALUE_V: {
			switch(column[i].type) {
				case SQLT_STR:
				case SQLT_LBI:
				case SQLT_LNG: {
					Txt s;
					GetColumn(i, s);
					f = Val(s);
					break;
				}
				case SQLT_INT: {
					i32 n;
					GetColumn(i, n);
					f = Val(n);
					break;
				}
				case SQLT_FLT: {
					double d;
					GetColumn(i, d);
					f = Val(d);
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

void OCI7Connection::Cancel() {
	oci7.ocan(&cda);
	parse = true;
}

void OCI7Connection::SetError() {
	i32 code = cda.rc;
	if(code == 0 || code == 1002) return;
	session->SetError(session->GetErrorMsg(code), ToTxt());
	parse = true;
}

SqlSession& OCI7Connection::GetSession() const {
	ASSERT(session);
	return *session;
}

Txt OCI7Connection::GetUser() const {
	ASSERT(session);
	return session->user;
}

Txt OCI7Connection::ToTxt() const
{
	Txt lg;
	bool quotes = false;
	i32 argn = 0;
	for(tukk q = statement; *q; q++) {
		if(*q== '\'' && q[1] != '\'')
			quotes = !quotes;
		if(!quotes && *q == '?') {
			if(argn < param.GetCount()) {
				const Param& m = param[argn++];
				if(m.type == SQLT_LBI)
					lg.Cat("<Raw data>");
				else
				if(m.IsNull())
					lg << "Null";
				else
				switch(m.type) {
				case SQLT_STR:
					lg.Cat('\'');
					lg += (tukk ) m.Data();
					lg.Cat('\'');
					break;
				case SQLT_INT:
					lg << *(i32k *) m.Data();
					break;
				case SQLT_FLT:
					lg << *(const double *) m.Data();
					break;
				case SQLT_DAT:
//					u8k *data = m.Data();
					lg << OciDecodeTime(m.Data());
					//(i32)data[3] << '.' << (i32)data[2] << '.' <<
					//	  i32(data[0] - 100) * 100 + data[1] - 100;
					break;
				}
			}
			else
				lg += "<not supplied>";
		}
		else
			lg += *q;
	}
	return lg;
}

OCI7Connection::OCI7Connection(Oracle7& s) : session(&s), oci7(s.oci7) {
	eof = true;
	memset(&cda, 0, sizeof(cda));
	CHECK(!oci7.oopen(&cda, (cda_def *) (session->lda), 0, -1, -1, 0, -1));
	LinkAfter(&s.clink);
}

void OCI7Connection::Clear() {
	if(session) {
		oci7.oclose(&cda);
		session = NULL;
	}
}

OCI7Connection::~OCI7Connection() {
	Clear();
}

bool Oracle7::Open(const Txt& connect) {
	Close();
	::memset(lda, 0, sizeof(lda));
	::memset(hda, 0, sizeof(hda));
	if(!oci7.Load()) {
		SetError(t_("Ошибка при загрузки библиотеки клиента БД OCI7 Oracle."), t_("Подключение к серверу"));
		return false;
	}
	i32 code = oci7.olog((cda_def *)lda, hda, (OraText *)(tukk )connect, -1,
	                     NULL, -1, NULL, -1, OCI_LM_DEF);
	if(code) {
		SetError(GetErrorMsg(code), t_("Подключение к серверу баз данных"));
		return false;
	}
	connected = true;
	level = 0;
	ClearError();
	user = Sql(*this).Select("USER from DUAL");
	return true;
}

Txt Oracle7::GetErrorMsg(i32 code) const {
	u8 h[520];
	*h = 0;
	Txt r;
	oci7.oerhms((cda_def *)lda, (sb2)code, (OraText *)h, 512);
	for(u8 *s = h; *s; s++)
		r.Cat(*s < ' ' ? ' ' : *s);
	r << "(code " << code << ")";
	return r;
}

SqlConnection *Oracle7::CreateConnection() {
	return new OCI7Connection(*this);
}

void Oracle7::Close() {
	SessionClose();
	while(!clink.IsEmpty()) {
		clink.GetNext()->Clear();
		clink.GetNext()->Unlink();
	}
	if(connected) {
		oci7.ologof((cda_def *)lda);
		connected = false;
	}
}

Txt Oracle7::Spn() {
	return Format("TRANSACTION_LEVEL_%d", level);
}

void   Oracle7::PreExec() {
	bool ac = level == 0 && tmode == NORMAL;
	if(autocommit != ac) {
		autocommit = ac;
		Stream *s = GetTrace();
		if(autocommit) {
			if(s) *s << "%autocommit on;\n";
			oci7.ocon((cda_def *)lda);
		}
		else {
			if(s) *s << "%autocommit off;\n";
			oci7.ocof((cda_def *)lda);
		}
	}
}

void   Oracle7::PostExec() {
}

void   Oracle7::Begin() {
	if(Stream *s = GetTrace())
		*s << user << "(OCI7) -> StartTransaction(level " << level << ")\n";
	level++;
//	ClearError();
	if(level > 1)
		Sql(*this).Execute("savepoint " + Spn());
}

void   Oracle7::Commit() {
	i32 time = msecs();
//	ASSERT(tmode == ORACLE || level > 0);
	if(level)
		level--;
//	else
//		ClearError();
	if(level == 0) {
		oci7.ocom((cda_def *)lda);
//		if(Stream *s = GetTrace())
//			*s << "%commit;\n";
	}
	if(Stream *s = GetTrace())
		*s << NFormat("----- %s (OCI7) -> Commit(level %d) %d ms\n", user, level, msecs(time));
}

void   Oracle7::Rollback() {
	ASSERT(tmode == ORACLE || level > 0);
	if(level > 1)
		Sql(*this).Execute("rollback to savepoint " + Spn());
	else {
		oci7.orol((cda_def *)lda);
		if(Stream *s = GetTrace())
			*s << "%rollback;\n";
	}
	if(level)
		level--;
//	else
//		ClearError();
	if(Stream *s = GetTrace())
		*s << user << "(OCI7) -> Rollback(level " << level << ")\n";
}

Txt Oracle7::Savepoint() {
	static dword i;
	i = (i + 1) & 0x8fffffff;
	Txt s = Sprintf("SESSION_SAVEPOINT_%d", i);
	Sql(*this).Execute("savepoint " + s);
	return s;
}

void   Oracle7::RollbackTo(const Txt& savepoint) {
	Sql(*this).Execute("rollback to savepoint " + savepoint);
}

bool   Oracle7::IsOpen() const {
	return connected;
}

Oracle7::Oracle7(T_OCI7& oci7_)
: oci7(oci7_)
{
	connected = false;
	autocommit = false;
	level = 0;
	tmode = NORMAL;
	Dialect(ORACLE);
}

Oracle7::~Oracle7() {
//	ASSERT(level == 0);
	Close();
}

Vec<Txt> Oracle7::EnumUsers()
{
	Sql cursor(*this);
	return OracleSchemaUsers(cursor);
}

Vec<Txt> Oracle7::EnumDatabases()
{
	Sql cursor(*this);
	return OracleSchemaUsers(cursor);
}

Vec<Txt> Oracle7::EnumTables(Txt database)
{
	Sql cursor(*this);
	return OracleSchemaTables(cursor, database);
}

Vec<Txt> Oracle7::EnumViews(Txt database)
{
	Sql cursor(*this);
	return OracleSchemaViews(cursor, database);
}

Vec<Txt> Oracle7::EnumSequences(Txt database)
{
	Sql cursor(*this);
	return OracleSchemaSequences(cursor, database);
}

Vec<Txt> Oracle7::EnumPrimaryKey(Txt database, Txt table)
{
	Sql cursor(*this);
	return OracleSchemaPrimaryKey(cursor, database, table);
}

Txt Oracle7::EnumRowID(Txt database, Txt table)
{
	Sql cursor(*this);
	return OracleSchemaRowID(cursor, database, table);
}

Vec<Txt> Oracle7::EnumReservedWords()
{
	return OracleSchemaReservedWords();
}

}
