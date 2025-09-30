#ifdef _DEBUG
void BreakWhenPicked(uk ptr);
void BREAK_WHEN_PICKED__(uk ptr);

template <class T>
void BREAK_WHEN_PICKED(T& x)
{
	BREAK_WHEN_PICKED__(&x);
}
#endif

template <class T>
inline void DeepCopyConstructFill(T *t, const T *end, const T& x) {
	while(t != end)
		new(t++) T(clone(x));
}

template <class T>
inline void Construct(T *t, const T *lim) {
	while(t < lim)
		new(t++) T;
}

template <class T>
inline void Destroy(T *t, const T *end)
{
	while(t != end) {
		t->~T();
		t++;
	}
}

template <class T, class S>
inline void DeepCopyConstruct(T *t, const S *s, const S *end) {
	while(s != end)
		new (t++) T(clone(*s++));
}

template <class T>
class Buffer : Moveable< Buffer<T> > {
	T *ptr;

	void Malloc(size_t size) {
		if(std::is_trivially_destructible<T>::value)
			ptr = (T *)MemoryAlloc(size * sizeof(T));
		else {
			uk p = MemoryAlloc(size * sizeof(T) + 16);
			*(size_t *)p = size;
			ptr = (T *)((u8 *)p + 16);
		}
	}
	void New(size_t size) {
		Malloc(size);
		Construct(ptr, ptr + size);
	}
	void New(size_t size, const T& in) {
		Malloc(size);
		DeepCopyConstructFill(ptr, ptr + size, in);
	}
	void Free() {
		if(ptr) {
			if(std::is_trivially_destructible<T>::value)
				MemoryFree(ptr);
			else {
				uk p = (u8 *)ptr - 16;
				size_t size = *(size_t *)p;
				Destroy(ptr, ptr + size);
				MemoryFree(p);
			}
		}
	}

public:
	operator T*()                        { return ptr; }
	operator const T*() const            { return ptr; }
	T *operator~()                       { return ptr; }
	const T *operator~() const           { return ptr; }
	T          *Get()                    { return ptr; }
	const T    *Get() const              { return ptr; }
	T          *begin()                  { return ptr; }
	const T    *begin() const            { return ptr; }

	void Alloc(size_t size)              { Clear(); New(size); }
	void Alloc(size_t size, const T& in) { Clear(); New(size, in); }

	void Clear()                         { Free(); ptr = NULL; }
	bool IsEmpty() const                 { return ptr == NULL; }

	Buffer()                             { ptr = NULL; }
	Buffer(size_t size)                  { New(size); }
	Buffer(size_t size, const T& init)   { New(size, init); }
	~Buffer()                            { Free(); }

	void operator=(Buffer&& v)           { if(&v != this) { Clear(); ptr = v.ptr; v.ptr = NULL; } }
	Buffer(Buffer&& v)                   { ptr = v.ptr; v.ptr = NULL; }

	Buffer(size_t size, std::initializer_list<T> init) {
		Malloc(size); T *t = ptr; for(const auto& i : init) new (t++) T(i);
	}
	Buffer(std::initializer_list<T> init) : Buffer(init.size(), init) {}
};

template <class T>
class One : MoveableAndDeepCopyOption< One<T> > {
	mutable T  *ptr;

	void        Free()                     { if(ptr && ptr != (T*)1) delete ptr; }
	template <class TT>
	void        Pick(One<TT>&& data)       { ptr = data.Detach(); }

public:
	void        Attach(T *data)            { Free(); ptr = data; }
	T          *Detach()                   { T *t = ptr; ptr = NULL; return t; }
	void        Clear()                    { Free(); ptr = NULL; }

	void        operator=(T *data)         { Attach(data); }

	template <class TT>
	void        operator=(One<TT>&& d)     { if((uk )this != (uk )&d) { Free(); Pick(pick(d)); }}

	const T    *operator->() const         { ASSERT(ptr); return ptr; }
	T          *operator->()               { ASSERT(ptr); return ptr; }
	const T    *operator~() const          { return ptr; }
	T          *operator~()                { return ptr; }
	const T    *Get() const                { return ptr; }
	T          *Get()                      { return ptr; }
	const T&    operator*() const          { ASSERT(ptr); return *ptr; }
	T&          operator*()                { ASSERT(ptr); return *ptr; }

