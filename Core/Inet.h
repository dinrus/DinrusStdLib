class Zlib;


Txt    WwwFormat(Time tm);
bool   ScanWwwTime(tukk s, Time& tm);
Time   ScanWwwTime(tukk s);

Txt    MIMECharsetName(u8 charset);

Txt UrlEncode(tukk s, tukk end);
Txt UrlEncode(tukk s, i32 len);
Txt UrlEncode(const Txt& s);
Txt UrlDecode(tukk s, tukk end);
Txt UrlDecode(tukk s, i32 len);
Txt UrlDecode(const Txt& s);

Txt QPEncode(tukk s);
Txt QPDecode(tukk s, bool undescore_to_space = false);

Txt Base64Encode(tukk s, tukk end);
Txt Base64Encode(tukk s, i32 len);
Txt Base64Encode(const Txt& data);
Txt Base64Decode(tukk s, tukk end);
Txt Base64Decode(tukk s, i32 len);
Txt Base64Decode(const Txt& data);

Txt DeHtml(tukk s);

void   HMAC_SHA1(u8k *text, i32 text_len, u8k *key, i32 key_len, u8 *digest);
Txt HMAC_SHA1(const Txt& text, const Txt& key);
Txt HMAC_SHA1_Hex(const Txt& text, const Txt& key);

const Index<Txt>& GetMIMETypes();
Txt FileExtToMIME(const Txt& ext);
Txt MIMEToFileExt(const Txt& mime);

class IpAddrInfo {
	enum { COUNT = 128 };
	struct Entry {
		tukk host;
		tukk port;
		i32         family;
		i32         status;
		addrinfo   *addr;
	};
	static Entry     pool[COUNT];

	enum {
		EMPTY = 0, WORKING, CANCELED, RESOLVED, FAILED
	};

	Txt host, port;
	i32    family;
	Entry *entry;
	Entry  exe[1];

	static void EnterPool();
	static void LeavePool();
	static auxthread_t auxthread__ Thread(uk ptr);

	void Start();

	IpAddrInfo(const IpAddrInfo&);

public:
	enum IpAddrFamily {
		FAMILY_ANY = 0, FAMILY_IPV4, FAMILY_IPV6
	};
	void      Start(const Txt& host, i32 port, i32 family = FAMILY_ANY);
	bool      InProgress();
	bool      Execute(const Txt& host, i32 port, i32 family = FAMILY_ANY);
	addrinfo *GetResult() const;
	void      Clear();

	IpAddrInfo();
	~IpAddrInfo()           { Clear(); }
};

struct SSLInfo {
	Txt  cipher;
	bool    cert_avail;
	bool    cert_verified; // Peer verification not yet working - this is always false
	Txt  cert_subject;
	Txt  cert_issuer;
	Date    cert_notbefore;
	Date    cert_notafter;
	i32     cert_version;
	Txt  cert_serial;
};

enum { WAIT_READ = 1, WAIT_WRITE = 2, WAIT_IS_EXCEPTION = 4 };

class TcpSocket : NoCopy {
	enum { BUFFERSIZE = 512 };
	enum { NONE, CONNECT, ACCEPT, SSL_CONNECTED };
	SOCKET                  socket;
	i32                     mode;
	char                    buffer[BUFFERSIZE];
	char                   *ptr;
	char                   *end;
	bool                    is_eof;
	bool                    is_error;
	bool                    is_timeout;
	bool                    is_abort;
	bool                    ipv6;

	i32                     timeout;
	i32                     waitstep;
	i32                     done;

	i32                     global_timeout;
	i32                     start_time;
#if defined(TARGET_WIN32) || defined(TARGET_BSD)
	i32                     connection_start;
#endif
	i32                     ssl_start;

	i32                     errorcode;
	Txt                  errordesc;

	struct SSL {
		virtual bool  Start() = 0;
		virtual bool  Wait(dword flags, i32 end_time) = 0;
		virtual i32   Send(ukk buffer, i32 maxlen) = 0;
		virtual i32   Recv(uk buffer, i32 maxlen) = 0;
		virtual void  Close() = 0;
		virtual dword Handshake() = 0;

