#include "../Core.h"

namespace drx {

#define LLOG(x)  // LOG(x)

static inline void sDeXmlChar(TxtBuf& result, char chr, u8 charset, bool escapelf)
{
	/**/ if(chr == '<')  result.Cat("&lt;");
	else if(chr == '>')  result.Cat("&gt;");
	else if(chr == '&')  result.Cat("&amp;");
	else if(chr == '\'') result.Cat("&apos;");
	else if(chr == '\"') result.Cat("&quot;");
	else if((u8)chr < ' ' && (escapelf || chr != '\n' && chr != '\t' && chr != '\r'))
		result.Cat(Format("&#x%02x;", (u8)chr));
	else if(!(chr & 0x80) || charset == CHARSET_UTF8) result.Cat(chr);
	else result.Cat(ToUtf8(ToUnicode(chr, charset)));
}

Txt DeXml(tukk s, u8 charset, bool escapelf)
{
	if(charset == CHARSET_DEFAULT)
		charset = GetDefaultCharset();
	TxtBuf result;
	for(; *s; s++)
		sDeXmlChar(result, *s, charset, escapelf);
	return Txt(result);
}

Txt DeXml(tukk s, tukk end, u8 charset, bool escapelf)
{
	if(charset == CHARSET_DEFAULT)
		charset = GetDefaultCharset();
	TxtBuf result;
	for(; s < end; s++)
		sDeXmlChar(result, *s, charset, escapelf);
	return Txt(result);
}

Txt DeXml(const Txt& s, u8 charset, bool escapelf)
{
	return DeXml(~s, s.End(), charset, escapelf);
}

Txt XmlHeader(tukk encoding, tukk version, tukk standalone)
{
	TxtBuf r;
	r << "<?xml version=\"" << version << "\"";
	if(encoding)
		r << " encoding=\"" << encoding << "\"";
	if(standalone)
		r << " standalone=\"" << standalone << "\"";
	r << " ?>\r\n";
	return Txt(r);
}

Txt XmlPI(tukk text)
{
	TxtBuf r;
	r << "<?" << text << "?>\r\n";
	return Txt(r);
}

Txt XmlDecl(tukk text)
{
	TxtBuf r;
	r << "<!" << text << ">\r\n";
	return Txt(r);
}

Txt XmlDocType(tukk text)
{
	return XmlDecl("DOCTYPE " + Txt(text));
}

Txt XmlDoc(tukk name, tukk xmlbody)
{
	return XmlHeader() + XmlDocType(name) + XmlTag(name)(xmlbody);
}

Txt XmlComment(tukk text)
{
	TxtBuf out;
	out << "<!-- " << text << " -->\r\n";
	return Txt(out);
}

Txt  XmlTag::operator()()
{
	return tag + "/>";
}

XmlTag& XmlTag::Tag(tukk s)
{
	tag.Clear();
	end.Clear();
	tag << '<' << s;
	end << "</" << s << '>';
	return *this;
}

Txt  XmlTag::operator()(tukk text)
{
	TxtBuf r;
	r << tag << ">";
	tukk s = text;
	bool wastag = true;
	bool wasslash = true;
	bool first = true;
	while(*s) {
		tukk b = s;
		while(*s == ' ' || *s == '\t')
			s++;
		if(s[0] == '<') {
			if(first)
				r << "\r\n";
			if(wastag && (wasslash || s[1] != '/'))
				r.Cat('\t');
		}
		else
		if(first) {
			r << text << end;
			return Txt(r);
		}
		first = false;
		wasslash = false;
		char last = 0;
		while(*s && *s != '\n' && *s != '\r') {
			if(*s == '<')
				wasslash = s[1] == '/';
			if(*s == '/' && s[1] == '>')
				wasslash = true;
			last = *s++;
		}
		wastag = last == '>';
		if(*s == '\r')
			s++;
		if(*s == '\n')
			s++;
		r.Cat(b, s);
	}
	if(!first)
		r << "\r\n";
	r << end;
	return Txt(r);
}

Txt  XmlTag::Text(tukk text, u8 charset)
{
	TxtBuf r;
	return r << tag << '>' << DeXml(text, charset) << end;
}

Txt XmlTag::PreservedText(tukk text, u8 charset)
{
	TxtBuf r;
	return r << tag << " xml:spaces=\"preserve\">" << DeXml(text, charset) << end;
}

XmlTag& XmlTag::operator()(tukk attr, tukk val)
{
	tag << ' ' << attr << "=\"" << DeXml(val, CHARSET_DEFAULT, true) << "\"";
	return *this;
}

XmlTag& XmlTag::operator()(tukk attr, i32 q)
{
	return operator()(attr, AsTxt(q));
}

XmlTag& XmlTag::operator()(tukk attr, double q)
{
	return operator()(attr, AsTxt(q));
}

force_inline
Txt XmlParser::Convert(TxtBuf& b)
{
	if(acharset == scharset)
		return b;
	return ToCharset(acharset, b, scharset);
}

void XmlParser::Ent(TxtBuf& out)
{
	LoadMore();
	i32 outconst = 0;
	tukk t = ++term;
	if(*t == '#') {
		if(*++t == 'X' || *t == 'x') {
			for(u8 c; (c = ctoi(*++t)) < 16; outconst = 16 * outconst + c)
				;
		}
		else {
			while(IsDigit(*t))
				outconst = 10 * outconst + *t++ - '0';
		}
		out.Cat(ToUtf8(outconst));
		if(*t == ';')
			t++;
		term = t;
		return;
	}
	tukk b = t;
	while(*t && *t != ';')
		t++;
	if(*t == ';') {
		i32 q = entity.Find(Txt(b, t));
		if(q >= 0) {
			out.Cat(entity[q]);
			term = t + 1;
			return;
		}
	}
	if(!relaxed)
		throw XmlError("Неизвестная сущность");
	out.Cat('&');
}

inline static bool IsXmlNameChar(i32 c)
{
	return IsAlNum(c) || c == '.' || c == '-' || c == '_' || c == ':';
}

void XmlParser::LoadMore0()
{ // WARNING: Invalidates pointers to buffer
	if(in && !in->IsEof()) {
		i32 pos = i32(term - begin);
		if(len - pos < MCHARS) {
			LLOG("LoadMore0 " << pos << ", " << len);
			begincolumn = GetColumn0();
			len -= pos;
			memmove(buffer, term, len);
			term = begin = buffer;
			len += in->Get(~buffer + len, CHUNK);
			buffer[len] = '\0';
		}
	}
}

bool XmlParser::More()
{ // WARNING: Invalidates pointers to buffer
	begincolumn = GetColumn();
	if(!in || in->IsEof())
		return false;
	LLOG("More " << (i32)CHUNK);
	len = in->Get(buffer, CHUNK);
	buffer[len] = '\0';
	term = begin = buffer;
	return len;
}

void XmlParser::SkipWhites()
{
	while(HasMore() && (u8)*term <= ' ') {
		if(*term == '\n')
			line++;
		term++;
	}
	LoadMore();
}

void XmlParser::ReadAttr(TxtBuf& attrval, i32 c)
{
	term++;
	while(HasMore() && *term != c)
		if(*term == '&')
			Ent(attrval);
		else
			attrval.Cat(*term++);
	if(*term == c)
		term++;
}

void XmlParser::Next()
{
	nattr.Clear();
	nattr1 = nattrval1 = Null;
	if(empty_tag) {
		empty_tag = false;
		type = XML_END;
		LLOG("XML_END (пустой тэг) " << tagtext);
		return;
	}

	type = Null;
	TxtBuf raw_text;
	for(;;) {
		if(!HasMore()) {
			type = XML_EOF;
			LLOG("XML_EOF");
			return;
		}
		LoadMore();
		if(*term == '<') {
			if(term[1] == '!' && term[2] == '[' &&
			   term[3] == 'C' && term[4] == 'D' && term[5] == 'A' && term[6] == 'T' && term[7] == 'A' &&
			   term[8] == '[') { // ![CDATA[
				term += 9;
				LLOG("CDATA");
				for(;;) {
					if(!HasMore())
						throw XmlError("Неоконченн. CDATA");
					LoadMore();
					if(term[0] == ']' && term[1] == ']' && term[2] == '>') { // ]]>
						term += 3;
						break;
					}
					if(*term == '\n')
						line++;
					raw_text.Cat(*term++);
				}
				type = XML_TEXT;
				continue;
			}
			else
				break;
		}
		if(*term == '\n')
			line++;
		if(*term == '&') {
			Ent(raw_text);
			type = XML_TEXT;
		}
		else {
			if((u8)*term > ' ')
				type = XML_TEXT;
			raw_text.Cat(*term++);
		}
	}
	cdata = Convert(raw_text);

	if(cdata.GetCount() && (npreserve || preserveall))
		type = XML_TEXT;

	if(type == XML_TEXT)
		return;

	term++;
	LoadMore();
	if(*term == '!') {
		tagtext.Clear();
		type = XML_DECL;
		term++;
		if(term[0] == '-' && term[1] == '-') {
			type = XML_COMMENT;
			term += 2;
			for(;;) {
				LoadMore();
				if(term[0] == '-' && term[1] == '-' && term[2] == '>')
					break;
				if(!HasMore())
					throw XmlError("Неоконченный коммент");
				if(*term == '\n')
					line++;
				tagtext.Cat(*term++);
			}
			term += 3;
			return;
		}
		bool intdt = false;
		for(;;) {
			LoadMore();
			if (*term == '[')
				intdt = true;
			if(*term == '>' && intdt == false) {
				term++;
				break;
			}
			if(intdt == true && term[0] == ']' && term[1] == '>') {
				tagtext.Cat(*term++);
				term++;
				break;
			}
			if(!HasMore())
				throw XmlError("Неоконченное объявление");
			if(*term == '\n')
				line++;
			tagtext.Cat(*term++);
		}
		LLOG("XML_DECL " << tagtext);
	}
	else
	if(*term == '?') {
		tagtext.Clear();
		type = XML_PI;
		term++;
		for(;;) {
			LoadMore();
			if(term[0] == '?' && term[1] == '>') {
				term += 2;
				LLOG("XML_PI " << tagtext);
				if(!tagtext.StartsWith("xml "))
					return;
				i32 q = tagtext.Find("encoding");
				if(q < 0)
					return;
				q = tagtext.Find('\"', q);
				if(q < 0)
					return;
				q++;
				i32 w = tagtext.Find('\"', q);
				if(w < 0)
					return;
				q = CharsetByName(tagtext.Mid(q, w));
				if(q)
					scharset = q;
				return;
			}
			if(!HasMore())
				throw XmlError("Неоконченное инфо об обработке");
			if(*term == '\n')
				line++;
			tagtext.Cat(*term++);
		}
	}
	else
	if(*term == '/') {
		type = XML_END;
		term++;
		tukk t = term;
		while(IsXmlNameChar(*term))
			term++;
		tagtext = Txt(t, term);
		LLOG("XML_END " << tagtext);
		if(*term != '>')
			throw XmlError("Неоконченный конечный тэг");
		term++;
	}
	else {
		type = XML_TAG;
		tukk t = term;
		while(IsXmlNameChar(*term))
			term++;
		tagtext = Txt(t, term);
		LLOG("XML_TAG " << tagtext);
		for(;;) {
			SkipWhites();
			if(*term == '>') {
				term++;
				break;
			}
			if(term[0] == '/' && term[1] == '>') {
				cdata.Clear();
				empty_tag = true;
				term += 2;
				break;
			}
			if(!HasMore())
				throw XmlError("Неоконченный тэг");
			LoadMore();
			tukk t = term++;
			while((u8)*term > ' ' && *term != '=' && *term != '>')
				term++;
			Txt attr(t, term);
			SkipWhites();
			if(*term == '=') {
				term++;
				SkipWhites();
				TxtBuf attrval;
				if(*term == '\"')
					ReadAttr(attrval, '\"');
				else
				if(*term == '\'')
					ReadAttr(attrval, '\'');
				else
					while(HasMore() && (u8)*term > ' ' && *term != '>' && *term != '/')
						if(*term == '&')
							Ent(attrval);
						else
							attrval.Cat(*term++);
				if(attr == "xml:space" && attrval.GetLength() == 8 && !memcmp(~attrval, "preserve", 8))
					npreserve = true;
				Txt aval = Convert(attrval);
				if(IsNull(nattr1)) {
					nattr1 = attr;
					nattrval1 = aval;
				}
				else
					nattr.Add(attr, aval);
			}
		}
	}
}

void XmlParser::RegisterEntity(const Txt& id, const Txt& text)
{
	entity.Add(id, text);
}

bool   XmlParser::IsEof()
{
	return type == XML_EOF;
}

bool   XmlParser::IsTag()
{
	return type == XML_TAG;
}

Txt XmlParser::ReadTag(bool next)
{
	if(type != XML_TAG)
		throw XmlError("Ожидался тэг");
	LLOG("ReadTag " << tagtext);
	Txt h = tagtext;
	if(next) {
		if(!raw)
			stack.Add(Nesting(h, npreserve));
		attr = pick(nattr);
		attr1 = nattr1;
		attrval1 = nattrval1;
		Next();
	}
	return h;
}

bool  XmlParser::Tag(tukk tag)
{
	if(IsTag() && tagtext == tag) {
		LLOG("Tag " << tagtext);
		if(!raw)
			stack.Add(Nesting(tagtext, npreserve));
		attr = pick(nattr);
		attr1 = nattr1;
		attrval1 = nattrval1;
		Next();
		return true;
	}
	return false;
}

bool  XmlParser::Tag(const Txt& tag)
{
	if(IsTag() && tagtext == tag) {
		LLOG("Tag " << tagtext);
		if(!raw)
			stack.Add(Nesting(tagtext, npreserve));
		attr = pick(nattr);
		attr1 = nattr1;
		attrval1 = nattrval1;
		Next();
		return true;
	}
	return false;
}

void  XmlParser::PassTag(tukk tag)
{
	if(!Tag(tag))
		throw XmlError(Txt().Cat() << '\'' << tag << "\' ожидался");
}

void  XmlParser::PassTag(const Txt& tag)
{
	if(!Tag(tag))
		throw XmlError(Txt().Cat() << '\'' << tag << "\' ожидался");
}

Txt XmlParser::ReadEnd(bool next)
{
	if(type != XML_END)
		throw XmlError("Ожидался конечный тэг");
	Txt x = tagtext;
	if(next)
		Next();
	return x;
}

bool  XmlParser::IsEnd()
{
	return type == XML_END;
}

bool  XmlParser::End()
{
	if(IsEof())
		throw XmlError("Неожиданный конец файла");
	if(IsEnd()) {
		LLOG("EndTag " << tagtext);
		if(!raw) {
			if(stack.IsEmpty())
				throw XmlError(Format("Неожидавшийся конечный тэг: </%s>", tagtext));
			if(stack.Top().tag != tagtext && !relaxed) {
				LLOG("Tag/end-tag mismatch: <" << stack.Top().tag << "> </" << tagtext << ">");
				throw XmlError(Format("Несоответствие тэга/конечного тэга: <%s> </%s>", stack.Top().tag, tagtext));
			}
			stack.Drop();
		}
		npreserve = (!stack.IsEmpty() && stack.Top().preserve_blanks);
		Next();
		return true;
	}
	return false;
}

bool XmlParser::End(tukk tag)
{
	if(IsEnd() && tagtext == tag) {
		Next();
		return true;
	}
	return false;
}

bool XmlParser::End(const Txt& tag)
{
	if(IsEnd() && tagtext == tag) {
		Next();
		return true;
	}
	return false;
}

void  XmlParser::PassEnd()
{
	if(!End())
		throw XmlError(Txt().Cat() << "Ожидался \'" << (stack.GetCount() ? stack.Top().tag : Txt()) << "\' конечный тэг");
}

void XmlParser::PassEnd(tukk tag)
{
	if(!End(tag))
		throw XmlError(Txt().Cat() << "Ожидался \'" << tag << "\' конечный тэг");
}

bool  XmlParser::TagE(tukk tag)
{
	if(Tag(tag)) {
		SkipEnd();
		return true;
	}
	return false;
}

void  XmlParser::PassTagE(tukk tag)
{
	PassTag(tag);
	SkipEnd();
}

bool XmlParser::TagElseSkip(tukk tag)
{
	if(Tag(tag))
		return true;
	Skip();
	return false;
}

bool XmlParser::LoopTag(tukk tag)
{
	while(!End()) {
		if(Tag(tag))
			return true;
		Skip();
	}
	return false;
}

VecMap<Txt, Txt> XmlParser::PickAttrs()
{
	if(!IsNull(attr1))
		pick(const_cast<VecMap<Txt, Txt>&>(attr).Insert(0, attr1, attrval1));
	return pick(attr);
}

i32   XmlParser::Int(tukk id, i32 def) const
{
	if(id == attr1) return ScanInt(attrval1);
	i32 q = attr.Find(id);
	return q < 0 ? def : ScanInt(attr[q]);
}

double XmlParser::Double(tukk id, double def) const
{
	if(id == attr1) return ScanDouble(attrval1);
	i32 q = attr.Find(id);
	return q < 0 ? def : ScanDouble(attr[q]);
}

bool  XmlParser::IsText()
{
	if(npreserve || preserveall)
		return cdata.GetCount();
	tukk e = cdata.End();
	for(tukk s = cdata.Begin(); s < e; s++)
		if((u8)*s > ' ')
			return true;
	return false;
}

Txt XmlParser::ReadText()
{
	Txt h = cdata;
	cdata.Clear();
	if(type == XML_TEXT)
		Next();
	return h;
}

Txt XmlParser::ReadTextE()
{
	TxtBuf out;
	for(;;) {
		Txt t = ReadText();
		if(!IsNull(t))
			out.Cat(t);
		else if(IsEnd()) {
			PassEnd();
			return Txt(out);
		}
		else
			Skip();
	}
}

bool   XmlParser::IsDecl()
{
	return type == XML_DECL;
}

Txt XmlParser::ReadDecl(bool next)
{
	if(!IsDecl())
		throw XmlError("Ожидалось объявление");
	Txt h = tagtext;
	if(next)
		Next();
	return h;
}

bool   XmlParser::IsPI()
{
	return type == XML_PI;
}

Txt XmlParser::ReadPI(bool next)
{
	if(!IsPI())
		throw XmlError("Ожидалось инфо об обработке");
	Txt h = tagtext;
	if(next)
		Next();
	return h;
}

bool   XmlParser::IsComment()
{
	return type == XML_COMMENT;
}

Txt XmlParser::ReadComment(bool next)
{
	if(!IsComment())
		throw XmlError("Ожидался коммент");
	Txt h = tagtext;
	if(next)
		Next();
	return h;
}

void XmlParser::Skip()
{
	if(IsEof())
		throw XmlError("Ожидался конец файла");
	if(cdata.GetCount() && type != XML_TEXT)
		cdata.Clear();
	else
	if(IsTag() && !raw) {
		Txt n = ReadTag();
		while(!End()) {
			if(IsEof())
				throw XmlError("Неожиданный конец файла при пропуске тэга \'" + n + "\'");
			Skip();
		}
	}
	else
		Next();
}

void XmlParser::SkipEnd()
{
	while(!IsEnd()) Skip();
	PassEnd();
}

i32 XmlParser::GetColumn0() const
{
	tukk s = term;
	i32 n = 0;
	while(s > begin && *(s - 1) != '\n') {
		s--;
		n++;
	}
	return n + begincolumn;
}

void XmlParser::Init()
{
	RegisterEntity("lt", "<");
	RegisterEntity("gt", ">");
	RegisterEntity("amp", "&");
	RegisterEntity("apos", "\'");
	RegisterEntity("quot", "\"");
	relaxed = false;
	empty_tag = false;
	npreserve = false;
	line = 1;
	preserveall = false;
	begincolumn = 0;
	in = NULL;
	len = 0;
	raw = false;
	acharset = GetDefaultCharset();
	scharset = CHARSET_UTF8;
}

XmlParser::XmlParser(tukk s)
{
	Init();
	begin = term = s;
	len = INT_MAX;
	try { // Need to catch first error as construction is usually outside try-catch client block
		Next();
	}
	catch(XmlError) {
		begin = term = s;
		len = INT_MAX;
	}
}

XmlParser::XmlParser(Stream& in_)
{
	Init();
	buffer.Alloc(CHUNK + MCHARS + 1);
	begin = term = "";
	in = &in_;
	try {
		Next();
	}
	catch(XmlError) {
		begin = term = "";
	}
}

i32 XmlNode::FindTag(tukk _tag) const
{
	Txt tag = _tag;
	for(i32 i = 0; i < node.GetCount(); i++)
		if(node[i].type == XML_TAG && node[i].text == tag)
			return i;
	return -1;
}

XmlNode& XmlNode::Add(tukk tag)
{
	XmlNode& m = node.Add();
	m.CreateTag(tag);
	return m;
}

XmlNode& XmlNode::GetAdd(tukk tag)
{
	i32 q = FindTag(tag);
	return q >= 0 ? node[q] : Add(tag);
}

const XmlNode& XmlNode::Void()
{
	static XmlNode h;
	return h;
}

const XmlNode& XmlNode::operator[](tukk tag) const
{
	i32 q = FindTag(tag);
	return q < 0 ? Void() : node[q];
}

void XmlNode::Remove(i32 i)
{
	node.Remove(i);
}

void XmlNode::Remove(tukk tag)
{
	i32 q = FindTag(tag);
	if(q >= 0)
		node.Remove(q);
}

Txt XmlNode::GatherText() const
{
	Txt r;
	for(i32 i = 0; i < GetCount(); i++)
		if(node[i].IsText())
			r << node[i].GetText();
	return r;
}

bool XmlNode::HasTags() const
{
	for(i32 i = 0; i < GetCount(); i++)
		if(node[i].IsTag())
			return true;
	return false;
}

i32  XmlNode::AttrInt(tukk id, i32 def) const
{
	Txt x = Attr(id);
	CParser p(x);
	return p.IsInt() ? p.ReadInt() : def;
}

XmlNode& XmlNode::SetAttr(tukk id, const Txt& text)
{
	if(!attr)
		attr.Create();
	attr->GetAdd(id) = text;
	return *this;
}

void XmlNode::SetAttrs(VecMap<Txt, Txt>&& a)
{
	if(a.GetCount() == 0)
		attr.Clear();
	else {
		if(!attr)
			attr.Create();
		*attr = pick(a);
	}
}

XmlNode& XmlNode::SetAttr(tukk id, i32 i)
{
	SetAttr(id, AsTxt(i));
	return *this;
}

void XmlNode::Shrink()
{
	if(attr) {
		if(attr->GetCount() == 0)
			attr.Clear();
		else
			attr->Shrink();
	}
	node.Shrink();
}

XmlNode::XmlNode(const XmlNode& n, i32)
{
	type = n.type;
	text = n.text;
	node = clone(n.node);
	if(n.attr) {
		attr.Create();
		*attr = clone(*n.attr);
	}
}

bool Ignore(XmlParser& p, dword style)
{
	if((XML_IGNORE_DECLS & style) && p.IsDecl() ||
	   (XML_IGNORE_PIS & style) && p.IsPI() ||
	   (XML_IGNORE_COMMENTS & style) && p.IsComment()) {
		p.Skip();
		return true;
	}
	return false;
}

static XmlNode sReadXmlNode(XmlParser& p, ParseXmlFilter *filter, dword style)
{
	XmlNode m;
	if(p.IsTag()) {
		Txt tag = p.ReadTag();
		if(!filter || filter->DoTag(tag)) {
			m.CreateTag(tag);
			m.SetAttrs(p.PickAttrs());
			while(!p.End())
				if(!Ignore(p, style)) {
					XmlNode n = sReadXmlNode(p, filter, style);
					if(n.GetType() != XML_DOC) // tag was ignored
						m.Add() = pick(n);
				}
			if(filter)
				filter->EndTag();
		}
		else
			p.SkipEnd();
		return m;
	}
	if(p.IsPI()) {
		m.CreatePI(p.ReadPI());
		return m;
	}
	if(p.IsDecl()) {
		m.CreateDecl(p.ReadDecl());
		return m;
	}
	if(p.IsComment()) {
		m.CreateComment(p.ReadComment());
		return m;
	}
	if(p.IsText()) {
		m.CreateText(p.ReadText());
		m.Shrink();
		return m;
	}
	p.ReadText(); // skip empty text
	return m;
}

void ParseXmlFilter::EndTag() {}

XmlNode ParseXML(XmlParser& p, dword style, ParseXmlFilter *filter)
{
	XmlNode r;
	while(!p.IsEof())
		if(!Ignore(p, style)) {
			XmlNode n = sReadXmlNode(p, filter, style);
			if(n.GetType() != XML_DOC) // tag was ignored
				r.Add() = pick(n);
			else {
				if(p.IsRelaxed())
					p.Skip();
				else
					throw XmlError("Неожиданный текст");
			}
		}
	return r;
}

XmlNode ParseXML(XmlParser& p, dword style)
{
	return ParseXML(p, style, NULL);
}

XmlNode ParseXML(tukk s, dword style)
{
	XmlParser p(s);
	return ParseXML(p, style);
}

XmlNode ParseXML(Stream& in, dword style)
{
	XmlParser p(in);
	return ParseXML(p, style);
}

XmlNode ParseXMLFile(tukk path, dword style)
{
	FileIn in(path);
	if(!in)
		throw XmlError("Не удаётся открыть входной файл!");
	return ParseXML(in, style);
}

XmlNode ParseXML(XmlParser& p, ParseXmlFilter& filter, dword style)
{
	return ParseXML(p, style, &filter);
}

XmlNode ParseXML(tukk s, ParseXmlFilter& filter, dword style)
{
	XmlParser p(s);
	return ParseXML(p, filter, style);
}

XmlNode ParseXML(Stream& in, ParseXmlFilter& filter, dword style)
{
	XmlParser p(in);
	return ParseXML(p, filter, style);
}

XmlNode ParseXMLFile(tukk path, ParseXmlFilter& filter, dword style)
{
	FileIn in(path);
	if(!in)
		throw XmlError("Не удаётся открыть входной файл!");
	return ParseXML(in, filter, style);
}

bool ShouldPreserve(const Txt& s)
{
	if(*s == ' ' || *s == '\t' || *s == '\n')
		return true;
	tukk l = s.Last();
	if(*l == ' ' || *l == '\t' || *l == '\n')
		return true;
	l = s.End();
	for(tukk x = s; x < l; x++)
		if(*x == '\t' || *x == '\n' || *x == ' ' && x[1] == ' ')
			return true;
	return false;
}

static void sAsXML(Stream& out, const XmlNode& node, dword style, const Txt& indent)
{
	if(style & XML_HEADER) {
		Txt h = XmlHeader();
		if(!(style & XML_PRETTY))
			h.Trim(h.GetCount() - 2);
		out << h;
	}
	if(style & XML_DOCTYPE)
		for(i32 i = 0; i < node.GetCount(); i++) {
			const XmlNode& m = node.Node(i);
			if(m.GetType() == XML_TAG) {
				Txt h = XmlDocType(m.GetText());
				h.Trim(h.GetCount() - 2);
				out << h;
				break;
			}
		}
	style &= ~(XML_HEADER|XML_DOCTYPE);
	switch(node.GetType()) {
	case XML_PI:
		out << indent << "<?" << node.GetText() << "?>\r\n";
		break;
	case XML_DECL:
		out << indent << "<!" << node.GetText() << ">\r\n";
		break;
	case XML_COMMENT:
		out << indent << "<!--" << node.GetText() << "-->\r\n";
		break;
	case XML_DOC:
		for(i32 i = 0; i < node.GetCount(); i++)
			sAsXML(out, node.Node(i), style, indent);
		break;
	case XML_TEXT:
		out << DeXml(node.GetText());
		break;
	case XML_TAG:
		XmlTag tag(node.GetText());
		for(i32 i = 0; i < node.GetAttrCount(); i++)
			tag(node.AttrId(i), node.Attr(i));
		if(node.GetCount()) {
			out << indent << tag.GetBegin();
			Txt indent2 = (style & XML_PRETTY) && node[0].IsTag() && node[node.GetCount() - 1].IsTag() ?
			                 indent + '\t' : Txt();
			for(i32 i = 0; i < node.GetCount(); i++)
				sAsXML(out, node.Node(i), style, indent2);
			if(indent2.GetCount())
				out << indent;
			out << tag.GetEnd();
		}
		else
			out << indent << tag();
	}
}

void AsXML(Stream& out, const XmlNode& n, dword style)
{
	sAsXML(out, n, style, style & XML_PRETTY ? "\r\n" : "");
}

Txt AsXML(const XmlNode& n, dword style)
{
	TxtStream ss;
	AsXML(ss, n, style);
	return ss.GetResult();
}

bool AsXMLFile(tukk path, const XmlNode& n, dword style)
{
	FileOut out(path);
	if(!out)
		return false;
	AsXML(out, n, style);
	out.Close();
	return !out.IsError();
}

bool IgnoreXmlPaths::DoTag(const Txt& id)
{
	Txt new_path;
	if(path.GetCount())
		new_path = path.Top();
	new_path << '/' << id;
	if(list.Find(new_path) >= 0)
		return false;
	path.Add(new_path);
	return true;
}

void IgnoreXmlPaths::EndTag()
{
	path.Drop();
}

IgnoreXmlPaths::IgnoreXmlPaths(tukk s)
{
	list = Split(s, ';');
}

}