	template <class TT, class... Args>
	TT&         Create(Args&&... args)     { TT *q = new TT(std::forward<Args>(args)...); Attach(q); return *q; }
	template <class TT> // with C++ conforming compiler, this would not be needed - GCC bug workaround
	TT&         Create()                   { TT *q = new TT; Attach(q); return *q; }
	template <class... Args>
	T&          Create(Args&&... args)     { T *q = new T(std::forward<Args>(args)...); Attach(q); return *q; }
	T&          Create()                   { T *q = new T; Attach(q); return *q; }

	template <class TT>
	bool        Is() const                 { return dynamic_cast<const TT *>(ptr); }

	bool        IsEmpty() const            { return !ptr; }

	operator bool() const                  { return ptr; }

	Txt ToTxt() const                { return ptr ? AsTxt(*ptr) : "<пусто>"; }

	One()                                  { ptr = NULL; }
	One(T *newt)                           { ptr = newt; }
	template <class TT>
	One(One<TT>&& p)                       { Pick(pick(p)); }
	One(const One<T>& p, i32)              { ptr = p.IsEmpty() ? NULL : new T(clone(*p)); }
	One(const One<T>& p) = delete;
	void operator=(const One<T>& p) = delete;
	~One()                                 { Free(); }
};

template <class T, class... Args>
One<T> MakeOne(Args&&... args) {
	One<T> r;
	r.Create(std::forward<Args>(args)...);
	return r;
}

template <class U> class Index;

template <class T>
class Vec : public MoveableAndDeepCopyOption< Vec<T> > {
	T       *vector;
	i32      items;
	i32      alloc;

	static void    RawFree(T *ptr)            { if(ptr) MemoryFree(ptr); }
	static T      *RawAlloc(i32& n);

	void     Zero()                          { vector = NULL; items = alloc = 0; }
	void     Pick(Vec<T>&& v);

	T       *Rdd()                           { return vector + items++; }

	void     Free();
	void     __DeepCopy(const Vec& src);
	T&       Get(i32 i) const                { ASSERT(i >= 0 && i < items); return vector[i]; }
	bool     ReAlloc(i32 alloc);
	void     ReAllocF(i32 alloc);
	bool     GrowSz();
	void     GrowF();
	T&       GrowAdd(const T& x);
	T&       GrowAdd(T&& x);
	void     RawInsert(i32 q, i32 count);

	template <class U> friend class Index;

public:
	T&       Add()                           { if(items >= alloc) GrowF(); return *(::new(Rdd()) T); }
	T&       Add(const T& x)                 { return items < alloc ? *(new(Rdd()) T(x)) : GrowAdd(x); }
	T&       Add(T&& x)                      { return items < alloc ? *(::new(Rdd()) T(pick(x))) : GrowAdd(pick(x)); }
	template <class... Args>
	T&       Create(Args&&... args)          { if(items >= alloc) GrowF(); return *(::new(Rdd()) T(std::forward<Args>(args)...)); }
	void     AddN(i32 n);
	const T& operator[](i32 i) const         { return Get(i); }
	T&       operator[](i32 i)               { return Get(i); }
	const T& Get(i32 i, const T& def) const  { return i >= 0 && i < GetCount() ? Get(i) : def; }
	T&       Get(i32 i, T& def)              { return i >= 0 && i < GetCount() ? Get(i) : def; }
	i32      GetCount() const                { return items; }
	bool     IsEmpty() const                 { return items == 0; }
	void     Trim(i32 n);
	void     SetCount(i32 n);
	void     SetCount(i32 n, const T& init);
	void     SetCountR(i32 n);
	void     SetCountR(i32 n, const T& init);
	void     Clear();

	T&       At(i32 i)                  { if(i >= items) SetCountR(i + 1); return (*this)[i]; }
	T&       At(i32 i, const T& x)      { if(i >= items) SetCountR(i + 1, x); return (*this)[i]; }

	void     Shrink()                   { if(items != alloc) ReAllocF(items); }
	void     Reserve(i32 n);
	i32      GetAlloc() const           { return alloc; }

	void     Set(i32 i, const T& x, i32 count);
	T&       Set(i32 i, const T& x)     { Set(i, x, 1); return Get(i); }
	T&       Set(i32 i, T&& x)          { return At(i) = pick(x); }
	template <class Range>
	void     SetRange(i32 i, const Range& r);

