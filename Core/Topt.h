template <class I>
inline void IterSwap(I a, I b) { Swap(*a, *b); }

struct EmptyClass {};

template <class T, class B = EmptyClass>
class RelOps : public B
{
public:
	friend bool operator >  (const T& a, const T& b)  { return b < a; }
	friend bool operator != (const T& a, const T& b)  { return !(a == b); }
	friend bool operator <= (const T& a, const T& b)  { return !(b < a); }
	friend bool operator >= (const T& a, const T& b)  { return !(a < b); }
};

template <class U, class V, class B = EmptyClass>
class AddOps : public B
{
public:
	friend U& operator -= (U& a, const V& b)          { a += -b; return a; }
	friend U  operator +  (const U& a, const V& b)    { U x(a); x += b; return x; }
	friend U  operator -  (const U& a, const V& b)    { U x(a); x += -b; return x; }
};

template <class T, class B = EmptyClass>
class PostfixOps : public B
{
public:
	friend T operator ++ (T& i, i32)                  { T x = i; ++i; return x; }
	friend T operator -- (T& i, i32)                  { T x = i; --i; return x; }
};

template <class T, i32 (*compare)(T a, T b), class B = EmptyClass>
class CompareRelOps : public B
{
public:
	friend bool operator <  (T a, T b) { return (*compare)(a, b) <  0; }
	friend bool operator >  (T a, T b) { return (*compare)(a, b) >  0; }
	friend bool operator == (T a, T b) { return (*compare)(a, b) == 0; }
	friend bool operator != (T a, T b) { return (*compare)(a, b) != 0; }
	friend bool operator <= (T a, T b) { return (*compare)(a, b) <= 0; }
	friend bool operator >= (T a, T b) { return (*compare)(a, b) >= 0; }
};

template <class T, class B = EmptyClass>
struct Comparable : public B
{
public:
	friend bool operator <  (const T& a, const T& b) { return a.Compare(b) < 0; }
	friend bool operator >  (const T& a, const T& b) { return a.Compare(b) > 0; }
	friend bool operator == (const T& a, const T& b) { return a.Compare(b) == 0; }
	friend bool operator != (const T& a, const T& b) { return a.Compare(b) != 0; }
	friend bool operator <= (const T& a, const T& b) { return a.Compare(b) <= 0; }
	friend bool operator >= (const T& a, const T& b) { return a.Compare(b) >= 0; }
	friend i32  SgnCompare(const T& a, const T& b)   { return a.Compare(b); }
};

template <class T>
i32 NumberCompare__(const T& a, const T& b)
{
	if(a < b) return -1;
	if(a > b) return 1;
	return 0;
}

inline i32 SgnCompare(const char& a, const char& b)                     { return NumberCompare__(a, b); }
inline i32 SgnCompare(i8k& a, i8k& b)       { return NumberCompare__(a, b); }
inline i32 SgnCompare(u8k& a, u8k& b)   { return NumberCompare__(a, b); }
inline i32 SgnCompare(const short& a, const short& b)                   { return NumberCompare__(a, b); }
inline i32 SgnCompare(u16k& a, u16k& b) { return NumberCompare__(a, b); }
inline i32 SgnCompare(i32k& a, i32k& b)                       { return NumberCompare__(a, b); }
inline i32 SgnCompare(u32k& a, u32k& b)     { return NumberCompare__(a, b); }
inline i32 SgnCompare(const long& a, const long& b)                     { return NumberCompare__(a, b); }
inline i32 SgnCompare(u64k& a, u64k& b)   { return NumberCompare__(a, b); }
inline i32 SgnCompare(const bool& a, const bool& b)                     { return NumberCompare__(a, b); }
inline i32 SgnCompare(const z64& a, const z64& b)                   { return NumberCompare__(a, b); }
inline i32 SgnCompare(zu64k& a, zu64k& b)                 { return NumberCompare__(a, b); }
inline i32 SgnCompare(const float& a, const float& b)                   { return NumberCompare__(a, b); }
inline i32 SgnCompare(const double& a, const double& b)                 { return NumberCompare__(a, b); }

template<class T>
inline i32 SgnCompare(const T& a, const T& b)
{
	return a.Compare(b);
}

struct CombineCompare {
	i32 result;

