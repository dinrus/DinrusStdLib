#ifndef _ESC_ESC_H_
#define _ESC_ESC_H_

#include <drx/Core/Core.h>


namespace drx {

enum  EscTypeKind { ESC_VOID, ESC_DOUBLE, ESC_ARRAY, ESC_MAP, ESC_LAMBDA, ESC_INT64 };

Txt    EscTypeName(i32 sv_type);

struct EscEscape;
class  EscLambda;
struct EscHandle;
struct Esc;

class EscVal : Moveable<EscVal> {
	struct RefCount {
		Atomic   refcount;
		RefCount()              { refcount = 1; }
	};

	i32              type;
	mutable hash_t   hash;

	struct EscMap;
	struct EscArray;

	union {
		double         number;
		z64            i64_;
		EscArray      *array;
		EscMap        *map;
		EscLambda     *lambda;
	};

	void                  Free();
	void                  Assign(const EscVal& s);

	void                  InitTxt(const WTxt& w);
	Vec<EscVal>&       CloneArray();

	VecMap<EscVal, EscVal>& CloneMap();

	static i32             total;
	static i32             max_total;

public:
	static i32             GetTotalCount();
	static void            SetMaxTotalCount(i32 n);
	static i32             GetMaxTotalCount();

	bool IsVoid() const                          { return type == ESC_VOID; }
	EscVal();

	bool                   IsNumber() const      { return findarg(type, ESC_DOUBLE, ESC_INT64) >= 0; }
	double                 GetNumber() const;
	bool                   IsInt64() const       { return type == ESC_INT64; }
	z64                    GetInt64() const;
	bool                   IsInt() const;
	i32                    GetInt() const;
	EscVal(double n);
	EscVal(z64 n);
	EscVal(i32 n);

	bool                    IsArray() const      { return type == ESC_ARRAY; }
	const Vec<EscVal>&   GetArray() const;
	EscVal                  ArrayGet(i32 i) const;
	EscVal                  ArrayGet(i32 i, i32 n) const;
	bool                    ArraySet(i32 i, EscVal val);
	bool                    Replace(i32 i, i32 n, EscVal a);
	void                    SetEmptyArray();
	void                    ArrayAdd(EscVal val);
	bool                    Append(EscVal a);

	operator WTxt() const;
	operator Txt() const                   { return operator WTxt().ToTxt(); }

	EscVal(tukk s)                         { InitTxt(WTxt(s)); }
	EscVal(const WTxt& s)                  { InitTxt(s); }
	EscVal(const Txt& s)                   { InitTxt(s.ToWTxt()); }

	bool                                 IsMap() const         { return type == ESC_MAP; }
	const VecMap<EscVal, EscVal>&     GetMap() const;
	EscVal                               MapGet(EscVal key) const;
	void                                 MapSet(EscVal key, EscVal value);
	void                                 SetEmptyMap();

	bool                                 IsLambda() const     { return type == ESC_LAMBDA; }
	const EscLambda&                     GetLambda() const;
	EscLambda&                           CreateLambda();


	void    Escape(tukk method, Event<EscEscape&> escape);
	void    Escape(tukk method, EscHandle *h, Event<EscEscape&> escape);
	bool    HasNumberField(tukk id) const;
	i32     GetFieldInt(tukk id) const;

	i32     GetType() const                                   { return type; }
	Txt     GetTypeName() const                               { return DRX::EscTypeName(type); }

	i32     GetCount() const;

	hash_t   GetHashVal() const;
	bool     operator==(const EscVal& a) const;
	bool     operator!=(const EscVal& a) const              { return !(*this == a); }

	Txt ToTxt(i32 maxlen = INT_MAX, i32 indent_step = 4, bool hex = false, i32 indent = 0) const;

	EscVal& operator=(const EscVal& s);
	EscVal(const EscVal& src);

	~EscVal();
};

struct EscVal::EscArray : EscVal::RefCount {
	Vec<EscVal> array;

	void     Retain()        { AtomicInc(refcount); }
	void     Release()       { if(AtomicDec(refcount) == 0) delete this; }
};

struct EscVal::EscMap : EscVal::RefCount {
	VecMap<EscVal, EscVal> map;
	i32                           count;

	void     Retain()        { AtomicInc(refcount); }
	void     Release()       { if(AtomicDec(refcount) == 0) delete this; }

	EscMap()                 { count = 0; }
};

struct EscHandle {
	Atomic   refcount;

	void       Retain()        { AtomicInc(refcount); }
	void       Release()       { if(AtomicDec(refcount) == 0) delete this; }

	EscHandle()                { refcount = 0; }
	virtual ~EscHandle()       {}
};

class EscLambda {
	Atomic   refcount;

	void     Retain()        { AtomicInc(refcount); }
	void     Release()       { if(AtomicDec(refcount) == 0) delete this; }

	EscLambda()                 { refcount = 1; varargs = false; handle = NULL; }
	~EscLambda()                { if(handle) handle->Release(); }

	friend class EscVal;

public:
	Vec<Txt>           arg;
	Vec<Txt>           def;
	Vec<bool>          inout;
	Txt                   code;
	EscHandle            *handle;
	Event<EscEscape&>     escape;
	bool                  varargs;
	Txt                   filename;
	i32                   line;

private:
	EscLambda(const EscLambda&);
	void operator=(const EscLambda&);
};

template <>
inline hash_t GetHashVal(const EscVal& v)
{
	return v.GetHashVal();
}

bool     IsTrue(const EscVal& a);

void     SkipBlock(CParser& p);
EscVal   ReadLambda(CParser& p);
EscVal   ReadLambda(tukk s);

struct Esc : public CParser {
	struct SRVal : Moveable<SRVal> {
		EscVal *lval;
		EscVal  rval;
		EscVal  sbs;