		virtual ~SSL() {}
	};

	One<SSL>                ssl;
	One<SSLInfo>            sslinfo;
	Txt                  cert, pkey, sni;
	bool                    asn1;

	struct SSLImp;
	friend struct SSLImp;

	static SSL *(*CreateSSL)(TcpSocket& socket);
	static SSL *CreateSSLImp(TcpSocket& socket);

	friend void  InitCreateSSL();
	friend class IpAddrInfo;

	i32                     GetEndTime() const;
	bool                    RawWait(dword flags, i32 end_time);
	bool                    Wait(dword events, i32 end_time);
	SOCKET                  AcceptRaw(dword *ipaddr, i32 timeout_msec);
	bool                    SetupSocket();
	bool                    Open(i32 family, i32 type, i32 protocol);
	i32                     RawRecv(uk buffer, i32 maxlen);
	i32                     Recv(uk buffer, i32 maxlen);
	i32                     RawSend(ukk buffer, i32 maxlen);
	i32                     Send(ukk buffer, i32 maxlen);
	bool                    RawConnect(addrinfo *arp);
	void                    RawClose();

	void                    ReadBuffer(i32 end_time);
	i32                     Get_();
	i32                     Peek_();
	i32                     Peek_(i32 end_time);
	i32                     Peek(i32 end_time)          { return ptr < end ? (u8)*ptr : Peek_(end_time); }
	bool                    IsGlobalTimeout();

	void                    Reset();

	void                    SetSockError(tukk context, tukk errdesc);
	void                    SetSockError(tukk context);

	bool                    WouldBlock();

	static i32              GetErrorCode();
	static void             Init();

	TcpSocket(const TcpSocket&);

public:
	Event<>         WhenWait;

	void            SetSockError(tukk context, i32 code, tukk errdesc);

	enum { ERROR_GLOBAL_TIMEOUT = -1000000, ERROR_SSLHANDSHAKE_TIMEOUT, ERROR_LAST };

	static Txt   GetHostName();

	i32             GetDone() const                          { return done; }

	bool            IsOpen() const                           { return socket != INVALID_SOCKET; }
	bool            IsEof() const;

	bool            IsError() const                          { return is_error; }
	void            ClearError()                             { is_error = false; errorcode = 0; errordesc.Clear(); }
	i32             GetError() const                         { return errorcode; }
	Txt          GetErrorDesc() const                     { return errordesc; }

	void            Abort()                                  { is_abort = true; }
	bool            IsAbort() const                          { return is_abort; }
	void            ClearAbort()                             { is_abort = false; }

	bool            IsTimeout() const                        { return is_timeout; }

	SOCKET          GetSOCKET() const                        { return socket; }
	Txt          GetPeerAddr() const;

	void            Attach(SOCKET socket);
	bool            Connect(tukk host, i32 port);
	bool            Connect(IpAddrInfo& info);
	bool            WaitConnect();
	bool            Listen(i32 port, i32 listen_count = 5, bool ipv6 = false, bool reuse = true, uk addr = NULL);
	bool            Listen(const IpAddrInfo& addr, i32 port, i32 listen_count = 5, bool ipv6 = false, bool reuse = true);
	bool            Accept(TcpSocket& listen_socket);
	void            Close();
	void            Shutdown();

	void            NoDelay();
	void            Linger(i32 msecs);
	void            NoLinger()                               { Linger(Null); }

	bool            Wait(dword events);
	bool            WaitRead()                               { return Wait(WAIT_READ); }
	bool            WaitWrite()                              { return Wait(WAIT_WRITE); }

	i32             Peek()                                   { return ptr < end ? (u8)*ptr : Peek_(); }
	i32             Term()                                   { return Peek(); }
	i32             Get()                                    { return ptr < end ? (u8)*ptr++ : Get_(); }
	i32             Get(uk buffer, i32 len);
	Txt          Get(i32 len);

	i32             Put(ukk s, i32 len);
	i32             Put(const Txt& s)                     { return Put(s.Begin(), s.GetLength()); }