	template <class T>
	CombineCompare& operator()(const T& a, const T& b) { if(!result) result = SgnCompare(a, b); return *this; }

	operator i32() const                               { return result; }

	CombineCompare()                                   { result = 0; }

	template <class T>
	CombineCompare(const T& a, const T& b)             { result = 0; operator()(a, b); }
};

template <class T>
inline void Fill(T *dst, const T *lim, const T& x) {
	while(dst < lim)
		*dst++ = x;
}

template <class T>
inline void Copy(T *dst, const T *src, const T *lim) {
	while(src < lim)
		*dst++ = *src++;
}

template <class T>
inline void Copy(T *dst, const T *src, i32 n) {
	Copy(dst, src, src + n);
}

inline void Fill(char *t, tukk lim, const char& x)
{ memset8(t, x, size_t(lim - t)); }
inline void Copy(char *dst, tukk src, tukk lim)
{ memcpy8(dst, src, size_t((u8 *)lim - (u8 *)src)); }

inline void Fill(i8 *t, i8k *lim, i8k& x)
{ memset8(t, x, size_t(lim - t)); }
inline void Copy(i8 *dst, i8k *src, i8k *lim)
{ memcpy8(dst, src, size_t((u8 *)lim - (u8 *)src)); }

inline void Fill(u8 *t, u8k *lim, u8k& x)
{ memset8(t, x, size_t(lim - t)); }
inline void Copy(u8 *dst, u8k *src, u8k *lim)
{ memcpy8(dst, src, size_t((u8 *)lim - (u8 *)src)); }

#ifdef NO_MOVEABLE_CHECK

template <class T>
inline void AssertMoveable(T *) {}

#define MoveableTemplate(T)

template <class T, class B = EmptyClass>
class Moveable : public B
{
};

template <class T>
struct Moveable_ {
};

#define NTL_MOVEABLE(T)

#else

template <class T>
inline void AssertMoveablePtr(T, T) {}

template <class T>
inline void AssertMoveable0(T *t) { AssertMoveablePtr(&**t, *t); }
// COMPILATION ERROR HERE MEANS TYPE T WAS NOT MARKED AS Moveable

template <class T, class B = EmptyClass>
struct Moveable : public B {
	friend void AssertMoveable0(T *) {}
};

template <class T>
struct Moveable_ {
	friend void AssertMoveable0(T *) {}
};

template <class T>
inline void AssertMoveable(T *t = 0) { if(t) AssertMoveable0(t); }

#if defined(COMPILER_MSC) || defined(COMPILER_GCC) && (__GNUC__ < 4 || __GNUC_MINOR__ < 1)
	#define NTL_MOVEABLE(T) inline void AssertMoveable0(T *) {}
#else
	#define NTL_MOVEABLE(T) template<> inline void AssertMoveable<T>(T *) {}
#endif

#endif

NTL_MOVEABLE(bool)
NTL_MOVEABLE(char)
NTL_MOVEABLE(i8)
NTL_MOVEABLE(u8)
NTL_MOVEABLE(short)
NTL_MOVEABLE(unsigned short)
NTL_MOVEABLE(i32)
NTL_MOVEABLE(u32)
NTL_MOVEABLE(long)
NTL_MOVEABLE(unsigned long)
NTL_MOVEABLE(z64)
NTL_MOVEABLE(zu64)
NTL_MOVEABLE(float)
NTL_MOVEABLE(double)
NTL_MOVEABLE(uk )
NTL_MOVEABLE(ukk )

#if defined(_NATIVE_WCHAR_T_DEFINED) || defined(COMPILER_GCC)
NTL_MOVEABLE(wchar_t)
#endif

template <class T, class B = EmptyClass>
class WithClone : public B {
public:
	friend T clone(const T& src) { T c(src, 1); return c; }
};

template <class T, class B = EmptyClass>
class DeepCopyOption : public B {
public:
#ifdef DEPRECATED
	friend T& operator<<=(T& dest, const T& src)
	{ if(&dest != &src) { (&dest)->~T(); ::new(&dest) T(src, 1); } return dest; }
#endif
	friend T  clone(const T& src) { T c(src, 1); return c; }
};

