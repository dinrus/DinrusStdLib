class   Id;
class   Val;
class   ValArray;
class   ValMap;
class   XmlIO;
class   JsonIO;

class   Ref;
struct  ValTypeRef;

template <class T>
void Jsonize(JsonIO& io, T& var);

template <class T>
void Xmlize(XmlIO& xml, T& var);

const u32 VOID_V    = 0;

const u32 INT_V     = 1;
const u32 DOUBLE_V  = 2;
const u32 TXT_V  = 3;
const u32 DATE_V    = 4;
const u32 TIME_V    = 5;

const u32 ERROR_V   = 6;

const u32 VALUE_V   = 7;

const u32 WTXT_V = 8;

const u32 VALUEARRAY_V = 9;

const u32 INT64_V  = 10;//i64
const u32 INT64_V2  = 11;//z64
const u32 BOOL_V   = 12;

const u32 VALUEMAP_V   = 13;

const u32 UNKNOWN_V = (u32)0xffffffff;

template <class T>
inline u32 ValTypeNo(const T *)                 { return StaticTypeNo<T>() + 0x8000000; }

template<> inline u32 ValTypeNo(i32k*)     { return INT_V; }
template<> inline u32 ValTypeNo(const z64*)     { return INT64_V2; }
template<> inline u32 ValTypeNo(const i64*)     { return INT64_V; }
template<> inline u32 ValTypeNo(const double*)  { return DOUBLE_V; }
template<> inline u32 ValTypeNo(const bool*)    { return BOOL_V; }
template<> inline u32 ValTypeNo(const Txt*)     { return TXT_V; }
template<> inline u32 ValTypeNo(const WTxt*)    { return WTXT_V; }
template<> inline u32 ValTypeNo(const Date*)    { return DATE_V; }
template<> inline u32 ValTypeNo(const Time*)    { return TIME_V; }
template<> inline u32 ValTypeNo(const Val*)     { return VALUE_V; }

template <class T, u32 type = UNKNOWN_V, class B = EmptyClass>
class ValType : public B {
public:
	static u32 ValTypeNo()                      { return type == UNKNOWN_V ? StaticTypeNo<T>() + 0x8000000 : type; }
	friend u32 ValTypeNo(const T*)              { return T::ValTypeNo(); }

	bool     IsNullInstance() const                 { return false; }
	void     Serialize(Stream& s)                   { NEVER(); }
	void     Xmlize(XmlIO& xio)                     { NEVER(); }
	void     Jsonize(JsonIO& jio)                   { NEVER(); }
	hash_t   GetHashVal() const                     { return 0; }
	bool     operator==(const T&) const             { NEVER(); return false; }
	Txt      ToTxt() const                          { return typeid(T).name(); }
	i32      Compare(const T&) const                { NEVER(); return 0; }
	i32      PolyCompare(const Val&) const          { NEVER(); return 0; }

	operator ValTypeRef();
};

template <class T, u32 type, class B = EmptyClass> // Backward compatiblity
class AssignValTypeNo : public ValType<T, type, B> {};

template <class T>
u32 GetValTypeNo() { return ValTypeNo((T*)NULL); }

class Val : Moveable_<Val> {
public:
	class Void {
	protected:
		Atomic  refcount;

	public:
		void               Retain()                    { AtomicInc(refcount); }
		void               Release()                   { if(AtomicDec(refcount) == 0) delete this; }
		i32                GetRefCount() const         { return refcount; }

		virtual bool       IsNull() const              { return true; }
		virtual void       Serialize(Stream& s)        {}
		virtual void       Xmlize(XmlIO& xio)          {}
		virtual void       Jsonize(JsonIO& jio)        {}
		virtual hash_t     GetHashVal() const          { return 0; }
		virtual bool       IsEqual(const Void *p)      { return false; }
		virtual bool       IsPolyEqual(const Val& v)   { return false; }
		virtual Txt        AsTxt() const               { return ""; }
		virtual i32        Compare(const Void *p)      { return 0; }
		virtual i32        PolyCompare(const Val& p)   { return 0; }

		Void()                                         { refcount = 1; }
		virtual ~Void()                                {}

		friend class Val;
	};

