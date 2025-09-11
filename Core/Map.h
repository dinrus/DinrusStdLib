template <class K, class V>
struct KeyValRef {
	const K& key;
	V&       value;

	KeyValRef(const K& key, V& value) : key(key), value(value) {}
};

template <class Map, class K, class V>
struct MapKVRange {
	Map& map;

	struct Iterator {
		Map& map;
		i32  ii;

		void SkipUnlinked()               { while(ii < map.GetCount() && map.IsUnlinked(ii)) ii++; }

		void operator++()                 { ++ii; SkipUnlinked(); }
		KeyValRef<K, V> operator*()     { return KeyValRef<K, V>(map.GetKey(ii), map[ii]); }
		bool operator!=(Iterator b) const { return ii != b.ii; }

		Iterator(Map& map, i32 ii) : map(map), ii(ii) { SkipUnlinked(); }
	};

	Iterator begin() const             { return Iterator(map, 0); }
	Iterator end() const               { return Iterator(map, map.GetCount()); }

	MapKVRange(Map& map) : map(map) {}
};

template <class K, class T, class V>
class AMap {
protected:
	Index<K> key;
	V        value;

	template <class KK>           i32 FindAdd_(KK&& k);
	template <class KK, class TT> i32 FindAdd_(KK&& k, TT&& init);
	template <class KK>           T&  Put_(KK&& k);
	template <class KK, class TT> i32 Put_(KK&& k, TT&& x);
	template <class KK>           i32 PutDefault_(KK&& k);
	template <class KK>           i32 FindPut_(KK&& k);
	template <class KK, class TT> i32 FindPut_(KK&& k, TT&& init);
	template <class KK>           T&  GetAdd_(KK&& k);
	template <class KK, class TT> T&  GetAdd_(KK&& k, TT&& init);
	template <class KK>           T&  GetPut_(KK&& k);
	template <class KK, class TT> T&  GetPut_(KK&& k, TT&& init);

public:
	T&       Add(const K& k, const T& x)            { key.Add(k); return value.Add(x); }
	T&       Add(const K& k, T&& x)                 { key.Add(k); return value.Add(pick(x)); }
	T&       Add(const K& k)                        { key.Add(k); return value.Add(); }
	T&       Add(K&& k, const T& x)                 { key.Add(pick(k)); return value.Add(x); }
	T&       Add(K&& k, T&& x)                      { key.Add(pick(k)); return value.Add(pick(x)); }
	T&       Add(K&& k)                             { key.Add(pick(k)); return value.Add(); }

	i32      Find(const K& k) const                 { return key.Find(k); }
	i32      FindNext(i32 i) const                  { return key.FindNext(i); }
	i32      FindLast(const K& k) const             { return key.FindLast(k); }
	i32      FindPrev(i32 i) const                  { return key.FindPrev(i); }

	i32      FindAdd(const K& k)                    { return FindAdd_(k); }
	i32      FindAdd(const K& k, const T& init)     { return FindAdd_(k, init); }
	i32      FindAdd(const K& k, T&& init)          { return FindAdd_(k, pick(init)); }
	i32      FindAdd(K&& k)                         { return FindAdd_(pick(k)); }
	i32      FindAdd(K&& k, const T& init)          { return FindAdd_(pick(k), init); }
	i32      FindAdd(K&& k, T&& init)               { return FindAdd_(pick(k), pick(init)); }

	T&       Put(const K& k)                        { return Put_(k); }
	i32      Put(const K& k, const T& x)            { return Put_(k, x); }
	i32      Put(const K& k, T&& x)                 { return Put_(k, pick(x)); }
	T&       Put(K&& k)                             { return Put_(pick(k)); }
	i32      Put(K&& k, const T& x)                 { return Put_(pick(k), x); }
	i32      Put(K&& k, T&& x)                      { return Put_(pick(k), pick(x)); }

	i32      PutDefault(const K& k)                 { return PutDefault_(k); }
	i32      PutDefault(K&& k)                      { return PutDefault_(pick(k)); }

	i32      FindPut(const K& k)                    { return FindPut_(k); }
	i32      FindPut(const K& k, const T& init)     { return FindPut_(k, init); }
	i32      FindPut(const K& k, T&& init)          { return FindPut_(k, pick(init)); }
	i32      FindPut(K&& k)                         { return FindPut_(pick(k)); }
	i32      FindPut(K&& k, const T& init)          { return FindPut_(pick(k), init); }
	i32      FindPut(K&& k, T&& init)               { return FindPut_(pick(k), pick(init)); }

