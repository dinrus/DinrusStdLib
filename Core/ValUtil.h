Val Scan(dword stdtype, const Txt& text, const Val& def = Null, bool *hastime = NULL);

inline const Txt&     Nvl(const Txt& a, const Txt& b)    { return IsNull(a) ? b : a; }
inline i32            Nvl(i32 a, i32 b)                        { return IsNull(a) ? b : a; }
inline i64            Nvl(i64 a, i64 b)                        { return IsNull(a) ? b : a; }
inline z64            Nvl(z64 a, z64 b)                        { return IsNull(a) ? b : a; }
inline double         Nvl(double a, double b)                  { return IsNull(a) ? b : a; }
inline Date           Nvl(Date a, Date b)                      { return IsNull(a) ? b : a; }
inline Time           Nvl(Time a, Time b)                      { return IsNull(a) ? b : a; }

inline i32            Nvl(i32 a)                               { return Nvl(a, 0); }
inline i64            Nvl(i64 a)                               { return Nvl(a, (i64)0); }
inline z64            Nvl(z64 a)                               { return Nvl(a, (z64)0); }
inline double         Nvl(double a)                            { return Nvl(a, 0.0); }

template <class T>
inline T              Nvl(T a, T b, T c)                       { return Nvl(Nvl(a, b), c); }

template <class T>
inline T              Nvl(T a, T b, T c, T d)                  { return Nvl(Nvl(a, b), c, d); }

template <class T>
inline T              Nvl(T a, T b, T c, T d, T e)             { return Nvl(Nvl(a, b), c, d, e); }

i32 StdValCompare(const Val& a, const Val& b, const LangInfo& f);
i32 StdValCompare(const Val& a, const Val& b, i32 language);
i32 StdValCompare(const Val& a, const Val& b);

i32 StdValCompareDesc(const Val& a, const Val& b, const LangInfo& f);
i32 StdValCompareDesc(const Val& a, const Val& b, i32 language);
i32 StdValCompareDesc(const Val& a, const Val& b);

struct ValOrder {
	virtual bool operator()(const Val& a, const Val& b) const = 0;
	virtual ~ValOrder() {}
};

struct StdValOrder : ValOrder {
	i32 language;

	virtual bool operator()(const Val& a, const Val& b) const { return StdValCompare(a, b, language) < 0; }

	StdValOrder(i32 l = -1) : language(l) {}
};

struct FnValOrder : ValOrder {
	i32 (*fn)(const Val& a, const Val& b);

	virtual bool operator()(const Val& a, const Val& b) const { return (*fn)(a, b) < 0; }

	FnValOrder(i32 (*fn)(const Val& a, const Val& b)) : fn(fn) {}
};

struct ValPairOrder {
	virtual bool operator()(const Val& keya, const Val& valuea, const Val& keyb, const Val& valueb) const = 0;
	virtual ~ValPairOrder() {}
};

struct StdValPairOrder : ValPairOrder {
	i32 language;

	virtual bool operator()(const Val& keya, const Val& valuea, const Val& keyb, const Val& valueb) const;

	StdValPairOrder(i32 l = -1);
};

struct FnValPairOrder : ValPairOrder {
	i32 (*fn)(const Val& k1, const Val& v1, const Val& k2, const Val& v2);

	virtual bool operator()(const Val& keya, const Val& valuea, const Val& keyb, const Val& valueb) const;

	FnValPairOrder(i32 (*fn)(const Val& k1, const Val& v1, const Val& k2, const Val& v2)) : fn(fn) {}
};

i32 CompareTxts(const Val& a, const Val& b, const LangInfo& f); // used by StdCompareVal

class Id : Moveable<Id> {
	Txt id;

public:
	const Txt&     ToTxt() const                    { return id; }
	hash_t         GetHashVal() const            { return DRX::GetHashVal(id); }
	bool           IsNull() const                { return DRX::IsNull(id); }

