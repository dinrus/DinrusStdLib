struct IndexCommon {
	enum { HIBIT = 0x80000000 };

	struct Hash : Moveable<Hash> {
		i32   next; // also link for unlinked items...
		dword hash;
		i32   prev;
	};

	i32         *map;
	Hash        *hash;
	dword        mask;
	i32          unlinked;


	static i32 empty[1];

	static dword Smear(hash_t h)          { return FoldHash(h) | HIBIT; }

	void Link(i32& m, Hash& hh, i32 ii);
	void Link(i32 ii, dword sh);
	void Del(i32& m, Hash& hh, i32 ii);
	void Ins(i32& m, Hash& hh, i32 ii);

	void MakeMap(i32 count);
	void Remap(i32 count);
	void Reindex(i32 count);
	void GrowMap(i32 count);
	void FreeMap();
	void Free();

	void Set(i32 ii, dword h);

	Vec<i32> GetUnlinked() const;

	void Clear();
	void Trim(i32 n, i32 count);
	void Sweep(i32 n);
	void Reserve(i32 n);
	void Shrink();
	void AdjustMap(i32 count, i32 alloc);

	void Copy(const IndexCommon& b, i32 count);
	void Pick(IndexCommon& b);
	void Swap(IndexCommon& b);

	IndexCommon();
	~IndexCommon();
};

template <class K, class T, class V> class AMap;

template <class T>
class Index : MoveableAndDeepCopyOption<Index<T>>, IndexCommon {
	Vec<T> key;

	static dword Smear(const T& k)   { return IndexCommon::Smear(GetHashVal(k)); }

	i32  FindFrom(i32 i, dword sh, const T& k, i32 end) const;
	i32  FindBack(i32 i, dword sh, const T& k, i32 end) const;


	void ReallocHash(i32 n);
	template <typename U> void GrowAdd(U&& k, dword sh);
	template <typename U> void AddS(i32& m, U&& k, dword sh);
	template <typename U> void AddS(U&& k, dword sh);

	template <class OP, class U> i32 FindAdd(U&& k, OP add_op);
	template <class U> i32 FindPut0(U&& k, bool& put);

	template <typename U> i32 Put0(U&& k, dword sh);
	template <typename U> void Set0(i32 i, U&& k);

	template <typename, typename, typename> friend class AMap;

	void        FixHash(bool makemap = true);

public:
	void        Add(const T& k)             { AddS(k, Smear(k)); }
	void        Add(T&& k)                  { AddS(pick(k), Smear(k)); }
	Index&      operator<<(const T& x)      { Add(x); return *this; }
	Index&      operator<<(T&& x)           { Add(pick(x)); return *this; }

	i32         Find(const T& k) const;
	i32         FindNext(i32 i) const;
	i32         FindLast(const T& k) const;
	i32         FindPrev(i32 i) const;

	i32         FindAdd(const T& k)         { return FindAdd(k, []{}); }
	i32         FindAdd(T&& k)              { return FindAdd(pick(k), []{}); }

	i32         Put(const T& k)             { return Put0(k, Smear(k)); }
	i32         Put(T&& k)                  { return Put0(pick(k), Smear(k)); }
	i32         FindPut(const T& k, bool& p){ return FindPut0(k, p); }
	i32         FindPut(T&& k, bool& p)     { return FindPut0(pick(k), p); }
	i32         FindPut(const T& k)         { bool p; return FindPut0(k, p); }
	i32         FindPut(T&& k)              { bool p; return FindPut0(pick(k), p); }

	void        Unlink(i32 i);
	i32         UnlinkKey(const T& k);
	bool        IsUnlinked(i32 i) const      { return hash[i].hash == 0; }
	bool        HasUnlinked() const          { return unlinked >= 0; }
	Vec<i32> GetUnlinked() const          { return IndexCommon::GetUnlinked(); }

	void        Sweep();

	void        Set(i32 i, const T& k)       { Set0(i, k); }
	void        Set(i32 i, T&& k)            { Set0(i, pick(k)); }

	const T&    operator[](i32 i) const      { return key[i]; }
	i32         GetCount() const             { return key.GetCount(); }
	bool        IsEmpty() const              { return key.IsEmpty(); }

	void        Clear()                      { key.Clear(); IndexCommon::Clear(); }

