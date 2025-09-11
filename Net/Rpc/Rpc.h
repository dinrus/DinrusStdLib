// NOTE: Because of clash with Windows rpc.h, Core/Core.h has to be included before this header
// For the same reason implementation files are in 'imp' subdirectory

#ifndef Rpc_Rpc_h
#define Rpc_Rpc_h

namespace drx {

enum {
	RPC_SERVER_JSON_ERROR = -32700,    // Parse error
	RPC_SERVER_JSON_REQUEST_ERROR = -32600, // Invalid Request
	RPC_UNKNOWN_METHOD_ERROR = -32601, // Method not found
	RPC_SERVER_PARAM_ERROR = -32602, // Invalid params
	RPC_SERVER_PROCESSING_ERROR = -32000, // Server error

	RPC_SERVER_XML_ERROR,

	RPC_CLIENT_HTTP_ERROR = -1000000,
	RPC_CLIENT_XML_ERROR,
	RPC_CLIENT_JSON_ERROR,
	RPC_CLIENT_RESPONSE_ERROR,
};

struct ValTypeMismatch {};

Txt FormatIso8601(Time t);
Time   ScanIso8601(const Txt& p);
Val  JsonRpcData(const Val& v);

struct RawJsonText {
	Txt json;
};

void ValCheck(bool b);

void ValGet(i32& n, const Val& v);
void ValGet(z64& n, const Val& v);
void ValGet(bool& b, const Val& v);
void ValGet(Txt& s, const Val& v);
void ValGet(double& x, const Val& v);
void ValGet(Date& x, const Val& v);
void ValGet(Time& x, const Val& v);
void ValGet(Val& t, const Val& v);
void ValGet(ValArray& va, const Val& v);
void ValGet(ValMap& vm, const Val& v);

template <class T>
void ValGet(Array<T>& x, const Val& v)
{
	ValCheck(IsValArray(v));
	ValArray va = v;
	x.SetCount(va.GetCount());
	for(i32 i = 0; i < va.GetCount(); i++)
		ValGet(x[i], va[i]);
}

template <class T>
void ValGet(Vec<T>& x, const Val& v)
{
	ValCheck(IsValArray(v));
	ValArray va = v;
	x.SetCount(va.GetCount());
	for(i32 i = 0; i < va.GetCount(); i++)
		ValGet(x[i], va[i]);
}

template <class T>
void ValGet(ArrayMap<Txt, T>& x, const Val& v)
{
	ValCheck(IsValMap(v));
	ValMap vm = v;
	const Index<Val>& k = vm.GetKeys();
	ValArray va = vm.GetVals();
	x.Clear();
	for(i32 i = 0; i < k.GetCount(); i++)
		ValGet(x.Add(k[i]), va[i]);
}

template <class T>
void ValGet(VecMap<Txt, T>& x, const Val& v)
{
	ValCheck(IsValMap(v));
	ValMap vm = v;
	const Index<Val>& k = vm.GetKeys();
	ValArray va = vm.GetVals();
	x.Clear();
	for(i32 i = 0; i < k.GetCount(); i++)
		ValGet(x.Add(k[i]), va[i]);
}

template <class T>
void ValGet(ArrayMap<i32, T>& x, const Val& v)
{
	ValCheck(IsValMap(v));
	ValMap vm = v;
	const Index<Val>& k = vm.GetKeys();
	ValArray va = vm.GetVals();
	x.Clear();
	for(i32 i = 0; i < k.GetCount(); i++)
		ValGet(x.Add(atoi(AsTxt(k[i]))), va[i]);
}

template <class T>
void ValGet(VecMap<i32, T>& x, const Val& v)
{
	ValCheck(IsValMap(v));
	ValMap vm = v;
	const Index<Val>& k = vm.GetKeys();
	ValArray va = vm.GetVals();
	x.Clear();
	for(i32 i = 0; i < k.GetCount(); i++)
		ValGet(x.Add(atoi(AsTxt(k[i]))), va[i]);
}

void ValPut(Val& v, i32 n);
void ValPut(Val& v, z64 n);
void ValPut(Val& v, const Txt& s);
void ValPut(Val& v, tukk s);
void ValPut(Val& v, double x);
void ValPut(Val& v, bool x);
void ValPut(Val& v, const Date& x);
void ValPut(Val& v, const Time& x);
void ValPut(Val& v, const Val& t);
void ValPut(Val& v, const ValArray& va);
void ValPut(Val& v, const ValMap& vm);
void ValPut(Val& v, const Json& json);

template <class T>
Val AsXmlRpcVal(const T& x)
{
	Val vs;
	ValPut(vs, x);
	return vs;
}

template <class T>
void ValPut(Val& v, const Array<T>& x)
{
	ValArray va;
	for(i32 i = 0; i < x.GetCount(); i++)
		va.Add(AsXmlRpcVal(x[i]));
	v = va;
}

template <class T>
void ValPut(Val& v, const Vec<T>& x)
{
	ValArray va;
	for(i32 i = 0; i < x.GetCount(); i++)
		va.Add(AsXmlRpcVal(x[i]));
	v = va;
}

template <class T>
void ValPut(Val& v, const ArrayMap<Txt, T>& x)
{
	ValMap vm;
	for(i32 i = 0; i < x.GetCount(); i++)
		if(!x.IsUnlinked(i))
			vm.Add(x.GetKey(i), AsXmlRpcVal(x[i]));
	v = vm;
}

template <class T>
void ValPut(Val& v, const VecMap<Txt, T>& x)
{
	ValMap vm;
	for(i32 i = 0; i < x.GetCount(); i++)
		if(!x.IsUnlinked(i))
			vm.Add(x.GetKey(i), AsXmlRpcVal(x[i]));
	v = vm;
}

template <class T>
void ValPut(Val& v, const ArrayMap<i32, T>& x)
{
	ValMap vm;
	for(i32 i = 0; i < x.GetCount(); i++)
		if(!x.IsUnlinked(i))
			vm.Add(AsTxt(x.GetKey(i)), AsXmlRpcVal(x[i]));
	v = vm;
}

template <class T>
void ValPut(Val& v, const VecMap<i32, T>& x)
{
	ValMap vm;
	for(i32 i = 0; i < x.GetCount(); i++)
		if(!x.IsUnlinked(i))
			vm.Add(AsTxt(x.GetKey(i)), AsXmlRpcVal(x[i]));
	v = vm;
}

Val      ParseXmlRpcVal(XmlParser& p);
Val      ParseXmlRpcParam(XmlParser& p);
ValArray ParseXmlRpcParams(XmlParser& p);

struct RpcGet {
	Val v;
	