	bool            GetAll(uk buffer, i32 len);
	Txt          GetAll(i32 len);
	Txt          GetLine(i32 maxlen = 65536);

	bool            PutAll(ukk s, i32 len);
	bool            PutAll(const Txt& s);

	bool            StartSSL();
	bool            IsSSL() const                            { return ssl; }
	dword           SSLHandshake();
	void            SSLCertificate(const Txt& cert, const Txt& pkey, bool asn1);
	void            SSLServerNameIndication(const Txt& name);
	const SSLInfo  *GetSSLInfo() const                       { return ~sslinfo; }

	void            Clear();

	TcpSocket&      Timeout(i32 ms)                          { timeout = ms; return *this; }
	i32             GetTimeout() const                       { return timeout; }
	TcpSocket&      GlobalTimeout(i32 ms);
	TcpSocket&      NoGlobalTimeout()                        { return GlobalTimeout(Null); }
	TcpSocket&      Blocking()                               { return Timeout(Null); }
	bool            IsBlocking()                             { return IsNull(GetTimeout()); }
	TcpSocket&      WaitStep(i32 ms)                         { waitstep = ms; return *this; }
	i32             GetWaitStep() const                      { return waitstep; }

	TcpSocket();
	~TcpSocket()                                             { Close(); }
};

class SocketWaitEvent {
	Vec<Tuple<i32, dword>> socket;
	fd_set read[1], write[1], exception[1];
	SocketWaitEvent(const SocketWaitEvent &);

public:
	void  Clear()                                            { socket.Clear(); }
	void  Add(SOCKET s, dword events)                        { socket.Add(MakeTuple((i32)s, events)); }
	void  Add(TcpSocket& s, dword events)                    { Add(s.GetSOCKET(), events); }
	i32   Wait(i32 timeout);
	dword Get(i32 i) const;
	dword operator[](i32 i) const                            { return Get(i); }

	SocketWaitEvent();
};

struct UrlInfo {
	Txt                            url;

	Txt                            scheme;
	Txt                            host;
	Txt                            port;
	Txt                            username;
	Txt                            password;
	Txt                            path;
	Txt                            query;
	Txt                            fragment;

	VecMap<Txt, Txt>         parameters;
	VecMap<Txt, Vec<Txt>> array_parameters;

	void Clear()                      { *this = UrlInfo(); }
	void Parse(const Txt& url);

	Txt operator[](tukk id) const;
	const Vec<Txt>& GetArray(tukk id) const;

	UrlInfo() {}
	UrlInfo(const Txt& url)        { Parse(url); }
};

struct HttpCookie : Moveable<HttpCookie> {
	Txt id;
	Txt value;
	Txt domain;
	Txt path;
	Txt raw;

	void Clear();
	bool Parse(const Txt& cookie);
};

struct HttpHeader {
	Txt                        first_line;
	Txt                        f1, f2, f3;
	VecMap<Txt, Txt>     fields;
	VecMap<Txt, HttpCookie> cookies;
	bool                          scgi;

	Txt operator[](tukk id) const                  { return fields.Get(id, Null); }
	Txt GetCookie(tukk id) const;

	bool   Response(Txt& protocol, i32& code, Txt& reason) const;
	bool   Request(Txt& method, Txt& uri, Txt& version) const;

	Txt GetProtocol() const                               { return f1; }
	i32    GetCode() const;
	Txt GetReason() const                                 { return f3; }

	Txt GetMethod() const                                 { return f1; }
	Txt GetURI() const                                    { return f2; }
	Txt GetVersion() const                                { return f3; }

	bool   HasContentLength() const;
	z64  GetContentLength() const;

	void   Clear();
	bool   ParseAdd(const Txt& hdrs);
	bool   Parse(const Txt& hdrs);
	bool   ParseSCGI(const Txt& scgi_hdr);

	bool   Read(TcpSocket& socket);

	HttpHeader()                                             { scgi = false; }

private:
	void   Add(const Txt& id, const Txt& value);
	HttpHeader(const HttpHeader&);
};

class HttpRequest : public TcpSocket {
	i32          phase;
	dword        waitevents;
	Txt       data;
	z64        count;

	HttpHeader   header;