	operator const Txt&() const                  { return ToTxt(); }
	const Txt&     operator~() const             { return ToTxt(); }
	bool           operator==(const Id& b) const { return id == b.id; }
	bool           operator==(const Txt& b) const { return id == b; }
	bool           operator==(tukk b) const { return id == b; }
	bool           operator!=(const Id& b) const { return id != b.id; }
	operator bool() const                        { return id.GetCount(); }

	Id()                                         {}
	Id(const Txt& s)                             { id = s; }
	Id(tukk s)                                   { id = s; }
};

struct RefManager {
	virtual i32   GetType() = 0;
	virtual Val   GetVal(ukk )              { return Null; }
	virtual bool  IsNull(ukk )              { return false; }
	virtual void  SetVal(uk , const Val& v) { NEVER(); }
	virtual void  SetNull(uk )              { NEVER(); }
	virtual ~RefManager() {}
};

template <class T>
struct StdRef : public RefManager {
	virtual void  SetVal(uk p, const Val& v) { *(T *) p = (T)v; }
	virtual Val   GetVal(ukk p)           { return *(const T *) p; }
	virtual i32   GetType()                         { return GetValTypeNo<T>(); }
	virtual bool  IsNull(ukk p)             { return DRX::IsNull(*(T *) p); }
	virtual void  SetNull(uk p)                  { DRX::SetNull(*(T *)p); }
	virtual ~StdRef() {}
};

class Ref : Moveable<Ref> {
protected:
	RefManager *m;
	void       *ptr;
	struct      ValRef;

public:
	dword    GetType() const           { return m ? m->GetType() : VOID_V; }
	bool     IsNull() const            { return m ? m->IsNull(ptr) : true; }

	void       *GetVoidPtr() const     { return ptr; }
	RefManager *GetManager() const     { return m; }

	template <class T>
	bool     Is() const                { return GetType() == GetValTypeNo<T>(); } // VALUE_V!!!
	template <class T>
	T&       Get() const               { ASSERT(GetValTypeNo<T>() == GetType()); return *(T *)GetVoidPtr(); }

	void     SetNull()                 { if(m) m->SetNull(ptr); }
	Val      GetVal() const            { return m ? m->GetVal(ptr) : Val(); }
	void     SetVal(const Val& v)  { ASSERT(m); m->SetVal(ptr, v); }

	operator Val() const             { return GetVal(); }
	Val      operator~() const         { return GetVal(); }
	Ref&     operator=(const Val& v) { SetVal(v); return *this; }

	Ref(Txt& s);
	Ref(WTxt& s);
	Ref(i32& i);
	Ref(z64& i);
	Ref(double& d);
	Ref(bool& b);
	Ref(Date& d);
	Ref(Time& t);
	Ref(Val& v);
	Ref(uk _ptr, RefManager *_m)       { ptr = _ptr, m = _m; }
	Ref(const ValTypeRef& r);
	Ref()                              { ptr = m = NULL; }
};

template <class T>
T& GetRef(Ref r, T *x = NULL) {
	ASSERT(GetValTypeNo<T>() == r.GetType());
	return *(T *) r.GetVoidPtr();
}

inline Txt&     RefTxt(Ref f)  { return GetRef<Txt>(f); }
inline WTxt&    RefWTxt(Ref f) { return GetRef<WTxt>(f); }
inline i32&     RefInt(Ref f)     { return GetRef<i32>(f); }
inline i64&     RefInt64(Ref f)   { return GetRef<i64>(f); }
inline z64&     RefZ64(Ref f)   { return GetRef<z64>(f); }
inline double&  RefDouble(Ref f)  { return GetRef<double>(f); }
inline bool&    RefBool(Ref f)    { return GetRef<bool>(f); }
inline Date&    RefDate(Ref f)    { return GetRef<Date>(f); }
inline Time&    RefTime(Ref f)    { return GetRef<Time>(f); }
inline Val&     RefVal(Ref f)   { ASSERT(f.GetType() == VALUE_V);
                                    return *(Val *)f.GetVoidPtr(); }