	struct Sval {
		bool       (*IsNull)(ukk p);
		void       (*Serialize)(uk p, Stream& s);
		void       (*Xmlize)(uk p, XmlIO& xio);
		void       (*Jsonize)(uk p, JsonIO& jio);
		hash_t     (*GetHashVal)(ukk p);
		bool       (*IsEqual)(ukk p1, ukk p2);
		bool       (*IsPolyEqual)(ukk p, const Val& v);
		Txt        (*AsTxt)(ukk p);
		i32        (*Compare)(ukk p1, ukk p2);
		i32        (*PolyCompare)(ukk p1, const Val& p2);
	};

protected:
	enum { TXT = 0, REF = 255, VOIDV = 3 };

	static VecMap<u32, Void* (*)()>& Typemap();
	static Sval *svo[256];
	static Index<Txt>& NameNdx();
	static Index<u32>&  TypeNdx();

	static void   AddName(u32 type, tukk name);
	static i32    GetType(tukk name);
	static Txt    GetName(u32 type);
	static void   RegisterStd();

	friend void ValRegisterHelper();

	Txt      data;
	Void    *&ptr()                    { ASSERT(IsRef()); return *(Void **)&data; }
	Void     *ptr() const              { ASSERT(IsRef()); return *(Void **)&data; }

	void     SetRefType(u32 type)      { ASSERT(IsRef()); ((i32 *)&data)[2] = type; }
	u32      GetRefType() const        { ASSERT(IsRef()); return ((i32 *)&data)[2]; }

	bool     IsTxt() const             { return !data.IsSpecial(); }
	bool     Is(u8 v) const            { return data.IsSpecial(v); }
	bool     IsRef() const             { return Is(REF); }
	void     InitRef(Void *p, u32 t) { data.SetSpecial(REF); ptr() = p; SetRefType(t); }
	void     RefRelease();
	void     RefRetain();
	void     FreeRef()                 { if(IsRef()) RefRelease(); }
	void     Free()                    { FreeRef(); data.Clear(); }
	void     SetLarge(const Val& v);

	template <class T>
	void     InitSmall(const T& init);
	template <class T>
	T&       GetSmallRaw() const;
	template <class T>
	T&       GetSmall() const;

	i32      GetOtherInt() const;
	z64      GetOtherInt64() const;
	double   GetOtherDouble() const;
	bool     GetOtherBool() const;
	Date     GetOtherDate() const;
	Time     GetOtherTime() const;
	Txt      GetOtherTxt() const;
	hash_t   GetOtherHashVal() const;

	bool     IsPolyEqual(const Val& v) const;

	enum VSMALL { SMALL };

	template <class T>
	Val(const T& value, VSMALL);

	template <class T> friend Val SvoToVal(const T& x);

	Txt     GetName() const;

	i32     PolyCompare(const Val& v) const;
	i32     Compare2(const Val& v) const;

	Vec<Val>&  UnShareArray();

	const Vec<Val>& GetVA() const;

#if defined(_DEBUG) && defined(COMPILER_GCC)
	uint32  magic[4];
	void    Magic()               { magic[0] = 0xc436d851; magic[1] = 0x72f67c76; magic[2] = 0x3e5e10fd; magic[3] = 0xc90d370b; }
	void    ClearMagic()          { magic[0] = magic[1] = magic[2] = magic[3] = 0; }
#else
	void    Magic()               {}
	void    ClearMagic()          {}
#endif

#ifndef DEPRECATED
	static  void Register(u32 w, Void* (*c)(), tukk name = NULL);
#endif

	Val(Void *p) = delete;

public:
	template <class T>
	static  void Register(tukk name = NULL);
	template <class T>
	static  void SvoRegister(tukk name = NULL);

	u32      GetType() const;
	bool     IsError() const         { return GetType() == ERROR_V; }
	bool     IsVoid() const          { return Is(VOIDV) || IsError(); }
	bool     IsNull() const;

	template <class T>
	bool     Is() const;
	template <class T>
	const T& To() const;
	template <class T>
	const T& Get() const;

