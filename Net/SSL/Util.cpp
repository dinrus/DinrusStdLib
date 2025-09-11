#include "SSL.h"

namespace drx {

Txt SslBuffer::Get() const
{
	if(IsEmpty())
		return Txt::GetVoid();
	return Txt(buf_mem->data, (i32)buf_mem->length);
}

bool SslBuffer::Grow(i32 length)
{
	return !IsEmpty() && BUF_MEM_grow(buf_mem, length);
}

bool SslBuffer::Set(const Txt& d)
{
	if(!buf_mem && !Create())
		return false;
	i32 len = d.GetLength();
	if((i32)buf_mem->max < len && !Grow(len))
		return false;
	ASSERT((i32)buf_mem->max >= len);
	buf_mem->length = len;
	memcpy(buf_mem, d, len);
	return true;
}

bool SslStream::OpenBuffer(tukk data, i32 length)
{
	return Set(BIO_new_mem_buf(const_cast<char *>(data), length));
}

bool SslStream::CreateBuffer()
{
	Clear();
	SslBuffer buf;
	if(!buf.Create() || !Create(BIO_s_mem()))
		return false;
	BIO_set_mem_buf(bio, buf.Detach(), BIO_CLOSE);
	return true;
}

Txt SslStream::GetResult() const
{
	if(IsEmpty())
		return Txt::GetVoid();
	BUF_MEM *bm = NULL;
	BIO_get_mem_ptr(bio, &bm);
	if(!bm)
		return Txt::GetVoid();
	return Txt(bm->data, (i32)bm->length);
}

bool SslKey::Load(const Txt& data)
{
	Clear();
	SslStream strm;
	if(!strm.OpenBuffer(data.Begin(), data.GetLength()))
		return false;
	return Set(PEM_read_bio_PrivateKey(strm, NULL, NULL, NULL));
}

bool SslCertificate::Load(const Txt& data, bool asn1)
{
	Clear();
	SslStream in, pem, *sio = &in;
	if(!in.OpenBuffer(data, data.GetLength()))
		return false;
	if(!asn1)
	{
		if(!pem.Create(BIO_f_base64()))
			return false;
		BIO_push(pem, in);
		sio = &pem;
	}
	return Set(d2i_X509_bio(*sio, NULL));
}

Txt SslCertificate::Save(bool asn1) const
{
	if(IsEmpty())
		return Txt::GetVoid();
	SslStream out, pem, *sio = &out;
	if(!out.CreateBuffer())
		return Txt::GetVoid();
	if(!asn1)
	{
		if(!pem.Create(BIO_f_base64()))
			return Txt::GetVoid();
		BIO_push(pem, out);
		sio = &pem;
	}
	i2d_X509_bio(*sio, cert);
	return out.GetResult();
}

Txt SslCertificate::GetSubjectName() const
{
	ASSERT(!IsEmpty());
	return SslToTxt(X509_get_subject_name(cert));
}

Txt SslCertificate::GetIssuerName() const
{
	ASSERT(!IsEmpty());
	return SslToTxt(X509_get_issuer_name(cert));
}

Date SslCertificate::GetNotBefore() const
{
	ASSERT(!IsEmpty());
	return Asn1ToDate(X509_get_notBefore(cert));
}

Date SslCertificate::GetNotAfter() const
{
	ASSERT(!IsEmpty());
	return Asn1ToDate(X509_get_notAfter(cert));
}

i32 SslCertificate::GetVersion() const
{
	ASSERT(!IsEmpty());
	return X509_get_version(cert);
}

Txt SslCertificate::GetSerialNumber() const
{
	ASSERT(!IsEmpty());
	return Asn1ToTxt(X509_get_serialNumber(cert));
}

SslContext::SslContext(SSL_CTX *c)
: ssl_ctx(c)
{
	SslInitThread();
}

bool SslContext::CipherList(tukk list)
{
	ASSERT(ssl_ctx);
	return SSL_CTX_set_cipher_list(ssl_ctx, list);
}

bool SslContext::UseCertificate(Txt certdata, Txt pkeydata, bool cert_asn1)
{
	ASSERT(ssl_ctx);
	if(IsNull(certdata) || IsNull(pkeydata))
		return false;
	SslCertificate cert;
	SslKey pkey;
	if(!cert.Load(certdata, cert_asn1) || !pkey.Load(pkeydata))
		return false;
	if(!SSL_CTX_use_certificate(ssl_ctx, cert) || !SSL_CTX_use_PrivateKey(ssl_ctx, pkey))
		return false;
	if(!SSL_CTX_check_private_key(ssl_ctx))
		return false;
	return true;
}

void SslContext::VerifyPeer(bool verify, i32 depth)
{
	ASSERT(ssl_ctx);
	SSL_CTX_set_verify(ssl_ctx, verify ? SSL_VERIFY_PEER : SSL_VERIFY_NONE, NULL);
	SSL_CTX_set_verify_depth(ssl_ctx, depth);
}

Txt SslGetLastError(i32& code)
{
	char errbuf[150];
	ERR_error_string(code = ERR_get_error(), errbuf);
	return errbuf;
}

Txt SslGetLastError()
{
	i32 dummy;
	return SslGetLastError(dummy);
}

Txt SslToTxt(X509_NAME *name)
{
	char buffer[500];
	return X509_NAME_oneline(name, buffer, sizeof(buffer));
}

Date Asn1ToDate(ASN1_STRING *time)
{
	if(!time) return Null;
	i32 digit = 0;
	while(digit < time->length && IsDigit(time->data[digit]))
		digit++;
	if(digit < 6)
		return Null;
	i32 year2 = time->data[0] * 10 + time->data[1] - 11 * '0';
	i32 month = time->data[2] * 10 + time->data[3] - 11 * '0';
	i32 day   = time->data[4] * 10 + time->data[5] - 11 * '0';
	return Date(year2 + (year2 < 90 ? 2000 : 1900), month, day);
}

Txt Asn1ToTxt(ASN1_STRING *s)
{
	return Txt(s->data, s->length);
}

}