template <class T>
Ref AsRef(T& x) {
	return Ref(&x, &Single< StdRef<T> >());
}

struct ValTypeRef {
	RefManager *m;
	void       *ptr;
};

inline
Ref::Ref(const ValTypeRef& r)
{
	ptr = r.ptr;
	m = r.m;
}

template <class T, dword type, class B>
ValType<T, type, B>::operator ValTypeRef()
{
	ValTypeRef h;
	h.ptr = this;
	h.m = &Single< StdRef<T> >();
	return h;
}

#define E__Val(I)   Val p##I
#define E__Ref(I)     Ref p##I

// ---------------------- Val Array

class ValArray : public ValType<ValArray, VALUEARRAY_V, Moveable<ValArray> > {
	struct Data : Val::Void {
		virtual bool         IsNull() const;
		virtual void         Serialize(Stream& s);
		virtual void         Xmlize(XmlIO& xio);
		virtual void         Jsonize(JsonIO& jio);
		virtual hash_t       GetHashVal() const;
		virtual bool         IsEqual(const Val::Void *p);
		virtual Txt          AsTxt() const;
		virtual i32          Compare(const Val::Void *p);

		i32 GetRefCount() const     { return refcount; }
		Vec<Val>& Clone();

		Vec<Val> data;
	};
	struct NullData : Data {};
	Data *data;

	static Vec<Val> VoidData;

	Vec<Val>& Create();
	Vec<Val>& Clone();
	void  Init0();

	friend Val::Void *ValArrayDataCreate();
	friend class Val;
	friend class ValMap;

public:
	ValArray()                              { Init0(); }
	ValArray(const ValArray& v);
	ValArray(ValArray&& v);
	ValArray(Vec<Val>&& values);
	explicit ValArray(const Vec<Val>& values, i32 deep);
	ValArray(std::initializer_list<Val> init)    { Init0(); for(const auto& i : init) { Add(i); }}
	~ValArray();

	ValArray& operator=(const ValArray& v);
	ValArray& operator=(Vec<Val>&& values)  { *this = ValArray(pick(values)); return *this; }

	operator Val() const;
	ValArray(const Val& src);

	ValArray(const Nuller&)                 { Init0(); }
	bool IsNullInstance() const               { return IsEmpty(); }

	void Clear();
	void SetCount(i32 n);
	void SetCount(i32 n, const Val& v);
	i32  GetCount() const                     { return data->data.GetCount(); }
	bool IsEmpty() const                      { return data->data.IsEmpty(); }

	void Add(const Val& v);
	ValArray& operator<<(const Val& v)    { Add(v); return *this; }
	void Set(i32 i, const Val& v);
	const Val& Get(i32 i) const;
	Val GetAndClear(i32 i);
	const Vec<Val>& Get() const          { return data ? data->data : VoidData; }
	Vec<Val> Pick();

	void Remove(i32 i, i32 count = 1);
	void Remove(const Vec<i32>& ii);
	void Insert(i32 i, const ValArray& va);
	void Append(const ValArray& va)         { Insert(GetCount(), va); }

	const Val& operator[](i32 i) const      { return Get(i); }

	Val& At(i32 i);

	hash_t   GetHashVal() const             { return data->GetHashVal(); }
	void     Serialize(Stream& s);
	void     Jsonize(JsonIO& jio);
	void     Xmlize(XmlIO& xio);
	Txt      ToTxt() const;

	bool     operator==(const ValArray& v) const;
	bool     operator!=(const ValArray& v) const  { return !operator==(v); }

	i32      Compare(const ValArray& b) const;
	bool     operator<=(const ValArray& x) const { return Compare(x) <= 0; }
	bool     operator>=(const ValArray& x) const { return Compare(x) >= 0; }
	bool     operator<(const ValArray& x) const  { return Compare(x) < 0; }
	bool     operator>(const ValArray& x) const  { return Compare(x) > 0; }

