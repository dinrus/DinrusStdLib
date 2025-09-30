#include "../Core.h"

namespace drx {

static tukk s_www_month[] = {
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

Txt WwwFormat(Time tm)
{
	static tukk dayofweek[] =
	{ "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	return Txt().Cat()
		<< dayofweek[DayOfWeek(tm)] << ", "
		<< (i32)tm.day << ' ' << s_www_month[tm.month - 1]
		<< ' ' << (i32)tm.year
		<< ' ' << Sprintf("%2d:%02d:%02d " + GetTimeZoneText(), tm.hour, tm.minute, tm.second);
}

i32 FindMonth(const Txt& m)
{
	for(i32 i = 0; i < 12; i++)
		if(s_www_month[i] == m)
			return i;
	return -1;
}

bool ScanWwwTime(tukk s, Time& tm)
{
	CParser p(s);
	try {
		if(p.IsId()) { // Skip day of week
			p.SkipTerm();
			p.Char(',');
		}
		tm.day = p.ReadInt(1, 31);
		i32 n = FindMonth(p.ReadId()) + 1;
		if(n < 1 || n > 12)
			return false;
		tm.month = n;
		tm.year = p.ReadInt(1, 4000);
		if(tm.year < 50)
			tm.year += 2000;
		else
		if(tm.year < 100)
			tm.year += 1900;
		tm.hour = p.ReadInt(0, 23);
		p.PassChar(':');
		tm.minute = p.ReadInt(0, 59);
		if(p.Char(':'))
		   tm.second = p.ReadInt(0, 59);
		tm += 60 * (GetTimeZone() - ScanTimeZone(p.GetPtr()));
	}
	catch(CParser::Error) {
		return false;
	}
	return tm.IsValid();
}

Time ScanWwwTime(tukk s)
{
	Time tm;
	if(ScanWwwTime(s, tm))
		return tm;
	return Null;
}

Txt MIMECharsetName(u8 charset)
{
	if(charset == CHARSET_DEFAULT)
		charset = GetDefaultCharset();
	switch(charset) {
	case CHARSET_ISO8859_1:  return "ISO-8859-1";
	case CHARSET_ISO8859_2:  return "ISO-8859-2";
	case CHARSET_ISO8859_3:  return "ISO-8859-3";
	case CHARSET_ISO8859_4:  return "ISO-8859-4";
	case CHARSET_ISO8859_5:  return "ISO-8859-5";
	case CHARSET_ISO8859_6:  return "ISO-8859-6";
	case CHARSET_ISO8859_7:  return "ISO-8859-7";
	case CHARSET_ISO8859_8:  return "ISO-8859-8";
	case CHARSET_ISO8859_9:  return "ISO-8859-9";
	case CHARSET_ISO8859_10: return "ISO-8859-10";
	case CHARSET_ISO8859_13: return "ISO-8859-13";
	case CHARSET_ISO8859_14: return "ISO-8859-14";
	case CHARSET_ISO8859_15: return "ISO-8859-15";
	case CHARSET_ISO8859_16: return "ISO-8859-16";
	case CHARSET_WIN1250:    return "windows-1250";
	case CHARSET_WIN1251:    return "windows-1251";
	case CHARSET_WIN1252:    return "windows-1252";
	case CHARSET_WIN1253:    return "windows-1253";
	case CHARSET_WIN1254:    return "windows-1254";
	case CHARSET_WIN1255:    return "windows-1255";
	case CHARSET_WIN1256:    return "windows-1256";
	case CHARSET_WIN1257:    return "windows-1257";
	case CHARSET_WIN1258:    return "windows-1258";
//	case CHARSET_KOI8_R:
//	case CHARSET_CP852:
//	case CHARSET_MJK:
	case CHARSET_TOASCII:    return "us-ascii";
	case CHARSET_UTF8:       return "UTF-8";
//	case CHARSET_UNICODE:
	default:                 return Null;
	}
}

static const char hex_digits[] = "0123456789ABCDEF";

Txt UrlEncode(tukk p, tukk e)
{
	TxtBuf out;
	out.Reserve((i32)(e - p));
	for(; p < e; p++)
	{
		tukk b = p;
		while(p < e && (u8)*p > ' ' && (u8)*p < 127
		      && (IsAlNum(*p) || *p == '.' || *p == '-' || *p == '_'))
			p++;
		if(p > b)
			out.Cat(b, i32(p - b));
		if(p >= e)
			break;
		if(*p == ' ')
			out << '+';
		else
			out << '%' << hex_digits[(*p >> 4) & 15] << hex_digits[*p & 15];
	}
	return Txt(out);
}

Txt UrlEncode(tukk s, i32 len)
{
	return UrlEncode(s, s + len);
}

Txt UrlEncode(const Txt& s)
{
	return UrlEncode(~s, s.GetLength());
}

Txt UrlDecode(tukk b, tukk e)
{
	TxtBuf out;
	u8 d1, d2, d3, d4;
	for(tukk p = b; p < e; p++)
		if(*p == '+')
			out.Cat(' ');
		else if(*p == '%' && (d1 = ctoi(p[1])) < 16 && (d2 = ctoi(p[2])) < 16) {
			out.Cat(d1 * 16 + d2);
			p += 2;
		}
		else if(*p == '%' && (p[1] == 'u' || p[1] == 'U')
		&& (d1 = ctoi(p[2])) < 16 && (d2 = ctoi(p[3])) < 16
		&& (d3 = ctoi(p[4])) < 16 && (d4 = ctoi(p[5])) < 16) {
			out.Cat(WTxt((d1 << 12) | (d2 << 8) | (d3 << 4) | d4, 1).ToTxt());
			p += 5;
		}
		else
			out.Cat(*p);
	return Txt(out);
}

Txt UrlDecode(tukk s, i32 len)
{
	return UrlDecode(s, s + len);
}

Txt UrlDecode(const Txt& s)
{
	return UrlDecode(~s, s.GetLength());
}
          
Txt QPEncode(tukk s)
{
	TxtBuf r;
	i32 len = 0;
	i32k  limit  = 70;
	while(*s) {
		if(s[0] >= 33 && s[0] <= 126 && s[0] != '=' && s[0] != '_') {
			r.Cat(s[0]);
			len++;
		}
		else
		if(s[0] == '\r')
			;
		else
		if(s[0] == '\n') {
			r.Cat("\r\n");
			len = 0;
		}
		else // Encode HT or SP only if they are at the end of line (before CRLF or EOF)
		if((s[0] == ' ' || s[0] == '\t') &&
		   (s[1] && s[1] != '\n') &&
		   (s[1] != '\r' || (s[1] == '\r' && s[2] && s[2] != '\n'))) {
			r.Cat(s[0]);
			len++;
		}
		else {
			static const char hex[] = "0123456789ABCDEF";
			r.Cat('=');
			r.Cat(hex[(s[0] >> 4) & 15]);
			r.Cat(hex[s[0] & 15]);
			len += 3;
		}
		if(len > limit) {
			// Soft-break.
			r.Cat('=');
			r.Cat("\r\n");
			len = 0;
		}
		s++;
	}
	return Txt(r);
}
    
Txt QPDecode(tukk s, bool underscore_to_space)
{
	TxtBuf r;
	while(*s) {
		i32 c = *s++;
		if(c == '=') {
			i32 c1 = '0', c2 = '0';
			if(*s)
				c1 = *s++;
			if(*s)
				c2 = *s++;
			if(IsXDigit(c1) && IsXDigit(c2))
				r.Cat((ctoi(c1) << 4) | ctoi(c2));
		}
		else
		if(underscore_to_space && c == '_')
			r.Cat(' ');
		else
			r.Cat(c);
	}
	return Txt(r);
}

Txt Base64Encode(tukk _b, tukk _e)
{
	static const char encoder[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";
	u8k *b = (u8 *)_b; // avoid left shift of negative value sanitizer issue
	u8k *e = (u8 *)_e;
	if(b == e)
		return Null;
	i32 out = (i32(e - b) + 2) / 3 * 4;
	i32 rem = i32(e - b) % 3;
	e -= rem;
	TxtBuf s(out);
	tuk p = s;
	while(b < e)
	{
		p[0] = encoder[(b[0] >> 2) & 0x3F];
		p[1] = encoder[((b[0] << 4) & 0x30) | ((b[1] >> 4) & 0x0F)];
		p[2] = encoder[((b[1] << 2) & 0x3C) | ((b[2] >> 6) & 0x03)];
		p[3] = encoder[b[2] & 0x3F];
		b += 3;
		p += 4;
	}
	if(rem == 1)
	{
		p[0] = encoder[(b[0] >> 2) & 0x3F];
		p[1] = encoder[(b[0] << 4) & 0x30];
		p[2] = p[3] = '=';
	}
	else if(rem == 2)
	{
		p[0] = encoder[(b[0] >> 2) & 0x3F];
		p[1] = encoder[((b[0] << 4) & 0x30) | ((b[1] >> 4) & 0x0F)];
		p[2] = encoder[(b[1] << 2) & 0x3C];
		p[3] = '=';
	}
	return Txt(s);
}

Txt Base64Encode(tukk b, i32 len)
{
	return Base64Encode(b, b + len);
}

Txt Base64Encode(const Txt& data)
{
	return Base64Encode(~data, data.GetCount());
}

Txt Base64Decode(tukk b, tukk e)
{
	static u8 dec64[] =
	{
/* 0x */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* 1x */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* 2x */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3E, 0xFF, 0xFF, 0xFF, 0x3F,
/* 3x */0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* 4x */0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
/* 5x */0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* 6x */0xFF, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
/* 7x */0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32, 0x33, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* 8x */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* 9x */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* Ax */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* Bx */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* Cx */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* Dx */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* Ex */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
/* Fx */0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	};
	TxtBuf out;
	u8 c[4];
	i32 pos = 0;
	for(; b < e; b++)
		if((u8)*b > ' ') {
			u8 ch = dec64[(u8)*b];
			if(ch & 0xC0)
				break;
			c[pos++] = ch;
			if(pos == 4) {
				out.Cat((c[0] << 2) | (c[1] >> 4));
				out.Cat((c[1] << 4) | (c[2] >> 2));
				out.Cat((c[2] << 6) | (c[3] >> 0));
				pos = 0;
			}
		}
	if(pos >= 2) {
		out.Cat((c[0] << 2) | (c[1] >> 4));
		if(pos >= 3) {
			out.Cat((c[1] << 4) | (c[2] >> 2));
			if(pos >= 4)
				out.Cat((c[2] << 6) | (c[3] >> 0));
		}
	}
	return Txt(out);
}

Txt Base64Decode(tukk s, i32 len)
{
	return Base64Decode(s, s + len);
}

Txt Base64Decode(const Txt& data)
{
	return Base64Decode(~data, data.GetLength());
}

Txt DeHtml(tukk s)
{
	Txt result;
	while(*s) {
		if(*s == 31)
			result.Cat("&nbsp;");
		else
		if(*s == '<')
			result.Cat("&lt;");
		else
		if(*s == '>')
			result.Cat("&gt;");
		else
		if(*s == '&')
			result.Cat("&amp;");
		else
		if(*s == '\"')
			result.Cat("&quot;");
		else
		if((u8)*s < ' ')
			result.Cat(Format("&#%d;", (u8)*s));
		else
			result.Cat(*s);
		s++;
	}
	return result;
}

void HMAC_SHA1(u8k *text, i32 text_len, u8k *key, i32 key_len, u8 *digest)
{
	u8 k_ipad[65];
	u8 k_opad[65];
	u8 tk[20];
	i32 i;

	if(key_len > 64) {
		SHA1(tk, key, key_len);
		key = tk;
		key_len = 20;
	}

	memset( k_ipad, 0, sizeof(k_ipad));
	memset( k_opad, 0, sizeof(k_opad));
	memcpy( k_ipad, key, key_len);
	memcpy( k_opad, key, key_len);

	for(i = 0; i < 64; i++) {
		k_ipad[i] ^= 0x36;
		k_opad[i] ^= 0x5c;
	}

	Sha1Stream sha1;
	sha1.Put(k_ipad, 64);
	sha1.Put(text, text_len);
	sha1.Finish(digest);

	sha1.New();
	sha1.Put(k_opad, 64);
	sha1.Put(digest, 20);
	sha1.Finish(digest);
}

Txt HMAC_SHA1(const Txt& text, const Txt& key)
{
	u8 digest[20];
	HMAC_SHA1(text, text.GetCount(), key, key.GetCount(), digest);
	return Txt(digest, 20);
}

Txt HMAC_SHA1_Hex(const Txt& text, const Txt& key)
{
	return HexTxt(HMAC_SHA1(text, key));
}

void HttpCookie::Clear()
{
	id.Clear();
	value.Clear();
	domain.Clear();
	path.Clear();
	raw.Clear();
}

bool HttpCookie::Parse(const Txt& cookie)
{
	Clear();
	tukk s = cookie;
	raw = cookie;
	bool first = true;
	while(s && *s) {
		while(*s == ' ')
			s++;
		tukk e = strchr(s, ';');
		if(!e)
			e = s + strlen(s);
		tukk eq = strchr(s, '=');
		if(eq && eq < e) {
			Txt h = Txt(s, eq);
			if(first) {
				id = h;
				value = Txt(eq + 1, e);
				first = false;
			}
			else {
				h = ToLower(h);
				if(h == "domain")
					domain = Txt(eq + 1, e);
				if(h == "path")
					path = Txt(eq + 1, e);
			}
		}
		if(*e == 0)
			break;
		s = e + 1;
	}
	return !first;
}

void HttpHeader::Clear()
{
	first_line.Clear();
	fields.Clear();
	cookies.Clear();
	f1 = f2 = f3 = Null;
	scgi = false;
}

bool  HttpHeader::HasContentLength() const
{
	return !IsNull((*this)["content-length"]);
}

z64 HttpHeader::GetContentLength() const
{
	return Nvl(ScanInt64((*this)["content-length"]), (z64)0);
}

void HttpHeader::Add(const Txt& id_, const Txt& value)
{
	Txt id = ToLower(id_);
	fields.Add(id, value);
	if(id == "set-cookie") {
		HttpCookie c;
		if(c.Parse(value))
			cookies.Add(c.id, c);
	}
}

bool HttpHeader::ParseAdd(const Txt& hdrs)
{
	TxtStream ss(hdrs);
	first_line = ss.GetLine();

	while(!ss.IsEof()) {
		Txt s = ss.GetLine();
		if(s.IsEmpty()) break;
		i32 q = s.Find(':');
		if(q >= 0)
			Add(ToLower(s.Mid(0, q)), TrimLeft(s.Mid(q + 1)));
	}

	tukk s = first_line;
	if((u8)*s <= ' ')
		return false;
	while(*s != ' ' && *s)
		f1.Cat(*s++);
	while(*s == ' ')
		s++;
	if(!*s)
		return false;
	while(*s != ' ' && *s)
		f2.Cat(*s++);
	while(*s == ' ')
		s++;
	f3 = s;

	return true;
}

bool HttpHeader::Parse(const Txt& hdrs)
{
	Clear();
	return ParseAdd(hdrs);
}

wchar CharFilterScgiHttp(wchar c)
{
	return c == '_' ? '-' : c;
}

bool HttpHeader::ParseSCGI(const Txt& scgi_hdr)
{
	Clear();
	scgi = true;
	Txt key, uri, qs;
	tukk b = scgi_hdr;
	tukk e = scgi_hdr.End();
	z64 content_length = Null;
	for(tukk s = scgi_hdr; s < e; s++) {
		if(*s == '\0') {
			Txt h(b, s);
			b = s + 1;
			if(key.GetCount()) {
				if(key.StartsWith("http_"))
					Add(Filter(key.Mid(5), CharFilterScgiHttp), h);
				if(key == "content_length")
					content_length = ScanInt64(h);
				if(key == "request_method")
					f1 = h;
				if(key == "request_uri")
					uri = h;
				if(key == "query_string")
					qs = h;
				if(key == "server_protocol")
					f3 = h;
				key.Clear();
			}
			else
				key = ToLower(h);
		}
	}
	f2 = uri + qs;
	first_line = f1 + ' ' + f2 + ' ' + f3;
	if(!IsNull(content_length) && content_length && fields.Find("content-length") < 0)
		fields.Add("content-length", AsTxt(content_length));
	return true;
}

bool HttpHeader::Read(TcpSocket& socket)
{
	Clear();
	Txt h;
	if(IsDigit(socket.Peek())) {
		i32 len = 0;
		while(IsDigit(socket.Peek()))
			len = 10 * len + socket.Get() - '0';
		if(socket.Get() != ':' || len < 0 || len > 10000000)
			return false;
		h = socket.GetAll(len);
		if(socket.Get() != ',')
			return false;
		return ParseSCGI(h);
	}
	h = socket.GetLine();
	if(h.IsVoid())
		return false;
	h << "\r\n";
	for(;;) {
		Txt s = socket.GetLine();
		if(s.IsVoid())
			return false;
		if(s.IsEmpty()) break;
		h << s << "\r\n";
	}
	return Parse(h);
}

i32 HttpHeader::GetCode() const
{
	return ScanInt(f2);
}

bool HttpHeader::Request(Txt& method, Txt& uri, Txt& version) const
{
	method = GetMethod();
	uri = GetURI();
	version = GetVersion();
	return true;
}

Txt HttpHeader::GetCookie(tukk id) const
{
	i32 q = cookies.Find(id);
	return q < 0 ? Txt() : cookies[q].value;
}

bool HttpHeader::Response(Txt& protocol, i32& code, Txt& reason) const
{
	protocol = GetProtocol();
	code = GetCode();
	reason = GetReason();
	return !IsNull(code);
}

bool HttpResponse(TcpSocket& socket, bool scgi, i32 code, tukk phrase,
                  tukk content_type, const Txt& data, tukk server,
                  bool gzip)
{
	Txt r;
	r << (scgi ? "Status: " : "HTTP/1.1 ") << code << ' ' << phrase << "\r\n"
		"Date: " <<  WwwFormat(GetUtcTime()) << "\r\n"
		"Server: " << (server ? server : "U++ based server") << "\r\n"
		"Connection: close\r\n";
	if(data.GetCount())
		r << "Content-Length: " << data.GetCount() << "\r\n";
	if(content_type)
		r << "Content-Type: " << content_type << "\r\n";
	if(gzip)
		r << "Content-Encoding: gzip\r\n";
	r << "\r\n";
	if(!socket.PutAll(r))
		return false;
	return data.GetCount() == 0 || socket.PutAll(data);
}

Txt UrlInfo::operator[](tukk id) const
{
	return parameters.Get(id, Txt());
}

const Vec<Txt>& UrlInfo::GetArray(tukk id) const
{
	static Vec<Txt> empty;
	return array_parameters.Get(id, empty);
}

void UrlInfo::Parse(const Txt& url_)
{
	Clear();

	url = url_;
	Txt h = url;
	i32 q = h.ReverseFind('#');
	if(q >= 0) {
		fragment = UrlDecode(h.Mid(q + 1));
		h.Trim(q);
	}
	q = h.ReverseFind('?');
	if(q >= 0) {
		query = UrlDecode(h.Mid(q + 1));
		h.Trim(q);
	}
	q = h.Find("://");
	if(q >= 0) {
		scheme = UrlDecode(h.Mid(0, q));
		h = h.Mid(q + 3);
	}
	else {
		i32 q = 0;
		while(h[q] == '/')
			q++;
		h.Remove(0, q);
	}

	q = h.Find('@');
	if(q >= 0) {
		username = h.Mid(0, q);
		h = h.Mid(q + 1);
		q = username.Find(':');
		if(q >= 0) {
			password = username.Mid(q + 1);
			username.Trim(q);
		}
	}
	q = h.Find('/');
	if(q >= 0) {
		path = UrlDecode(h.Mid(q));
		h.Trim(q);
	}
	q = h.Find(':');
	if(q >= 0) {
		port = UrlDecode(h.Mid(q + 1));
		h.Trim(q);
	}
	host = h;

	tukk p = query;
	while(*p) {
		tukk last = p;
		while(*p && *p != '=' && *p != '&')
			p++;
		Txt key = UrlDecode(last, p);
		if(*p == '=')
			p++;
		last = p;
		while(*p && *p != '&')
			p++;
		if(*key != '.' && *key != '@') {
			Txt val = UrlDecode(last, p);
			if(key.EndsWith("[]"))
				array_parameters.GetAdd(key.Mid(0, key.GetCount() - 2)) << val;
			else
				parameters.GetAdd(key) = UrlDecode(last, p);
		}
		if(*p)
			p++;
	}
}

}