	void     Remove(i32 i, i32 count = 1);
	void     Remove(i32k *sorted_list, i32 n);
	void     Remove(const Vec<i32>& sorted_list);
	template <class Condition>
	void     RemoveIf(Condition c);

	void     InsertN(i32 i, i32 count = 1);
	T&       Insert(i32 i)              { InsertN(i); return Get(i); }
	void     Insert(i32 i, const T& x, i32 count);
	T&       Insert(i32 i, const T& x)  { Insert(i, x, 1); return Get(i); }
	T&       Insert(i32 i, T&& x);
	void     Insert(i32 i, const Vec& x);
	void     Insert(i32 i, const Vec& x, i32 offset, i32 count);
	void     Insert(i32 i, Vec&& x);
	template <class Range>
	void     InsertRange(i32 i, const Range& r);
	void     Append(const Vec& x)               { Insert(GetCount(), x); }
	void     Append(const Vec& x, i32 o, i32 c) { Insert(GetCount(), x, o, c); }
	void     Append(Vec&& x)                    { Insert(GetCount(), pick(x)); }
	template <class Range>
	void     AppendRange(const Range& r)           { InsertRange(GetCount(), r); }

	void     InsertSplit(i32 i, Vec<T>& v, i32 from);

	void     Swap(i32 i1, i32 i2)    { DRX::Swap(Get(i1), Get(i2)); }

	void     Drop(i32 n = 1)         { ASSERT(n <= GetCount()); Trim(items - n); }
	T&       Top()                   { ASSERT(GetCount()); return Get(items - 1); }
	const T& Top() const             { ASSERT(GetCount()); return Get(items - 1); }
	T        Pop()                   { T h = pick(Top()); Drop(); return h; }

	operator T*()                    { return (T*)vector; }
	operator const T*() const        { return (T*)vector; }

	Vec&  operator<<(const T& x)  { Add(x); return *this; }
	Vec&  operator<<(T&& x)       { Add(pick(x)); return *this; }

#ifdef DRX
	void     Serialize(Stream& s)                        { StreamContainer(s, *this); }
	void     Xmlize(XmlIO& xio, tukk itemtag = "item");
	void     Jsonize(JsonIO& jio);
	Txt      ToTxt() const;
	hash_t   GetHashVal() const                        { return HashBySerialize(*this); }
	template <class B> bool operator==(const B& b) const { return IsEqualRange(*this, b); }
#ifndef CPP_20
	template <class B> bool operator!=(const B& b) const { return !operator==(b); }
#endif
	template <class B> i32  Compare(const B& b) const    { return CompareRanges(*this, b); }
	template <class B> bool operator<=(const B& x) const { return Compare(x) <= 0; }
	template <class B> bool operator>=(const B& x) const { return Compare(x) >= 0; }
	template <class B> bool operator<(const B& x) const  { return Compare(x) < 0; }
	template <class B> bool operator>(const B& x) const  { return Compare(x) > 0; }
#endif

	Vec()                                  { Zero(); }
	explicit Vec(i32 n)                    { items = n; vector = RawAlloc(n); alloc = n; Construct(vector, vector + n); }
	explicit Vec(i32 n, const T& init)     { items = n; vector = RawAlloc(n); alloc = n; DeepCopyConstructFill(vector, vector + n, init); }
	~Vec() {
		Free();
		return; // Constraints:
		AssertMoveable((T *)0);  // T must be moveable
	}

// Pick assignment & copy. Picked source can only do Clear(), ~Vec(), operator=, operator <<=
	Vec(Vec&& v)               { Pick(pick(v)); }
	void operator=(Vec&& v)       { if(this != &v) { Free(); Pick(pick(v)); } }

