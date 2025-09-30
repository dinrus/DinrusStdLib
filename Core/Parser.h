inline bool iscib(wchar c) {
		switch(c) {
		case_id_rus: return true;
			}
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$';
}

inline bool iscid(wchar c) {

		return iscib(c) || (c >= '0' && c <= '9');
}

class CParser {
protected:
	tukk        term;
	tukk        wspc;
	tukk        lineptr;
	i32         line;
	Txt         fn;
	bool        skipspaces;
	bool        skipcomments;
	bool        nestcomments;
	bool        uescape;

	bool        Spaces0();
	tukk        IsId0(tukk s) const;
	bool        Id0(tukk id);
	void        DoSpaces()                    { if(skipspaces) Spaces(); }
	dword       ReadHex();
	bool        ReadHex(dword& hex, i32 n);

public:
	struct Error : public Exc                 { Error(tukk s) : Exc(s) {} };

	void   ThrowError(tukk s);
	void   ThrowError()                       { ThrowError(""); }

	bool   Spaces()                           { wspc = term; return ((u8)*term <= ' ' || *term == '/') && Spaces0(); }
	char   PeekChar() const                   { return *term; }
	char   GetChar();

	bool   IsChar(char c) const               { return *term == c; }
	bool   IsChar2(char c1, char c2) const    { return term[0] == c1 && term[1] == c2; }
	bool   IsChar3(char c1, char c2, char c3) const { return term[0] == c1 && term[1] == c2 && term[2] == c3; }
	bool   Char(char c);
	bool   Char2(char c1, char c2);
	bool   Char3(char c1, char c2, char c3);
	void   PassChar(char c);
	void   PassChar2(char c1, char c2);
	void   PassChar3(char c1, char c2, char c3);
	bool   Id(tukk s)                  { return term[0] == s[0] && (s[1] == 0 || term[1] == s[1]) && Id0(s); }
	void   PassId(tukk s);
	bool   IsId() const                       { return iscib(*term); }
	bool   IsId(tukk s) const          { return term[0] == s[0] && (s[1] == 0 || term[1] == s[1]) && IsId0(s); }
	Txt    ReadId();
	Txt    ReadIdh();
	Txt    ReadIdt();
	bool   IsInt() const;
	i32    Sgn();
	i32    ReadInt();
	i32    ReadInt(i32 min, i32 max);
	z64    ReadInt64();
	z64    ReadInt64(z64 min, z64 max);
	bool   IsNumber() const                   { return IsDigit(*term); }
	bool   IsNumber(i32 base) const;
	u32    ReadNumber(i32 base = 10);
	zu64   ReadNumber64(i32 base = 10);
	bool   IsDouble() const                   { return IsInt(); }
	bool   IsDouble2() const;
	double ReadDouble();
	double ReadDoubleNoE();
	bool   IsTxt() const                   { return IsChar('\"'); };
	Txt    ReadOneTxt(bool chkend = true);
	Txt    ReadTxt(bool chkend = true);
	Txt    ReadOneTxt(i32 delim, bool chkend = true);
	Txt    ReadTxt(i32 delim, bool chkend = true);

	void   Skip();
	void   SkipTerm()                         { Skip(); }

	struct Pos {
		tukk     ptr;
		tukk     wspc;
		tukk     lineptr;
		i32      line;
		Txt      fn;

		i32 GetColumn(i32 tabsize = 4) const;

		Pos(tukk ptr = NULL, i32 line = 1, Txt fn = Null) : ptr(ptr), line(line), fn(fn) {}
	};

	tukk GetPtr() const                { return (tukk )term; }
	tukk GetSpacePtr() const           { return (tukk )wspc; }

	Pos         GetPos() const;
	void        SetPos(const Pos& pos);

	bool   IsEof() const                      { return *term == '\0'; }
	operator bool() const                     { return *term; }

	i32    GetLine() const                    { return line; }
	i32    GetColumn(i32 tabsize = 4) const;
	Txt    GetFName() const                { return fn; }

	static Txt LineInfoComment(const Txt& filename, i32 line = 1, i32 column = 1);
	Txt        GetLineInfoComment(i32 tabsize = 4) const;
	enum { LINEINFO_ESC = '\2' };

	void   Set(tukk ptr, tukk fn, i32 line = 1);
	void   Set(tukk ptr);

	CParser& SkipSpaces(bool b = true)        { skipspaces = b; return *this; }
	CParser& NoSkipSpaces()                   { skipspaces = false; return *this; }
	CParser& UnicodeEscape(bool b = true)     { uescape = b; return *this; }
	CParser& SkipComments(bool b = true);
	CParser& NoSkipComments()                 { return SkipComments(false); }
	CParser& NestComments(bool b = true);
	CParser& NoNestComments()                 { return NestComments(false); }

	CParser(tukk ptr);
	CParser(tukk ptr, tukk fn, i32 line = 1);
	CParser();
};

inline bool CParser::Char(char c)
{
	if(IsChar(c)) {
		term++;
		DoSpaces();
		return true;
	}
	return false;
}

inline bool CParser::Char2(char c1, char c2)
{
	if(IsChar2(c1, c2)) {
		term += 2;
		DoSpaces();
		return true;
	}
	return false;
}

inline bool CParser::Char3(char c1, char c2, char c3)
{
	if(IsChar3(c1, c2, c3)) {
		term += 3;
		DoSpaces();
		return true;
	}
	return false;
}

enum {
	ASCTXT_SMART     = 0x01,
	ASCTXT_OCTALHI   = 0x02,
	ASCTXT_JSON      = 0x04,
};

Txt AsCTxt(tukk s, tukk end, i32 linemax = INT_MAX, tukk linepfx = NULL,
                 dword flags = 0);
Txt AsCTxt(tukk s, i32 linemax = INT_MAX, tukk linepfx = NULL,
                 dword flags = 0);
Txt AsCTxt(const Txt& s, i32 linemax = INT_MAX, tukk linepfx = NULL,
                 dword flags = 0);