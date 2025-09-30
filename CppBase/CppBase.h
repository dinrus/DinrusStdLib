#ifndef DOCPP_H
#define DOCPP_H

#include <drx/Core/Core.h>

namespace drx {
	
// These functions can only but executed by single thread, unless said otherwise
	
bool   IsCPPFile(const Txt& file);
bool   IsHFile(const Txt& path);

Txt    GetStdDefs();

void   SetPPDefs(const Txt& defs);

void   InvalidatePPCache();

void   PPSync(const Txt& include_path);

enum Kind {
	STRUCT,
	STRUCTTEMPLATE,
	TYPEDEF,
	CONSTRUCTOR,
	DESTRUCTOR,
	FUNCTION,
	INSTANCEFUNCTION,
	CLASSFUNCTION,
	FUNCTIONTEMPLATE,
	INSTANCEFUNCTIONTEMPLATE,
	CLASSFUNCTIONTEMPLATE,
	INLINEFRIEND,
	VARIABLE,
	INSTANCEVARIABLE,
	CLASSVARIABLE,
	ENUM,
	MACRO,
	FRIENDCLASS,
	NAMESPACE,
	FLAGTEST,
};

inline bool IsCppType(i32 i)
{
	return (i >= STRUCT && i <= TYPEDEF) || i == FRIENDCLASS;
}

inline bool IsCppCode(i32 i) {
	return i >= CONSTRUCTOR && i <= INLINEFRIEND;
};

inline bool IsCppData(i32 i) {
	return i >= VARIABLE && i <= ENUM;
}

inline bool IsCppMacro(i32 i) {
	return i == MACRO;
}

inline bool IsCppTemplate(i32 i) {
	return i == STRUCTTEMPLATE || (i >= FUNCTIONTEMPLATE && i <= CLASSFUNCTIONTEMPLATE);
}

void   CleanPP();
void   SerializePPFiles(Stream& s);
void   SweepPPFiles(const Index<Txt>& keep);

void InvalidateFileTimeCache();
void InvalidateFileTimeCache(const Txt& path);
Time GetFileTimeCached(const Txt& path);

Txt                  NormalizeSourcePath(const Txt& path, const Txt& currdir);
Txt                  NormalizeSourcePath(const Txt& path);

void                 ClearSources();
const Index<Txt>&    GetAllSources();
void                 GatherSources(const Txt& path);
Txt                  GetMasterFile(const Txt& file);
const VecMap<Txt, Txt>& GetAllSourceMasters();

tukk *CppKeyword();

enum CppAccess {
	PUBLIC,
	PROTECTED,
	PRIVATE,
};

enum FileTypeEnum {
	FILE_H,
	FILE_HPP,
	FILE_CPP,
	FILE_C,
	FILE_OTHER,
};

struct CppItem {
	Txt         qitem;
	Txt         item;
	Txt         name;
	Txt         uname;
	Txt         natural;
	Txt         type;
	Txt         qtype;
	Txt         tparam; // complete template parameters list, like '<class T>'
	Txt         param;
	Txt         pname;
	Txt         ptype; // fn: types of parameters, struct: base classes
	Txt         qptype;
	Txt         tname;
	Txt         ctname;
	Txt         using_namespaces;
	u8           access = PUBLIC;
	u8           kind = STRUCT;
	i16          at = 0;
	bool           virt = false;

	bool           decla = false;
	bool           lvalue = false;
	bool           isptr = false;
	
	u8             filetype = FILE_OTHER;
	bool           impl = false;
	i32            file = 0;
	i32            line = 0;

	bool           qualify = true;

	bool           IsType() const      { return IsCppType(kind); }
	bool           IsCode() const      { return IsCppCode(kind); }
	bool           IsData() const      { return IsCppData(kind); }
	bool           IsMacro() const     { return IsCppMacro(kind); }
	bool           IsFlagTest() const  { return kind == FLAGTEST; }
	bool           IsTemplate() const  { return IsCppTemplate(kind); }
	
	void           Serialize(Stream& s);
	