	operator Txt() const             { return IsTxt() ? data : GetOtherTxt(); }
	operator WTxt() const;
	operator Date() const            { return Is(DATE_V) ? GetSmallRaw<Date>() : GetOtherDate(); }
	operator Time() const            { return Is(TIME_V) ? GetSmallRaw<Time>() : GetOtherTime(); }
	operator double() const          { return Is(DOUBLE_V) ? GetSmallRaw<double>() : GetOtherDouble(); }
	operator float() const           { return float(Is(DOUBLE_V) ? GetSmallRaw<double>() : GetOtherDouble()); }
	operator i32() const             { return Is(INT_V) ? GetSmallRaw<i32>() : GetOtherInt(); }
	operator i64() const             { return Is(INT64_V) ? GetSmallRaw<i64>() : GetOtherInt64(); }
	operator z64() const             { return Is(INT64_V2) ? GetSmallRaw<z64>() : GetOtherInt64(); }
	operator bool() const            { return Is(BOOL_V) ? GetSmallRaw<bool>() : GetOtherBool(); }
	std::string  ToStd() const       { return operator Txt().ToStd(); }
	std::wstring ToWStd() const      { return operator WTxt().ToStd(); }

	Val(const Txt& s) : data(s) { Magic(); }
	Val(const WTxt& s);
	Val(tukk s) : data(s)   { Magic(); }
	Val(i32 i)                     : data(i, INT_V, Txt::SPECIAL) { Magic(); }
	Val(i64 i)                     : data(i, INT64_V, Txt::SPECIAL) { Magic(); }
	Val(z64 i)                     : data(i, INT64_V2, Txt::SPECIAL) { Magic(); }
	Val(double d)                  : data(d, DOUBLE_V, Txt::SPECIAL) { Magic(); }
	Val(bool b)                    : data(b, BOOL_V, Txt::SPECIAL) { Magic(); }
	Val(Date d)                    : data(d, DATE_V, Txt::SPECIAL) { Magic(); }
	Val(Time t)                    : data(t, TIME_V, Txt::SPECIAL) { Magic(); }
	Val(const Nuller&)             : data((i32)Null, INT_V, Txt::SPECIAL) { Magic(); }
	Val(const std::string& s) : Val(Txt(s)) {}
	Val(const std::wstring& s) : Val(WTxt(s)) {}

	bool operator==(const Val& v) const;
	bool operator!=(const Val& v) const { return !operator==(v); }
	bool IsSame(const Val& v) const;

	i32  Compare(const Val& v) const;
	bool operator<=(const Val& x) const { return Compare(x) <= 0; }
	bool operator>=(const Val& x) const { return Compare(x) >= 0; }
	bool operator<(const Val& x) const  { return Compare(x) < 0; }
	bool operator>(const Val& x) const  { return Compare(x) > 0; }

	Txt ToTxt() const;
	Txt operator ~() const              { return ToTxt(); }
	Txt GetTypeName() const             { return GetName(); }

	void  Serialize(Stream& s);
	void  Xmlize(XmlIO& xio);
	void  Jsonize(JsonIO& jio);

	hash_t GetHashVal() const;

	Val& operator=(const Val& v);
	Val(const Val& v);

	i32   GetCount() const;
	const Val& operator[](i32 i) const;
	const Val& operator[](const Txt& key) const;
	const Val& operator[](tukk key) const;
	const Val& operator[](const Id& key) const;

	Val& At(i32 i);
	Val& operator()(i32 i)              { return At(i); }
	void   Add(const Val& src);
	template <typename T>
	Val& operator<<(const T& src)       { Add(src); return *this; }

	Val& GetAdd(const Val& key);
	Val& operator()(const Txt& key);
	Val& operator()(tukk key);
	Val& operator()(const Id& key);

	Val()                               : data((i32)Null, VOIDV, Txt::SPECIAL) { Magic(); }
	~Val()                              { ClearMagic(); if(IsRef()) RefRelease(); }

	Val(Void *p, u32 type)            { InitRef(p, type); Magic(); }
	const Void *GetVoidPtr() const      { ASSERT(IsRef()); return ptr(); }

	friend void Swap(Val& a, Val& b)    { Swap(a.data, b.data); }

	typedef ConstIteratorOf<Vec<Val>> const_iterator;