	void     Insert(i32 i, std::initializer_list<T> init);
	void     Append(std::initializer_list<T> init) { Insert(GetCount(), init); }
	Vec(std::initializer_list<T> init) { vector = NULL; items = alloc = 0; Insert(0, init); }

// Deep copy
	Vec(const Vec& v, i32)     { __DeepCopy(v); }

// Standard container interface
	const T         *begin() const          { return (T*)vector; }
	const T         *end() const            { return (T*)vector + items; }
	T               *begin()                { return (T*)vector; }
	T               *end()                  { return (T*)vector + items; }

// Optimizations
	friend void Swap(Vec& a, Vec& b)  { DRX::Swap(a.items, b.items); DRX::Swap(a.alloc, b.alloc); DRX::Swap(a.vector, b.vector); }

#ifdef DEPRECATED
	T&       DoIndex(i32 i)             { return At(i); }
	T&       DoIndex(i32 i, const T& x) { return At(i, x); }
	T&       AddPick(T&& x)             { return items < alloc ? *(::new(Rdd()) T(pick(x))) : GrowAdd(pick(x)); }
	i32      GetIndex(const T& item) const;
	T&       InsertPick(i32 i, T&& x)   { return Insert(i, pick(x)); }
	void     InsertPick(i32 i, Vec&& x) { Insert(i, pick(x)); }
	void     AppendPick(Vec&& x)                { InsertPick(GetCount(), pick(x)); }
	typedef T       *Iterator;
	typedef const T *ConstIterator;
	ConstIterator    GetIter(i32 i) const   { ASSERT(i >= 0 && i <= items); return begin() + i; }
	Iterator         GetIter(i32 i)         { ASSERT(i >= 0 && i <= items); return begin() + i; }
	typedef T        ValType;
	friend void Append(Vec& dst, const Vec& src)         { dst.Append(src); }

	STL_VECTOR_COMPATIBILITY(Vec<T>)
#endif
};

template <class T>
class Array : public MoveableAndDeepCopyOption< Array<T> > {
protected:
#ifdef _DEBUG
	typedef T *PointerType;
#else
	typedef uk PointerType;
#endif
	Vec<PointerType> vector;

	void     Free();
	void     __DeepCopy(const Array& v);
	T&       Get(i32 i) const                           { return *(T *)vector[i]; }
	T      **GetPtr(i32 i) const                        { return (T **)vector.begin() + i; }

	void     Del(PointerType *ptr, PointerType *lim)              { while(ptr < lim) delete (T *) *ptr++; }
	void     Init(PointerType *ptr, PointerType *lim)             { while(ptr < lim) *ptr++ = new T; }
	void     Init(PointerType *ptr, PointerType *lim, const T& x) { while(ptr < lim) *ptr++ = new T(clone(x)); }

public:
	T&       Add()                           { T *q = new T; vector.Add(q); return *q; }
	T&       Add(const T& x)                 { T *q = new T(x); vector.Add(q); return *q; }
	T&       Add(T&& x)                      { T *q = new T(pick(x)); vector.Add(q); return *q; }
	T&       Add(T *newt)                    { vector.Add(newt); return *newt; }
	T&       Add(One<T>&& one)               { ASSERT(one); return Add(one.Detach()); }
	template<class... Args>
	T&       Create(Args&&... args)          { T *q = new T(std::forward<Args>(args)...); Add(q); return *q; }
	template<class TT, class... Args>
	TT&      Create(Args&&... args)          { TT *q = new TT(std::forward<Args>(args)...); Add(q); return *q; }
	const T& operator[](i32 i) const         { return Get(i); }
	T&       operator[](i32 i)               { return Get(i); }
	const T& Get(i32 i, const T& def) const  { return i >= 0 && i < GetCount() ? Get(i) : def; }
	T&       Get(i32 i, T& def)              { return i >= 0 && i < GetCount() ? Get(i) : def; }
	i32      GetCount() const                { return vector.GetCount(); }
	bool     IsEmpty() const                 { return vector.IsEmpty(); }
	void     Trim(i32 n);
	void     SetCount(i32 n);
	void     SetCount(i32 n, const T& init);
	void     SetCountR(i32 n);
	void     SetCountR(i32 n, const T& init);
	void     Clear()                    { Free(); vector.Clear(); }

	T&       At(i32 i)                  { if(i >= GetCount()) SetCountR(i + 1); return Get(i); }
	T&       At(i32 i, const T& x)      { if(i >= GetCount()) SetCountR(i + 1, x); return Get(i); }

	void     Shrink()                   { vector.Shrink(); }
	void     Reserve(i32 xtra)          { vector.Reserve(xtra); }
	i32      GetAlloc() const           { return vector.GetAlloc(); }

