#include <drx/Core/Core.h>
#include <drx/Net/Rpc/Rpc.h>

namespace drx {

#define LLOG(x)  // LOG(x)

static bool sLogRpcCalls;
static bool sLogRpcCallsCompress = true;

void LogRpcRequests(bool b)
{
	sLogRpcCalls = b;
}

void LogRpcRequestsCompress(bool b)
{
	sLogRpcCallsCompress = b;
}

RpcRequest& RpcRequest::Url(tukk url)
{
	shorted = true;
	if(url && *url) {
		HttpRequest::Url(url);
		shorted = false;
	}
	shouldExecute = true;
	return *this;
}

RpcRequest& RpcRequest::Method(tukk name)
{
	shouldExecute = true;
	method = name;
	data.Reset();
	error.Clear();
	return *this;
}

void RpcRequest::Init()
{
	ContentType("text/xml");
	RequestTimeout(30000);
	MaxRetries(0);
	json = false;
	supports_i8 = false;
}

RpcRequest::RpcRequest(tukk url)
{
	Url(url);
	Init();
}

RpcRequest::RpcRequest()
{
	Url(NULL);
	Init();
}

Txt XmlRpcExecute(const Txt& request, tukk group, tukk peeraddr, bool& json);
Txt XmlRpcExecute(const Txt& request, tukk group, tukk peeraddr);

RpcGet RpcRequest::Retry()
{
	ClearError();
	shouldExecute = true;
	return Execute();
}

Val JsonRpcData(const Val& v)
{
	if(IsDateTime(v) && !IsNull(v))
		return FormatIso8601(v);
	return v;
}

Txt RpcExecuteShorted(const Txt& request_);

RpcGet RpcRequest::Execute()
{
	if(!shouldExecute)
		return RpcGet();
	shouldExecute = false;
	Txt request;
	if(json) {
		ContentType("application/json");
		static Atomic id;
		Json json;
		json("jsonrpc", "2.0")
		    ("method", method);
		if(data.out.GetCount()) {
			JsonArray a;
			for(i32 i = 0; i < data.out.GetCount(); i++) {
				const Val& v = data.out[i];
				if(v.Is<RawJsonText>())
					a.CatRaw(v.To<RawJsonText>().json);
				else
					a << JsonRpcData(v);
			}
			json("params", a);
		}
		else
		if(data.out_map.GetCount()) {
			Json m;
			for(i32 i = 0; i < data.out_map.GetCount(); i++) {
				const Val& v = data.out_map.GetVal(i);
				Txt key = (Txt)data.out_map.GetKey(i);
				if(v.Is<RawJsonText>())
					m.CatRaw(key, v.To<RawJsonText>().json);
				else
					m(key, JsonRpcData(v));
			}
			json("params", m);
		}
		json("id", id);
		AtomicInc(id);
		request = ~json;
	}
	else {
		ContentType("text/xml");
		request = XmlHeader();
		if(protocol_version.GetCount())
			request << "<!--protocolVersion=\"" << protocol_version << "\"-->\r\n";
		request << XmlTag("methodCall")(XmlTag("methodName")(method) + FormatXmlRpcParams(data.out, supports_i8));
	}
	if(sLogRpcCalls) {
		if(sLogRpcCallsCompress)
			RLOG("=== XmlRpc call request:\n" << CompressLog(request));
		else
			RLOG("=== XmlRpc call request:\n" << request);
	}
	Txt response;
	New();
	if(shorted)
		response = RpcExecuteShorted(request);
	else
		response = Post(request).Execute();
	if(sLogRpcCalls) {
		if(sLogRpcCallsCompress)
			RLOG("=== XmlRpc call response:\n" << CompressLog(response));
		else
			RLOG("=== XmlRpc call response:\n" << response);
	}
	RpcGet h;
	if(IsNull(response)) {
		faultCode = RPC_CLIENT_HTTP_ERROR;
		faultTxt = GetErrorDesc();
		error = "Http request failed: " + faultTxt;
		LLOG(error);
		h.v = ErrorVal(error);
		return h;
	}
	if(json) {
		try {
			Val r = ParseJSON(response);
			if(IsValMap(r)) {
				ValMap m = r;
				Val result = m["result"];
				if(!result.IsVoid()) {
					data.in.Clear();
					data.in.Add(result);
					data.ii = 0;
					h.v = result;
					return h;
				}
				Val e = m["error"];
				if(IsValMap(e)) {
					Val c = e["code"];
					Val m = e["message"];
					if(IsNumber(c) && IsTxt(m)) {
						faultCode = e["code"];
						faultTxt = e["message"];
						error.Clear();
						error << "Failed '" << faultTxt << "' (" << faultCode << ')';
						LLOG(s);
						h.v = ErrorVal(error);
						return h;
					}
				}
			}
			Txt s;
			faultTxt = "Неправильный ответ";
			faultCode = RPC_CLIENT_RESPONSE_ERROR;
			error = faultTxt;
			LLOG(error);
			h.v = ErrorVal(error);
			return h;
		}
		catch(CParser::Error e) {
			Txt s;
			faultTxt = e;
			faultCode = RPC_CLIENT_JSON_ERROR;
			error.Clear();
			error << "Ошибка JSON: " << faultTxt;
			LLOG(error);
			h.v = ErrorVal(error);
			return h;
		}
	}
	else {
		XmlParser p(response);
		try {
			p.ReadPI();
			while(p.IsComment())
				p.ReadComment();
			p.PassTag("methodResponse");
			if(p.Tag("fault")) {
				Val m = ParseXmlRpcVal(p);
				if(IsValMap(m)) {
					ValMap mm = m;
					faultTxt = mm["faultTxt"];
					faultCode = mm["faultCode"];
					error.Clear();
					error << "Неудача '" << faultTxt << "' (" << faultCode << ')';
					LLOG(s);
					h.v = ErrorVal(error);
					return h;
				}
			}
			else {
				data.in = ParseXmlRpcParams(p);
				data.ii = 0;
				p.PassEnd();
			}
		}
		catch(XmlError e) {
			Txt s;
			faultTxt = e;
			faultCode = RPC_CLIENT_XML_ERROR;
			error.Clear();
			error << "Ошибка XML: " << faultTxt;
			LLOG(error << ": " << p.GetPtr());
			h.v = ErrorVal(error);
			return h;
		}
		h.v = data.in.GetCount() ? data.in[0] : Null;
		return h;
	}
}

void RpcRequest::ClearError()
{
	faultCode = 0;
	faultTxt.Clear();
	error.Clear();
}

}