	typedef ConstIteratorOf<Vec<Val>> const_iterator;

#ifdef DEPRECATED
	typedef Val            value_type;
	typedef const_iterator ConstIterator;
	typedef const Val&     const_reference;
	typedef i32            size_type;
	typedef i32            difference_type;
#endif

	const_iterator         begin() const                      { return Get().begin(); }
	const_iterator         end() const                        { return Get().end(); }
};

template<>
Txt AsTxt(const ValArray& v);

class ValMap : public ValType<ValMap, VALUEMAP_V, Moveable<ValMap> >{
	struct Data : Val::Void {
		virtual bool       IsNull() const;
		virtual void       Serialize(Stream& s);
		virtual void       Xmlize(XmlIO& xio);
		virtual void       Jsonize(JsonIO& jio);
		virtual hash_t     GetHashVal() const;
		virtual bool       IsEqual(const Val::Void *p);
		virtual Txt        AsTxt() const;
		virtual i32        Compare(const Val::Void *p);

		const Val& Get(const Val& k) const {
			i32 q = key.Find(k);
			return q >= 0 ? value[q] : ErrorVal();
		}
		Val& GetAdd(const Val& k) {
			i32 i = key.Find(k);
			if(i < 0) {
				i = value.GetCount();
				key.Add(k);
			}
			return value.At(i);
		}
		Val& At(i32 i) {
			ASSERT(i < value.GetCount());
			return value.At(i);
		}

		Index<Val> key;
		ValArray   value;
	};

	struct NullData : Data {};
	Data *data;

	Data& Create();
	static void Clone(Data *&ptr);
	force_inline
	static ValMap::Data& UnShare(ValMap::Data *&ptr) { if(ptr->GetRefCount() != 1) Clone(ptr); return *ptr; }
	Data& UnShare() { return UnShare(data); }
	void  Init0();
	void  FromArray(const ValArray& va);

	friend Val::Void *ValMapDataCreate();
	friend class Val;

public:
	ValMap()                                    { Init0(); }
	ValMap(const ValMap& v);
	ValMap(const ValArray& va)                  { FromArray(va); }
	ValMap(Index<Val>&& k, Vec<Val>&& v);
	ValMap(VecMap<Val, Val>&& m);
	ValMap(const Index<Val>& k, const Vec<Val>& v, i32 deep);
	ValMap(const VecMap<Val, Val>& m, i32 deep);
	ValMap(std::initializer_list<std::pair<Val, Val>> init) { Init0(); for(const auto& i : init) { Add(i.first, i.second); }}
	~ValMap();

	ValMap& operator=(const ValMap& v);
	ValMap& operator=(VecMap<Val, Val>&& m) { *this = ValMap(pick(m)); return *this; }

	operator Val() const;
	ValMap(const Val& src);

	ValMap(const Nuller&)                           { Init0(); }
	bool IsNullInstance() const                     { return IsEmpty(); }

	void Clear();
	i32  GetCount() const                           { return data->value.GetCount(); }
	bool IsEmpty() const                            { return data->value.IsEmpty(); }
	const Val& GetKey(i32 i) const                { return data->key[i]; }
	const Val& GetVal(i32 i) const              { return data->value[i]; }
	i32  Find(const Val& key) const               { return data ? data->key.Find(key) : -1; }
	i32  FindNext(i32 ii) const                     { return data ? data->key.FindNext(ii) : -1; }

	void Add(const Val& key, const Val& value);
	void Add(const Txt& key, const Val& value) { Add(Val(key), value); }
	void Add(tukk key, const Val& value)   { Add(Val(key), value); }
	void Add(i32 key, const Val& value)           { Add(Val(key), value); }
	void Add(Id key, const Val& value)            { Add(Val(key.ToTxt()), value); }

