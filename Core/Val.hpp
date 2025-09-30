inline
Val::Val(const Val& v)
:	data(Txt::SPECIAL)
{
	if(v.IsRef() || v.data.IsLarge())
		SetLarge(v);
	else
		data.SetSmall(v.data);
	Magic();
}

template<>
inline bool IsPolyEqual(const bool& x, const Val& v) {
	return (v.Is<double>() && i32(x) == double(v))
	    || (v.Is<z64>() && i32(x) == z64(v))
	    || (v.Is<i64>() && i32(x) == i64(v))
	    || (v.Is<i32>() && i32(x) == i32(v));
}

template<>
inline bool IsPolyEqual(i32k& x, const Val& v) {
	return (v.Is<double>() && x == double(v))
	    || (v.Is<z64>() && x == z64(v))
	    || (v.Is<i64>() && x == i64(v));
}

template<>
inline bool IsPolyEqual(const z64& x, const Val& v) {
	return v.Is<double>() && double(x) == double(v);
}

template<>
inline bool IsPolyEqual(const Date& x, const Val& v) {
	return v.Is<Time>() && ToTime(x) == Time(v);
}

template<>
inline bool IsPolyEqual(const WTxt& x, const Val& v) {
	return v.GetType() == TXT_V && WTxt(v) == x;
}

template<class T>
inline i32  PolyCompare(const T& a, const Val& b)
{
	return a.PolyCompare(b);
}

template<>
inline i32 PolyCompare(const WTxt& x, const Val& v) {
	return IsTxt(v) && SgnCompare(x, WTxt(v));
}

template<>
inline i32 PolyCompare(const Txt& x, const Val& v) {
	return IsTxt(v) && SgnCompare((WTxt)x, (WTxt)v);
}

template<>
inline i32 PolyCompare(const bool& x, const Val& v) {
	return v.Is<i64>() ? SgnCompare((i64)x, (i64)v)
	     : v.Is<z64>() ? SgnCompare((z64)x, (z64)v)
	     : IsNumber(v) ? SgnCompare((double)x, (double)v)
	     : 0;
}

template<>
inline i32 PolyCompare(i32k& x, const Val& v) {
	return v.Is<i64>() ? SgnCompare((i64)x, (i64)v)
	     : v.Is<z64>() ? SgnCompare((z64)x, (z64)v)
	     : IsNumber(v) ? SgnCompare((double)x, (double)v)
	     : 0;
}

template<>
inline i32 PolyCompare(const z64& x, const Val& v) {
	return v.Is<double>() ? SgnCompare((double)x, (double)v)
	     : IsNumber(v) ? SgnCompare((i64)x, (i64)v)
	     : IsNumber(v) ? SgnCompare((z64)x, (z64)v)
	     : 0;
}

template<>
inline i32 PolyCompare(const double& x, const Val& v) {
	return IsNumber(v) ? SgnCompare((double)x, (double)v) : 0;
}

template<>
inline i32 PolyCompare(const Date& x, const Val& v) {
	return v.Is<Time>() && SgnCompare(ToTime(x), Time(v));
}

template<>
inline i32 PolyCompare(const Time& x, const Val& v) {
	return v.Is<Date>() && SgnCompare(ToTime(x), Time(v));
}

template<>
inline hash_t ValGetHashVal(const bool& x) {
	return DRX::GetHashVal((z64)x);
}

template<>
inline hash_t ValGetHashVal(i32k& x) {
	return DRX::GetHashVal((z64)x);
}

template<>
inline hash_t ValGetHashVal(const double& x) {
	if(x >= (double)INT64_MIN && x <= (double)INT64_MAX && (z64)x == x)
		return DRX::GetHashVal((z64)x); // we want this to be equal to other number types
	return DRX::GetHashVal(x);
}

template<>
inline hash_t ValGetHashVal(const Date& x) {
	return DRX::GetHashVal(ToTime(x));
}

template<>
inline hash_t ValGetHashVal(const WTxt& x) {
	return DRX::GetHashVal(x.ToTxt());
}

template <class T>
class RawValRep : public Val::Void {
public:
	virtual bool  IsNull() const              { return false; }

	T v;

	enum VPICK { PICK };
	enum VDEEP { DEEP };
	
	const T& Get() const                      { return v; }
	T&       Get()                            { return v; }
	
	RawValRep(const T& v) : v(v)             {}
	RawValRep(T&& v, VPICK) : v(pick(v))     {}
	RawValRep(const T& v, VDEEP) : v(v, 1)   {}
	RawValRep()                              {}
};