template <class T, class B = EmptyClass>
class MoveableAndDeepCopyOption : public B {
	friend void AssertMoveable0(T *) {}
#ifdef DEPRECATED
	friend T& operator<<=(T& dest, const T& src)
	{ if(&dest != &src) { (&dest)->~T(); ::new(&dest) T(src, 1); } return dest; }
#endif
	friend T  clone(const T& src) { T c(src, 1); return c; }
};

template <class T>
class WithDeepCopy : public T {
public:
	WithDeepCopy(const T& a) : T(a, 1)                 {}
	WithDeepCopy(const T& a, i32) : T(a, 1)            {}

	WithDeepCopy(const WithDeepCopy& a) : T(a, 1)      {}
	WithDeepCopy& operator=(const WithDeepCopy& a)     { (T&)*this = pick(T(a, 1)); return *this; }

	WithDeepCopy(T&& a) : T(pick(a))                   {}
	WithDeepCopy& operator=(T&& a)                     { (T&)*this = pick(a); return *this; }

	WithDeepCopy(WithDeepCopy&& a) : T(pick(a))        {}
	WithDeepCopy& operator=(WithDeepCopy&& a)          { (T&)*this = pick(a); return *this; }

	WithDeepCopy()                                     {}

#ifdef CPP_20
	template <class B> bool operator==(const B& b) const { return IsEqualRange(*this, b); }
#endif
};

// compatibility hacks

#define STL_ITERATOR_COMPATIBILITY \
	typedef ptrdiff_t                        difference_type; \
    typedef std::random_access_iterator_tag  iterator_category; \
    typedef T                                value_type; \
    typedef T                                pointer; \
    typedef T                                reference; \

#define STL_BI_COMPATIBILITY(C) \
	typedef T             value_type; \
	typedef ConstIterator const_iterator; \
	typedef const T&      const_reference; \
	typedef i32           size_type; \
	typedef i32           difference_type; \
	const_iterator        Begin() const          { return begin(); } \
	const_iterator        End() const            { return end(); } \
	void                  clear()                { Clear(); } \
	size_type             size() const           { return GetCount(); } \
	typedef Iterator      iterator; \
	typedef T&            reference; \
	bool                  empty() const          { return IsEmpty(); } \
	iterator              Begin()                { return begin(); } \
	iterator              End()                  { return end(); } \

#define STL_MAP_COMPATIBILITY(C) \
	typedef T             value_type; \
	typedef ConstIterator const_iterator; \
	typedef const T&      const_reference; \
	typedef i32           size_type; \
	typedef i32           difference_type; \
	const_iterator        Begin() const          { return B::begin(); } \
	const_iterator        End() const            { return B::end(); } \
	void                  clear()                { B::Clear(); } \
	size_type             size() const           { return B::GetCount(); } \
	typedef Iterator      iterator; \
	typedef T&            reference; \
	bool                  empty() const          { return B::IsEmpty(); } \
	iterator              Begin()                { return B::begin(); } \
	iterator              End()                  { return B::end(); } \

#define STL_SORTED_MAP_COMPATIBILITY(C) \
	typedef T             value_type; \
	typedef ConstIterator const_iterator; \
	typedef const T&      const_reference; \
	typedef i32           size_type; \
	typedef i32           difference_type; \
	const_iterator        Begin() const          { return begin(); } \
	const_iterator        End() const            { return end(); } \
	void                  clear()                { B::Clear(); } \
	size_type             size() const           { return B::GetCount(); } \
	typedef Iterator      iterator; \
	typedef T&            reference; \
	bool                  empty() const          { return B::GetCount() == 0; } \
	iterator              Begin()                { return begin(); } \
	iterator              End()                  { return end(); } \

#define STL_VECTOR_COMPATIBILITY(C) \
	typedef T             value_type; \
	typedef ConstIterator const_iterator; \
	typedef const T&      const_reference; \
	typedef i32           size_type; \
	typedef i32           difference_type; \
	const_iterator        Begin() const          { return begin(); } \
	const_iterator        End() const            { return end(); } \
	void                  clear()                { Clear(); } \
	size_type             size() const           { return GetCount(); } \
	typedef Iterator      iterator; \
	typedef T&            reference; \
	iterator              Begin()                { return begin(); } \
	iterator              End()                  { return end(); } \
	reference             front()                { return (*this)[0]; } \
	const_reference       front() const          { return (*this)[0]; } \
	reference             back()                 { return Top(); } \
	const_reference       back() const           { return Top(); } \
	bool                  empty() const          { return IsEmpty(); } \
	void                  push_back(const T& x)  { Add(x); } \
	void                  pop_back()             { Drop(); } \


