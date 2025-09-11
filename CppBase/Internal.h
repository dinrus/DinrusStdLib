#ifndef _CppBase_Internal_h_
#define _CppBase_Internal_h_

namespace drx {
	
tukk SkipTxt(tukk s);

void RemoveComments(Txt& l, bool& incomment);

const VecMap<Txt, Txt>& GetNamespaceMacros();
const Index<Txt>&             GetNamespaceEndMacros();

struct CppMacro : Moveable<CppMacro> {
	Txt        param;
	Txt        body;
	u8          md5[16];
	
	Txt Define(tukk s);
	void   SetUndef()                { body = "\x7f"; }
	bool   IsUndef() const           { return body[0] == '\x7f' && body[1] == '\0'; }

	Txt Expand(const Vec<Txt>& p, const Vec<Txt>& ep) const;

	void   Serialize(Stream& s);
	
	Txt ToTxt() const;
};

enum PPItemType {
	PP_DEFINES,
	PP_INCLUDE,
	PP_USING,
	PP_NAMESPACE,
	PP_NAMESPACE_END
};

struct PPItem {
	i32      type;
	Txt   text;
	i32      segment_id;
	
	void     Serialize(Stream& s) { s % type % text % segment_id; }
};

struct PPMacro : Moveable<PPMacro> {
	CppMacro  macro;
	i32       segment_id;        // a group of macros in single file, between other elements (include, namespace. using, undef...)
	i32       line;              // line in file
	i32       undef_segment_id;  // macro has matching undef in the same file within this segment
	
	void   Serialize(Stream& s) { s % macro % segment_id % line % undef_segment_id; }
	Txt ToTxt() const     { return AsTxt(macro) + " " + AsTxt(segment_id); }
	
	PPMacro()                   { segment_id = undef_segment_id = 0; }
};

struct PPFile { // contains "macro extract" of file, only info about macros defined and namespaces
	Time           filetime;
	Array<PPItem>  item;
	Index<Txt>  includes;
	Vec<Txt> keywords;
	Txt         md5sum;

	void Parse(Stream& in);
	void Serialize(Stream& s) { s % filetime % item % includes % keywords % md5sum; }
	void Dump() const;

private:
	Vec<i32>    ppmacro;   // indicies of macros in sAllMacros

	void CheckEndNamespace(Vec<i32>& namespace_block, i32 level, Md5Stream& md5);
};

PPMacro            *FindPPMacro(const Txt& id, Index<i32>& segment_id, i32& segmenti);
const     CppMacro *FindMacro(const Txt& id, Index<i32>& segment_id, i32& segmenti);

Txt GetIncludePath();

Txt GetSegmentFile(i32 segment_id);

void  MakePP(const Index<Txt>& paths); // this is the only place to change PPFile info, cannot be run concurrently with anything else

const PPFile& GetPPFile(tukk path); // only returns information created by MakePP

Txt GetIncludePath(const Txt& s, const Txt& filedir);
bool   IncludesFile(const Txt& parent_path, const Txt& header_path);

struct FlatPP {
	Index<i32>    segment_id;
	Index<Txt> usings;
};

const FlatPP& GetFlatPPFile(tukk path); // with #includes resolved

struct Cpp {
	static Index<Txt>        kw;

	bool                        incomment;
	bool                        done;
	
	Index<Txt>               visited;

	Index<i32>                  segment_id; // segments of included macros
	VecMap<Txt, PPMacro>  macro; // macros defined
	i32                         std_macros; // standard macros (keywords and trick - fixed)
	Index<Txt>               notmacro; // accelerator / expanding helper
	Txt                      prefix_macro; // for handling multiline macros

	Txt                      output; // preprocessed file
//	Index<Txt>               usedmacro;
	Index<Txt>               namespace_using; // 'using namespace' up to start of file
	Vec<Txt>              namespace_stack; // namspace up to start of file
	
	Md5Stream                   md5;
	
	void   Define(tukk s);

	void   ParamAdd(Vec<Txt>& param, tukk b, tukk e);
	Txt Expand(tukk s);
	void   DoFlatInclude(const Txt& header_path);
	void   Do(const Txt& sourcefile, Stream& in, const Txt& currentfile, bool get_macros);

	bool   Preprocess(const Txt& sourcefile, Stream& in, const Txt& currentfile,
	                  bool just_get_macros = false);

	Txt GetDependeciesMd5(const Vec<Txt>& m);
	