	void           Dump(Stream& s) const;
	Txt            ToTxt() const;
};

i32 FindItem(const Array<CppItem>& x, const Txt& qitem);
i32 FindName(const Array<CppItem>& x, const Txt& name, i32 i = 0);

struct CppBase : ArrayMap<Txt, Array<CppItem> > {
	Txt            types_md5;
	Index<Txt>     namespaces;

	bool           IsType(i32 i) const;
	void           Sweep(const Index<i32>& file, bool keep = true);
	void           RemoveFiles(const Index<i32>& remove_file) { Sweep(remove_file, false); }
	void           RemoveFile(i32 filei);
	void           Append(CppBase&& base);

	void           Dump(Stream& s);
};

class ScopeInfo { // information about scope
	bool           bvalid; // baselist is valid
	bool           nvalid; // scopes is valid
	Vec<Txt>       baselist; // list of all base classes of scope
	Vec<Txt>       scopes; // list of scopes (drx::Txt::Init::, drx::Txt::, drx::)
	i32            scopei; // index of this scope in base
	Txt            usings; // using namespaces contained in scopes
	
	void           Bases(i32 i, Vec<i32>& g);
	void           Init();

public:
	const CppBase&            base;
	VecMap<Txt, Txt> cache;

	const Vec<Txt>& GetBases();
	const Vec<Txt>& GetScopes(const Txt& usings);
	i32                   GetScope() const              { return scopei; }
	void                  NoBases()                     { baselist.Clear(); bvalid = true; }

	ScopeInfo(const CppBase& base, i32 scopei = -1);
	ScopeInfo(i32 scopei, const CppBase& base);
	ScopeInfo(const CppBase& base, const Txt& scope);
	ScopeInfo(const ScopeInfo& f);
};

struct ParserContext {
	struct Context {
		Txt            ns;
		Txt            scope;
		Txt            ctname;
		Vec<i32>       tparam;
		Index<i32>     typenames;
		i32            access;
		Txt            namespace_using;

		void operator<<=(const Context& t);

		Txt  Dump() const;
		
		Context() {}
		rval_default(Context);
	};

	Context     context;
	Txt         current_scope;
	Txt         current_key;
	Txt         current_name;
	CppItem     current;
	bool        inbody;

	struct Local : Moveable<Local> {
		Txt    type;
		bool   isptr;
		i32    line;
	};

	VecMap<Txt, Local>  local;

	Vec<Txt>       GetNamespaces() const;
	bool           IsInBody() const                   { return inbody; }
};

// Parse CAN be run in parallel
void Parse(CppBase& base, const Txt& src, i32 file, i32 filetype, const Txt& path,
           Event<i32, const Txt&> error, const Vec<Txt>& namespace_stack,
           const Index<Txt>& namespace_using);


Txt NoTemplatePars(const Txt& type);

// PreprocessParse CAN be run in parallel
void PreprocessParse(CppBase& base, Stream& in, i32 file, const Txt& path,
                     Event<i32, const Txt&> error);
Txt PreprocessCpp(const Txt& src, const Txt& path);

ParserContext AssistParse(const Txt& src, const Txt& path_, Event<i32, const Txt&> error,
                          Function<Txt(Txt, Txt, Txt)> qualify);

void SimpleParse(CppBase& cpp, const Txt& txt, const Txt& cls);

void   Qualify(CppBase& base);

Txt Qualify(const CppBase& base, const Txt& scope, const Txt& type, const Txt& usings);

Txt ResolveTParam(const CppBase& codebase, const Txt& type, const Vec<Txt>& tparam);
void   ResolveTParam(const CppBase& codebase, Vec<Txt>& type, const Vec<Txt>& tparam);

Txt ParseTemplatedType(const Txt& type, Vec<Txt>& tparam);

bool HasCPPFileKeyword(const Txt& path, const Txt& id);

Txt GetPPMD5(const Txt& fn);

// GetDependeciesMD5 CAN be run in parallel
Txt GetDependeciesMD5(const Txt& path, Index<Txt>& visited);

Index<Txt> GetExpressionType(const CppBase& codebase, const ParserContext& parser, const Vec<Txt>& xp);

}

#endif
