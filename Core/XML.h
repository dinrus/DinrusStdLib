Txt DeXml(tukk s, u8 charset = CHARSET_DEFAULT, bool escapelf = false);
Txt DeXml(tukk s, tukk end, u8 charset = CHARSET_DEFAULT, bool escapelf = false);
Txt DeXml(const Txt& s, u8 charset = CHARSET_DEFAULT, bool escapelf = false);
Txt XmlPI(tukk text);
Txt XmlHeader(tukk encoding = "UTF-8", tukk version = "1.0", tukk standalone = NULL);
Txt XmlDecl(tukk text);
Txt XmlDocType(tukk text);
Txt XmlDoc(tukk name, tukk xmlbody);
Txt XmlComment(tukk text);

class XmlTag : Moveable<XmlTag> {
	Txt tag;
	Txt end;

public:
	XmlTag& Tag(tukk s);

	Txt  operator()();
	Txt  operator()(tukk text);
	Txt  operator()(const Txt& text)                        { return operator()(~text); }
	Txt  Text(tukk s, u8 charset = CHARSET_DEFAULT);
	Txt  Text(const Txt& s, u8 charset = CHARSET_DEFAULT) { return Text(~s, charset); }
	Txt  PreservedText(tukk s, u8 charset = CHARSET_DEFAULT);
	Txt  PreservedText(const Txt& s, u8 charset = CHARSET_DEFAULT) { return PreservedText(~s, charset); }

	Txt  GetBegin() const                                      { return tag + '>'; }
	Txt  GetEnd() const                                        { return end; }

	XmlTag& operator()(tukk attr, tukk val);
	XmlTag& operator()(tukk attr, i32 q);
	XmlTag& operator()(tukk attr, double q);

	XmlTag() {}
	XmlTag(tukk tag)                                       { Tag(tag); }
};

enum { XML_DOC, XML_TAG, XML_END, XML_TEXT, XML_DECL, XML_PI, XML_COMMENT, XML_EOF };

struct XmlError : public Exc
{
	XmlError(tukk s) : Exc(s) {}
};

class XmlParser {
	enum {
#ifdef flagTEST_XML // This is for testing purposes only to increase boundary condition frequency
		MCHARS = 128,
		CHUNK = 256
#else
		MCHARS = 256,
		CHUNK = 16384
#endif
	};

	struct Nesting {
		Nesting(Txt tag = Null, bool blanks = false) : tag(tag), preserve_blanks(blanks) {}
		Txt tag;
		bool   preserve_blanks;
	};

	VecMap<Txt, Txt> entity;

	Stream                   *in;
	Buffer<char>              buffer;
	i32                       len;
	i32                       begincolumn;
	const char               *begin;
	const char               *term;

	Txt                    attr1, attrval1;
	VecMap<Txt, Txt> attr;
	Array<Nesting>            stack;

	i32                       type;
	Txt                    nattr1, nattrval1;
	VecMap<Txt, Txt> nattr;
	Txt                    tagtext;
	Txt                    cdata;
	bool                      empty_tag;
	bool                      npreserve, preserveall;
	bool                      relaxed;
	bool                      raw;

	i32                       line;

	u8                      acharset;
	u8                      scharset;

	void                      Init();
	void                      LoadMore0();
	void                      LoadMore()              { if(len - (term - begin) < MCHARS) LoadMore0(); }
	bool                      More();
	bool                      HasMore()               { return *term || More(); }
	void                      Ent(TxtBuf& out);
	void                      Next();
	void                      ReadAttr(TxtBuf& b, i32 c);
	Txt                    ReadTag(bool next);
	Txt                    ReadEnd(bool next);
	Txt                    ReadDecl(bool next);
	Txt                    ReadPI(bool next);
	Txt                    ReadComment(bool next);
	i32                       GetColumn0() const;
	Txt                    Convert(TxtBuf& b);

public:
	void   SkipWhites();

	void   RegisterEntity(const Txt& id, const Txt& text);

	bool   IsEof();
	tukk GetPtr() const                                { return term; }