		SRVal()                    { lval = NULL; }
		SRVal(const EscVal& v)   { lval = NULL; rval = v; }
		SRVal(double n)            { lval = NULL; rval = n; }
		SRVal(z64 n)             { lval = NULL; rval = n; }
		SRVal(zu64 n)            { lval = NULL; rval = (z64)n; }
		SRVal(bool n)              { lval = NULL; rval = (z64)n; }
	};

	ArrayMap<Txt, EscVal>& global;
	EscVal                    self;
	ArrayMap<Txt, EscVal>  var;

	i32      skipexp;
	i32      loop;
	bool     no_break, no_return, no_continue;
	i32&     op_limit;
	i32      r_stack_level;
	EscVal return_value;

	static i32 stack_level;

	void       OutOfMemory();

	void       TestLimit();
	double     DoCompare(const EscVal& a, const EscVal& b, tukk op);
	double     DoCompare(const SRVal& a, tukk op);
	Txt        ReadName();
	EscVal     ExecuteLambda(const Txt& id, EscVal lambda, SRVal self, Vec<SRVal>& arg);

	void       Assign(EscVal& val, const Vec<EscVal>& sbs, i32 si, const EscVal& src);

	EscVal     Get(const SRVal& val);
	void       Assign(const SRVal& val, const EscVal& src);

	EscVal     GetExp();

	double Number(const EscVal& a, tukk oper);
	z64  Int(const EscVal& a, tukk oper);
	double Number(const SRVal& a, tukk oper);
	z64  Int(const SRVal& a, tukk oper);

	EscVal   MulArray(EscVal array, EscVal times);

	void  Subscript(SRVal& r, SRVal _self, Txt id);
	void  Subscript(SRVal& r);
	void  Term(SRVal& r);
	void  Unary(SRVal& r);
	void  Mul(SRVal& r);
	void  Add(SRVal& r);
	void  Shift(SRVal& r);
	void  Compare(SRVal& r);
	void  Equal(SRVal& r);
	void  BinAnd(SRVal& r);
	void  BinXor(SRVal& r);
	void  BinOr(SRVal& r);
	void  And(SRVal& r);
	void  Or(SRVal& r);
	void  Cond(SRVal& r);
	void  Assign(SRVal& r);
	void  Exp(SRVal& r);

	void  SkipTerm();
	void  SkipStatement();
	void  SkipExp();
	bool  PCond();
	void  FinishSwitch();
	void  DoStatement();

	void  Run();

	Esc(ArrayMap<Txt, EscVal>& global, tukk s, i32& oplimit,
	    const Txt& fn, i32 line = 1)
	: CParser(s, fn, line), global(global), op_limit(oplimit)
	{ r_stack_level = stack_level;  skipexp = false; }
	~Esc() { stack_level = r_stack_level; }
};

struct EscEscape {
	Esc&            esc;
	EscVal         self;
	Array<EscVal>& arg;
	EscVal         ret_val;
	Txt            id;

	EscVal&      operator[](i32 i)             { return arg[i]; }
	i32          GetCount() const              { return arg.GetCount(); }
	void         operator=(const EscVal& v)  { ret_val = v; }
	operator     EscVal&()                   { return ret_val; }
	void         ThrowError(tukk s)     { esc.ThrowError(s); }

	Txt          DumpType(i32 i);
	Txt          InCall();

	void         CheckNumber(i32 i);
	void         CheckArray(i32 i);
	void         CheckMap(i32 i);

	double       Number(i32 i);
	i32          Int(i32 i);

	EscEscape(Esc& esc, EscVal self, Array<EscVal>& arg)
	  : esc(esc), self(self), arg(arg) {}
};

void Escape(ArrayMap<Txt, EscVal>& globals, tukk function, void (*escape)(EscEscape& e));

inline // resolve overloading with Function...
void Escfn(ArrayMap<Txt, EscVal>& globals, tukk function, void (*escape)(EscEscape& e))
{
	return Escape(globals, function, escape);
}

void Escape(ArrayMap<Txt, EscVal>& globals, tukk function, Event<EscEscape&> escape);

void Scan(ArrayMap<Txt, EscVal>& global, tukk code, tukk filename = "");

void StdLib(ArrayMap<Txt, EscVal>& global);

void     LambdaArgs(CParser& p, EscLambda& l);

EscVal Execute(ArrayMap<Txt, EscVal>& global, EscVal *self,
                 const EscVal& lambda, Vec<EscVal>& arg, i32 oplimit = 50000);
EscVal Execute(ArrayMap<Txt, EscVal>& global, EscVal *self,
                 tukk name, Vec<EscVal>& arg, i32 oplimit = 50000);
EscVal Execute(ArrayMap<Txt, EscVal>& global, tukk name, i32 oplimit = 50000);

EscVal Evaluatex(tukk expression, ArrayMap<Txt, EscVal>& global, i32 oplimit = 50000);
EscVal Evaluate(tukk expression, ArrayMap<Txt, EscVal>& global, i32 oplimit = 50000);

EscVal   EscFromStdVal(const Val& v);
Val      StdValFromEsc(const EscVal& v);
void     StdValLib(ArrayMap<Txt, EscVal>& global);

bool     IsDate(const EscVal& v);
bool     IsTime(const EscVal& v);

Txt      Expand(const Txt& doc, ArrayMap<Txt, EscVal>& global,
                i32 oplimit = 50000, Txt (*format)(const Val& v) = StdFormat);

}

#endif