	typedef Cpp CLASSNAME;
};

enum tk_Keywords {
	Tmarker_before_first = 255,
#define CPPID(x)   tk_##x,
#include "keyword.i"
#undef  CPPID
};

enum t_Terms {
	t_eof,
	t_string = -200,
	t_integer,
	t_double,
	t_character,
	t_dblcolon,       // ::
	t_mulass,         // *=
	t_divass,         // /=
	t_modass,         // %=
	t_xorass,         // ^=
	t_neq,            // <>
	t_dot_asteriks,   // .*
	t_elipsis,        // ...
	t_inc,            // ++
	t_addass,         // +=
	t_dec,            // -
	t_arrow_asteriks, // ->*
	t_arrow,          // ->
	t_subass,         // -=
	t_and,            // &
	t_andass,         // &=
	t_or,             // |
	t_orass,          // |=
	t_eq,             // ==
	t_shl,            // <<
	t_shlass,         // <<=
	t_le,             // <=
	t_shr,            // >>
	t_shrass,         // >>=
	t_ge,             // >=
	te_integeroverflow,
	te_badcharacter,
	te_badstring,
};

class LexSymbolStat
{
public:
	LexSymbolStat();
	void Reset(i32 minSymbol, i32 maxSymbol);
	void IncStat(i32 symbol);
	i32  GetStat(i32 symbol) const;
	i32  SumStat(const Vec<i32> & symbols) const;
	void Merge(const LexSymbolStat & other);

private:
	Vec<i32> v;
	i32 minSymbol;
};


class Lex {
#ifdef _DEBUG
	tukk pp;
#endif
	tukk ptr;
	tukk pos;

	Index<Txt> id;
	i32           endkey;
	i32           braceslevel;

	struct Term  : Moveable<Term>{
		tukk   ptr;
		i32    code;
		Txt    text;
		double number;
		bool   grounding;
		
		Term() { grounding = false; }
	};

	bool statsCollected;
	LexSymbolStat symbolStat;
	BiVec<Term> term;
	i32            body;
	bool           grounding;

	bool Char(i32 c)                 { if(*ptr == c) { ptr++; return true; } else return false; }
	void AddCode(i32 code)           { Term& tm = term.AddTail(); tm.code = code; tm.ptr = pos; tm.grounding = grounding; }
	void AssOp(i32 noass, i32 ass)   { AddCode(Char('=') ? ass : noass); }
	void Next();
	bool Prepare(i32 pos);
	const char  GetCharacter();
	void ThrowError(tukk e);

public:
	Event<const Txt&> WhenError;

	struct Grounding {};

	i32         Code(i32 pos = 0);
	bool        IsId(i32 pos = 0);
	Txt      Id(i32 pos = 0);
	i32         Int(i32 pos = 0);
	double      Double(i32 pos = 0);
	i32         Chr(i32 pos = 0);
	Txt      Text(i32 pos = 0);

	void        Get(i32 n = 1);
	i32         GetCode()                   { i32 q = Code(); Get(); return q; }
	Txt      GetId()                     { Txt q = Id(); Get(); return q; }
	i32         GetInt()                    { i32 q = Int(); Get(); return q; }
	double      GetDouble()                 { double q = Double(); Get(); return q; }
	i32         GetChr()                    { i32 q = Chr(); Get(); return q; }
	Txt      GetText()                   { Txt q = Text(); Get(); return q; }

	i32         Id(const Txt& s)         { return id.FindAdd(s) + 256; }

	i32         GetBracesLevel() const      { return braceslevel; }
	void        ClearBracesLevel()          { braceslevel = 0; }
	
	void        BeginBody()                 { body++; }
	void        EndBody()                   { body--; }
	void        ClearBody()                 { body = 0; }
	bool        IsBody() const              { return body; }
	bool        IsGrounded()                { Code(); return term.GetCount() && term[0].grounding; }
	void        SkipToGrounding();

	tukk Pos(i32 pos = 0);
	i32         operator[](i32 pos)         { return Code(pos); }
	operator    i32()                       { return Code(0); }
	void        operator++()                { return Get(); }
	
	void        Dump(i32 pos);

	void        Init(tukk s);
	void        StartStatCollection();
	const LexSymbolStat & FinishStatCollection();

	Lex();
};

struct SrcFile {
	SrcFile();
	rval_default(SrcFile);

	Txt      text;
	Vec<i32> linepos;
	i32 preprocessorLinesRemoved;
	i32 blankLinesRemoved;
	i32 commentLinesRemoved;
};

struct Parser;

SrcFile PreProcess(Stream& in, Parser& parser);

Txt CppItemKindAsTxt(i32 kind);

struct Parser : ParserContext {
	struct Decla {
		bool    s_static = false;
		bool    s_extern = false;
		bool    s_register = false;
		bool    s_mutable = false;
		bool    s_explicit = false;
		bool    s_virtual = false;
		Txt  name;
		bool    function = false;
		bool    type_def = false;
		bool    isfriend = false;
		bool    istemplate = false;
		bool    istructor = false;
		bool    isdestructor = false;
		bool    isptr = false;
		bool    nofn = false;
		bool    oper = false;
		bool    castoper = false;