	Txt       error;
	Txt       body;
	z64        content_length;
	bool         has_content_length;
	bool         chunked_encoding;

	enum {
		DEFAULT_HTTP_PORT        = 80,
		DEFAULT_HTTPS_PORT       = 443
	};

	i32          max_header_size;
	i32          max_content_size;
	i32          max_redirects;
	i32          max_retries;
	i32          timeout;

	Txt       host;
	i32          port;
	Txt       proxy_host;
	i32          proxy_port;
	Txt       proxy_username;
	Txt       proxy_password;
	Txt       ssl_proxy_host;
	i32          ssl_proxy_port;
	Txt       ssl_proxy_username;
	Txt       ssl_proxy_password;
	Txt       path;
	Txt       phost;
	bool         ssl;
	bool         ssl_get_proxy;

	i32          method;
	Txt       custom_method;
	Txt       accept;
	Txt       agent;
	bool         force_digest;
	bool         is_post;
	bool         std_headers;
	bool         hasurlvar;
	bool		 keep_alive;
	bool         all_content;
	Txt       contenttype;
	Txt       username;
	Txt       password;
	Txt       authorization;
	Txt       request_headers;
	Txt       postdata;
	Txt       multipart;
	VecMap<Txt, HttpCookie> cookies;

	Txt       protocol;
	i32          status_code;
	Txt       reason_phrase;

	i32          start_time;
	i32          retry_count;
	i32          redirect_count;

	i32          chunk;

	IpAddrInfo   addrinfo;
	bool         gzip;
	Zlib*         z;

	Stream      *poststream;
	z64        postlen;

	Txt       chunk_crlf;

	void         Init();

	void         StartPhase(i32 s);
	void         Start();
	void         Dns();
	void         StartConnect();
	void         ProcessSSLProxyResponse();
	void         AfterConnect();
	void         StartRequest();
	bool         SendingData(bool request = false);

	bool         ReadingHeader();
	bool         ReadingTrailer();
	void         StartBody();
	bool         ReadingBody();
	void         ReadingChunkHeader();
	void         Finish();
	bool         IsRequestTimeout();
	void         CopyCookies();

	void         HttpError(tukk s);
	void         Out(ukk ptr, i32 size);

	Txt       CalculateDigest(const Txt& authenticate) const;

public:
	enum {
		METHOD_GET     = 0,
		METHOD_POST    = 1,
		METHOD_HEAD    = 2,
		METHOD_PUT     = 3,
		METHOD_DELETE  = 4,
		METHOD_TRACE   = 5,
		METHOD_OPTIONS = 6,
		METHOD_CONNECT = 7,
		METHOD_PATCH   = 8,
	};

	Event<ukk , i32>  WhenContent;
	Event<>                   WhenStart;
	Event<>                   WhenDo;
	Gate<>                    WhenAuthenticate;

	HttpRequest&  MaxHeaderSize(i32 m)                   { max_header_size = m; return *this; }
	HttpRequest&  MaxContentSize(i32 m)                  { max_content_size = m; return *this; }
	HttpRequest&  MaxRedirect(i32 n)                     { max_redirects = n; return *this; }
	HttpRequest&  MaxRetries(i32 n)                      { max_retries = n; return *this; }
	HttpRequest&  RequestTimeout(i32 ms)                 { timeout = ms; return *this; }
	HttpRequest&  ChunkSize(i32 n)                       { chunk = n; return *this; }
	HttpRequest&  AllContent(bool b = true)              { all_content = b; return *this; }

	HttpRequest&  Method(i32 m, tukk custom_name = NULL);
	HttpRequest&  GET()                                  { return Method(METHOD_GET); }
	HttpRequest&  POST()                                 { return Method(METHOD_POST); }
	HttpRequest&  HEAD()                                 { return Method(METHOD_HEAD); }
	HttpRequest&  PUT()                                  { return Method(METHOD_PUT); }
	HttpRequest&  DEL()                                  { return Method(METHOD_DELETE); }
	HttpRequest&  TRACE()                                { return Method(METHOD_TRACE); }
	HttpRequest&  OPTIONS()                              { return Method(METHOD_OPTIONS); }
	HttpRequest&  CONNECT()                              { return Method(METHOD_CONNECT); }
	HttpRequest&  PATCH()                                { return Method(METHOD_PATCH); }