	const_iterator         begin() const                      { return GetVA().begin(); }
	const_iterator         end() const                        { return GetVA().end(); }

#ifdef DEPRECATED
	typedef Val            value_type;
	typedef const_iterator ConstIterator;
	typedef const Val&     const_reference;
	typedef i32            size_type;
	typedef i32            difference_type;

	static  void Register(u32 w, Void* (*c)(), tukk name = NULL);
#endif
};

struct ValTypeError : Exc {
	Val   src;
	i32   target;

	ValTypeError(const Txt& text, const Val& src, i32 target);
};

template <class T> bool  FitsSvoVal()                    { return sizeof(T) <= 8; }
template <class T> Val   SvoToVal(const T& x)            { return Val(x, Val::SMALL); }

template <class T> Val RichToVal(const T& data);

template <class T> Val RawToVal(const T& data);
template <class T> Val RawPickToVal(T&& data);
template <class T> Val RawDeepToVal(const T& data);
template <class T> T&  CreateRawVal(Val& v);

Val        ErrorVal(tukk s);
Val        ErrorVal(const Txt& s);
const Val& ErrorVal();

template <class T>
inline bool IsPolyEqual(const T& x, const Val& v) {
	return false;
}

template <class T>
inline hash_t ValGetHashVal(const T& x) {
	return DRX::GetHashVal(x);
}

#define VALUE_COMPARE_V(T, VT) \
inline bool operator==(const Val& v, T x)   { return v.Is<VT>() ? (VT)v == x : v == Val(x); } \
inline bool operator==(T x, const Val& v)   { return v.Is<VT>() ? (VT)v == x : v == Val(x); } \
inline bool operator!=(const Val& v, T x)   { return v.Is<VT>() ? (VT)v != x : v != Val(x); } \
inline bool operator!=(T x, const Val& v)   { return v.Is<VT>() ? (VT)v != x : v != Val(x); } \

#define VALUE_COMPARE(T) VALUE_COMPARE_V(T, T)

VALUE_COMPARE(i32)
VALUE_COMPARE(z64)
VALUE_COMPARE(i64)
VALUE_COMPARE(double)
VALUE_COMPARE(bool)
VALUE_COMPARE(Date)
VALUE_COMPARE(Time)
VALUE_COMPARE(Txt)
VALUE_COMPARE(WTxt)
VALUE_COMPARE_V(tukk , Txt)
VALUE_COMPARE_V(wtukk , WTxt)

inline bool IsVoidValTypeNo(i32 q)     { return (u32)q == VOID_V; }
inline bool IsErrorValTypeNo(i32 q)    { return (u32)q == ERROR_V; }
inline bool IsTxtValTypeNo(i32 q)      { return (u32)q == TXT_V || (u32)q == WTXT_V; }

inline bool IsIntegerValTypeNo(i32 q)  { return (u32)q == INT_V || (u32)q == INT64_V || (u32)q == INT64_V2 || (u32)q == BOOL_V; }
inline bool IsFloatValTypeNo(i32 q)    { return (u32)q == DOUBLE_V; }

inline bool IsNumberValTypeNo(i32 q)   { return IsIntegerValTypeNo(q) || IsFloatValTypeNo(q); }
inline bool IsDateTimeValTypeNo(i32 q) { return (u32)q == DATE_V || (u32)q == TIME_V; }

inline bool IsVoid(const Val& v)       { return v.IsVoid(); }
inline bool IsError(const Val& v)      { return v.IsError(); }
inline bool IsTxt(const Val& v)        { return v.Is<Txt>() || v.Is<WTxt>(); }
inline bool IsNumber(const Val& v)     { return v.Is<double>() || v.Is<i32>() || v.Is<z64>() || v.Is<bool>(); }
inline bool IsDateTime(const Val& v)   { return v.Is<Date>() || v.Is<Time>(); }
inline bool IsValArray(const Val& v)   { return v.GetType() == VALUEARRAY_V || v.GetType() == VALUEMAP_V; }
inline bool IsValMap(const Val& v)     { return IsValArray(v); }

Txt         GetErrorText(const Val& v);

inline bool          IsNull(const Val& v)               { return v.IsNull(); }
inline const Val&    Nvl(const Val& a, const Val& b)    { return IsNull(a) ? b : a; }

inline bool          IsNaN(const Val& v)                { return v.Is<double>() && IsNaN((double)v); }