template <class T>
T *DeclPtr__();

template <class Range>
using ValTypeOfArray = typename std::remove_reference<decltype((*DeclPtr__<Range>())[0])>::type;

template <class V>
class ConstIIterator {
	typedef ValTypeOfArray<V> T;

	const V       *cont;
	i32            ii;
	struct NP { i32 dummy; };

public:
	const T&       operator*() const       { return (*cont)[ii]; }
	const T       *operator->() const      { return &(*cont)[ii]; }
	const T&       operator[](i32 i) const { return (*cont)[ii + i]; }

	ConstIIterator& operator++()           { ++ii; return *this; }
	ConstIIterator& operator--()           { --ii; return *this; }
	ConstIIterator  operator++(i32)        { ConstIIterator t = *this; ++ii; return t; }
	ConstIIterator  operator--(i32)        { ConstIIterator t = *this; --ii; return t; }

	ConstIIterator& operator+=(i32 d)      { ii += d; return *this; }
	ConstIIterator& operator-=(i32 d)      { ii -= d; return *this; }

	ConstIIterator  operator+(i32 d) const { return ConstIIterator(*cont, ii + d); }
	ConstIIterator  operator-(i32 d) const { return ConstIIterator(*cont, ii - d); }

	i32  operator-(const ConstIIterator& b) const   { return ii - b.ii; }

	bool operator==(const ConstIIterator& b) const  { return ii == b.ii; }
	bool operator!=(const ConstIIterator& b) const  { return ii != b.ii; }
	bool operator<(const ConstIIterator& b) const   { return ii < b.ii; }
	bool operator>(const ConstIIterator& b) const   { return ii > b.ii; }
	bool operator<=(const ConstIIterator& b) const  { return ii <= b.ii; }
	bool operator>=(const ConstIIterator& b) const  { return ii >= b.ii; }

	operator bool() const     { return ii < 0; }

	ConstIIterator()          {}
	ConstIIterator(NP *null)  { ASSERT(null == NULL); ii = -1; }
	ConstIIterator(const V& _cont, i32 ii) : cont(&_cont), ii(ii) {}

	STL_ITERATOR_COMPATIBILITY
};

template <class V>
class IIterator {
	typedef ValTypeOfArray<V> T;

	V             *cont;
	i32            ii;
	struct NP { i32 dummy; };

public:
	T&       operator*()              { return (*cont)[ii]; }
	T       *operator->()             { return &(*cont)[ii]; }
	T&       operator[](i32 i)        { return (*cont)[ii + i]; }

	const T& operator*() const        { return (*cont)[ii]; }
	const T *operator->() const       { return &(*cont)[ii]; }
	const T& operator[](i32 i) const  { return (*cont)[ii + i]; }

	IIterator& operator++()           { ++ii; return *this; }
	IIterator& operator--()           { --ii; return *this; }
	IIterator  operator++(i32)        { IIterator t = *this; ++ii; return t; }
	IIterator  operator--(i32)        { IIterator t = *this; --ii; return t; }

	IIterator& operator+=(i32 d)      { ii += d; return *this; }
	IIterator& operator-=(i32 d)      { ii -= d; return *this; }

	IIterator  operator+(i32 d) const { return IIterator(*cont, ii + d); }
	IIterator  operator-(i32 d) const { return IIterator(*cont, ii - d); }

	i32  operator-(const IIterator& b) const   { return ii - b.ii; }

	bool operator==(const IIterator& b) const  { return ii == b.ii; }
	bool operator!=(const IIterator& b) const  { return ii != b.ii; }
	bool operator<(const IIterator& b) const   { return ii < b.ii; }
	bool operator>(const IIterator& b) const   { return ii > b.ii; }
	bool operator<=(const IIterator& b) const  { return ii <= b.ii; }
	bool operator>=(const IIterator& b) const  { return ii >= b.ii; }

	operator bool() const                      { return ii < 0; }

	IIterator()          {}
	IIterator(NP *null)  { ASSERT(null == NULL); ii = -1; }
	IIterator(V& _cont, i32 ii) : cont(&_cont), ii(ii) {}