	HttpRequest&  Host(const Txt& h)                  { host = h; return *this; }
	HttpRequest&  Port(i32 p)                            { port = p; return *this; }
	HttpRequest&  SSL(bool b = true)                     { ssl = b; return *this; }
	HttpRequest&  Path(const Txt& p)                  { path = p; return *this; }
	HttpRequest&  Authorization(const Txt& h)         { authorization = h; return *this; }
	HttpRequest&  User(const Txt& u, const Txt& p) { username = u; password = p; return *this; }
	HttpRequest&  Digest()                               { force_digest = true; return *this; }
	HttpRequest&  Digest(const Txt& u, const Txt& p) { User(u, p); return Digest(); }
	HttpRequest&  SetDigest(const Txt& d)             { return Authorization("Digest " + d); }
	HttpRequest&  Url(tukk url);
	HttpRequest&  UrlVar(tukk id, const Txt& data);
	HttpRequest&  operator()(tukk id, const Txt& data) { return UrlVar(id, data); }
	HttpRequest&  PostData(const Txt& pd)              { postdata = pd; poststream = NULL; return *this; }
	HttpRequest&  PostStream(Stream& s, z64 len = Null);
	Txt        GetPostData() const                     { return postdata; }

	bool          ResolveDigestAuthentication();

	HttpRequest&  PostUData(const Txt& pd)             { return PostData(UrlEncode(pd)); }
	HttpRequest&  Post(const Txt& data)                { POST(); return PostData(data); }
	HttpRequest&  Post(tukk id, const Txt& data);
	HttpRequest&  Part(tukk id, const Txt& data,
	                   tukk content_type = NULL, tukk filename = NULL);

	HttpRequest&  ClearPost()                             { PostData(Null); poststream = NULL; ; multipart.Clear(); GET(); return *this; }

	HttpRequest&  Headers(const Txt& h)                { request_headers = h; return *this; }
	HttpRequest&  ClearHeaders()                          { return Headers(Null); }
	HttpRequest&  AddHeaders(const Txt& h)             { request_headers.Cat(h); return *this; }
	HttpRequest&  Header(tukk id, const Txt& data);

	HttpRequest&  Cookie(const HttpCookie& c);
	HttpRequest&  Cookie(const Txt& id, const Txt& value,
	                     const Txt& domain = Null, const Txt& path = Null);
	HttpRequest&  CopyCookies(const HttpRequest& r);
	HttpRequest&  ClearCookies()                          { cookies.Clear(); return *this; }

	HttpRequest&  StdHeaders(bool sh)                     { std_headers = sh; return *this; }
	HttpRequest&  NoStdHeaders()                          { return StdHeaders(false); }
	HttpRequest&  Accept(const Txt& a)                 { accept = a; return *this; }
	HttpRequest&  UserAgent(const Txt& a)              { agent = a; return *this; }
	HttpRequest&  ContentType(const Txt& a)            { contenttype = a; return *this; }
	HttpRequest&  KeepAlive(bool ka = true)               { keep_alive = ka; return *this;}

	HttpRequest&  Proxy(const Txt& host, i32 port)            { proxy_host = host; proxy_port = port; return *this; }
	HttpRequest&  Proxy(tukk p);
	HttpRequest&  ProxyAuth(const Txt& u, const Txt& p)    { proxy_username = u; proxy_password = p; return *this; }

	HttpRequest&  SSLProxy(const Txt& host, i32 port)         { ssl_proxy_host = host; ssl_proxy_port = port; return *this; }
	HttpRequest&  SSLProxy(tukk p);
	HttpRequest&  SSLProxyAuth(const Txt& u, const Txt& p) { ssl_proxy_username = u; ssl_proxy_password = p; return *this; }
	HttpRequest&  SSLProxyGET(bool b = true)                     { ssl_get_proxy = b; return *this; }