template <class T>
class RichValRep : public RawValRep<T> {
public:
	virtual bool       IsNull() const                { return DRX::IsNull(this->v); }
	virtual void       Serialize(Stream& s)          { s % this->v; }
	virtual void       Xmlize(XmlIO& xio)            { drx::Xmlize(xio, this->v); }
	virtual void       Jsonize(JsonIO& jio)          { drx::Jsonize(jio, this->v); }
	virtual hash_t     GetHashVal() const          { return DRX::ValGetHashVal(this->v); }
	virtual bool       IsEqual(const Val::Void *p) { ASSERT(dynamic_cast<const RawValRep<T> *>(p));
	                                                   return static_cast<const RawValRep<T> *>(p)->Get() == this->v; }
	virtual bool       IsPolyEqual(const Val& b)   { return DRX::IsPolyEqual(this->v, b); }
	virtual Txt     AsTxt() const              { return DRX::AsTxt(this->v); }
	virtual i32        Compare(const Val::Void *p) { ASSERT(dynamic_cast<const RawValRep<T> *>(p));
	                                                   return SgnCompare(this->v, static_cast<const RawValRep<T> *>(p)->Get()); }
	virtual i32        PolyCompare(const Val& b)   { return drx::PolyCompare(this->v, b); }

	RichValRep(const T& v) : RawValRep<T>(v)     {}
	RichValRep()                                   {}

	static Val::Void *Create()                     { return new RichValRep; }
};

template <class T>
struct SvoFn {
	static bool       IsNull(ukk p)                      { return DRX::IsNull(*(T *)p); }
	static void       Serialize(uk p, Stream& s)              { s % *(T*)p; }
	static void       Xmlize(uk p, XmlIO& xio)                { drx::Xmlize(xio, *(T*)p); }
	static void       Jsonize(uk p, JsonIO& jio)              { drx::Jsonize(jio, *(T*)p); }
	static hash_t     GetHashVal(ukk p)                { return DRX::ValGetHashVal(*(T*)p); }
	static bool       IsEqual(ukk p1, ukk p2)    { return *(T*)p1 == *(T*)p2; }
	static bool       IsPolyEqual(ukk p, const Val& v) { return DRX::IsPolyEqual(*(T*)p, v); }
	static Txt     AsTxt(ukk p)                    { return DRX::AsTxt(*(T*)p); }
	static i32        Compare(ukk p1, ukk p2)    { return SgnCompare(*(T*)p1, *(T*)p2); }
	static i32        PolyCompare(ukk p1, const Val& p2) { return DRX::PolyCompare(*(T*)p1, p2); }
};

#define SVO_FN(id, T) \
	static Val::Sval id = { \
		SvoFn<T>::IsNull, SvoFn<T>::Serialize, SvoFn<T>::Xmlize, SvoFn<T>::Jsonize, \
		SvoFn<T>::GetHashVal, SvoFn<T>::IsEqual, \
		SvoFn<T>::IsPolyEqual, SvoFn<T>::AsTxt, \
		SvoFn<T>::Compare, SvoFn<T>::PolyCompare \
	};

template <class T>
void Val::InitSmall(const T& init)
{
	ASSERT(sizeof(T) <= 8);
	SVO_FN(sval, T)
	i32 typeno = GetValTypeNo<T>();
	ASSERT(typeno >= 0 && typeno < 256);
	svo[typeno] = &sval;
	data.SetSpecial(typeno);
	new(&data) T(init);
}

template <class T>
inline T& Val::GetSmallRaw() const
{
	return *(T*)&data; // Silence compiler warning
}

template <class T>
T& Val::GetSmall() const
{
	dword t = GetValTypeNo<T>();
	ASSERT(t < 255);
	if(Is((u8)t))
		return GetSmallRaw<T>();
	Txt h = Txt() << "Неправильное преобразование значения: "
	                    << GetName() << " -> " << typeid(T).name();
	throw ValTypeError(h, *this, t);
	return *(T*)&data; // Silence compiler warning
}

template <class T>
void Val::SvoRegister(tukk name)
{
	dword t = GetValTypeNo<T>();
	ASSERT(t < 255);
	SVO_FN(sval, T)
	svo[t] = &sval;
	AddName(t, name);
}

template <class T>
inline const T& Val::To() const
{
	dword t = GetValTypeNo<T>();
	ASSERT(t != VALUEARRAY_V && t != VALUEMAP_V);
#ifndef _DEBUG
	if(t == VALUEARRAY_V) {
		ASSERT(ptr()->GetType() == VALUEARRAY_V);
		return *(T*)this; // Illegal, but works -> better than crash in release mode
	}
	if(t == VALUEMAP_V) {
		ASSERT(ptr()->GetType() == VALUEMAP_V);
		return *(T*)this; // Illegal, but works -> better than crash in release mode
	}
#endif
	if(IsRef()) {
		const RawValRep<T> *x = dynamic_cast<const RawValRep<T>*>(ptr());
		if(x)
			return x->Get();
	}
	else
	if(t == TXT_V) {
		ASSERT(IsTxt());
		return *reinterpret_cast<const T*>(&data); // Only active when T is Txt
	}
	else
	if(t < 255 && Is((u8)t))
		return GetSmallRaw<T>();
	throw ValTypeError(Txt().Cat() << "Неправильное преобразование значения: "
	                     << GetName() << " -> " << typeid(T).name(),
	                     *this, t);
}