	template <class T>
	operator T() const { T x; ValGet(x, v); return x; }

	template <class T>
	T Get() const { T x; ValGet(x, v); return x; } // Ugly workaround for MSC compiler bug

	Txt ToTxt() const { return v.ToTxt(); }
};

struct XmlRpcDo;

struct RpcData {
	Txt     peeraddr;
	ValArray in;
	i32        ii;
	ValMap   in_map;
	ValArray out;
	ValMap   out_map;

	i32      GetRemainingCount() const  { return in.GetCount() - ii; }
	bool     HasMore() const            { return GetRemainingCount() > 0; }

	Val Get()                         { if(ii >= in.GetCount()) return Val(); return in[ii++]; }
	
	template <class T>
	RpcData& operator>>(T& x)           { ValGet(x, Get()); return *this; }
	
	RpcGet   operator++(i32)            { RpcGet h; h.v = Get(); return h; }
	
	RpcGet   operator[](tukk id) { RpcGet h; h.v = in_map[id]; return h; }
	
	template <class T>
	RpcData& operator<<(const T& x)     { ASSERT(out_map.GetCount() == 0); Val v; ValPut(v, x); out.Add(v); return *this; }

	template <class T>
	void        Set(i32 i, const T& x)  { ASSERT(out_map.GetCount() == 0); Val v; ValPut(v, x); out.Set(i, v); }

	template <class T>
	void operator=(const T& x)          { out.Clear(); operator<<(x); }

	void        Reset()                 { in.Clear(); out.Clear(); ii = 0; }
	
	void        EndRpc();

	RpcData() { ii = 0; rpc = NULL; }

private:
	friend struct XmlRpcDo;

