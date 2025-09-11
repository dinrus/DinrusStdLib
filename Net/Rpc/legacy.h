enum {
	XMLRPC_SERVER_JSON_ERROR = RPC_SERVER_JSON_ERROR,
	XMLRPC_SERVER_JSON_REQUEST_ERROR = RPC_SERVER_JSON_REQUEST_ERROR,
	XMLRPC_UNKNOWN_METHOD_ERROR = RPC_UNKNOWN_METHOD_ERROR,
	XMLRPC_SERVER_PARAM_ERROR = RPC_SERVER_PARAM_ERROR,
	XMLRPC_SERVER_PROCESSING_ERROR = RPC_SERVER_PROCESSING_ERROR,

	XMLRPC_SERVER_XML_ERROR = RPC_SERVER_XML_ERROR,

	XMLRPC_CLIENT_HTTP_ERROR = RPC_CLIENT_HTTP_ERROR,
	XMLRPC_CLIENT_XML_ERROR = RPC_CLIENT_XML_ERROR,
	XMLRPC_CLIENT_JSON_ERROR = RPC_CLIENT_JSON_ERROR,
	XMLRPC_CLIENT_RESPONSE_ERROR = RPC_CLIENT_RESPONSE_ERROR,
};

#define XMLRPC_METHOD(x)  RPC_METHOD(x)

#define XMLRPC_GMETHOD(x, group) RPC_GMETHOD(x, group)

typedef RpcError XmlRpcError;

typedef RpcData XmlRpcData;

inline
void   SetXmlRpcMethodFilter(Txt (*filter)(const Txt& methodname))
{
	SetRpcMethodFilter(filter);
}

inline
bool   XmlRpcPerform(TcpSocket& http, tukk group)
{
	return RpcPerform(http, group);
}

inline
bool   XmlRpcServerLoop(i32 port, tukk group = NULL)
{
	return RpcServerLoop(port, group);
}

inline
void   ThrowXmlRpcError(i32 code, tukk s)
{
	ThrowRpcError(code, s);
}

inline
void   ThrowXmlRpcError(tukk s)
{
	ThrowRpcError(s);
}

inline
void LogXmlRpcRequests(bool b = true)
{
	LogRpcRequests(b);
}

inline
void SetXmlRpcServerTrace(Stream& s, i32 level = 1)
{
	SetRpcServerTrace(s, level);
}
