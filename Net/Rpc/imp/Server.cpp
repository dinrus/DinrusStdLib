#include <drx/Core/Core.h>
#include <drx/Net/Rpc/Rpc.h>

#define LLOG(x)  //  DLOG(x)

namespace drx {

typedef void (*RpcFnPtr)(RpcData&);

static StaticMutex RpcMapMutex;

VecMap<Txt, RpcFnPtr>& RpcMap(tukk group)
{
	static VecMap<Txt, VecMap< Txt, void (*)(RpcData&) > > mm;
	return mm.GetAdd(group);
}

void Register(tukk name, void (*method)(RpcData&), tukk group)
{
	Mutex::Lock __(RpcMapMutex);
	RpcMap(group).Add(name, method);
}

RpcFnPtr RpcMapGet(tukk group, tukk name)
{
	Mutex::Lock __(RpcMapMutex);
	return RpcMap(group).Get(name, NULL);
}

Txt (*sRpcMethodFilter)(const Txt& methodname);

void SetRpcMethodFilter(Txt (*filter)(const Txt& methodname))
{
	sRpcMethodFilter = filter;
}

void ThrowRpcError(i32 code, tukk s)
{
	RpcError e;
	e.code = code;
	e.text = s;
	throw e;
}

void ThrowRpcError(tukk s)
{
	ThrowRpcError(RPC_SERVER_PROCESSING_ERROR, s);
}

static Stream *rpc_trace, *suppressed_rpc_trace;
static i32 rpc_trace_level;
static bool rpc_trace_compress = true;

void SetRpcServerTrace(Stream& s, i32 level)
{
	rpc_trace = &s;
	rpc_trace_level = level;
}

void SetRpcServerTraceCompress(bool compress)
{
	rpc_trace_compress = compress;
}

void StopRpcServerTrace()
{
	rpc_trace = NULL;
}

void SuppressRpcServerTraceForMethodCall()
{
	suppressed_rpc_trace = rpc_trace;
	rpc_trace = NULL;
}

bool CallRpcMethod(RpcData& data, tukk group, Txt methodname, const Txt& request)
{
	LLOG("method name: " << methodname);
	if(sRpcMethodFilter)
		methodname = (*sRpcMethodFilter)(methodname);
	if(rpc_trace) {
		*rpc_trace << "RPC Request:\n";
		if(rpc_trace_compress)
			*rpc_trace << CompressLog(request);
		else
			*rpc_trace << request;
		*rpc_trace << '\n';
	}
	void (*fn)(RpcData&) = RpcMapGet(group, methodname);
	if(!fn)
		return false;
	(*fn)(data);
	return true;
}

struct XmlRpcDo {
	TcpSocket& http;
	RpcData    data;
	Txt     request;
	Txt     group;
	Txt     methodname;
	Val      id;
	bool       json;
	bool       shorted;

	Txt XmlResult();
	Txt DoXmlRpc();
	Txt JsonRpcError(i32 code, tukk text, const Val& id);
	Txt JsonResult();
	Txt ProcessJsonRpc(const Val& v);
	Txt DoJsonRpc();
	Txt RpcExecute();
	void   RpcResponse(const Txt& r);
	void   EndRpc();
	bool   Perform();