	STL_ITERATOR_COMPATIBILITY
};

unsigned Pow2Bound(unsigned i);
unsigned PrimeBound(unsigned i);

hash_t memhash(ukk ptr, size_t size);

template <class T>
inline hash_t GetHashVal(const T& x)                         { return x.GetHashVal(); }

template<> inline hash_t GetHashVal(const char& a)           { return (hash_t)a; }
template<> inline hash_t GetHashVal(i8k& a)    { return (const hash_t)a; }
template<> inline hash_t GetHashVal(u8k& a)  { return (const hash_t)a; }
template<> inline hash_t GetHashVal(const short& a)          { return (const hash_t)a; }
template<> inline hash_t GetHashVal(u16k& a) { return (const hash_t)a; }
template<> inline hash_t GetHashVal(i32k& a)            { return (const hash_t)a; }
template<> inline hash_t GetHashVal(u32k& a)   { return (const hash_t)a; }
template<> inline hash_t GetHashVal(const long& a)           { return (const hash_t)a; }
template<> inline hash_t GetHashVal(u64k& a)  { return (const hash_t)a; }
template<> inline hash_t GetHashVal(const bool& a)           { return (const hash_t)a; }
template<> inline hash_t GetHashVal(const wchar_t& a)        { return (const hash_t)a; }

#ifdef HASH64
template<> inline hash_t GetHashVal(const z64& a)          { return (const hash_t)a; }
template<> inline hash_t GetHashVal(zu64k& a)         { return (const hash_t)a; }
#else
template<> inline hash_t GetHashVal(const z64& a)          { return HASH_CONST2 * (hash_t)a + (hash_t)(a >> 32); }
template<> inline hash_t GetHashVal(zu64k& a)         { return GetHashVal((z64)a); }
#endif

template<> inline hash_t GetHashVal(const double& a)         { return memhash(&a, sizeof(a)); }
template<> inline hash_t GetHashVal(const float& a)          { return memhash(&a, sizeof(a)); }

inline hash_t GetPtrHashVal(ukk a)                   { return (hash_t)(uintptr_t)a; }

template <class T>
inline hash_t GetHashVal(T *ptr)                             { return GetPtrHashVal(reinterpret_cast<ukk >(ptr)); }

struct CombineHash {
	hash_t hash;

	template <class T> CombineHash& Do(const T& x)                  { Put(GetHashVal(x)); return *this; }

public:
	CombineHash& Put(hash_t h)                                      { hash = HASH_CONST2 * hash + h; return *this; }

	operator hash_t() const                                         { return hash; }

	CombineHash()                                                   { hash = HASH_CONST1; }
	template <class T>
	CombineHash(const T& h1)                                        { hash = HASH_CONST1; Do(h1); }
	template <class T, class U>
	CombineHash(const T& h1, const U& h2)                           { hash = HASH_CONST1; Do(h1); Do(h2); }
	template <class T, class U, class V>
	CombineHash(const T& h1, const U& h2, const V& h3)              { hash = HASH_CONST1; Do(h1); Do(h2); Do(h3); }
	template <class T, class U, class V, class W>
	CombineHash(const T& h1, const U& h2, const V& h3, const W& h4)	{ hash = HASH_CONST1; Do(h1); Do(h2); Do(h3); Do(h4); }

	template <class T> CombineHash& operator<<(const T& x)          { Do(x); return *this; }
};

template <i32 size>
struct Data_S_ : Moveable< Data_S_<size> >
{
	u8 filler[size];
};

template <class C>
bool IsEqualMap(const C& a, const C& b)
{
	if(a.GetCount() != b.GetCount())
		return false;
	for(i32 i = 0; i < a.GetCount(); i++)
		if(a.GetKey(i) != b.GetKey(i) || a[i] != b[i])
			return false;
	return true;
}

template <class C>
i32 CompareMap(const C& a, const C& b)
{
	i32 n = min(a.GetCount(), b.GetCount());
	for(i32 i = 0; i < n; i++) {
		i32 q = SgnCompare(a.GetKey(i), b.GetKey(i));
		if(q)
			return q;
		q = SgnCompare(a[i], b[i]);
		if(q)
			return q;
	}
	return SgnCompare(a.GetCount(), b.GetCount());
}