	T&       Get(const K& k)                        { return value[Find(k)]; }
	const T& Get(const K& k) const                  { return value[Find(k)]; }
	const T& Get(const K& k, const T& d) const      { i32 i = Find(k); return i >= 0 ? value[i] : d; }

	T&       GetAdd(const K& k)                     { return GetAdd_(k); }
	T&       GetAdd(const K& k, const T& x)         { return GetAdd_(k, x); }
	T&       GetAdd(const K& k, T&& x)              { return GetAdd_(k, pick(x)); }
	T&       GetAdd(K&& k)                          { return GetAdd_(pick(k)); }
	T&       GetAdd(K&& k, const T& x)              { return GetAdd_(pick(k), x); }
	T&       GetAdd(K&& k, T&& x)                   { return GetAdd_(pick(k), pick(x)); }

	T&       GetPut(const K& k)                     { return GetPut_(k); }
	T&       GetPut(const K& k, const T& x)         { return GetPut_(k, x); }
	T&       GetPut(const K& k, T&& x)              { return GetPut_(k, pick(x)); }
	T&       GetPut(K&& k)                          { return GetPut_(pick(k)); }
	T&       GetPut(K&& k, const T& x)              { return GetPut_(pick(k), x); }
	T&       GetPut(K&& k, T&& x)                   { return GetPut_(pick(k), pick(x)); }

	void     SetKey(i32 i, const K& k)              { key.Set(i, k); }
	void     SetKey(i32 i, K&& k)                   { key.Set(i, pick(k)); }

	T       *FindPtr(const K& k)                    { i32 i = Find(k); return i >= 0 ? &value[i] : NULL; }
	const T *FindPtr(const K& k) const              { i32 i = Find(k); return i >= 0 ? &value[i] : NULL; }

	T       *FindLastPtr(const K& k)                { i32 i = FindLast(k); return i >= 0 ? &value[i] : NULL; }
	const T *FindLastPtr(const K& k) const          { i32 i = FindLast(k); return i >= 0 ? &value[i] : NULL; }

	void     Unlink(i32 i)                          { key.Unlink(i); }
	i32      UnlinkKey(const K& k, unsigned h)      { return key.UnlinkKey(k, h); }
	i32      UnlinkKey(const K& k)                  { return key.UnlinkKey(k); }
	bool     IsUnlinked(i32 i) const                { return key.IsUnlinked(i); }
	void     Sweep();
	bool     HasUnlinked() const                    { return key.HasUnlinked(); }

	const T& operator[](i32 i) const                { return value[i]; }
	T&       operator[](i32 i)                      { return value[i]; }
	i32      GetCount() const                       { return value.GetCount(); }
	bool     IsEmpty() const                        { return value.IsEmpty(); }
	void     Clear()                                { key.Clear(); value.Clear(); }
	void     Shrink()                               { value.Shrink(); key.Shrink(); }
	void     Reserve(i32 xtra)                      { value.Reserve(xtra); key.Reserve(xtra); }
	i32      GetAlloc() const                       { return value.GetAlloc(); }

	unsigned GetHash(i32 i) const                   { return key.GetHash(i); }

	void     Drop(i32 n = 1)                        { key.Drop(n); value.Drop(n); }
	T&       Top()                                  { return value.Top(); }
	const T& Top() const                            { return value.Top(); }
	const K& TopKey() const                         { return key.Top(); }
//	T        Pop()                                  { T h = Top(); Drop(); return h; }
	K        PopKey()                               { K h = TopKey(); Drop(); return h; }
	void     Trim(i32 n)                            { key.Trim(n); value.Trim(n); }

	const K& GetKey(i32 i) const                    { return key[i]; }

	void     Remove(i32k *sl, i32 n)           { key.Remove(sl, n); value.Remove(sl, n); }
	void     Remove(const Vec<i32>& sl)          { Remove(sl, sl.GetCount()); }
	template <typename P> void RemoveIf(P p)        { Remove(FindAlli(*this, p)); }

	void     Serialize(Stream& s);
	void     Xmlize(XmlIO& xio);
	void     Jsonize(JsonIO& jio);
	Txt   ToTxt() const;
	hash_t   GetHashVal() const                   { return HashBySerialize(*this); }
	bool     operator==(const AMap& b) const        { ASSERT(!HasUnlinked()); return IsEqualMap(*this, b); }
	bool     operator!=(const AMap& b) const        { return !operator==(b); }
	i32      Compare(const AMap& b) const           { ASSERT(!HasUnlinked()); return CompareMap(*this, b); }
	bool     operator<=(const AMap& x) const        { return Compare(x) <= 0; }
	bool     operator>=(const AMap& x) const        { return Compare(x) >= 0; }
	bool     operator<(const AMap& x) const         { return Compare(x) < 0; }
	bool     operator>(const AMap& x) const         { return Compare(x) > 0; }