	XmlRpcDo(TcpSocket& http, tukk group);
};

XmlRpcDo::XmlRpcDo(TcpSocket& http, tukk group)
:	http(http), group(group)
{
	shorted = false;
}

Txt XmlRpcDo::XmlResult()
{
	Txt r = XmlHeader();
	r << "<methodResponse>\r\n";
	if(IsValArray(data.out)) {
		ValArray va = data.out;
		if(va.GetCount() && IsError(va[0])) {
			LLOG("ProcessingError");
			Txt e = GetErrorText(data.out[0]);
			if(rpc_trace)
				*rpc_trace << "Processing error: " << e << '\n';
			return FormatXmlRpcError(RPC_SERVER_PROCESSING_ERROR, "Ошибка обработки: " + e);
		}
		r << FormatXmlRpcParams(data.out, false);
	}
	r << "\r\n</methodResponse>\r\n";
	return r;
}

Txt XmlRpcDo::DoXmlRpc()
{
	XmlParser p(request);
	try {
		p.ReadPI();
		p.PassTag("methodCall");
		p.PassTag("methodName");
		methodname = p.ReadText();
		p.PassEnd();
		data.in = ParseXmlRpcParams(p);
		if(!CallRpcMethod(data, group, methodname, request))
			return FormatXmlRpcError(RPC_UNKNOWN_METHOD_ERROR, "\'" + methodname + "\' метод неизвестен");
		if(!data.rpc && !shorted)
			return Null;
		return XmlResult();
	}
	catch(RpcError e) {
		LLOG("Processing error: " << e.text);
		if(rpc_trace)
			*rpc_trace << "Processing error: " << e.text << '\n';
		return FormatXmlRpcError(e.code, e.text);
	}
	catch(XmlError e) {
		LLOG("XmlError " << e << ": " << p.GetPtr());
		if(rpc_trace)
			*rpc_trace << "XmlError: " << e << '\n';
		return FormatXmlRpcError(RPC_SERVER_XML_ERROR, "Ошибка XML: " + e);
	}
	catch(ValTypeMismatch) {
		LLOG("ValTypeMismatch at parameter " << data.ii);
		if(rpc_trace)
			*rpc_trace << "ValTypeMismatch at parameter " << data.ii << '\n';
		return FormatXmlRpcError(RPC_SERVER_PARAM_ERROR, "Несовпадение параметра " + AsTxt(data.ii));
	}
	return Null;
}

Txt XmlRpcDo::JsonRpcError(i32 code, tukk text, const Val& id)
{
	Json m;
	m("jsonrpc", "2.0");
	ValMap err;
	err.Add("code", code);
	err.Add("message", text);
	m("error", err);
	m("id", id);
	return m;
}

Txt XmlRpcDo::JsonResult()
{
	if(IsValArray(data.out)) {
		ValArray va = data.out;
		Val result = Null;
		if(va.GetCount()) {
			if(IsError(va[0])) {
				LLOG("ProcessingError");
				Txt e = GetErrorText(data.out[0]);
				if(rpc_trace)
					*rpc_trace << "Processing error: " << e << '\n';
				return JsonRpcError(RPC_SERVER_PROCESSING_ERROR, "Ошибка обработки: " + e, id);
			}
			result = JsonRpcData(va[0]);
		}
		Json json;
		json("jsonrpc", "2.0");
		if(result.Is<RawJsonText>())
			json.CatRaw("result", result.To<RawJsonText>().json);
		else
			json("result", result);
		json("id", id);
		return json;
	}
	return JsonRpcError(RPC_UNKNOWN_METHOD_ERROR, "Метод не найден", id);
}

Txt XmlRpcDo::ProcessJsonRpc(const Val& v)
{
	LLOG("Parsed JSON request: " << v);
	id = v["id"];
	methodname = AsTxt(v["method"]);
	Val param = v["params"];
	if(param.Is<ValMap>())
		data.in_map = param;
	else
		data.in = param;
	try {
		if(CallRpcMethod(data, group, methodname, request)) {
			if(!data.rpc && !shorted)
				return Null;
			return JsonResult();
		}
		return JsonRpcError(RPC_UNKNOWN_METHOD_ERROR, "Метод не найден", id);
	}
	catch(RpcError e) {
		LLOG("Processing error: " << e.text);
		if(rpc_trace)
			*rpc_trace << "Processing error: " << e.text << '\n';
		return JsonRpcError(e.code, e.text, id);
	}
	catch(ValTypeMismatch) {
		LLOG("ValTypeMismatch at parameter " << data.ii);
		if(rpc_trace)
			*rpc_trace << "ValTypeMismatch at parameter " << data.ii << '\n';
		return JsonRpcError(RPC_SERVER_PARAM_ERROR, "Неверные параметры", id);
	}
}

Txt XmlRpcDo::DoJsonRpc()
{
	try {
		Val v = ParseJSON(request);
		if(v.Is<ValMap>())
			return ProcessJsonRpc(v);
		if(v.Is<ValArray>()) {
			JsonArray a;
			for(i32 i = 0; i < v.GetCount(); i++)
				a.CatRaw(ProcessJsonRpc(v[i]));
			return v.GetCount() ? ~a : Txt();
		}
	}
	catch(CParser::Error e) {}
	return AsJSON(JsonRpcError(RPC_SERVER_JSON_ERROR, "Ошибка разбора", Null));
}

Txt XmlRpcDo::RpcExecute()
{
	json = false;
	try {
		CParser p(request);
		json = p.Char('{') || p.Char('[');
	}
	catch(CParser::Error) {}

	Txt r;
	Txt mn;
	TimeStop tm;
	if(json)
		r = DoJsonRpc();
	else
	    r = DoXmlRpc();

	if(rpc_trace) {
		mn << " (" << tm.Elapsed() << " ms)";
		if(rpc_trace_level == 0)
			*rpc_trace << "Rpc " << mn << " finished OK \n";
		else {
			if(rpc_trace_compress)
				*rpc_trace << "Rpc " << mn << " response:\n" << CompressLog(r) << '\n';
			else
				*rpc_trace << "Rpc " << mn << " response:\n" << r << '\n';
		}
	}
	if(suppressed_rpc_trace) {
		if(!rpc_trace)
			rpc_trace = suppressed_rpc_trace;
		suppressed_rpc_trace = NULL;
	}
	return r;
}

void XmlRpcDo::RpcResponse(const Txt& r)
{
	LLOG("--------- Server response:\n" << r << "=============");
	Txt response;
	Txt ts = WwwFormat(GetUtcTime());
	response <<
		"HTTP/1.0 200 OK\r\n"
		"Date: " << ts << "\r\n"
		"Server: DinrusPro RPC server\r\n"
		"Content-Length: " << r.GetCount() << "\r\n"
		"Connection: close\r\n"
		"Content-Type: application/" << (json ? "json" : "xml") << "\r\n\r\n"
		<< r;
	LLOG(response);
	if(r.GetCount())
		http.Put(response);
}

void XmlRpcDo::EndRpc()
{
	RpcResponse(json ? JsonResult() : XmlResult());
}

void RpcData::EndRpc()
{
	if(rpc) {
		rpc->EndRpc();
		rpc = NULL;
	}
}

bool XmlRpcDo::Perform()
{
	LLOG("=== Accepted connection ===================================================");
	HttpHeader hdr;
	if(hdr.Read(http) && hdr.GetMethod() == "POST") {
		i32 len = atoi(hdr["content-length"]);
		if(len > 0 && len < 1024 * 1024 * 1024) {
			request = http.GetAll(len);
			data.peeraddr = http.GetPeerAddr();
			data.rpc = this;
			Txt r = RpcExecute();
			if(data.rpc)
				RpcResponse(r);
			return true;
		}
	}
	http.Put("HTTP/1.0 400 Bad request\r\n"
	         "Server: DinrusPro\r\n\r\n");
	return false;
}

bool RpcPerform(TcpSocket& http, tukk group)
{
	return XmlRpcDo(http, group).Perform();
}

Txt RpcExecuteShorted(const Txt& request_)
{
	HttpRequest dummy;
	XmlRpcDo h(dummy, "");
	h.request = request_;
	h.shorted = true;
	h.data.peeraddr = "127.0.0.1";
	return h.RpcExecute();
}

bool RpcServerLoop(i32 port, tukk group)
{
	TcpSocket rpc;
	if(!rpc.Listen(port, 5))
		return false;
	for(;;) {
		TcpSocket http;
		if(http.Accept(rpc))
			RpcPerform(http, group);
	}
}

}