	bool   IsTag();
	Txt PeekTag()                                          { return ReadTag(false); }
	Txt ReadTag()                                          { return ReadTag(true); }
	bool   Tag(tukk tag);
	bool   Tag(const Txt& tag);
	void   PassTag(tukk tag);
	void   PassTag(const Txt& tag);
	bool   IsEnd();
	Txt PeekEnd()                                          { return ReadEnd(false); }
	Txt ReadEnd()                                          { return ReadEnd(true); }
	bool   End();
	bool   End(tukk tag);
	bool   End(const Txt& tag);
	void   PassEnd();
	void   PassEnd(tukk tag);
	bool   TagE(tukk tag);
	void   PassTagE(tukk tag);
	bool   TagElseSkip(tukk tag);
	bool   LoopTag(tukk tag);

	i32    GetAttrCount() const                               { return attr.GetCount() + !IsNull(attr1); }
	Txt GetAttr(i32 i) const                               { return i ? attr.GetKey(i - 1) : attr1; }
	bool   IsAttr(tukk id) const                       { return attr1 == id || attr.Find(id) >= 0; }
	Txt operator[](i32 i) const                            { return i ? attr[i - 1] : attrval1; }
	Txt operator[](tukk id) const                   { return attr1 == id ? attrval1 : attr.Get(id, Null); }
	i32    Int(tukk id, i32 def = Null) const;
	double Double(tukk id, double def = Null) const;

	bool   IsText();
	Txt PeekText()                                         { return cdata; }
	Txt ReadText();
	Txt ReadTextE();

	bool   IsDecl();
	Txt PeekDecl()                                         { return ReadDecl(false); }
	Txt ReadDecl()                                         { return ReadDecl(true); }

	bool   IsPI();
	Txt PeekPI()                                           { return ReadPI(false); }
	Txt ReadPI()                                           { return ReadPI(true); }

	bool   IsComment();
	Txt PeekComment()                                      { return ReadComment(false); }
	Txt ReadComment()                                      { return ReadComment(true); }

	void   Skip();
	void   SkipEnd();

	VecMap<Txt, Txt> PickAttrs();

	i32    GetLine() const                                    { return line; }
	i32    GetColumn() const                                  { return GetColumn0() + 1; }

	void   Relaxed(bool b = true)                             { relaxed = b; }
	bool   IsRelaxed() const                                  { return relaxed; }
	void   PreserveAllWhiteSpaces(bool b = true)              { preserveall = b; }
	void   Raw(bool b = true)                                 { raw = b; }

	XmlParser(tukk s);
	XmlParser(Stream& in);
};

class XmlNode : Moveable< XmlNode, DeepCopyOption<XmlNode> > {
	i32                              type;
	Txt                           text;
	Array<XmlNode>                   node;
	One< VecMap<Txt, Txt> > attr;

public:
	static const XmlNode& Void();
	bool           IsVoid() const                             { return this == &Void(); }

	i32            GetType() const                            { return type; }
	Txt         GetText() const                            { return text; }
	Txt         GetTag() const                             { return text; }
	bool           IsTag() const                              { return type == XML_TAG; }
	bool           IsTag(tukk tag) const               { return IsTag() && text == tag; }
	bool           IsText() const                             { return type == XML_TEXT; }

	void           Clear()                                    { text.Clear(); attr.Clear(); node.Clear(); type = -1; }
	void           CreateTag(tukk tag)                 { type = XML_TAG; text = tag; }
	void           CreateText(const Txt& txt)              { type = XML_TEXT; text = txt; }
	void           CreatePI(const Txt& pi)                 { type = XML_PI; text = pi; }
	void           CreateDecl(const Txt& decl)             { type = XML_DECL; text = decl; }
	void           CreateComment(const Txt& comment)       { type = XML_COMMENT; text = comment; }
	void           CreateDocument()                           { Clear(); }
	bool           IsEmpty() const                            { return type == XML_DOC && node.GetCount() == 0; }
	operator bool() const                                     { return !IsEmpty(); }

	i32            GetCount() const                           { return node.GetCount(); }
	XmlNode&       At(i32 i)                                  { return node.At(i); }
	const XmlNode& Node(i32 i) const                          { return node[i]; }
	const XmlNode& operator[](i32 i) const                    { return i >= 0 && i < node.GetCount() ? node[i] : Void(); }
	const XmlNode& operator[](tukk tag) const;
	XmlNode&       Add()                                      { return node.Add(); }
	void           Remove(i32 i);
	void           AddText(const Txt& txt)                 { Add().CreateText(txt); }
	i32            FindTag(tukk tag) const;
	XmlNode&       Add(tukk tag);
	XmlNode&       GetAdd(tukk tag);
	XmlNode&       operator()(tukk tag)                { return GetAdd(tag); }
	void           Remove(tukk tag);