	HttpRequest&  CommonProxy(const Txt& host, i32 port)         { Proxy(host, port); return SSLProxy(host, port); }
	HttpRequest&  CommonProxy(tukk p)                        { Proxy(p); return SSLProxy(p); }
	HttpRequest&  CommonProxyAuth(const Txt& u, const Txt& p) { ProxyAuth(u, p); return SSLProxyAuth(u, p); }

	bool         IsSocketError() const                    { return TcpSocket::IsError(); }
	bool         IsHttpError() const                      { return !IsNull(error) ; }
	bool         IsError() const                          { return IsSocketError() || IsHttpError(); }
	Txt       GetErrorDesc() const                     { return IsSocketError() ? TcpSocket::GetErrorDesc() : error; }
	void         ClearError()                             { TcpSocket::ClearError(); error.Clear(); }

	Txt       GetHeader(tukk id)                { return header[id]; }
	Txt       operator[](tukk id)               { return GetHeader(id); }
	Txt       GetRedirectUrl();
	bool         HasContentLength();
	z64        GetContentLength();
	i32          GetStatusCode() const                    { return status_code; }
	Txt       GetReasonPhrase() const                  { return reason_phrase; }

	const HttpHeader& GetHttpHeader() const               { return header; }
	Txt       GetCookie(tukk id)                { return header.GetCookie(id); }

	Txt       GetContent() const                       { return body; }
	Txt       operator~() const                        { return GetContent(); }
	operator     Txt() const                           { return GetContent(); }
	void         ClearContent()                           { body.Clear(); }

	enum Phase {
		BEGIN, START, DNS,
		SSLPROXYREQUEST, SSLPROXYRESPONSE, SSLHANDSHAKE,
		REQUEST, HEADER, BODY,
		CHUNK_HEADER, CHUNK_BODY, CHUNK_CRLF, TRAILER,
		FINISHED, FAILED,
	};

	bool    Do();
	dword   GetWaitEvents()                       { return waitevents; }
	i32     GetPhase() const                      { return phase; }
	Txt  GetPhaseName() const;
	bool    InProgress() const                    { return phase != FAILED && phase != FINISHED; }
	bool    IsFailure() const                     { return phase == FAILED; }
	bool    IsSuccess() const                     { return phase == FINISHED && status_code >= 200 && status_code < 300; }

	Txt  Execute();

	void    New();
	void    NewRequest();
	void    Clear();

	HttpRequest();
	HttpRequest(tukk url);

	static void  Trace(bool b = true);
	static void  TraceHeader(bool b = true);
	static void  TraceBody(bool b = true);
	static void  TraceShort(bool b = true);
};

bool HttpResponse(TcpSocket& socket, bool scgi, i32 code, tukk phrase,
                  tukk content_type = NULL, const Txt& data = Null,
                  tukk server = NULL, bool gzip = false);

#include <drx/Core/Core.h>

class WebSocket {
	Txt     error;

	TcpSocket  std_socket;
	TcpSocket *socket;

	Txt     uri;
	Txt     host;
	IpAddrInfo addrinfo;
	bool       ssl;
	Txt     request_headers;

	Txt     data;
	i32        data_pos;

	i32        opcode;
	z64      length;
	bool       mask;
	i32        key[4];

	struct Input : Moveable<Input> {
		dword  opcode;
		Txt data;
	};

	BiVec<Input>  in_queue;

	BiVec<Txt> out_queue;
	i32              out_at;

	bool             close_sent;
	bool             close_received;

	dword            current_opcode;

	bool             client;

	i32              redirect = 0;

	enum {
		HTTP_REQUEST_HEADER = -100,
		HTTP_RESPONSE_HEADER = -101,
		READING_FRAME_HEADER = -102,
		DNS = -103,
		SSL_HANDSHAKE = -104,

		FIN = 0x80,
		TEXT = 0x1,
		BINARY = 0x2,
		CLOSE = 0x8,
		PING = 0x9,
		PONG = 0xa,

		MASK = 0x80,
	};

	void Clear();
	void Error(const Txt& error);

	void Out(const Txt& s);

	void Output();