	XmlRpcDo *rpc;
};

Txt FormatXmlRpcVal(const Val& _v, bool supports_i8);
Txt FormatXmlRpcParam(const Val& param, bool supports_i8);
Txt FormatXmlRpcParams(const ValArray& params, bool supports_i8);

Txt FormatXmlRpcError(i32 code, tukk text);

void   Register(tukk name, void (*method)(RpcData&), tukk group = NULL);

#define RPC_METHOD(name) void name(RpcData& rpc); INITBLOCK { Register(#name, name); } void name(RpcData& rpc)
#define RPC_GMETHOD(name, group) void name(RpcData& rpc); INITBLOCK { Register(#name, name, group); } void name(RpcData& rpc)

struct RpcError {
	i32    code;
	Txt text;
};

void   SetRpcMethodFilter(Txt (*filter)(const Txt& methodname));
bool   RpcPerform(TcpSocket& http, tukk group);
bool   RpcServerLoop(i32 port, tukk group = NULL);

void   ThrowRpcError(i32 code, tukk s);
void   ThrowRpcError(tukk s);

class RpcRequest : public HttpRequest {
	bool       shorted;
	RpcData    data;
	Txt     method;
	Txt     error;
	Txt     faultTxt;
	i32        faultCode;
	bool       shouldExecute;
	bool       json, notification;
	bool       supports_i8;
	Txt     protocol_version;
	void       Init();

public:
	RpcRequest& Method(tukk name);

	template <class T>
	RpcRequest& operator<<(const T& x)             { data << x; return *this; }
	template <class T>
	void        Set(i32 i, const T& x)             { data.Set(i, x); }
	template <class T>
	RpcRequest& Named(tukk id, const T& x)  { data.out_map.Add(id, x); return *this; }

	RpcGet      Execute();
	RpcGet      Retry();

	template <class T>
	bool operator>>(T& x)                          { if(Execute().v.IsError()) return false;
	                                                 try { data >> x; } catch(ValTypeMismatch) { return false; } return true; }

	RpcRequest& operator()(tukk method)     { Method(method); return *this; }

#define E__Templ(I)  class COMBINE(T, I)
#define E__Decl(I)   const COMBINE(T, I)& COMBINE(p, I)
#define E__Param(I)  *this << COMBINE(p, I)
#define E__Body(I) \
	template <__List##I(E__Templ)> \
	RpcRequest& operator()(tukk method, __List##I(E__Decl)) { \
		Method(method); \
		__List##I(E__Param); \
		return *this; \
	}

	__Expand20(E__Body)

#undef E__Templ
#undef E__Decl
#undef E__Param
#undef E__Body

	Txt GetFaultTxt() const                               { return faultTxt; }
	i32    GetFaultCode() const                                 { return faultCode; }
	Txt GetError() const                                     { return error; }
	Txt GetMethod() const                                    { return method; }
	void   ClearError();
	
	RpcRequest& Url(tukk url);
	
	RpcRequest& JsonRpc()                                       { json = true; return *this; }
	RpcRequest& Notification()                                  { notification = true; return *this; }
	RpcRequest& SupportsI8()                                    { supports_i8 = true; protocol_version = "2.1"; return *this; }
	
	RpcRequest(tukk url);
	RpcRequest();
};

struct XmlRpcRequest : RpcRequest {
	XmlRpcRequest(tukk url) : RpcRequest(url) {}
	XmlRpcRequest() {}
};

struct JsonRpcRequest : RpcRequest {
	JsonRpcRequest(tukk url) : RpcRequest(url) { JsonRpc(); }
	JsonRpcRequest() { JsonRpc(); }
};

struct JsonRpcRequestNamed : RpcRequest {
	template <class T>
	JsonRpcRequestNamed& operator()(tukk id, const T& x)   { Named(id, x); return *this; }
	JsonRpcRequestNamed& operator()(tukk method)           { Method(method); return *this; }
	
	JsonRpcRequestNamed(tukk url) : RpcRequest(url)        { JsonRpc(); }
	JsonRpcRequestNamed()                                         { JsonRpc(); }
};

void LogRpcRequests(bool b = true);
void LogRpcRequestsCompress(bool b);

void SetRpcServerTrace(Stream& s, i32 level = 1);
void SetRpcServerTraceCompress(bool compress);
void StopRpcServerTrace();
void SuppressRpcServerTraceForMethodCall();

#include "legacy.h"

}

#endif