	void        Trim(i32 n = 0)              { IndexCommon::Trim(n, GetCount()); key.Trim(n); }
	void        Drop(i32 n = 1)              { Trim(GetCount() - 1); }
	const T&    Top() const                  { return key.Top(); }
	T           Pop()                        { T x = pick(Top()); Drop(); return x; }

	void        Reserve(i32 n);
	void        Shrink();
	i32         GetAlloc() const             { return key.GetAlloc(); }

	Vec<T>        PickKeys()              { Vec<T> r = pick(key); Clear(); return r; }
	const Vec<T>& GetKeys() const         { return key; }

	void     Remove(i32k *sorted_list, i32 count);
	void     Remove(const Vec<i32>& sorted_list)         { Remove(sorted_list, sorted_list.GetCount()); }
	template <typename Pred> void RemoveIf(Pred p)          { Remove(FindAlli(key, p)); }

	Index()                                                 {}
	Index(Index&& s) : key(pick(s.key))                     { IndexCommon::Pick(s); }
	Index(const Index& s, i32) : key(s.key, 0)              { ReallocHash(0); IndexCommon::Copy(s, key.GetCount()); } // TODO: Unlinked!
	explicit Index(Vec<T>&& s) : key(pick(s))            { FixHash(); }
	Index(const Vec<T>& s, i32) : key(s, 0)              { FixHash(); }

	Index& operator=(Vec<T>&& x)                         { key = pick(x); FixHash(); return *this; }
	Index& operator=(Index<T>&& x)                          { key = pick(x.key); IndexCommon::Pick(x); return *this; }

	Index(std::initializer_list<T> init) : key(init)        { FixHash(); }

	void     Serialize(Stream& s);
	void     Xmlize(XmlIO& xio, tukk itemtag = "key");
	void     Jsonize(JsonIO& jio);
	Txt   ToTxt() const;
	template <class B> bool operator==(const B& b) const { return IsEqualRange(*this, b); }
#ifndef CPP_20
	template <class B> bool operator!=(const B& b) const { return !operator==(b); }
#endif
	template <class B> i32  Compare(const B& b) const    { return CompareRanges(*this, b); }
	template <class B> bool operator<=(const B& x) const { return Compare(x) <= 0; }
	template <class B> bool operator>=(const B& x) const { return Compare(x) >= 0; }
	template <class B> bool operator<(const B& x) const  { return Compare(x) < 0; }
	template <class B> bool operator>(const B& x) const  { return Compare(x) > 0; }

// Standard container interface
	typedef ConstIteratorOf<Vec<T>> ConstIterator;
	ConstIterator begin() const                             { return key.begin(); }
	ConstIterator end() const                               { return key.end(); }

	friend void Swap(Index& a, Index& b)                    { a.IndexCommon::Swap(b); DRX::Swap(a.key, b.key); }

// deprecated:
#ifdef DEPRECATED
	T&       Insert(i32 i, const T& k)                      { key.Insert(i, k); FixHash(); return key[i]; }
	void     Remove(i32 i, i32 count)                       { key.Remove(i, count); FixHash(); }
	void     Remove(i32 i)                                  { Remove(i, 1); }
	i32      RemoveKey(const T& k)                          { i32 i = Find(k); if(i >= 0) Remove(i); return i; }

	unsigned GetHash(i32 i) const                           { return hash[i].hash; }

	Index& operator<<=(const Vec<T>& s)                  { *this = clone(s); return *this; }
	typedef T                ValType;
	typedef Vec<T>        ValContainer;
	ConstIterator  GetIter(i32 pos) const                   { return key.GetIter(pos); }

	void     ClearIndex()                    {}
	void     Reindex(i32 n)                  {}
	void     Reindex()                       {}

	typedef T             value_type;
	typedef ConstIterator const_iterator;
	typedef const T&      const_reference;
	typedef i32           size_type;
	typedef i32           difference_type;
	const_iterator        Begin() const          { return begin(); }
	const_iterator        End() const            { return end(); }
	void                  clear()                { Clear(); }
	size_type             size()                 { return GetCount(); }
	bool                  empty() const          { return IsEmpty(); }
#endif

#ifdef _DEBUG
	Txt Dump() const;
#endif
};