template <class T>
inline bool Val::Is() const
{
	dword t = GetValTypeNo<T>();
	if(IsRef() && GetRefType() == t)
		return true;
	if(t == TXT_V)
		return IsTxt();
	if(t == VOID_V)
		return IsVoid();
	return t < 255 && Is((u8)t);
}

inline
i32 Val::Compare(const Val& v) const
{
	if(Is(INT_V) && v.Is(INT_V))
		return SgnCompare(GetSmallRaw<i32>(), v.GetSmallRaw<i32>());
	if(Is(DOUBLE_V) && v.Is(DOUBLE_V))
		return SgnCompare(GetSmallRaw<double>(), v.GetSmallRaw<double>());
	return Compare2(v);
}

template <class T>
const T& GetStaticNull()
{
	static T *q;
	ONCELOCK {
		static T x;
		SetNull(x);
		q = &x;
	}
	return *q;
}

template <class T>
inline const T& Val::Get() const
{
#ifndef _DEBUG
	dword t = GetValTypeNo<T>();
	if(t == VALUEARRAY_V) {
		ASSERT(ptr()->GetType() == VALUEARRAY_V);
		return *(T*)this; // Illegal, but works -> better than crash in release mode
	}
	if(t == VALUEMAP_V) {
		ASSERT(ptr()->GetType() == VALUEMAP_V);
		return *(T*)this; // Illegal, but works -> better than crash in release mode
	}
#endif
	if(IsNull())
		return GetStaticNull<T>();
	return To<T>();
}

template <class T>
Val::Val(const T& x, VSMALL)
{
	InitSmall(x);
}

inline
hash_t Val::GetHashVal() const
{
	return IsTxt() ? data.GetCount() ? data.GetHashVal() : 0
	                  : GetOtherHashVal();
}

template <class T>
void Val::Register(tukk name)
{
	dword t = GetValTypeNo<T>();
	Val::Register(t, RichValRep<T>::Create, name);
}

inline
const Val& Val::operator[](tukk key) const
{
	return operator[](Txt(key));
}

inline
const Val& Val::operator[](const Id& key) const
{
	return operator[](~key);
}

template <class T>
inline Val RawToVal(const T& data)
{
	return Val(new RawValRep<T>(data), GetValTypeNo<T>());
}

template <class T>
inline Val RawPickToVal(T&& data)
{
	typedef RawValRep<T> R;
	return Val(new R(pick(data), R::PICK), GetValTypeNo<T>());
}

template <class T>
inline Val RawDeepToVal(const T& data)
{
	typedef RawValRep<T> R;
	return Val(new R(data, R::DEEP), GetValTypeNo<T>());
}

template <class T>
inline T& CreateRawVal(Val& v) {
	typedef RawValRep<T> R;
	R *r = new R;
	v = Val(r, GetValTypeNo<T>());
	return r->Get();
}

template <class T>
inline Val RichToVal(const T& data)
{
	return Val(new RichValRep<T>(data), GetValTypeNo<T>());
}

template <>
inline Val RichToVal(const Txt& data)
{
	return Val(data);
}

#ifdef DEPRECATED
template <class T> // use Val::Is
bool IsTypeRaw(const Val& value, T * = 0)                { return value.Is<T>(); }

template <class T> // use Val::Is
bool IsType(const Val& x, T* = 0)                        { return x.Is<T>(); }

template <class T>
struct RawVal : public Val { // use RawToVal and Val::To
	RawVal(const T& x) : Val(RawToVal(x))            {}
	static const T& Extract(const Val& v)                { return v.To<T>(); }
	static const T& Extract(const Val& v, const T& dflt) { return v.Is<T>() ? v.To<T>() : dflt; }
};

template <class T>
struct RichVal : public Val { // use RichToVal and Val::To
public:
	RichVal(const T& x) : Val(RichToVal<T>(x))       {}
	static void Register() init_                           { Val::Register<T>(); }
	static const T& Extract(const Val& v)                { return v.Get<T>(); }
};

template <class T> // use Val::To
inline const T& ValTo(const Val& v)                    { return v.To<T>(); }

template <class T> // use Val::To
inline const T& ValTo(const Val& v, const T& dflt)     { return v.Is<T>() ? v.To<T>() : dflt; }
#endif