	Txt            GatherText() const;
	Txt            operator~() const                          { return GatherText(); }
	bool           HasTags() const;

	i32            GetAttrCount() const                       { return attr ? attr->GetCount() : 0; }
	Txt            AttrId(i32 i) const                        { return attr->GetKey(i); }
	Txt            Attr(i32 i) const                          { return (*attr)[i]; }
	Txt            Attr(tukk id) const                 { return attr ? attr->Get(id, Null) : Txt(); }
	XmlNode&       SetAttr(tukk id, const Txt& val);
	i32            AttrInt(tukk id, i32 def = Null) const;
	XmlNode&       SetAttr(tukk id, i32 val);

	void           SetAttrs(VecMap<Txt, Txt>&& a);

	void           Shrink();

	rval_default(XmlNode);

	XmlNode(const XmlNode& n, i32);

	XmlNode()                                                 { type = XML_DOC; }

	typedef Array<XmlNode>::ConstIterator ConstIterator;
	ConstIterator          Begin() const                      { return node.Begin(); }
	ConstIterator          End() const                        { return node.End(); }

	typedef XmlNode        value_type;
	typedef ConstIterator  const_iterator;
	typedef const XmlNode& const_reference;
	typedef i32            size_type;
	typedef i32            difference_type;
	const_iterator         begin() const                      { return Begin(); }
	const_iterator         end() const                        { return End(); }
};

enum {
	XML_IGNORE_DECLS    = 0x01,
	XML_IGNORE_PIS      = 0x02,
	XML_IGNORE_COMMENTS = 0x04,
};

struct ParseXmlFilter {
	virtual bool DoTag(const Txt& tag) = 0;
	virtual void EndTag();
};

XmlNode ParseXML(XmlParser& p, dword style = XML_IGNORE_DECLS|XML_IGNORE_PIS|XML_IGNORE_COMMENTS);
XmlNode ParseXML(tukk s, dword style = XML_IGNORE_DECLS|XML_IGNORE_PIS|XML_IGNORE_COMMENTS);
XmlNode ParseXML(Stream& in, dword style = XML_IGNORE_DECLS|XML_IGNORE_PIS|XML_IGNORE_COMMENTS);
XmlNode ParseXMLFile(tukk path, dword style = XML_IGNORE_DECLS|XML_IGNORE_PIS|XML_IGNORE_COMMENTS);

XmlNode ParseXML(XmlParser& p, ParseXmlFilter& filter, dword style = XML_IGNORE_DECLS|XML_IGNORE_PIS|XML_IGNORE_COMMENTS);
XmlNode ParseXML(tukk s, ParseXmlFilter& filter, dword style = XML_IGNORE_DECLS|XML_IGNORE_PIS|XML_IGNORE_COMMENTS);
XmlNode ParseXML(Stream& in, ParseXmlFilter& filter, dword style = XML_IGNORE_DECLS|XML_IGNORE_PIS|XML_IGNORE_COMMENTS);
XmlNode ParseXMLFile(tukk path, ParseXmlFilter& filter, dword style = XML_IGNORE_DECLS|XML_IGNORE_PIS|XML_IGNORE_COMMENTS);

class IgnoreXmlPaths : public ParseXmlFilter {
public:
	virtual bool DoTag(const Txt& id);
	virtual void EndTag();

private:
	Index<Txt>  list;
	Vec<Txt> path;

public:
	IgnoreXmlPaths(tukk s);
};

enum {
	XML_HEADER  = 0x01,
	XML_DOCTYPE = 0x02,
	XML_PRETTY =  0x04,
};

void    AsXML(Stream& out, const XmlNode& n, dword style = XML_HEADER|XML_DOCTYPE|XML_PRETTY);
Txt  AsXML(const XmlNode& n, dword style = XML_HEADER|XML_DOCTYPE|XML_PRETTY);
bool    AsXMLFile(tukk path, const XmlNode& n, dword style = XML_HEADER|XML_DOCTYPE|XML_PRETTY);