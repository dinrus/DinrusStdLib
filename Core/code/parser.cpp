#include "../Core.h"

namespace drx {

#define LLOG(x) // DLOG(x)
#define LTIMING(x) //RTIMING(x)

void CParser::ThrowError(tukk s) {
	LLOG("CParser::Error: " << s);
	LLOG(~Txt(term, min((i32)strlen((tukk )term), 512)));
	Pos pos = GetPos();
	Error err(fn + Format("(%d,%d): ", line, pos.GetColumn()) + s);
//	err.term = (tukk )term;
	throw err;
}

CParser& CParser::SkipComments(bool b)
{
	skipcomments = b;
	term = wspc;
	Spaces0();
	return *this;
}

CParser& CParser::NestComments(bool b)
{
	nestcomments = b;
	term = wspc;
	Spaces0();
	return *this;
}

bool CParser::Spaces0() {
	LTIMING("Spaces");
	if(!term)
		return false;
	if((u8)*term > ' ' &&
	   !(term[0] == '/' && term[1] == '/') &&
	   !(term[0] == '/' && term[1] == '*'))
		return false;
	for(;;) {
		if(*term == LINEINFO_ESC) {
			term++;
			fn.Clear();
			while(*term) {
				if(*term == LINEINFO_ESC) {
					++term;
					break;
				}
				if(*term == '\3') {
					line = atoi(++term);
					while(*term) {
						if(*term == LINEINFO_ESC) {
							++term;
							break;
						}
						term++;
					}
					break;
				}
				fn.Cat(*term++);
			}
			continue;
		}
		else
		if(term[0] == '/' && term[1] == '/' && skipcomments) {
			term += 2;
			while(*term && *term != '\n')
				term++;
		}
		else
		if(term[0] == '/' && term[1] == '*' && skipcomments) {
			if(nestcomments) {
				i32 count = 1;
				term += 2;
				while(*term) {
					if(term[0] == '*' && term[1] == '/') {
						term += 2;
						count--;
						if (count == 0)
							break;
					}
					else if(term[0] == '/' && term[1] == '*')
						count++;

					if(*term++ == '\n') {
						line++;
						lineptr = term;
					}
				}
			}
			else {
				term += 2;
				while(*term) {
					if(term[0] == '*' && term[1] == '/') {
						term += 2;
						break;
					}
					if(*term++ == '\n') {
						line++;
						lineptr = term;
					}
				}
			}
		}
		if(!*term) break;
		if((u8)*term > ' ') break;
		if(*term == '\n') {
			line++;
			lineptr = term + 1;
		}
		term++;
	}
	return true;
}

Txt CParser::LineInfoComment(const Txt& file, i32 line, i32 column)
{
	return Txt().Cat() << (char)LINEINFO_ESC << file << '\3'
	                      << line << '\3' << column << (char)LINEINFO_ESC;
}

Txt CParser::GetLineInfoComment(i32 tabsize) const
{
	return LineInfoComment(GetFName(), GetLine(), GetColumn(tabsize));
}

tukk CParser::IsId0(tukk s) const {
	tukk t = term + 1;
	s++;
	while(*s) {
		if(*t != *s)
			return NULL;
		t++;
		s++;
	}
	return IsAlNum(*t) || *t == '_' ? NULL : t;
}

bool CParser::Id0(tukk s) {
	LTIMING("Id");
	tukk t = IsId0(s);
	if(!t)
		return false;
	term = t;
	DoSpaces();
	return true;
}

void CParser::PassId(tukk s) {
	LTIMING("PassId");
	if(!Id(s))
		ThrowError(Txt("отсутствует '") + s + "\'");
}

void CParser::PassChar(char c) {
	LTIMING("PassChar");
	if(!Char(c))
		ThrowError(Txt("отсутствует '") + c + "\'");
}

void CParser::PassChar2(char c1, char c2) {
	LTIMING("PassChar2");
	if(!Char2(c1, c2))
		ThrowError(Txt("отсутствует '") + c1 + c2 + "\'");
}

void CParser::PassChar3(char c1, char c2, char c3) {
	LTIMING("PassChar3");
	if(!Char3(c1, c2, c3))
		ThrowError(Txt("отсутствует '") + c1 + c2 + c3 + "\'");
}

Txt CParser::ReadId() {
	if(!IsId())
		ThrowError("отсутствует id");
	Txt result;
	tukk b = term;
	tukk p = b;
	while(iscid(*p))
		p++;
	term = p;
	DoSpaces();
	return Txt(b, (i32)(uintptr_t)(p - b));
}

Txt CParser::ReadIdh() {
	if(!IsId())
		ThrowError("отсутствует id");
	Txt result;
	tukk b = term;
	tukk p = b;
	while(iscid(*p) || *p == '-')
		p++;
	term = p;
	DoSpaces();
	return Txt(b, (i32)(uintptr_t)(p - b));
}

Txt CParser::ReadIdt() {
	if(!IsId())
		ThrowError("отсутствует id");
	TxtBuf result;
	i32 lvl = 0;
	while(IsAlNum(*term) || *term == '_' || *term == '<' || *term == '>' ||
	      *term == ':' || (*term == ',' || *term == ' ' ) && lvl > 0 ) {
		if(*term == '<') lvl++;
		if(*term == '>') lvl--;
		result.Cat(*term++);
	}
	DoSpaces();
	return Txt(result);
}

bool CParser::IsInt() const {
	LTIMING("IsInt");
	tukk t = term;
	if(*t == '-' || *t == '+') {
		t++;
		while(*t <= ' ')
			t++;
	}
	return IsDigit(*t);
}

i32  CParser::Sgn()
{
	i32 sign = 1;
	if(*term == '-') {
		sign = -1;
		term++;
	}
	else
	if(*term == '+')
		term++;
	Spaces();
	return sign;
}

i32  CParser::ReadInt() {
	LTIMING("ReadInt");
	i32 n;
	bool overflow = false;
	tukk t = ScanInt<char, u8, dword, i32, 10>(n, term, overflow);
	if(!t) ThrowError("отсутствует число");
	if(overflow) ThrowError("число слишком большое");
	term = t;
	DoSpaces();
	return n;
}

i32 CParser::ReadInt(i32 min, i32 max)
{
	i32 n = ReadInt();
	if(n < min || n > max)
		ThrowError("число вне диапазона");
	return n;
}

z64 CParser::ReadInt64()
{
	LTIMING("ReadInt64");
	z64 n;
	bool overflow = false;
	tukk t = ScanInt<char, u8, zu64, z64, 10>(n, term, overflow);
	if(!t) ThrowError("отсутствует число");
	if(overflow) ThrowError("число слишком большое");
	term = t;
	DoSpaces();
	return n;
}

z64 CParser::ReadInt64(z64 min, z64 max)
{
	z64 n = ReadInt64();
	if(n < min || n > max)
		ThrowError("число вне диапазона");
	return n;
}

bool CParser::IsNumber(i32 base) const
{
    if(IsDigit(*term)) {
        i32 q = *term - '0';
        return q >= 0 && q < base;
    }
	i32 q = ToUpper(*term) - 'A';
    return q >= 0 && q < base - 10;
}

u32  CParser::ReadNumber(i32 base)
{
	LTIMING("ReadNumber");

	u32 n;
	bool overflow = false;
	tukk t;

	switch(base) {
	case 10:
		t = ScanUint<char, u8, u32, 10>(n, term, overflow);
		break;
	case 8:
		t = ScanUint<char, u8, u32, 8>(n, term, overflow);
		break;
	case 2:
		t = ScanUint<char, u8, u32, 2>(n, term, overflow);
		break;
	case 16:
		t = ScanUint<char, u8, u32, 16>(n, term, overflow);
		break;
	default:
		u32 n = 0;
		i32 q = ctoi(*term);
		if(q < 0 || q >= base)
			ThrowError("отсутствует число");
		for(;;) {
			i32 c = ctoi(*term);
			if(c < 0 || c >= base)
				break;
			u32 n1 = n;
			n = base * n + c;
			if(n1 > n)
				ThrowError("число слишком большое");
			term++;
		}
		DoSpaces();
		return n;
	}
	if(!t) ThrowError("отсутсвует число");
	if(overflow) ThrowError("число слишком большое");
	term = t;
	DoSpaces();
	return n;
}

zu64  CParser::ReadNumber64(i32 base)
{
	LTIMING("ReadNumber64");

	u32 n;
	bool overflow = false;
	tukk t;

	switch(base) {
	case 10:
		t = ScanUint<char, u8, u32, 10>(n, term, overflow);
		break;
	case 8:
		t = ScanUint<char, u8, u32, 8>(n, term, overflow);
		break;
	case 2:
		t = ScanUint<char, u8, u32, 2>(n, term, overflow);
		break;
	case 16:
		t = ScanUint<char, u8, u32, 16>(n, term, overflow);
		break;
	default:
		zu64 n = 0;
		i32 q = ctoi(*term);
		if(q < 0 || q >= base)
			ThrowError("отсутсвует число");
		for(;;) {
			i32 c = ctoi(*term);
			if(c < 0 || c >= base)
				break;
			zu64 n1 = n;
			n = base * n + c;
			if(n1 > n)
				ThrowError("число слишком большое");
			term++;
		}
		DoSpaces();
		return n;
	}
	if(!t) ThrowError("отсутсвует число");
	if(overflow) ThrowError("число слишком большое");
	term = t;
	DoSpaces();
	return n;
}

bool CParser::IsDouble2() const
{
	tukk t = term;
	if(*t == '-' || *t == '+') {
		t++;
		while(*t <= ' ')
			t++;
	}
	if(*t == '.') {
		t++;
		while(*t <= ' ')
			t++;
	}
	return IsDigit(*t);
}

dword CParser::ReadHex()
{
	i32 hex = 0;
	while(IsXDigit(*++term))
		hex = (hex << 4) + ctoi(*term);
	return hex;
}

bool CParser::ReadHex(dword& hex, i32 n)
{
	hex = 0;
	while(n--) {
		if(!IsXDigit(*++term))
			return false;
		hex = (hex << 4) + ctoi(*term);
	}
	term++;
	return true;
}

Txt CParser::ReadOneTxt(i32 delim, bool chkend) {
	if(!IsChar(delim))
		ThrowError("отсутствует ткст");
	term++;
	TxtBuf result;
	for(;;) {
		if(*term == delim) {
			term++;
			DoSpaces();
			return Txt(result);
		}
		else
		if(*term == '\\') {
			switch(*++term) {
			case 'a': result.Cat('\a'); term++; break;
			case 'b': result.Cat('\b'); term++; break;
			case 't': result.Cat('\t'); term++; break;
			case 'v': result.Cat('\v'); term++; break;
			case 'n': result.Cat('\n'); term++; break;
			case 'r': result.Cat('\r'); term++; break;
			case 'f': result.Cat('\f'); term++; break;
			case 'x': {
				i32 hex = ReadHex();
				result.Cat(hex);
				break;
			}
			case 'u':
				if(uescape) {
					dword hex;
					if(!ReadHex(hex, 4))
						ThrowError("Незавершённый универсальный символ");
					if(hex >= 0xD800 && hex < 0xDBFF) {
						if(term[0] == '\\' && term[1] == 'u') {
							term++;
							dword hex2;
							if(!ReadHex(hex2, 4))
								ThrowError("Незавершённый универсальный символ");
							if(hex2 >= 0xDC00 && hex2 <= 0xDFFF) {
								result.Cat(ToUtf8(((hex & 0x3ff) << 10) | (hex2 & 0x3ff) + 0x10000));
								break;
							}
						}
						ThrowError("Неверная суррогатная пара UTF-16");
					}
					else
						result.Cat(ToUtf8(hex));
				}
				else
					result.Cat(*term++);
				break;
			case 'U':
				if(uescape) {
					dword hex;
					if(!ReadHex(hex, 8))
						ThrowError("Незавершённый универсальный символ");
					if(hex > 0x10ffff)
						ThrowError("Универсальный символ вне диапазона Юникода");
					result.Cat(ToUtf8(hex));
				}
				else
					result.Cat(*term++);
				break;
			default:
				if(*term >= '0' && *term <= '7') {
					i32 oct = *term++ - '0';
					if(*term >= '0' && *term <= '7')
						oct = 8 * oct + *term++ - '0';
					if(*term >= '0' && *term <= '7')
						oct = 8 * oct + *term++ - '0';
					result.Cat(oct);
				}
				else
					result.Cat(*term++);
				break;
			}
		}
		else {
			if((u8)*term < ' ' && *term != '\t') {
				if(chkend) {
					ThrowError("Незавершённый ткст");
					return Txt(result);
				}
				if(*term == '\0')
					return Txt(result);
			}
			result.Cat(*term++);
		}
	}
	DoSpaces();
	return Txt(result);
}

Txt CParser::ReadOneTxt(bool chkend)
{
	return ReadOneTxt('\"', chkend);
}

Txt CParser::ReadTxt(i32 delim, bool chkend) {
	LTIMING("ReadTxt");
	Txt result;
	do
		result.Cat(ReadOneTxt(delim, chkend));
	while(IsChar(delim));
	return result;
}

Txt CParser::ReadTxt(bool chkend)
{
	return ReadTxt('\"', chkend);
}

char CParser::GetChar()
{
	char c = *term++;
	if(c == '\n') {
		line++;
		lineptr = term;
	}
	return c;
}

void CParser::Skip()
{
	LTIMING("Skip");
	if(IsId())
		while(iscid(*term))
			term++;
	else
	if(IsNumber())
		while(IsDigit(*term))
			term++;
	else
	if(IsTxt())
		ReadTxt();
	else
	if(IsChar('\''))
		ReadTxt('\'', false);
	else
	if(*term) {
		if(*term == '\n') {
			line++;
			lineptr = term + 1;
		}
		term++;
	}
	DoSpaces();
}

CParser::Pos CParser::GetPos() const
{
	Pos p;
	p.line = line;
	p.fn = fn;
	p.ptr = term;
	p.wspc = wspc;
	p.lineptr = lineptr;
	return p;
}

i32 CParser::Pos::GetColumn(i32 tabsize) const
{
	i32 pos = 1;
	for(tukk s = lineptr; s < ptr; s++) {
		if(*s == CParser::LINEINFO_ESC) {
			s++;
			while(s < ptr && *s != CParser::LINEINFO_ESC)
				if(*s++ == '\3')
					pos = atoi(s);
		}
		else
		if(*s == '\t')
			pos = (pos + tabsize - 1) / tabsize * tabsize + 1;
		else
			pos++;
	}
	return pos;
}

i32 CParser::GetColumn(i32 tabsize) const
{
	return GetPos().GetColumn(tabsize);
}

void CParser::SetPos(const CParser::Pos& p)
{
	LLOG("SetPos " << p.fn << ":" << p.line);
	line = p.line;
	fn = p.fn;
	term = p.ptr;
	wspc = p.wspc;
	lineptr = p.lineptr;
	if(skipspaces)
		DoSpaces();
}

CParser::CParser(tukk ptr)
: term(ptr), wspc(ptr), lineptr(ptr)
{
	line = 1;
	skipspaces = skipcomments = true;
	nestcomments = false;
	uescape = true;
	Spaces();
}

CParser::CParser(tukk ptr, tukk fn, i32 line)
: term(ptr), wspc(ptr), lineptr(ptr), line(line), fn(fn)
{
	skipspaces = skipcomments = true;
	nestcomments = false;
	uescape = true;
	Spaces();
}

CParser::CParser()
{
	term = lineptr = wspc = NULL;
	line = 0;
	skipspaces = skipcomments = true;
	nestcomments = false;
	uescape = true;
}

void CParser::Set(tukk _ptr, tukk _fn, i32 _line)
{
	term = lineptr = wspc = _ptr;
	fn = _fn;
	line = _line;
	if(skipspaces)
		Spaces();
	LLOG("Set " << fn << ":" << line);
}

void CParser::Set(tukk _ptr)
{
	Set(_ptr, "", 1);
}

inline void NextCTxtLine(TxtBuf& t, tukk linepfx, i32& pl)
{
	t << "\"\r\n" << (linepfx ? linepfx : "") << "\"";
	pl = t.GetLength();
}

inline i32 HexDigit(i32 c)
{
	return "0123456789ABCDEF"[c & 15];
}

static inline void sCatHex(TxtBuf& t, word q)
{
	char h[6];
	h[0] = '\\';
	h[1] = 'u';
	h[2] = HexDigit(q >> 12);
	h[3] = HexDigit(q >> 8);
	h[4] = HexDigit(q >> 4);
	h[5] = HexDigit(q);
	t.Cat(h, 6);
}

Txt AsCTxt(tukk s, tukk lim, i32 linemax, tukk linepfx, dword flags)
{
	TxtBuf t;
	t.Cat('\"');
	i32 pl = 0;
	bool wasspace = false;
	u8 cs = GetDefaultCharset();
	bool toutf8 = GetDefaultCharset() != CHARSET_UTF8;
	while(s < lim) {
		if(t.GetLength() - pl > linemax && (!(flags & ASCTXT_SMART) || wasspace))
			NextCTxtLine(t, linepfx, pl);
		wasspace = *s == ' ';
		switch(*s) {
		case '\a': t.Cat("\\a"); break;
		case '\b': t.Cat("\\b"); break;
		case '\f': t.Cat("\\f"); break;
		case '\t': t.Cat("\\t"); break;
		case '\v': t.Cat("\\v"); break;
		case '\r': t.Cat("\\r"); break;
		case '\"': t.Cat("\\\""); break;
		case '\\': t.Cat("\\\\"); break;
		case '\n': t.Cat("\\n"); wasspace = true; break;
		default:
			if(flags & ASCTXT_JSON) {
				if((u8)*s < 32) {
					sCatHex(t, (u8)*s++);
				}
				else
				if((u8)*s >= 0x7f) {
					if((u8)*s == 0x7f) {
						sCatHex(t, 0x7f);
						s++;
					}
					else {
						tukk s0 = s;
						dword c = toutf8 ? ToUnicode((u8)*s++, cs) : FetchUtf8(s, lim);
						if(c < 0x10000)
							t.Cat(s0, s);
						else {
							c -= 0x10000;
							sCatHex(t, wchar(0xD800 + (0x3ff & (c >> 10))));
							sCatHex(t, wchar(0xDC00 + (0x3ff & c)));
						}
					}
				}
				else
					t.Cat(*s++);
				continue; // skip s++
			}
			else {
				if(u8(*s) < 32 || (u8)*s >= 0x7f && (flags & ASCTXT_OCTALHI) || (u8)*s == 0xff || (u8)*s == 0x7f) {
					char h[4];
					i32 q = (u8)*s;
					h[0] = '\\';
					h[1] = (3 & (q >> 6)) + '0';
					h[2] = (7 & (q >> 3)) + '0';
					h[3] = (7 & q) + '0';
					t.Cat(h, 4);
				}
				else
					t.Cat(*s);
			}
			break;
		}
		s++;
	}
	t.Cat('\"');
	return Txt(t);
}

Txt AsCTxt(tukk s, i32 linemax, tukk linepfx, dword flags)
{
	return AsCTxt(s, s + strlen(s), linemax, linepfx, flags);
}

Txt AsCTxt(const Txt& s, i32 linemax, tukk linepfx, dword flags)
{
	return AsCTxt(s, s.End(), linemax, linepfx, flags);
}

}