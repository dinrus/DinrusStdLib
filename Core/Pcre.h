#ifndef _plugin_pcre_pcre_h_
#define _plugin_pcre_pcre_h_

#include <drx/Core/Core.h>

#ifdef TARGET_WIN32
#define PCRE_EXP_DECL extern
#endif

#include "lib/pcre/pcre.h"

namespace drx {


class RegExp : public Moveable<RegExp> {
public:
	enum
	{
		PARTIAL   = PCRE_PARTIAL,
		/* compile options */
		//UNICODE   = PCRE_UTF8,
		UTF8      = PCRE_UTF8,
		CASELESS  = PCRE_CASELESS,
		MULTILINE = PCRE_MULTILINE,
		UNGREEDY  = PCRE_UNGREEDY,
		DOTALL    = PCRE_DOTALL,

		EXECUTE_OPTIONS = PCRE_ANCHORED
		                | PCRE_NEWLINE_CR
		                | PCRE_NEWLINE_LF
		                | PCRE_NEWLINE_CRLF
		                | PCRE_NEWLINE_ANYCRLF
		                | PCRE_NEWLINE_ANY
		                | PCRE_NOTBOL
		                | PCRE_NOTEOL
		                | PCRE_NOTEMPTY
		                | PCRE_NOTEMPTY_ATSTART
		                | PCRE_NO_START_OPTIMIZE
		                | PCRE_NO_UTF8_CHECK
		                | PCRE_PARTIAL_SOFT
		                | PCRE_PARTIAL_HARD,
		COMPILE_OPTIONS = PCRE_ANCHORED
		                | PCRE_AUTO_CALLOUT
		                | PCRE_BSR_ANYCRLF
		                | PCRE_BSR_UNICODE
		                | PCRE_CASELESS
		                | PCRE_DOLLAR_ENDONLY
		                | PCRE_DOTALL
		                | PCRE_DUPNAMES
		                | PCRE_EXTENDED
		                | PCRE_EXTRA
		                | PCRE_FIRSTLINE
		                | PCRE_JAVASCRIPT_COMPAT
		                | PCRE_MULTILINE
		                | PCRE_NEWLINE_CR
		                | PCRE_NEWLINE_LF
		                | PCRE_NEWLINE_CRLF
		                | PCRE_NEWLINE_ANYCRLF
		                | PCRE_NEWLINE_ANY
		                | PCRE_NO_AUTO_CAPTURE
		                | PCRE_NO_START_OPTIMIZE
		                | PCRE_UCP
		                | PCRE_UNGREEDY
		                | PCRE_UTF8
		                | PCRE_NO_UTF8_CHECK
	};

private:
	Txt   pattern;
	Txt   text;
	pcre* cpattern;
	pcre_extra* study;
	tukk error_string;
	i32 error_offset;
	i32 error_code;
	i32 pos[120]; // if modified-chk config.h as well, keep at multiple of 3
	i32 rc;
	bool first;
	i32 compile_options;
	i32 execute_options;

	bool ReplacePos(Txt& t, i32 p, i32 q, const Txt& r);
	i32  Replace0(Txt& t, const Vec<Txt>& rv, i32k& rv_count, i32& offset);
	Vec<Txt> ResolveBackRef(const Vec<Txt>& rv);
	Vec<Txt> Make_rv(const Txt& r);

public:
	void           Clear(bool freemem = false);
	void           SetOptions(i32 options);
	void           SetPattern(tukk p);
	void           SetPattern(const Txt &p);
	bool           Compile(bool recompile = false);
	bool           Study(bool restudy = false);
	i32            Execute(const Txt& t, i32 offset = 0);
	bool           Match(const Txt& t, bool copy = true);
	bool           FastMatch(const Txt& t);
	bool           GlobalMatch(const Txt& t);
	Txt            operator[](i32k i);
	i32            GetCount();
	Txt            GetTxt(i32 i);
	void           GetMatchPos(i32 i, i32& iPosStart, i32& iPosAfterEnd);

	i32            GetOffset() const;
	i32            GetLength() const;
	i32            GetSubOffset(i32 i) const;
	i32            GetSubLength(i32 i) const;

	i32            Replace(Txt& t, const Vec<Txt>& rv, bool backref=false);
	i32            Replace(Txt& t, const Txt& r, bool backref=false);
	i32            Replace(Txt& t, Event<Vec<Txt>&> cbr);

	i32            ReplaceGlobal(Txt& t, const Vec<Txt>& rv, bool backref=false);
	i32            ReplaceGlobal(Txt& t, const Txt& r, bool backref=false);
	i32            ReplaceGlobal(Txt& t, Event<Vec<Txt>&> cbr);

	Vec<Txt>    GetTxts();
	Txt            GetPattern() const { return pattern; }

	bool           IsError() { return error_code != 0; }
	tukk GetError() { return error_string; }
	i32            GetErrorCode() { return error_code; }

	RegExp(i32 options = UTF8);
	RegExp(tukk p, i32 options = UTF8);
	RegExp(const Txt &p, i32 options = UTF8);
	~RegExp();
};

}

#endif
