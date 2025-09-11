#ifndef _CodeEditor_Highlight_h_
#define _CodeEditor_Highlight_h_

#define CTIMING(x) // RTIMING(x)

struct HlStyle {
	Color color;
	bool  bold;
	bool  italic;
	bool  underline;
};

struct Isx : Moveable<Isx> { // Позиция '(', '['
	i32    line;
	i32    pos;

	void Serialize(Stream& s)    { s % line % pos; }

	friend bool operator==(Isx a, Isx b) { return a.line == b.line && a.pos == b.pos; }
	friend bool operator!=(Isx a, Isx b) { return !(a == b); }
};

struct IfState : Moveable<IfState> {
	enum        { IF = '0', ELIF, ELSE, ELSE_ERROR, ENDIF_ERROR };

	WTxt    iftext;
	short   ifline;
	char    state;

	void Serialize(Stream& s)         { s % iftext % ifline % state; }

	bool operator==(const IfState& b) const {
		return iftext == b.iftext && state == b.state && ifline == b.ifline;
	}

	IfState()                         { ifline = state = 0; }
};

struct HighlightSetup { // Глобальные настройки подсветки
public:
#define HL_COLOR(x, a, b)      x,
	enum {
#include "hl_color.i"
		HL_COUNT
	};
#undef HL_COLOR

	static HlStyle hl_style[HL_COUNT];
	static u8      hilite_scope;
	static u8      hilite_ifdef;
	static u8      hilite_bracket;
	static bool    thousands_separator;
	static bool    indent_spaces;
	static i32     indent_amount;
	static bool    no_parenthesis_indent;

	static const HlStyle& GetHlStyle(i32 i);
	static void           SetHlStyle(i32 i, Color c, bool bold = false, bool italic = false, bool underline = false);
	static void           LoadHlStyles(tukk s);
	static Txt            StoreHlStyles();
	static void           HostColors();
	static void           DarkTheme(bool host_colors = true);
	static void           WhiteTheme(bool host_colors = true);
	static void           DefaultHlStyles();
	static void           InitOnce();

	static const char    *GetHlName(i32 i);
	static bool           HasHlFont(i32 i);
};

struct HighlightOutput : HighlightSetup {
	Vec<LineEdit::Highlight>& v;
	LineEdit::Highlight          def;
	i32                          pos;

public:
	void SetChar(i32 pos, i32 chr)                    { v[pos].chr = chr; }
	void Set(i32 pos, i32 count, const HlStyle& ink);
	void SetFlags(i32 pos, i32 count, word flags);
	void SetFont(i32 pos, i32 count, const HlStyle& f);
	void SetPaper(i32 pos, i32 count, Color paper);
	void SetInk(i32 pos, i32 count, Color ink);
	void SetFlags(i32 count, word flags)              { SetFlags(pos, count, flags); }
	void Put(i32 count, const HlStyle& ink)           { Set(pos, count, ink); pos += count; }
	void Put(i32 count, const HlStyle& ink, Color paper);
	void Put(i32 count, const HlStyle& ink, const HlStyle& paper);
	void Put(const HlStyle& ink)                      { Put(1, ink); }
	void Put(const HlStyle& ink, word flags)          { Put(1, ink); v[pos - 1].flags = flags; }
	void Flags(word flags)                            { v[pos - 1].flags = flags; }
	i32  GetCount() const                             { return v.GetCount(); }

	wtukk CTxt(wtukk p);

	HighlightOutput(Vec<LineEdit::Highlight>& v);
	~HighlightOutput();
};

class EditorSyntax : public HighlightSetup { // Наследуется, чтобы стали доступны статические члены
	struct SyntaxDef {
		Event<One<EditorSyntax>&> factory;
		Txt                       patterns;
		Txt                       description;
	};

	static ArrayMap<Txt, SyntaxDef>& defs();

protected:
	bool                    ignore_errors;
	i32                     comments_lang;

public:
	virtual void            Clear();
	virtual void            ScanSyntax(wtukk ln, wtukk e, i32 line, i32 tab_size);
	virtual void            Serialize(Stream& s);
	virtual void            IndentInsert(CodeEditor& editor, i32 chr, i32 count);
	virtual bool            CheckBrackets(CodeEditor& e, z64& bpos0, z64& bpos); // СДЕЛАТЬ: Заменить генерным механизмом
	virtual void            CheckSyntaxRefresh(CodeEditor& e, z64 pos, const WTxt& text);
	virtual bool            CanAssist() const;
	virtual void            Highlight(wtukk s, wtukk end, HighlightOutput& hls,
	                                  CodeEditor *editor, i32 line, z64 pos);
	virtual Vec<IfState>    PickIfStack();
	virtual void            ReformatComment(CodeEditor& e);
	virtual ~EditorSyntax();

	static Color IfColor(char ifstate);

	void    Set(const Txt& s)           { CTIMING("Set"); if(s.GetCount() == 0) Clear(); else LoadFromTxt(*this, s); }
	Txt     Get()                          { CTIMING("Get"); return StoreAsTxt(*this); }

	void    IgnoreErrors()                 { ignore_errors = true; }
	void    SpellCheckComments(i32 lang)   { comments_lang = lang; }

	EditorSyntax()                         { Clear(); ignore_errors = false; }

	static void Register(tukk id, Event<One<EditorSyntax>&> factory,
	                     tukk exts, tukk description);
	static One<EditorSyntax> Create(tukk id);
	static Txt               GetSyntaxForFilename(tukk fn);
	static i32               GetSyntaxCount()             { return defs().GetCount(); }
	static Txt               GetSyntax(i32 i)             { return defs().GetKey(i); }
	static Txt               GetSyntaxDescription(i32 i);
	static Txt               GetSyntaxPatterns(i32 i)     { return defs()[i].patterns; }
};

#endif