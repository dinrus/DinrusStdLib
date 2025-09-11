#include "SSL.h"

#define LLOG(x)  // DLOG(x)

namespace drx {

struct TcpSocket::SSLImp : TcpSocket::SSL {
	virtual bool  Start();
	virtual bool  Wait(dword flags, i32 end_time);
	virtual i32   Send(ukk buffer, i32 maxlen);
	virtual i32   Recv(uk buffer, i32 maxlen);
	virtual void  Close();
	virtual dword Handshake();

	TcpSocket&     socket;
	SslContext     context;
	::SSL         *ssl;
	SslCertificate cert;

	i32            GetErrorCode(i32 res);
	Txt         GetErrorText(i32 code) const;
	void           SetSSLError(tukk context);
	void           SetSSLResError(tukk context, i32 res);
	bool           IsAgain(i32 res) const;
	
	SSLImp(TcpSocket& socket) : socket(socket) { ssl = NULL; LLOG("SSLImp(" << socket.GetSOCKET() << ")"); }
	~SSLImp();
};

TcpSocket::SSL *TcpSocket::CreateSSLImp(TcpSocket& socket)
{
	return new TcpSocket::SSLImp(socket);
}

void InitCreateSSL()
{
	TcpSocket::CreateSSL = TcpSocket::CreateSSLImp;
}

INITIALIZER(SSLSocket) {
	InitCreateSSL();
}

TcpSocket::SSLImp::~SSLImp()
{
	if(ssl)
		SSL_free(ssl);
}

void TcpSocket::SSLImp::SetSSLError(tukk context)
{
	i32 code;
	Txt text = SslGetLastError(code);
	socket.SetSockError(context, code, text);
}

tukk TcpSocketErrorDesc(i32 code);

i32 TcpSocket::SSLImp::GetErrorCode(i32 res)
{
	return SSL_get_error(ssl, res);
}

Txt TcpSocket::SSLImp::GetErrorText(i32 code) const
{
	Txt out;
	switch(code) {
#define SSLERR(c) case c: out = #c; break;
		SSLERR(SSL_ERROR_NONE)
		SSLERR(SSL_ERROR_SSL)
		SSLERR(SSL_ERROR_WANT_READ)
		SSLERR(SSL_ERROR_WANT_WRITE)
		SSLERR(SSL_ERROR_WANT_X509_LOOKUP)
		SSLERR(SSL_ERROR_SYSCALL)
		SSLERR(SSL_ERROR_ZERO_RETURN)
		SSLERR(SSL_ERROR_WANT_CONNECT)
#ifdef TARGET_WIN32
		SSLERR(SSL_ERROR_WANT_ACCEPT)
#endif
		default: out = "неизвестный код"; break;
	}
	return out;
}

void TcpSocket::SSLImp::SetSSLResError(tukk context, i32 res)
{
	i32 code = GetErrorCode(res);
	if(code == SSL_ERROR_SYSCALL) {
		socket.SetSockError(context);
		return;
	}
	Txt txt = GetErrorText(code);
	i32 err = ERR_get_error();
	if(err) {
		char h[260];
		ERR_error_string(err, h);
		txt << "; " << h;
	}
	socket.SetSockError(context, code, txt);
}

bool TcpSocket::SSLImp::IsAgain(i32 res) const
{
	res = SSL_get_error(ssl, res);
	return res == SSL_ERROR_WANT_READ ||
	       res == SSL_ERROR_WANT_WRITE ||
	       res == SSL_ERROR_WANT_CONNECT ||
	       res == SSL_ERROR_WANT_ACCEPT;
}

bool TcpSocket::SSLImp::Start()
{
	LLOG("SSL Start");

#if 0 // bug hunting
	i32 n = socket.GetTimeout(); _DBG_
	socket.Timeout(Null);
	socket.Wait(WAIT_WRITE);
	socket.Timeout(n);
#endif

	ERR_clear_error();

	if(!context.Create(socket.mode == CONNECT ? const_cast<SSL_METHOD *>(SSLv23_client_method())
	                                          : const_cast<SSL_METHOD *>(SSLv23_server_method()))) {
		SetSSLError("Start: SSL context.");
		return false;
	}
	if(socket.cert.GetCount())
		context.UseCertificate(socket.cert, socket.pkey, socket.asn1);
	if(!(ssl = SSL_new(context))) {
		SetSSLError("Start: SSL_new");
		return false;
	}

	if(socket.sni.GetCount()) {
		Buffer<char> h(socket.sni.GetCount() + 1);
		strcpy(~h, ~socket.sni);
		SSL_set_tlsext_host_name(ssl, h);
	}

	if(!SSL_set_fd(ssl, (i32)socket.GetSOCKET())) {
		SetSSLError("Start: SSL_set_fd");
		return false;
	}
	return true;
}

dword TcpSocket::SSLImp::Handshake()
{
	i32 res;
	ERR_clear_error();
	if(socket.mode == ACCEPT)
		res = SSL_accept(ssl);
	else
	if(socket.mode == CONNECT)
		res = SSL_connect(ssl);
	else
		return 0;
	if(res <= 0) {
		i32 code = GetErrorCode(res);
		if(code == SSL_ERROR_WANT_READ)
			return WAIT_READ;
		if(code == SSL_ERROR_WANT_WRITE)
			return WAIT_WRITE;
	#ifdef TARGET_WIN32
		if(code == SSL_ERROR_SYSCALL && socket.GetErrorCode() == WSAENOTCONN)
	#else
		if(code == SSL_ERROR_SYSCALL && socket.GetErrorCode() == ENOTCONN)
	#endif
			return WAIT_WRITE;
		SetSSLResError("SSL handshake", res);
		return 0;
	}
	socket.mode = SSL_CONNECTED;
	cert.Set(SSL_get_peer_certificate(ssl));
	SSLInfo& f = socket.sslinfo.Create();
	f.cipher = SSL_get_cipher(ssl);
	if(!cert.IsEmpty()) {
		f.cert_avail = true;
		f.cert_subject = cert.GetSubjectName();
		f.cert_issuer = cert.GetIssuerName();
		f.cert_serial = cert.GetSerialNumber();
		f.cert_notbefore = cert.GetNotBefore();
		f.cert_notafter = cert.GetNotAfter();
		f.cert_version = cert.GetVersion();
		f.cert_verified = SSL_get_verify_result(ssl) == X509_V_OK;
	}
	return 0;
}

bool TcpSocket::SSLImp::Wait(dword flags, i32 end_time)
{
	LLOG("SSL Wait");
	if((flags & WAIT_READ) && SSL_pending(ssl) > 0)
		return true;
	return socket.RawWait(flags, end_time);
}

i32 TcpSocket::SSLImp::Send(ukk buffer, i32 maxlen)
{
	LLOG("SSL Send " << maxlen);
	ERR_clear_error();
	i32 res = SSL_write(ssl, (tukk )buffer, maxlen);
	if(res > 0)
		return res;
	if(res == 0)
		socket.is_eof = true;
	else
	if(!IsAgain(res))
		SetSSLResError("SSL_write", res);
	return 0;
}

i32 TcpSocket::SSLImp::Recv(uk buffer, i32 maxlen)
{
	LLOG("SSL Recv " << maxlen);
	ERR_clear_error();
	i32 res = SSL_read(ssl, (char *)buffer, maxlen);
	if(res > 0)
		return res;
	if(res == 0)
		socket.is_eof = true;
	else
	if(!IsAgain(res))
		SetSSLResError("SSL_read", res);
	return 0;
}

void TcpSocket::SSLImp::Close()
{
	LLOG("SSL Close");
	SSL_shutdown(ssl);
	socket.RawClose();
	SSL_free(ssl);
	ssl = NULL;
}

}