	void     Set(i32 i, const T& x, i32 count);
	T&       Set(i32 i, const T& x)     { Set(i, x, 1); return Get(i); }
	T&       Set(i32 i, T&& x)          { return At(i) = pick(x); }
	void     Remove(i32 i, i32 count = 1);
	void     Remove(i32k *sorted_list, i32 n);
	void     Remove(const Vec<i32>& sorted_list);
	template <class Condition>
	void     RemoveIf(Condition c);
	void     InsertN(i32 i, i32 count = 1);
	T&       Insert(i32 i)              { InsertN(i); return Get(i); }
	void     Insert(i32 i, const T& x, i32 count);
	T&       Insert(i32 i, const T& x)  { Insert(i, x, 1); return Get(i); }
	T&       Insert(i32 i, T&& x);
	void     Insert(i32 i, const Array& x);
	void     Insert(i32 i, const Array& x, i32 offset, i32 count);
	template <class Range>
	void     InsertRange(i32 i, const Range& r);
	void     Insert(i32 i, Array&& x)             { vector.Insert(i, pick(x.vector)); }
	void     Append(const Array& x)               { Insert(GetCount(), x); }
	void     Append(const Array& x, i32 o, i32 c) { Insert(GetCount(), x, o, c); }
	void     Append(Array&& x)                    { InsertPick(GetCount(), pick(x)); }
	template <class Range>
	void     AppendRange(const Range& r)          { InsertRange(GetCount(), r); }

	void     Swap(i32 i1, i32 i2)                 { DRX::Swap(vector[i1], vector[i2]); }
	void     Move(i32 i1, i32 i2);

	T&       Set(i32 i, T *newt)        { delete (T *)vector[i]; vector[i] = newt; return *newt; }
	T&       Insert(i32 i, T *newt);

	void     Drop(i32 n = 1)            { Trim(GetCount() - n); }
	T&       Top()                      { return Get(GetCount() - 1); }
	const T& Top() const                { return Get(GetCount() - 1); }

	T       *Detach(i32 i)              { T *t = &Get(i); vector.Remove(i); return t; }
	T       *Swap(i32 i, T *newt)       { T *tmp = (T*)vector[i]; vector[i] = newt; return tmp; }
	T       *PopDetach()                { return (T *) vector.Pop(); }

	void     Swap(Array& b)             { Swap(vector, b.vector); }

	Array& operator<<(const T& x)       { Add(x); return *this; }
	Array& operator<<(T&& x)            { Add(pick(x)); return *this; }
	Array& operator<<(T *newt)          { Add(newt); return *this; }

	void     Serialize(Stream& s)       { StreamContainer(s, *this); }
	void     Xmlize(XmlIO& xio, tukk itemtag = "item");
	void     Jsonize(JsonIO& jio);
	Txt   ToTxt() const;
	hash_t   GetHashVal() const       { return HashBySerialize(*this); }

	template <class B> bool operator==(const B& b) const { return IsEqualRange(*this, b); }
#ifndef CPP_20
	template <class B> bool operator!=(const B& b) const { return !operator==(b); }
#endif
	template <class B> i32  Compare(const B& b) const    { return CompareRanges(*this, b); }
	template <class B> bool operator<=(const B& x) const { return Compare(x) <= 0; }
	template <class B> bool operator>=(const B& x) const { return Compare(x) >= 0; }
	template <class B> bool operator<(const B& x) const  { return Compare(x) < 0; }
	template <class B> bool operator>(const B& x) const  { return Compare(x) > 0; }

	Array()                                          {}
	explicit Array(i32 n) : vector(n)                { Init(vector.begin(), vector.end()); }
	explicit Array(i32 n, const T& init) : vector(n) { Init(vector.begin(), vector.end(), init); }
	~Array()                                         { Free(); }

// Pick assignment & copy. Picked source can only Clear(), ~Vec(), operator=, operator<<=
	Array(Array&& v) : vector(pick(v.vector))  {}
	void operator=(Array&& v)           { if(this != &v) { Free(); vector = pick(v.vector); } }

// Deep copy
	Array(const Array& v, i32)          { __DeepCopy(v); }

	void     Insert(i32 i, std::initializer_list<T> init);
	void     Append(std::initializer_list<T> init) { Insert(GetCount(), init); }
	Array(std::initializer_list<T> init) { Insert(0, init); }

	class Iterator;

	class ConstIterator {
	protected:
		T **ptr;
		ConstIterator(T **p)                    { ptr = p; }

		friend class Array<T>;
		struct NP { i32 dummy; };

	public:
		const T& operator*() const              { return **ptr; }
		const T *operator->() const             { return *ptr; }
		const T& operator[](i32 i) const        { return *ptr[i]; }

