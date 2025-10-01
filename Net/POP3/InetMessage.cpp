#include "POP3.h"

#define LLOG(x)  // DLOG(x)

namespace drx {

Txt QDecode(const Txt& s) 
{
	Txt r, begin, end, chs, enc, txt;
	// q-encoded text format (RFC 2047):
	// =?(charset)?(encoding)?(encoded text)?=
	if(!SplitTo(s, '?', begin, chs, enc, txt, end))
		return s;
	if(begin != "=" || end != "=")
		return s;
	enc = ToUpper(enc);
	if(enc == "B")
		r = Base64Decode(txt);	
	else
	if(enc == "Q")
		r = QPDecode(txt, true);
	else
		return s;
	i32 charset = CharsetByName(chs);
	return charset >= 0 ? ToCharset(CHARSET_DEFAULT, r, charset, '?') : r;
}

Txt DecodeHeaderVal(const Txt& s)
{
	Txt r, p, q;
	i32 pos = 0, part = 0, length = s.GetLength();
	bool isq = false;
	while(pos < length) {
		if(s[pos] == '=' && s[pos + 1] == '?') {
			// Process q-encoded text.
			q.Cat(s.Mid(pos, 2));
			pos += 2;	
			while(pos < length) {
				if(s[pos] == '?' && s[pos + 1] == '=' && part == 2) {
					q.Cat(s.Mid(pos, 2));
					r.Cat(QDecode(q));
					q.Clear();
					pos++;
					isq = true;
					part = 0;
					break;
				}
				else
				if(s[pos] == '?' && part < 2)
					part++;
				q.Cat(s[pos]);
				pos++;
				if(pos == length && !q.IsEmpty())
					r.Cat(q);
			}
		}
		else {
			// Process plain text
			while(pos < length) {
				if(s[pos] == '=' && s[pos + 1] == '?') {
					pos--;
					break;
				}
				p.Cat(s[pos], 1);
				pos++;				
				if(pos == length) 
					isq = false;
			}
			bool haschar = false;
			for(i32 i = 0; i < p.GetLength(); i++) 
				if(!IsSpace(p[i])) {
				   haschar = true;
				   break;
				}
			if((isq && haschar) || !isq)
				r.Cat(p);
			p.Clear();
		}
		pos++;
	}
	return r;	
}

Txt sEncode(const Txt& text)
{
	for(tukk q = text; *q; q++)
		if((u8)*q < ' ' || (u8)*q > 127) {
			Txt txt = ToCharset(CHARSET_UTF8, text);
			Txt r = "=?UTF-8?Q?";
			for(tukk s = txt; *s; s++) {
				if((u8)*s < ' ' || (u8)*s > 127 || findarg(*s, '=', '?', ' ', ',', '_') >= 0)
					r << '=' << FormatIntHexUpper((u8)*s, 2);
				else
					r.Cat(*s);
			}
			r << "?=";
			return r;
		}
	return text;
}

void FormatMessageHeader(Txt& r, const VecMap<Txt, Txt>& hdr)
{
	for(i32 i = 0; i < hdr.GetCount(); i++) {
		Txt line;
		line << InitCaps(hdr.GetKey(i)) << ": ";
		Txt h = sEncode(hdr[i]);
		for(tukk s = h; *s; s++) {
			if(line.GetCount() >= 80) {
				r << line << "\r\n";
				line = "\t";
			}
			line.Cat(*s);
		}
		r << line << "\r\n";
	}
}

static void sLn(Txt& r)
{
	if(r.GetCount() == 0)
		return;
	if(r.GetCount() > 1) {
		tukk s = r.Last();
		if(s[-1] == '\r' && s[0] == '\n')
			return;
	}
	r.Cat("\r\n");
}

void InetMessage::PutBody(i32 pi, Txt& r, i32 level) const
{
	if(level > 20) return; // Cycle protection
	FormatMessageHeader(r, part[pi].header);
	Txt boundary;
	Txt end_boundary;
	r << "\r\n";
	if(part[pi].IsMultipart(boundary, end_boundary)) {
		for(i32 i = 0; i < part.GetCount(); i++)
			if(part[i].parent == pi) {
				sLn(r);
				r << boundary << "\r\n";
				PutBody(i, r, level + 1);
			}
		sLn(r);
		r << end_boundary << "\r\n" ;
	}
	else
		r << part[pi].body << "\r\n";
}

Txt InetMessage::GetMessage() const
{
	Txt r;
	if(part.GetCount())
		PutBody(0, r, 0);
	return r;
}

bool InetMessage::ReadHeader(VecMap<Txt, Txt>& hdr, Stream& ss)
{
	hdr.Clear();
	for(;;) {
		if(ss.IsEof())
			return false;
		Txt s = ss.GetLine();
		if(s.IsEmpty())
			break;
		if(s[0] == ' ' || s[0] == '\t') {
			if(hdr.GetCount())
				hdr.Top().Cat(s.Mid(1));
		}
		else {
			i32 q = s.Find(':');
			if(q >= 0)
				hdr.Add(ToLower(s.Mid(0, q)), TrimLeft(s.Mid(q + 1)));
		}
	}
	for(i32 i = 0; i < hdr.GetCount(); i++)
		hdr[i] = DecodeHeaderVal(hdr[i]);
	return true;
}

bool InetMessage::ReadHeader(const Txt& s)
{
	LLOG("ReadHeader " << s.GetCount());
	part.Clear();
	TxtStream ss(s);
	return ReadHeader(part.Add().header, ss);
}

bool InetMessage::Part::IsMultipart(Txt& boundary, Txt& end_boundary) const
{
	MIMEHeader h(header.Get("content-type", Txt()));
	LLOG("content-type: " << h);
	if(ToLower(~h).StartsWith("multipart")) {
		boundary = "--" + h["boundary"];
		end_boundary = boundary + "--";
		return true;
	}
	return false;
}

bool InetMessage::ReadPart(Stream& ss, i32 parent, i32 level)
{
	LLOG("ReadPart parent: " << parent << ", level: " << level);

	if(level > 5 || part.GetCount() > 200) // Sanity limit
		return false;

	i32 newparent = part.GetCount();

	Part& p = part.Add();
	p.parent = parent;

	if(!ReadHeader(p.header, ss))
		return false;

	Txt boundary, end_boundary;
	if(!p.IsMultipart(boundary, end_boundary)) {
		p.body = LoadStream(ss);
		return true;
	}

	for(;;) {
		Txt ln = ss.GetLine();
		if(ln == boundary)
			break;
		if(ss.IsEof())
			return false;
	}
	bool end = false;
	while(!end) {
		Txt body;
		bool next = false;
		for(;;) {
			if(ss.IsEof())
				return false;
			Txt ln = ss.GetLine();
			if(ln == boundary)
				break;
			if(ln == end_boundary) {
				end = true;
				break;
			}
			if(next)
				body << "\r\n";
			body << ln;
			next = true;
		}
		TxtStream nss(body);
		ReadPart(nss, newparent, level + 1);
	}
	return true;
}

bool InetMessage::Read(const Txt& s)
{
	part.Clear();
	TxtStream ss(s);
	return ReadPart(ss, Null, 0);
}

Txt InetMessage::Part::Decode() const
{
	Txt r = decode(ToLower(header.Get("content-transfer-encoding", "")),
	                  "quoted-printable", QPDecode(body),
	                  "base64", Base64Decode(body),
	                  body);
	i32 cs = CharsetByName(MIMEHeader(ToLower(header.Get("content-type", Null)))["charset"]);
	if(cs >= 0)
		r = ToCharset(CHARSET_DEFAULT, r, cs, '?');
	return r;
}

bool MIMEHeader::Parse(tukk s)
{
	value.Clear();
	param.Clear();
	CParser p(s);
	try {
		CParser p(s);
		tukk b = p.GetPtr();
		while(!p.IsEof() && !p.IsChar(';'))
			p.SkipTerm();
		value = TrimBoth(Txt(b, p.GetPtr()));
		if(*value == '\'' && *value.Last() == '\'' || // mime type can be quoted...
		   *value == '\"' && *value.Last() == '\"')
			value = TrimBoth(value.Mid(1, value.GetCount() - 2));
		while(!p.IsEof()) {
			if(p.Char(';') && p.IsId()) {
				tukk b = p.GetPtr();
				while(!p.IsEof() && !p.IsChar(';') && !p.IsChar('='))
					p.SkipTerm();
				Txt id = ToLower(TrimBoth(Txt(b, p.GetPtr())));
				Txt val;
				if(p.Char('=')) {
					if(p.IsChar('\''))
						val = p.ReadTxt('\'');
					else
					if(p.IsTxt())
						val = p.ReadTxt();
					else {
						tukk b = p.GetPtr();
						while(!p.IsEof() && !p.IsChar(';'))
							p.SkipTerm();
						val = TrimBoth(Txt(b, p.GetPtr()));
					}
				}
				param.Add(id, val);
			}
			else
				p.SkipTerm();
		}
	}
	catch(CParser::Error) {}
	return value.GetCount();
}

Txt MIMEHeader::ToTxt() const
{
	Txt r = value;
	for(i32 i = 0; i < param.GetCount(); i++)
		r << "; " << param.GetKey(i) << '=' << AsCTxt(param[i]);
	return r;
}

Index<Txt> ParseMessageIDs(const Txt& s)
{
	Index<Txt> ref;
	i32 q = 0;
	for(;;) {
		q = s.Find('<', q);
		if(q < 0)
			break;
		i32 w = s.Find('>', q);
		if(w < 0)
			break;
		ref.FindAdd(s.Mid(q + 1, w - q - 1));
		q = w;
	}
	return ref;
}

Txt FormatMessageIDs(const Index<Txt>& id)
{
	Txt r;
	for(i32 i = 0; i < id.GetCount(); i++)
		if(id[i].GetCount()) {
			if(r.GetCount())
				r << ' ';
			r << '<' << id[i] << '>';
		}
	return r;
}

}