	void     Swap(AMap& x)                          { DRX::Swap(value, x.value); DRX::Swap(key, x.key); }
	const Index<K>&  GetIndex() const               { return key; }
	Index<K>         PickIndex()                    { return pick(key); }

	const Vec<K>& GetKeys() const                { return key.GetKeys(); }
	Vec<K>        PickKeys()                     { return key.PickKeys(); }

	const V&         GetVals() const              { return value; }
	V&               GetVals()                    { return value; }
	V                PickVals()                   { return pick(value); }

	MapKVRange<AMap, K, T> operator~()                   { return MapKVRange<AMap, K, T>(*this); }
	MapKVRange<const AMap, K, const T> operator~() const { return MapKVRange<const AMap, K, const T>(*this); }

	AMap& operator()(const K& k, const T& v)        { Add(k, v); return *this; }

	AMap()                                          {}
	AMap(const AMap& s, i32) : key(s.key, 0), value(s.value, 0) {}
	AMap(Index<K>&& ndx, V&& val) : key(pick(ndx)), value(pick(val)) {}
	AMap(Vec<K>&& ndx, V&& val) : key(pick(ndx)), value(pick(val)) {}
	AMap(std::initializer_list<std::pair<K, T>> init) { for(const auto& i : init) Add(clone(i.first), clone(i.second)); }

	typedef IteratorOf<V>           Iterator;
	typedef ConstIteratorOf<V>      ConstIterator;

	Iterator         begin()                        { return value.begin(); }
	Iterator         end()                          { return value.end(); }
	ConstIterator    begin() const                  { return value.begin(); }
	ConstIterator    end() const                    { return value.end(); }

#ifdef DEPRECATED
	typedef V                          ValContainer;
	typedef T                          ValType;

	typedef Vec<K> KeyContainer;
	typedef K         KeyType;
	typedef ConstIteratorOf<Index<K>> KeyConstIterator;

	friend i32     GetCount(const AMap& v)                        { return v.GetCount(); }
	i32      PutPick(const K& k, T&& x)                           { return Put(k, pick(x)); }
	T&       AddPick(const K& k, T&& x)                           { return Add(k, pick(x)); }
	i32      FindAddPick(const K& k, T&& init)                    { return FindAdd(k, pick(init)); }
	i32      FindPutPick(const K& k, T&& init)                    { return FindPut(k, pick(init)); }
	T&       GetAddPick(const K& k, T&& x)                        { return GetAdd(k, pick(x)); }
	T&       GetPutPick(const K& k, T&& x)                        { return GetPut(k, pick(x)); }

	KeyConstIterator KeyGetIter(i32 pos) const                    { return key.GetIter(pos); }
	Iterator         GetIter(i32 pos)                             { return value.GetIter(pos); }
	ConstIterator    GetIter(i32 pos) const                       { return value.GetIter(pos); }

	KeyConstIterator KeyBegin() const                             { return key.begin(); }
	KeyConstIterator KeyEnd() const                               { return key.end(); }

	T&       Insert(i32 i, const K& k)             { key.Insert(i, k); return value.Insert(i); }
	T&       Insert(i32 i, const K& k, const T& x) { key.Insert(i, k); return value.Insert(i, x); }
	T&       Insert(i32 i, const K& k, T&& x)      { key.Insert(i, k); return value.Insert(i, pick(x)); }
	T&       Insert(i32 i, K&& k)                  { key.Insert(i, pick(k)); return value.Insert(i); }
	T&       Insert(i32 i, K&& k, const T& x)      { key.Insert(i, pick(k)); return value.Insert(i, x); }
	T&       Insert(i32 i, K&& k, T&& x)           { key.Insert(i, pick(k)); return value.Insert(i, pick(x)); }

	void     Remove(i32 i)                         { key.Remove(i); value.Remove(i); }
	void     Remove(i32 i, i32 count)              { key.Remove(i, count); value.Remove(i, count); }
	i32      RemoveKey(const K& k);
#endif
};

