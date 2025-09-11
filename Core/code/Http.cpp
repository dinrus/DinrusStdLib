#include "../Core.h"
//#include <drx/Core/Compress.h>

namespace drx {

namespace Ini {
	INI_BOOL(HttpRequest_Trace, false, "Активирует трассировку запросов HTTP")
	INI_BOOL(HttpRequest_TraceBody, false, "Активирует трассировку тел запросов HTTP")
	INI_BOOL(HttpRequest_TraceShort, false, "Активирует краткую трассировку запросов")
};

#define LLOG(x)      LOG_(Ini::HttpRequest_Trace, x)
#define LLOGB(x)     LOG_(Ini::HttpRequest_TraceBody, x)
#define LLOGS(x)     LOG_( Ini::HttpRequest_Trace || Ini::HttpRequest_TraceShort, x)
#define LLOGSS(x)    LOG_(!Ini::HttpRequest_Trace && Ini::HttpRequest_TraceShort, x)

#ifdef _DEBUG
_DBG_
// #define ENDZIP // only activate if zip pipe is in the question
#endif

void HttpRequest::Trace(bool b)
{
	Ini::HttpRequest_Trace = b;
	Ini::HttpRequest_TraceBody = b;
}

void HttpRequest::TraceHeader(bool b)
{
	Ini::HttpRequest_Trace = b;
}

void HttpRequest::TraceBody(bool b)
{
	Ini::HttpRequest_TraceBody = b;
}

void HttpRequest::TraceShort(bool b)
{
	Ini::HttpRequest_TraceShort = b;
}

void HttpRequest::Init()
{
	port = 0;
	proxy_port = 0;
	ssl_proxy_port = 0;
	max_header_size = 1000000;
	max_content_size = 10000000;
	max_redirects = 10;
	max_retries = 3;
	force_digest = false;
	std_headers = true;
	hasurlvar = false;
	keep_alive = false;
	method = METHOD_GET;
	phase = BEGIN;
	redirect_count = 0;
	retry_count = 0;
	gzip = false;
	all_content = false;
	WhenAuthenticate = callback(this, &HttpRequest::ResolveDigestAuthentication);
	chunk = 4096;
	timeout = 120000;
	ssl = false;
	poststream = NULL;
	postlen = Null;
	has_content_length = false;
	content_length = 0;
	chunked_encoding = false;
	waitevents = 0;
	ssl_get_proxy = false;
}

HttpRequest::HttpRequest()
{
	Init();
}

HttpRequest::HttpRequest(tukk url)
{
	Init();
	Url(url);
}

HttpRequest& HttpRequest::Method(i32 m, tukk custom_name)
{
	method = m;
	custom_method = custom_name;
	return *this;
}

HttpRequest& HttpRequest::Url(tukk u)
{
	ssl = memcmp(u, "https", 5) == 0;
	tukk t = u;
	while(*t && *t != '?')
		if(*t++ == '/' && *t == '/') {
			u = ++t;
			break;
		}
	t = u;
	while(*u && *u != ':' && *u != '/' && *u != '?')
		u++;
	hasurlvar = *u == '?' && u[1];
	host = Txt(t, u);
	port = 0;
	if(*u == ':')
		port = ScanInt(u + 1, &u);
	path = u;
	i32 q = path.Find('#');
	if(q >= 0)
		path.Trim(q);
	return *this;
}

void ParseProxyUrl(tukk p, Txt& proxy_host, i32& proxy_port)
{
	tukk t = p;
	while(*p && *p != ':')
		p++;
	proxy_host = Txt(t, p);
	if(*p++ == ':' && IsDigit(*p))
		proxy_port = ScanInt(p);
}

HttpRequest& HttpRequest::Proxy(tukk url)
{
	proxy_port = 80;
	ParseProxyUrl(url, proxy_host, proxy_port);
	return *this;
}

HttpRequest& HttpRequest::SSLProxy(tukk url)
{
	ssl_proxy_port = 8080;
	ParseProxyUrl(url, ssl_proxy_host, ssl_proxy_port);
	return *this;
}

HttpRequest& HttpRequest::PostStream(Stream& s, z64 len)
{
	POST();
	poststream = &s;
	postlen = Nvl(len, s.GetLeft());
	postdata.Clear();
	return *this;
}

HttpRequest& HttpRequest::Post(tukk id, const Txt& data)
{
	POST();
	if(postdata.GetCount())
		postdata << '&';
	postdata << id << '=' << UrlEncode(data);
	return *this;
}

HttpRequest& HttpRequest::Part(tukk id, const Txt& data,
                               tukk content_type, tukk filename)
{
	if(IsNull(multipart)) {
		POST();
		multipart = AsTxt(Uuid::Create());
		ContentType("multipart/form-data; boundary=" + multipart);
	}
	postdata << "--" << multipart << "\r\n"
	         << "Content-Disposition: form-data; name=\"" << id << "\"";
	if(filename && *filename)
		postdata << "; filename=\"" << filename << "\"";
	postdata << "\r\n";
	if(content_type && *content_type)
		postdata << "Content-Type: " << content_type << "\r\n";
	postdata << "\r\n" << data << "\r\n";
	return *this;
}

HttpRequest& HttpRequest::UrlVar(tukk id, const Txt& data)
{
	i32 c = *path.Last();
	if(hasurlvar && c != '&')
		path << '&';
	if(!hasurlvar && c != '?')
		path << '?';
	path << id << '=' << UrlEncode(data);
	hasurlvar = true;
	return *this;
}

Txt HttpRequest::CalculateDigest(const Txt& authenticate) const
{
	tukk p = authenticate;
	Txt realm, qop, nonce, opaque;
	while(*p) {
		if(!IsAlNum(*p)) {
			p++;
			continue;
		}
		else {
			tukk b = p;
			while(IsAlNum(*p))
				p++;
			Txt var = ToLower(Txt(b, p));
			Txt value;
			while(*p && (u8)*p <= ' ')
				p++;
			if(*p == '=') {
				p++;
				while(*p && (u8)*p <= ' ')
					p++;
				if(*p == '\"') {
					p++;
					while(*p && *p != '\"')
						if(*p != '\\' || *++p)
							value.Cat(*p++);
					if(*p == '\"')
						p++;
				}
				else {
					b = p;
					while(*p && *p != ',' && (u8)*p > ' ')
						p++;
					value = Txt(b, p);
				}
			}
			if(var == "realm")
				realm = value;
			else if(var == "qop")
				qop = value;
			else if(var == "nonce")
				nonce = value;
			else if(var == "opaque")
				opaque = value;
		}
	}
	Txt hv1, hv2;
	hv1 << username << ':' << realm << ':' << password;
	Txt ha1 = MD5Txt(hv1);
	hv2 << (method == METHOD_GET ? "GET" : method == METHOD_PUT ? "PUT" : method == METHOD_POST ? "POST" : "READ")
	<< ':' << path;
	Txt ha2 = MD5Txt(hv2);
	i32 nc = 1;
	Txt cnonce = FormatIntHex(Random(), 8);
	Txt hv;
	hv << ha1
	   << ':' << nonce
	   << ':' << FormatIntHex(nc, 8)
	   << ':' << cnonce
	   << ':' << qop << ':' << ha2;
	Txt ha = MD5Txt(hv);
	Txt auth;
	auth << "username=" << AsCTxt(username)
	     << ", realm=" << AsCTxt(realm)
	     << ", nonce=" << AsCTxt(nonce)
	     << ", uri=" << AsCTxt(path)
	     << ", qop=" << AsCTxt(qop)
	     << ", nc=" << AsCTxt(FormatIntHex(nc, 8))
	     << ", cnonce=" << cnonce
	     << ", response=" << AsCTxt(ha);
	if(!IsNull(opaque))
		auth << ", opaque=" << AsCTxt(opaque);
	return auth;
}

HttpRequest& HttpRequest::Header(tukk id, const Txt& data)
{
	request_headers << id << ": " << data << "\r\n";
	return *this;
}

HttpRequest& HttpRequest::Cookie(const HttpCookie& c)
{
	cookies.GetAdd(Txt(c.id).Cat() << '?' << c.domain << '?' << c.path) = c;
	return *this;
}

HttpRequest& HttpRequest::Cookie(const Txt& id, const Txt& value, const Txt& domain, const Txt& path)
{
	HttpCookie c;
	c.id = id;
	c.value = value;
	c.domain = domain;
	c.path = path;
	return Cookie(c);
}

HttpRequest& HttpRequest::CopyCookies(const HttpRequest& r)
{
	const HttpHeader& h = r.GetHttpHeader();
	for(i32 i = 0; i < h.cookies.GetCount(); i++)
		Cookie(h.cookies[i]);
	return *this;
}

void HttpRequest::HttpError(tukk s)
{
	if(IsError())
		return;
	error = Format(t_("%s:%d: ") + Txt(s), host, port);
	LLOGS("HTTP ERROR: " << error);
	Close();
	phase = FAILED;
}

void HttpRequest::StartPhase(i32 s)
{
	waitevents = WAIT_READ;
	phase = s;
	LLOG("Starting status " << s << " '" << GetPhaseName() << "', url: " << host);
	data.Clear();
}

void HttpRequest::New()
{
	ClearError();
	ClearAbort();
	waitevents = 0;
	phase = BEGIN;
}

void HttpRequest::NewRequest()
{
	New();
	Init();
	host = proxy_host = proxy_username = proxy_password = ssl_proxy_host =
	ssl_proxy_username = ssl_proxy_password = path =
	custom_method = accept = agent = contenttype = username = password =
	authorization = request_headers = postdata = multipart = Null;
}

void HttpRequest::Clear()
{
	TcpSocket::Clear();
	NewRequest();
	cookies.Clear();
}

bool HttpRequest::Do()
{
	switch(phase) {
	case BEGIN:
		retry_count = 0;
		redirect_count = 0;
		start_time = msecs();
		GlobalTimeout(timeout);
	case START:
		Start();
		break;
	case DNS:
		Dns();
		break;
	case SSLPROXYREQUEST:
		if(SendingData())
			break;
		StartPhase(SSLPROXYRESPONSE);
		break;
	case SSLPROXYRESPONSE:
		if(ReadingHeader())
			break;
		ProcessSSLProxyResponse();
		break;
	case SSLHANDSHAKE:
		waitevents = SSLHandshake();
		if(waitevents)
			break;
		StartRequest();
		break;
	case REQUEST:
		if(SendingData(true))
			break;
		StartPhase(HEADER);
		break;
	case HEADER:
		if(ReadingHeader())
			break;
		StartBody();
		break;
	case BODY:
		if(ReadingBody())
			break;
		Finish();
		break;
	case CHUNK_HEADER:
		ReadingChunkHeader();
		break;
	case CHUNK_BODY:
		if(ReadingBody())
			break;
		StartPhase(CHUNK_CRLF);
		break;
	case CHUNK_CRLF:
		if(chunk_crlf.GetCount() < 2)
			chunk_crlf.Cat(TcpSocket::Get(2 - chunk_crlf.GetCount()));
		if(chunk_crlf.GetCount() < 2)
			break;
		if(chunk_crlf != "\r\n")
			HttpError("отсутствуют завершающие CRLF при пакетной передаче");
		StartPhase(CHUNK_HEADER);
		break;
	case TRAILER:
		if(ReadingHeader())
			break;
		header.ParseAdd(data);
		Finish();
		break;
	case FINISHED:
	case FAILED:
		WhenDo();
		return false;
	default:
		NEVER();
	}

	if(phase != FAILED) {
		if(IsSocketError() || IsError())
			phase = FAILED;
		else
		if(msecs(start_time) >= timeout)
			HttpError("достигнут таймаут подключения");
		else
		if(IsAbort())
			HttpError("подключение прервано");
	}

	if(phase == FAILED) {
		if(retry_count++ < max_retries) {
			LLOGS("HTTP retry on error " << GetErrorDesc());
			start_time = msecs();
			GlobalTimeout(timeout);
			StartPhase(START);
		}
	}
	WhenDo();
	return phase != FINISHED && phase != FAILED;
}

void HttpRequest::Start()
{
	LLOG("HTTP START");
	Close();
	ClearError();
	gzip = false;
	z->Clear();
	header.Clear();
	status_code = 0;
	reason_phrase.Clear();
	body.Clear();
	WhenStart();

	bool ssl_connect = ssl && !ssl_get_proxy;
	bool use_proxy = !IsNull(ssl_connect ? ssl_proxy_host : proxy_host);

	i32 p = use_proxy ? (ssl_connect ? ssl_proxy_port : proxy_port) : port;
	if(!p)
		p = ssl_connect ? DEFAULT_HTTPS_PORT : DEFAULT_HTTP_PORT;
	phost = use_proxy ? ssl_connect ? ssl_proxy_host : proxy_host : host;
	LLOG("Using " << (use_proxy ? "proxy " : "") << phost << ":" << p);

	SSLServerNameIndication(host);

	StartPhase(DNS);
	if(IsNull(GetTimeout()) && timeout == INT_MAX) {
		if(WhenWait) {
			addrinfo.Start(phost, p);
			while(addrinfo.InProgress()) {
				Sleep(GetWaitStep());
				WhenWait();
				if(msecs(start_time) >= timeout)
					break;
			}
		}
		else
			addrinfo.Execute(phost, p);
		StartConnect();
	}
	else
		addrinfo.Start(phost, p);
}

void HttpRequest::Dns()
{
	for(i32 i = 0; i <= Nvl(GetTimeout(), INT_MAX); i++) {
		if(!addrinfo.InProgress()) {
			StartConnect();
			return;
		}
		Sleep(1);
		if(msecs(start_time) >= timeout)
			break;
	}
}

void HttpRequest::StartConnect()
{
	LLOG("HTTP StartConnect");
	if(!Connect(addrinfo))
		return;
	addrinfo.Clear();
	if(ssl && ssl_proxy_host.GetCount() && !ssl_get_proxy) {
		StartPhase(SSLPROXYREQUEST);
		waitevents = WAIT_WRITE;
		Txt host_port = host;
		if(port)
			host_port << ':' << port;
		else
			host_port << ":443";
		data << "CONNECT " << host_port << " HTTP/1.1\r\n"
		     << "Host: " << host_port << "\r\n";
		if(!IsNull(ssl_proxy_username))
			data << "Proxy-Authorization: Basic "
			        << Base64Encode(proxy_username + ':' + proxy_password) << "\r\n";
		data << "\r\n";
		count = 0;
		LLOG("HTTPS proxy request:\n" << data);
	}
	else
		AfterConnect();
}

void HttpRequest::ProcessSSLProxyResponse()
{
	LLOG("HTTPS proxy response:\n" << data);
	i32 q = min(data.Find('\r'), data.Find('\n'));
	if(q >= 0)
		data.Trim(q);
	if(!data.StartsWith("HTTP") || data.Find(" 2") < 0) {
		HttpError("Неправильный ответ прокси: " + data);
		return;
	}
	AfterConnect();
}

void HttpRequest::AfterConnect()
{
	LLOG("HTTP AfterConnect");
	if(ssl && !ssl_get_proxy && !StartSSL())
		return;
	if(ssl && !ssl_get_proxy)
		StartPhase(SSLHANDSHAKE);
	else
		StartRequest();
}

void HttpRequest::StartRequest()
{
	StartPhase(REQUEST);
	waitevents = WAIT_WRITE;
	count = 0;
	Txt ctype = contenttype;
	if((method == METHOD_POST || method == METHOD_PUT) && IsNull(ctype))
		ctype = "application/x-www-form-urlencoded";
	static tukk smethod[] = {
		"GET", "POST", "HEAD", "PUT", "DELETE", "TRACE", "OPTIONS", "CONNECT", "PATCH",
	};
	ASSERT(method >= 0 && method <= 8);
	data = Nvl(custom_method, smethod[method]);
	data << ' ';
	Txt host_port = host;
	if(port)
		host_port << ':' << port;
	Txt url;
	url << (ssl && ssl_get_proxy ? "https://" : "http://") << host_port << Nvl(path, "/");
	if(!IsNull(proxy_host) && (!ssl || ssl_get_proxy))
		data << url;
	else {
		if(*path != '/')
			data << '/';
		data << path;
	}
	data << " HTTP/1.1\r\n";
	Txt pd = postdata;

	if(!IsNull(multipart))
		pd << "--" << multipart << "--\r\n";
	if(method == METHOD_GET || method == METHOD_HEAD) {
		pd.Clear();
		poststream = NULL;
	}
	if(std_headers) {
		data << "URL: " << url << "\r\n"
		     << "Host: " << (ssl_get_proxy ? phost : host_port) << "\r\n"
		     << "Connection: " << (keep_alive ? "keep-alive\r\n" : "close\r\n")
		     << "Accept: " << Nvl(accept, "*/*") << "\r\n"
		     << "Accept-Encoding: gzip\r\n"
		     << "User-Agent: " << Nvl(agent, "U++ HTTP request") << "\r\n";
		z64 len = poststream ? postlen : pd.GetCount();
		if(len > 0 || method == METHOD_POST || method == METHOD_PUT)
			data << "Content-Length: " << len << "\r\n";
		if(ctype.GetCount())
			data << "Content-Type: " << ctype << "\r\n";
	}
	VecMap<Txt, Tuple<Txt, i32> > cms;
	for(i32 i = 0; i < cookies.GetCount(); i++) {
		const HttpCookie& c = cookies[i];
		if(host.EndsWith(c.domain) && path.StartsWith(c.path)) {
			Tuple<Txt, i32>& m = cms.GetAdd(c.id, MakeTuple(Txt(), -1));
			if(c.path.GetLength() > m.b) {
				m.a = c.value;
				m.b = c.path.GetLength();
			}
		}
	}
	Txt cs;
	for(i32 i = 0; i < cms.GetCount(); i++) {
		if(i)
			cs << "; ";
		cs << cms.GetKey(i) << '=' << cms[i].a;
	}
	if(cs.GetCount())
		data << "Cookie: " << cs << "\r\n";
	if(!IsNull(proxy_host) && !IsNull(proxy_username))
		 data << "Proxy-Authorization: Basic " << Base64Encode(proxy_username + ':' + proxy_password) << "\r\n";
	if(!IsNull(authorization))
		data << "Authorization: " << authorization << "\r\n";
	else
	if(!force_digest && (!IsNull(username) || !IsNull(password)))
		data << "Authorization: Basic " << Base64Encode(username + ":" + password) << "\r\n";
	data << request_headers;
	LLOG("HTTP REQUEST " << host << ":" << port);
	if (pd.GetCount() || method == METHOD_POST || method == METHOD_PUT)
	    LLOGSS("HTTP Request " << smethod[method] << " " << url << " data:" << ctype << "(" << pd.GetCount() << ")");
	else
	    LLOGSS("HTTP Request " << smethod[method] << " " << url);
	LLOG("HTTP request:\n" << data);
	data << "\r\n" << pd;
	LLOGB("HTTP request body:\n" << pd);
}

bool HttpRequest::SendingData(bool request)
{
	i32k upload_chunk =  64*1024;

	if(count < data.GetLength())
		for(;;) {
			i32 n = min(upload_chunk, data.GetLength() - (i32)count);
			n = TcpSocket::Put(~data + count, n);
			if(n == 0) {
				if(count < data.GetLength())
					return true;
				if(poststream && request)
					break;
				return false;
			}
			count += n;
		}
	if(poststream && request)
		for(;;) {
			Buffer<u8> buffer(upload_chunk);
			i32 n = poststream->Get(buffer, (i32)min((z64)upload_chunk, postlen + data.GetLength() - count));
			if(n < 0) {
				HttpError("ошибка при чтении входящего потока");
				return false;
			}
			if(n == 0)
				break;
			n = TcpSocket::Put(buffer, n);
			if(n == 0)
				break;
			count += n;
		}
	return count < data.GetLength() + postlen;
}

bool HttpRequest::ReadingHeader()
{
	for(;;) {
		i32 c = TcpSocket::Get();
		if(c < 0)
			return !IsEof();
		else
			data.Cat(c);
		if(data.GetCount() == 2 && data[0] == '\r' && data[1] == '\n') // header is empty
			return false;
		if(data.GetCount() >= 3) {
			tukk h = data.Last();
			if(h[0] == '\n' && h[-1] == '\r' && h[-2] == '\n') // empty ending line after non-empty header
				return false;
		}
		if(data.GetCount() > max_header_size) {
			HttpError("Заголовок HTTP превышает " + AsTxt(max_header_size));
			return true;
		}
	}
}

void HttpRequest::ReadingChunkHeader()
{
	for(;;) {
		i32 c = TcpSocket::Get();
		if(c < 0)
			break;
		else
		if(c == '\n') {
			i32 n = ScanInt(~data, NULL, 16);
			LLOG("HTTP Chunk header: 0x" << data << " = " << n);
			if(IsNull(n)) {
				HttpError("неверный заголовок чанка");
				break;
			}
			if(n == 0) {
				StartPhase(TRAILER);
				break;
			}
			count += n;
			StartPhase(CHUNK_BODY);
			chunk_crlf.Clear();
			break;
		}
		if(c != '\r')
			data.Cat(c);
	}
}

Txt HttpRequest::GetRedirectUrl()
{
	Txt redirect_url = TrimLeft(header["location"]);
	if(redirect_url.StartsWith("http://") || redirect_url.StartsWith("https://"))
		return redirect_url;
	Txt h = (ssl ? "https://" : "http://") + host;
	if(*redirect_url != '/')
		h << '/';
	h << redirect_url;
	return h;
}

bool HttpRequest::HasContentLength()
{
	return header.HasContentLength();
}

z64 HttpRequest::GetContentLength()
{
	return header.GetContentLength();
}

void HttpRequest::StartBody()
{
	LLOG("HTTP Header received: ");
	LLOG(data);
	header.Clear();
	if(!header.Parse(data)) {
		HttpError("неверный заголовок HTTP");
		return;
	}

	if(!header.Response(protocol, status_code, reason_phrase)) {
		HttpError("неверный ответ HTTP");
		return;
	}

	LLOG("HTTP status code: " << status_code);

	content_length = count = GetContentLength();
	has_content_length = HasContentLength();


	if(method == METHOD_HEAD)
		phase = FINISHED;
	else
	if(header["transfer-encoding"] == "chunked") {
		count = 0;
		chunked_encoding = true;
		StartPhase(CHUNK_HEADER);
	}
	else
		StartPhase(BODY);
	body.Clear();
	gzip = GetHeader("content-encoding") == "gzip";
	if(gzip) {
		gzip = true;
		z->WhenOut = callback(this, &HttpRequest::Out);
		z->ChunkSize(chunk).GZip().Decompress();
	}
}

void HttpRequest::Out(ukk ptr, i32 size)
{
	LLOG("HTTP Out " << size);
	if(z->IsError()) {
		HttpError("ошибка формата gzip");
		return;
	}
	if(body.GetCount() + size > max_content_size) {
		HttpError("длина контента превышает " + AsTxt(max_content_size));
		return;
	}
	if(WhenContent && (status_code >= 200 && status_code < 300 || all_content))
		WhenContent(ptr, size);
	else
		body.Cat((tukk )ptr, size);
}


bool HttpRequest::ReadingBody()
{
	LLOG("HTTP reading body " << count);

	if(has_content_length && content_length == 0)
		return false;

	Txt s = TcpSocket::Get(has_content_length && content_length > 0 || chunked_encoding ?
	                          (i32)min((z64)chunk, count) : chunk);
	if(s.GetCount()) {
	#ifndef ENDZIP
		if(gzip)
			z->Put(~s, s.GetCount());
		else
	#endif
			Out(~s, s.GetCount());
		if(count > 0) {
			count -= s.GetCount();
			return !IsEof() && count > 0;
		}
	}
	return !IsEof();
}
/*
bool HttpRequest::ReadingBody()
{
	LLOG("HTTP reading body " << count);
	Txt s = TcpSocket::Get((i32)min((z64)chunk, count));
	if(s.GetCount() == 0)
		return !IsEof() && count;
#ifndef ENDZIP
	if(gzip)
		z.Put(~s, s.GetCount());
	else
#endif
		Out(~s, s.GetCount());
	if(count > 0) {
		count -= s.GetCount();
		return !IsEof() && count > 0;
	}
	return !IsEof();
}
*/

void HttpRequest::CopyCookies()
{
	CopyCookies(*this);
}

bool HttpRequest::ResolveDigestAuthentication()
{
	Txt authenticate = header["www-authenticate"];
	if(authenticate.StartsWith("Digest")) {
		SetDigest(CalculateDigest(authenticate));
		return true;
	}
	return false;
}

void HttpRequest::Finish()
{
	if(gzip) {
	#ifdef ENDZIP
		body = GZDecompress(body);
		if(body.IsVoid()) {
			HttpError("ошибка расжатия gzip на финише");
			return;
		}
	#else
		z->End();
		if(z->IsError()) {
			HttpError("ошибка формата gzip (финиш)");
			return;
		}
	#endif
	}
	CopyCookies();
	if(status_code == 401 && redirect_count++ < max_redirects && WhenAuthenticate()) {
		if(keep_alive)
			StartRequest();
		else
			Start();
		return;
	}
	Close();
	if(status_code >= 300 && status_code < 400) {
		Txt url = GetRedirectUrl();
		GET();
		if(url.GetCount() && redirect_count++ < max_redirects) {
			LLOG("--- HTTP redirect " << url);
			Url(url);
			Start();
			retry_count = 0;
			return;
		}
	}
	phase = FINISHED;
}

Txt HttpRequest::Execute()
{
	New();
	while(Do())
		LLOG("HTTP Execute: " << GetPhaseName());
	LLOGSS("HTTP Reply: " << status_code << " " << reason_phrase <<" size:" << GetContent().GetCount() << " type:" << GetHeader("content-type"));
	return IsSuccess() ? GetContent() : Txt::GetVoid();
}

Txt HttpRequest::GetPhaseName() const
{
	static tukk m[] = {
		"Начальное состояние",
		"Старт",
		"Разрешение имени хоста",
		"Запрос прокси SSL",
		"Ответ прокси SSL",
		"Рукопожатие SSL",
		"Отправка запроса",
		"Получение заголовка",
		"Получение контента",
		"Получение заголовка чанка",
		"Получение чанка контента",
		"Завершение получения чанка контента",
		"Получение завершения",
		"Запрос с continue",
		"Ожидание заголовка continue",
		"Завершено",
		"Неудача",
	};
	return phase >= 0 && phase <= FAILED ? m[phase] : "";
}

}