		Txt  tnames;
		Txt  type;
		Txt  natural;
	};

	struct Decl : Decla {
		Array<Decl> param;
		
		rval_default(Decl);
		Decl() {}
	};

	struct RecursionCounter
	{
		i32 change;
		i32 &count;
		RecursionCounter(i32 &count_i, i32 change_i = 1) :
		  change(change_i), count(count_i)
		{
		  count += change;
		}
		~RecursionCounter() { count -= change; }
	};

	SrcFile     file;
	Lex         lex;
	i32         filei;
	u8        filetype;
	Txt      title;
	i32         struct_level;

	Event<i32, const Txt&> err;

	i32     lpos, line;

	CppBase *base;

	i32    RPtr();

	bool   Key(i32 code);
	bool   EatBody();
	Txt ResolveAutoType();
	void   TryLambda();
	bool   Skipars(i32& q);

	void   Cv();
	Txt TType();
	Txt ReadType(Decla& d, const Txt& tname, const Txt& tparam);
	void   Qualifier(bool override_final = false);
	void   ParamList(Decl& d);
	void   Declarator(Decl& d, tukk p);
	void   EatInitializers();
	void   Vars(Array<Decl>& r, tukk p, bool type_def, bool more);
	void   ReadMods(Decla& d);
	Array<Decl> Declaration0(bool l0, bool more, const Txt& tname, const Txt& tparam);
	Array<Decl> Declaration(bool l0/* = false*/, bool more/* = false*/, const Txt& tname, const Txt& tparam);
	bool   IsParamList(i32 q);
	void   Elipsis(Decl& d);
	Decl&  Finish(Decl& d, tukk p);
	void   AddNamespace(const Txt& n, const Txt& name);
	bool   Scope(const Txt& tp, const Txt& tn);

	Txt TemplateParams(Txt& pnames);
	Txt TemplateParams();
	Txt TemplatePnames();
	Txt Name(Txt& h, bool& castoper, bool& oper);
	Txt Name(bool& castoper, bool& oper);
	Txt Constant();
	Txt ReadOper(bool& castoper);

	i32    GetLine(tukk pos);
	void   Line();
	void   Check(bool b, tukk err);
	void   CheckKey(i32 c);

	void   ClassEnum(const Txt& clss);
	bool   IsEnum(i32 i);
	bool   UsingNamespace();
	void   SetScopeCurrent();
	void   ScopeBody();
	void   DoNamespace();
	void   Do();
	Txt AnonymousName();
	Txt StructDeclaration(const Txt& tp, const Txt& tn);
	void   Enum(bool vars);

	CppItem& Item(const Txt& scope, const Txt& using_namespace, const Txt& item,
	              const Txt& name, bool impl);
	CppItem& Item(const Txt& scope, const Txt& using_namespace, const Txt& item,
	              const Txt& name);

	CppItem& Fn(const Decl& d, const Txt& templ, bool body,
	            const Txt& tname, const Txt& tparam);

	struct Error {};

	void   ThrowError(const Txt& e);
	void   Resume(i32 bl);

	void   MatchPars();
	bool   VCAttribute();
	bool   TryDecl();
	void   Statement();
	void   Locals(const Txt& type);
	Txt Tparam(i32& q);
	bool   IsNamespace(const Txt& scope);
	
	friend class Lex; // Fix to make Lex::ThrowError

	typedef Parser CLASSNAME;

public:
	void AddMacro(i32 lineno, const Txt& macro, i32 kind = MACRO);

	bool                      dobody;
	Function<Txt(Txt, Txt, Txt)> qualify; // used to qualify local variable names (needs main codebase and its mutex)
	i32                       currentScopeDepth;
	i32                       maxScopeDepth;
	
	Txt                    namespace_info;

	const SrcFile &getPreprocessedFile() { return file; }

	void  Do(Stream& in, CppBase& _base, i32 file, i32 filetype,
	         const Txt& title, Event<i32, const Txt&> _err,
	         const Vec<Txt>& typenames,
	         const Vec<Txt>& namespace_stack,
	         const Index<Txt>& namespace_using);

	Parser() : dobody(false) { lex.WhenError = OTVET(ThrowError); }
};

void   QualifyTypes(CppBase& base, const Txt& scope, CppItem& m);
Txt QualifyKey(const CppBase& base, const Txt& scope, const Txt& type, const Txt& usings);

const Array<CppItem>& GetTypeItems(const CppBase& codebase, const Txt& type);

Vec<Txt> MakeXP(tukk s);

};

#endif