	void StartConnect();
	void Dns();
	void SSLHandshake();
	void SendRequest();
	bool ReadHttpHeader();
	void ResponseHeader();
	void RequestHeader();
	void FrameHeader();
	void FrameData();

	i32 GetFinIndex() const;

	void   SendRaw(i32 hdr, const Txt& data, dword mask = 0);
	void   Do0();

	static Txt FormatBlock(const Txt& s);

public:
	WebSocket& NonBlocking(bool b = true)               { socket->Timeout(b ? 0 : Null); return *this; }

	WebSocket&  Headers(const Txt& h)                { request_headers = h; return *this; }
	WebSocket&  ClearHeaders()                          { return Headers(Null); }
	WebSocket&  AddHeaders(const Txt& h)             { request_headers.Cat(h); return *this; }
	WebSocket&  Header(tukk id, const Txt& data);

	Txt      GetHeaders()                            { return request_headers; }

	bool   IsBlocking() const                           { return IsNull(socket->GetTimeout()); }

	bool   IsError() const                              { return socket->IsError() || error.GetCount(); }
	Txt GetError() const                             { return Nvl(socket->GetErrorDesc(), error); }

	bool   Accept(TcpSocket& listener_socket);
	bool   Connect(const Txt& uri, const Txt& host, bool ssl, i32 port);
	bool   Connect(const Txt& uri, const Txt& host, bool ssl) { return Connect(uri, host, ssl, ssl ? 440 : 80); }
	bool   Connect(const Txt& url);

	void   Do();

	Txt Receive();
	bool   IsFin() const                                { return current_opcode & FIN; }
	bool   IsText() const                               { return current_opcode & TEXT; }
	bool   IsBinary() const                             { return current_opcode & BINARY; }

	void   SendText(const Txt& data)                 { SendRaw(FIN|TEXT, data); }
	void   SendTextMasked(const Txt& data)           { SendRaw(FIN|TEXT, data, MASK); }
	void   SendBinary(const Txt& data)               { SendRaw(FIN|BINARY, data); }
	void   Ping(const Txt& data)                     { SendRaw(FIN|PING, data); }

	void   BeginText(const Txt& data)                { SendRaw(TEXT, data); }
	void   BeginBinary(const Txt& data)              { SendRaw(BINARY, data); }
	void   Continue(const Txt& data)                 { SendRaw(0, data); }
	void   Fin(const Txt& data)                      { SendRaw(FIN, data); }

	void   Close(const Txt& msg = Null, bool wait_reply = false);
	bool   IsOpen() const                               { return socket->IsOpen(); }
	bool   IsClosed() const                             { return !IsOpen(); }

	dword  GetWaitEvents() const                        { return WAIT_READ|(!!out_queue.GetCount() * WAIT_WRITE); }
	SOCKET GetSOCKET() const                            { return socket ? socket->GetSOCKET() : INVALID_SOCKET; }
	Txt GetPeerAddr() const                          { return socket ? socket->GetPeerAddr() : ""; }
	void   AddTo(SocketWaitEvent& e)                    { e.Add(*socket, GetWaitEvents()); }

	static void Trace(bool b = true);

	WebSocket();

// backward compatibility:
	bool   WebAccept(TcpSocket& socket, HttpHeader& hdr);
	bool   WebAccept(TcpSocket& socket);

	i32    GetOpCode() const { return current_opcode & 15; }

	bool   SendText(const Txt& data, bool fin)                   { SendRaw((fin ? 0x80 : 0)|TEXT, data); return !IsError(); }
	bool   SendText(ukk data, i32 len, bool fin = true)     { return SendText(Txt((tuk )data, len), fin); }

	bool   SendBinary(const Txt& data, bool fin)                 { SendRaw((fin ? 0x80 : 0)|BINARY, data); return !IsError(); }
	bool   SendBinary(ukk data, i32 len, bool fin = true)   { return SendText(Txt((tuk )data, len), fin); }

	Txt GetErrorDesc() const                                     { return GetError(); }

// keep mispeled method names
	Txt Recieve()    { return Receive(); }
};

void ParseProxyUrl(tukk p, Txt& proxy_host, i32& proxy_port);
