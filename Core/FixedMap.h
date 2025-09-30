template <class K, class T, class V, class Less>
class FixedAMap {
protected:
	Vec<K> key;
	V value;

public:
	T&       Add(const K& k, const T& x)       { key.Add(k); return value.Add(x); }
	T&       AddPick(const K& k, T&& x)        { key.Add(k); return value.AddPick(pick(x)); }
	T&       Add(const K& k)                   { key.Add(k); return value.Add(); }
	
	void     Finish()                          { IndexSort(key, value, Less()); Shrink(); }

	i32      Find(const K& k) const            { return FindBinary(key, k, Less()); }
	i32      FindNext(i32 i) const             { return i + 1 < key.GetCount() && key[i] == key[i + 1] ? i + 1 : -1; }

	T&       Get(const K& k)                   { return value[Find(k)]; }
	const T& Get(const K& k) const             { return value[Find(k)]; }
	const T& Get(const K& k, const T& d) const { i32 i = Find(k); return i >= 0 ? value[i] : d; }

	T       *FindPtr(const K& k)               { i32 i = Find(k); return i >= 0 ? &value[i] : NULL; }
	const T *FindPtr(const K& k) const         { i32 i = Find(k); return i >= 0 ? &value[i] : NULL; }

	const T& operator[](i32 i) const           { return value[i]; }
	T&       operator[](i32 i)                 { return value[i]; }
	i32      GetCount() const                  { return value.GetCount(); }
	bool     IsEmpty() const                   { return value.IsEmpty(); }
	void     Clear()                           { key.Clear(); value.Clear(); }
	void     Shrink()                          { value.Shrink(); key.Shrink(); }
	void     Reserve(i32 xtra)                 { value.Reserve(xtra); key.Reserve(xtra); }
	i32      GetAlloc() const                  { return value.GetAlloc(); }

	const K& GetKey(i32 i) const               { return key[i]; }

#ifdef DRX
	void     Serialize(Stream& s);
	void     Xmlize(XmlIO& xio);
	void     Jsonize(JsonIO& jio);
	Txt   ToTxt() const;
#endif

	void     Swap(FixedAMap& x)                { DRX::Swap(value, x.value); DRX::Swap(key, x.key); }

	const Vec<K>& GetKeys() const           { return key; }
	Vec<K>        PickKeys()                { return pick(key); }

	const V&         GetVals() const         { return value; }
	V&               GetVals()               { return value; }
	V                PickVals()              { return pick(value); }
	
	FixedAMap& operator()(const K& k, const T& v)       { Add(k, v); return *this; }

	FixedAMap()                                         {}
	FixedAMap(const FixedAMap& s, i32) : key(s.key, 0), value(s.value, 0) {}
	FixedAMap(Vec<K>&& key, V&& val) : key(pick(key)), value(pick(val)) {}

	typedef ConstIteratorOf<V>  ConstIterator;
	typedef IteratorOf<V>       Iterator;

	Iterator         begin()                                      { return value.begin(); }
	Iterator         end()                                        { return value.end(); }
	ConstIterator    begin() const                                { return value.begin(); }
	ConstIterator    end() const                                  { return value.end(); }

#ifdef DEPRECATED
	typedef V                          ValContainer;
	typedef T                          ValType;

	typedef Vec<K> KeyContainer;
	typedef K         KeyType;
	typedef ConstIteratorOf<Vec<K>> KeyConstIterator;

	KeyConstIterator KeyBegin() const                             { return key.Begin(); }
	KeyConstIterator KeyEnd() const                               { return key.End(); }
	KeyConstIterator KeyGetIter(i32 pos) const                    { return key.GetIter(pos); }

	Iterator         GetIter(i32 pos)                             { return value.GetIter(pos); }
	ConstIterator    GetIter(i32 pos) const                       { return value.GetIter(pos); }
#endif
};

template <class K, class T, class Less = StdLess<K> >
class FixedVectorMap : public MoveableAndDeepCopyOption<FixedVectorMap<K, T, Less> >,
                       public FixedAMap< K, T, Vec<T>, Less > {
    typedef FixedAMap< K, T, Vec<T>, Less > B;
public:
	FixedVectorMap(const FixedVectorMap& s, i32) : FixedAMap<K, T, Vec<T>, Less>(s, 1) {}
	FixedVectorMap(Vec<K>&& key, Vec<T>&& val) : FixedAMap<K, T, Vec<T>, Less>(pick(key), pick(val)) {}
	FixedVectorMap()                                                       {}

	friend void    Swap(FixedVectorMap& a, FixedVectorMap& b)      { a.B::Swap(b); }

	typedef typename FixedAMap< K, T, Vec<T>, Less >::ConstIterator ConstIterator; // GCC bug (?)
	typedef typename FixedAMap< K, T, Vec<T>, Less >::Iterator      Iterator; // GCC bug (?)
	STL_MAP_COMPATIBILITY(FixedVectorMap<K _cm_ T _cm_ Less>)
};

template <class K, class T, class Less = StdLess<K> >
class FixedArrayMap : public MoveableAndDeepCopyOption< FixedArrayMap<K, T, Less> >,
                      public FixedAMap< K, T, Array<T>, Less > {
	typedef FixedAMap< K, T, Array<T>, Less > B;
public:
	T&        Add(const K& k, const T& x)          { return B::Add(k, x); }
	T&        Add(const K& k)                      { return B::Add(k); }
	T&        Add(const K& k, T *newt)             { B::key.Add(k); return B::value.Add(newt); }
	template <class TT, class... Args>
	TT&       Create(const K& k, Args&&... args)   { TT *q = new TT(std::forward<Args>(args)...); B::key.Add(k); return static_cast<TT&>(B::value.Add(q)); }

	FixedArrayMap(const FixedArrayMap& s, i32) : FixedAMap<K, T, Array<T>, Less>(s, 1) {}
	FixedArrayMap(Vec<K>&& ndx, Array<T>&& val) : FixedAMap<K, T, Array<T>, Less>(pick(ndx), pick(val)) {}
	FixedArrayMap() {}

	friend void    Swap(FixedArrayMap& a, FixedArrayMap& b)        { a.B::Swap(b); }

	typedef typename FixedAMap< K, T, Array<T>, Less >::ConstIterator ConstIterator; // GCC bug (?)
	typedef typename FixedAMap< K, T, Array<T>, Less >::Iterator      Iterator; // GCC bug (?)
	STL_MAP_COMPATIBILITY(FixedArrayMap<K _cm_ T _cm_ Less>)
};