template <class K, class T>
class VecMap : public MoveableAndDeepCopyOption<VecMap<K, T>>,
                  public AMap<K, T, Vec<T>> {
    typedef AMap<K, T, Vec<T>> B;
public:
	T        Pop()                            { T h = pick(B::Top()); B::Drop(); return h; }

	VecMap(const VecMap& s, i32) : AMap<K, T, Vec<T>>(s, 1) {}
	VecMap(Index<K>&& ndx, Vec<T>&& val) : AMap<K, T, Vec<T>>(pick(ndx), pick(val)) {}
	VecMap(Vec<K>&& ndx, Vec<T>&& val) : AMap<K, T, Vec<T>>(pick(ndx), pick(val)) {}
	VecMap()                                                       {}
	VecMap(std::initializer_list<std::pair<K, T>> init) : B::AMap(init) {}

	template <class KK, class TT>
	VecMap& operator()(KK&& k, TT&& v)                { B::Add(std::forward<KK>(k), std::forward<TT>(v)); return *this; }

	friend void    Swap(VecMap& a, VecMap& b)      { a.B::Swap(b); }

	typedef typename AMap< K, T, Vec<T>>::ConstIterator ConstIterator;
	typedef typename AMap< K, T, Vec<T>>::Iterator      Iterator;
	STL_MAP_COMPATIBILITY(VecMap<K _cm_ T _cm_ HashFn>)
};

template <class K, class T>
class ArrayMap : public MoveableAndDeepCopyOption<ArrayMap<K, T>>,
                 public AMap<K, T, Array<T>> {
	typedef AMap<K, T, Array<T>> B;
public:
	T&        Add(const K& k, const T& x)          { return B::Add(k, x); }
	T&        Add(const K& k, T&& x)               { return B::Add(k, pick(x)); }
	T&        Add(const K& k)                      { return B::Add(k); }
	T&        Add(const K& k, T *newt)             { B::key.Add(k); return B::value.Add(newt); }
	T&        Add(K&& k, const T& x)               { return B::Add(pick(k), x); }
	T&        Add(K&& k, T&& x)                    { return B::Add(pick(k), pick(x)); }
	T&        Add(K&& k)                           { return B::Add(pick(k)); }
	T&        Add(K&& k, T *newt)                  { B::key.Add(pick(k)); return B::value.Add(newt); }

	T&        Insert(i32 i, const K& k, T *newt)   { B::key.Insert(i, k); return B::value.Insert(i, newt); }
	T&        Insert(i32 i, K&& k, T *newt)        { B::key.Insert(i, pick(k)); return B::value.Insert(i, newt); }
	using B::Insert;

	template <class TT, class... Args>
	TT& Create(const K& k, Args&&... args)         { TT *q = new TT(std::forward<Args>(args)...); B::key.Add(k); return static_cast<TT&>(B::value.Add(q)); }
	template <class TT, class... Args>
	TT& Create(K&& k, Args&&... args)              { TT *q = new TT(std::forward<Args>(args)...); B::key.Add(pick(k)); return static_cast<TT&>(B::value.Add(q)); }

	T&        Set(i32 i, T *ptr)                   { return B::value.Set(i, ptr); }
	T        *PopDetach()                          { B::key.Drop(); return B::value.PopDetach(); }
	T        *Detach(i32 i)                        { B::key.Remove(i); return B::value.Detach(i); }
	T        *Swap(i32 i, T *newt)                 { return B::value.Swap(i, newt); }

	ArrayMap(const ArrayMap& s, i32) : AMap<K, T, Array<T>>(s, 1) {}
	ArrayMap(Index<K>&& ndx, Array<T>&& val) : AMap<K, T, Array<T>>(pick(ndx), pick(val)) {}
	ArrayMap(Vec<K>&& ndx, Array<T>&& val) : AMap<K, T, Array<T>>(pick(ndx), pick(val)) {}
	ArrayMap() {}

	ArrayMap(std::initializer_list<std::pair<K, T>> init) : B::AMap(init) {}

	template <class KK, class TT>
	ArrayMap& operator()(KK&& k, TT&& v)           { Add(std::forward<KK>(k), std::forward<TT>(v)); return *this; }

	friend void    Swap(ArrayMap& a, ArrayMap& b)        { a.B::Swap(b); }

	typedef typename AMap< K, T, Array<T>>::ConstIterator ConstIterator;
	typedef typename AMap< K, T, Array<T>>::Iterator      Iterator;
	STL_MAP_COMPATIBILITY(ArrayMap<K _cm_ T _cm_ HashFn>)
};