		ConstIterator& operator++()             { ptr++; return *this; }
		ConstIterator& operator--()             { ptr--; return *this; }
		ConstIterator  operator++(i32)          { ConstIterator t = *this; ++*this; return t; }
		ConstIterator  operator--(i32)          { ConstIterator t = *this; --*this; return t; }

		ConstIterator& operator+=(i32 i)        { ptr += i; return *this; }
		ConstIterator& operator-=(i32 i)        { ptr -= i; return *this; }

		ConstIterator operator+(i32 i) const    { return ptr + i; }
		ConstIterator operator-(i32 i) const    { return ptr - i; }

		i32  operator-(ConstIterator x) const   { return (i32)(ptr - x.ptr); }

		bool operator==(ConstIterator x) const  { return ptr == x.ptr; }
		bool operator!=(ConstIterator x) const  { return ptr != x.ptr; }
		bool operator<(ConstIterator x) const   { return ptr < x.ptr; }
		bool operator>(ConstIterator x) const   { return ptr > x.ptr; }
		bool operator<=(ConstIterator x) const  { return ptr <= x.ptr; }
		bool operator>=(ConstIterator x) const  { return ptr >= x.ptr; }

		operator bool() const                   { return ptr; }

		ConstIterator()                         {}
		ConstIterator(NP *null)                 { ASSERT(null == NULL); ptr = NULL; }

		STL_ITERATOR_COMPATIBILITY
	};

	class Iterator : public ConstIterator {
		friend class Array<T>;
		Iterator(T **p) : ConstIterator(p)      {}
		typedef ConstIterator B;
		struct NP { i32 dummy; };

	public:
		T& operator*()                          { return **B::ptr; }
		T *operator->()                         { return *B::ptr; }
		T& operator[](i32 i)                    { return *B::ptr[i]; }

		const T& operator*() const              { return **B::ptr; }
		const T *operator->() const             { return *B::ptr; }
		const T& operator[](i32 i) const        { return *B::ptr[i]; }

		Iterator& operator++()                  { B::ptr++; return *this; }
		Iterator& operator--()                  { B::ptr--; return *this; }
		Iterator  operator++(i32)               { Iterator t = *this; ++*this; return t; }
		Iterator  operator--(i32)               { Iterator t = *this; --*this; return t; }

		Iterator& operator+=(i32 i)             { B::ptr += i; return *this; }
		Iterator& operator-=(i32 i)             { B::ptr -= i; return *this; }

		Iterator operator+(i32 i) const         { return B::ptr + i; }
		Iterator operator-(i32 i) const         { return B::ptr - i; }

		i32      operator-(Iterator x) const    { return B::operator-(x); }

		Iterator()                               {}
		Iterator(NP *null) : ConstIterator(null) {}

		STL_ITERATOR_COMPATIBILITY
	};

// Standard container interface
	Iterator         begin()                    { return (T **)vector.begin(); }
	Iterator         end()                      { return (T **)vector.end(); }
	ConstIterator    begin() const              { return (T **)vector.begin(); }
	ConstIterator    end() const                { return (T **)vector.end(); }

// Optimalization
	friend void Swap(Array& a, Array& b)                   { DRX::Swap(a.vector, b.vector); }
	//GCC forced move from Iterator, ugly workaround
private:
	static void IterSwap0(Iterator a, Iterator b)          { DRX::Swap(*a.ptr, *b.ptr); }
public:
	friend void IterSwap(Iterator a, Iterator b)           { Array<T>::IterSwap0(a, b); }

#ifdef DEPRECATED
	i32      GetIndex(const T& item) const;
	void     InsertPick(i32 i, Array&& x)       { vector.InsertPick(i, pick(x.vector)); }
	void     AppendPick(Array&& x)              { InsertPick(GetCount(), pick(x)); }
	T&       DoIndex(i32 i)                     { return At(i); }
	T&       DoIndex(i32 i, const T& x)         { return At(i, x); }
	T&       AddPick(T&& x)                     { T *q = new T(pick(x)); vector.Add(q); return *q; }
	T&       InsertPick(i32 i, T&& x)           { return Insert(i, pick(x)); }
	typedef T        ValType;
	Iterator         GetIter(i32 pos)           { return (T **)vector.GetIter(pos); }
	ConstIterator    GetIter(i32 pos) const     { return (T **)vector.GetIter(pos); }
#endif
	STL_VECTOR_COMPATIBILITY(Array<T>)
};