	ValMap& operator()(const Val& key, const Val& value)  { Add(key, value); return *this; }
	ValMap& operator()(const Txt& key, const Val& value) { Add(Val(key), value); return *this; }
	ValMap& operator()(tukk key, const Val& value)   { Add(Val(key), value); return *this; }
	ValMap& operator()(i32 key, const Val& value)           { Add(Val(key), value); return *this; }
	ValMap& operator()(Id key, const Val& value)            { Add(Val(key.ToTxt()), value); return *this; }

	void Set(const Val& key, const Val& value);
	void Set(const Txt& key, const Val& value) { Set(Val(key), value); }
	void Set(tukk key, const Val& value)   { Set(Val(key), value); }
	void Set(i32 key, const Val& value)           { Set(Val(key), value); }
	void Set(Id key, const Val& value)            { Set(Val(key.ToTxt()), value); }

	void SetAt(i32 i, const Val& v);
	void SetKey(i32 i, const Val& key);
	void SetKey(i32 i, const Txt& key)           { SetKey(i, Val(key)); }
	void SetKey(i32 i, tukk key)             { SetKey(i, Val(key)); }
	void SetKey(i32 i, i32 key)                     { SetKey(i, Val(key)); }
	void SetKey(i32 i, Id key)                      { SetKey(i, Val(key.ToTxt())); }

	i32  RemoveKey(const Val& key);
	i32  RemoveKey(const Txt& key)           { return RemoveKey(Val(key)); }
	i32  RemoveKey(tukk key)                 { return RemoveKey(Val(key)); }
	i32  RemoveKey(i32 key)                  { return RemoveKey(Val(key)); }
	i32  RemoveKey(Id key)                   { return RemoveKey(Val(key.ToTxt())); }
	void Remove(i32 i);

	const Index<Val>& GetKeys() const           { return data->key; }
	ValArray GetVals() const                    { return data->value; }

	operator ValArray() const                   { return GetVals(); }

	VecMap<Val, Val> Pick();

	const Val& operator[](const Val& key) const  { return data->Get(key); }
	const Val& operator[](const Txt& key) const { return operator[](Val(key)); }
	const Val& operator[](tukk key) const   { return operator[](Val(key)); }
	const Val& operator[](i32k key) const     { return operator[](Val(key)); }
	const Val& operator[](const Id& key) const     { return operator[](Val(key.ToTxt())); }

	Val& GetAdd(const Val& key)                  { return UnShare().GetAdd(key); }
	Val& operator()(const Val& key)              { return GetAdd(key); }
	Val& operator()(const Txt& key)             { return operator()(Val(key)); }
	Val& operator()(tukk key)               { return operator()(Val(key)); }
	Val& operator()(i32k key)                 { return operator()(Val(key)); }
	Val& operator()(const Id& key)                 { return operator()(Val(key.ToTxt())); }
	Val& At(i32 i)                                 { return UnShare().At(i); }

	Val GetAndClear(const Val& key);

	hash_t   GetHashVal() const                   { return data->GetHashVal(); }
	void     Serialize(Stream& s);
	void     Jsonize(JsonIO& jio);
	void     Xmlize(XmlIO& xio);
	Txt      ToTxt() const                       { return data->AsTxt(); }

	bool operator==(const ValMap& v) const;
	bool operator!=(const ValMap& v) const        { return !operator==(v); }

	i32      Compare(const ValMap& b) const;
	bool     operator<=(const ValMap& x) const    { return Compare(x) <= 0; }
	bool     operator>=(const ValMap& x) const    { return Compare(x) >= 0; }
	bool     operator<(const ValMap& x) const     { return Compare(x) < 0; }
	bool     operator>(const ValMap& x) const     { return Compare(x) > 0; }

	bool     IsSame(const ValMap& b) const;
};

class ValGen {
public:
	virtual Val  Get() = 0;
	Val operator++()  { return Get(); }
	virtual ~ValGen() {}
};


#include "Val.hpp"
#include "ValUtil.hpp"