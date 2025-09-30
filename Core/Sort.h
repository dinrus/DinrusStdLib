template <class I, class Less>
force_inline
void OrderIter2__(I a, I b, const Less& less)
{
	if(less(*b, *a))
		IterSwap(a, b);
}

template <class I, class Less>
force_inline // 2-3 compares, 0-2 swaps
void OrderIter3__(I x, I y, I z, const Less& less)
{
    if(less(*y, *x)) {
	    if(less(*z, *y))
	        IterSwap(x, z);
	    else {
		    IterSwap(x, y);
		    if(less(*z, *y))
		        IterSwap(y, z);
	    }
    }
    else
    if(less(*z, *y)) {
        IterSwap(y, z);
        if(less(*y, *x))
            IterSwap(x, y);
    }
}

template <class I, class Less>
force_inline // 3-6 compares, 0-5 swaps
void OrderIter4__(I x, I y, I z, I u, const Less& less)
{
	OrderIter3__(x, y, z, less);
	if(less(*z, *u)) return;
	IterSwap(z, u);
	if(less(*y, *z)) return;
	IterSwap(y, z);
	if(less(*x, *y)) return;
	IterSwap(x, y);
}

template <class I, class Less>
force_inline // 4-10 compares, 0-9 swaps
void OrderIter5__(I x, I y, I z, I u, I v, const Less& less)
{
	OrderIter4__(x, y, z, u, less);
	if(less(*u, *v)) return;
	IterSwap(u, v);
	if(less(*z, *u)) return;
	IterSwap(z, u);
	if(less(*y, *z)) return;
	IterSwap(y, z);
	if(less(*x, *y)) return;
	IterSwap(x, y);
}

template <class I, class Less>
force_inline // 9-15 compares, 0-14 swaps
void OrderIter6__(I x, I y, I z, I u, I v, I w, const Less& less)
{
	OrderIter5__(x, y, z, u, v, less);
	if(less(*v, *w)) return;
	IterSwap(v, w);
	if(less(*u, *v)) return;
	IterSwap(u, v);
	if(less(*z, *u)) return;
	IterSwap(z, u);
	if(less(*y, *z)) return;
	IterSwap(y, z);
	if(less(*x, *y)) return;
	IterSwap(x, y);
}

template <class I, class Less>
force_inline // 16-26 compares, 0-20 swaps
void OrderIter7__(I x, I y, I z, I u, I v, I w, I q, const Less& less)
{
	OrderIter6__(x, y, z, u, v, w, less);
	if(less(*w, *q)) return;
	IterSwap(w, q);
	if(less(*v, *w)) return;
	IterSwap(v, w);
	if(less(*u, *v)) return;
	IterSwap(u, v);
	if(less(*z, *u)) return;
	IterSwap(z, u);
	if(less(*y, *z)) return;
	IterSwap(y, z);
	if(less(*x, *y)) return;
	IterSwap(x, y);
}

dword  Random(dword n);

template <class I, class Less>
void Sort__(I l, I h, const Less& less)
{
	i32 count = i32(h - l);
	I middle = l + (count >> 1);        // get the middle element
	for(;;) {
		switch(count) {
		case 0:
		case 1: return;
		case 2: OrderIter2__(l, l + 1, less); return;
		case 3: OrderIter3__(l, l + 1, l + 2, less); return;
		case 4: OrderIter4__(l, l + 1, l + 2, l + 3, less); return;
		case 5: OrderIter5__(l, l + 1, l + 2, l + 3, l + 4, less); return;
		case 6: OrderIter6__(l, l + 1, l + 2, l + 3, l + 4, l + 5, less); return;
		case 7: OrderIter7__(l, l + 1, l + 2, l + 3, l + 4, l + 5, l + 6, less); return;
		}
		if(count > 1000) { // median of 5, 2 of them random elements
			middle = l + (count >> 1); // iterators cannot point to the same object!
			I q = l + 1 + (i32)Random((count >> 1) - 2);
			I w = middle + 1 + (i32)Random((count >> 1) - 2);
			OrderIter5__(l, q, middle, w, h - 1, less);
		}
		else // median of 3
			OrderIter3__(l, middle, h - 1, less);

		I pivot = h - 2;
		IterSwap(pivot, middle); // move median pivot to h - 2
		I i = l;
		I j = h - 2; // l, h - 2, h - 1 already sorted above
		for(;;) { // Hoare’s partition (modified):
			while(less(*++i, *pivot));
			do
				if(!(i < j)) goto done;
			while(!less(*--j, *pivot));
			IterSwap(i, j);
		}
	done:
		IterSwap(i, h - 2);                 // put pivot back in between partitions

		I ih = i;
		while(ih + 1 != h && !less(*i, *(ih + 1))) // Find middle range of elements equal to pivot
			++ih;

		i32 count_l = i32(i - l);
		if(count_l == 1) // this happens if there are many elements equal to pivot, filter them out
			for(I q = ih + 1; q != h; ++q)
				if(!less(*i, *q))
					IterSwap(++ih, q);

		i32 count_h = i32(h - ih) - 1;

		if(count_l < count_h) {       // recurse on smaller partition, tail on larger
			Sort__(l, i, less);
			l = ih + 1;
			count = count_h;
		}
		else {
			Sort__(ih + 1, h, less);
			h = i;
			count = count_l;
		}

		if(count > 8 && min(count_l, count_h) < (max(count_l, count_h) >> 2)) // If unbalanced, randomize the next step
			middle = l + 1 + (i32)Random(count - 2); // Random pivot selection
		else
			middle = l + (count >> 1); // the middle is probably still the best guess otherwise
	}
}

template <class Range, class Less>
void Sort(Range&& c, const Less& less)
{
	Sort__(c.begin(), c.end(), less);
}

template <class Range>
void Sort(Range&& c)
{
	Sort__(c.begin(), c.end(), std::less<ValTypeOf<Range>>());
}

template <class T>
struct StableSortItem__ {
	const T& value;
	i32      index;

	StableSortItem__(const T& value, i32 index) : value(value), index(index) {}
};

template <class II, class T>
struct StableSortIterator__ {
	II          ii;
	i32        *vi;

	typedef StableSortIterator__<II, T> Iter;

	Iter&       operator ++ ()               { ++ii; ++vi; return *this; }
	Iter&       operator -- ()               { --ii; --vi; return *this; }
	Iter        operator +  (i32 i) const    { return Iter(ii + i, vi + i); }
	Iter        operator -  (i32 i) const    { return Iter(ii - i, vi - i); }
	i32         operator -  (Iter b) const   { return (i32)(ii - b.ii); }
	bool        operator == (Iter b) const   { return ii == b.ii; }
	bool        operator != (Iter b) const   { return ii != b.ii; }
	bool        operator <  (Iter b) const   { return ii <  b.ii; }
	bool        operator <= (Iter b) const   { return ii <=  b.ii; }

	StableSortItem__<T> operator*() const    { return StableSortItem__<T>(*ii, *vi); }

	friend void IterSwap(Iter a, Iter b)     { IterSwap(a.ii, b.ii); IterSwap(a.vi, b.vi); }

	StableSortIterator__(II ii, i32 *vi) : ii(ii), vi(vi) {}
};

template <class T, class Less>
struct StableSortLess__ {
	const Less& less;

	bool operator()(const StableSortItem__<T>& a, const StableSortItem__<T>& b) const {
		if(less(a.value, b.value)) return true;
		return less(b.value, a.value) ? false : a.index < b.index;
	}

	StableSortLess__(const Less& less) : less(less) {}
};

template <class Range, class Less>
void StableSort(Range&& r, const Less& less)
{
	auto begin = r.begin();
	auto end = r.end();
	typedef ValTypeOf<Range> VT;
	typedef decltype(begin) I;
	i32 count = (i32)(uintptr_t)(end - begin);
	Buffer<i32> h(count);
	for(i32 i = 0; i < count; i++)
		h[i] = i;
	Sort__(StableSortIterator__<I, VT>(begin, ~h), StableSortIterator__<I, VT>(end, ~h + count),
	       StableSortLess__<VT, Less>(less));
}

template <class Range>
void StableSort(Range&& r)
{
	StableSort(r, std::less<ValTypeOf<Range>>());
}

template <class II, class VI, class K>
struct IndexSortIterator__
{
	typedef IndexSortIterator__<II, VI, K> Iter;

	IndexSortIterator__(II ii, VI vi) : ii(ii), vi(vi) {}

	Iter&       operator ++ ()               { ++ii; ++vi; return *this; }
	Iter&       operator -- ()               { --ii; --vi; return *this; }
	const K&    operator *  () const         { return *ii; }
	Iter        operator +  (i32 i) const    { return Iter(ii + i, vi + i); }
	Iter        operator -  (i32 i) const    { return Iter(ii - i, vi - i); }
	i32         operator -  (Iter b) const   { return (i32)(ii - b.ii); }
	bool        operator == (Iter b) const   { return ii == b.ii; }
	bool        operator != (Iter b) const   { return ii != b.ii; }
	bool        operator <  (Iter b) const   { return ii <  b.ii; }
	bool        operator <= (Iter b) const   { return ii <=  b.ii; }
	friend void IterSwap    (Iter a, Iter b) { IterSwap(a.ii, b.ii); IterSwap(a.vi, b.vi); }

	II          ii;
	VI          vi;
};

template <class MasterRange, class Range2, class Less>
void IndexSort(MasterRange&& r, Range2&& r2, const Less& less)
{
	ASSERT(r.GetCount() == r2.GetCount());
	typedef decltype(r.begin()) I;
	typedef decltype(r2.begin()) I2;
	typedef ValTypeOf<MasterRange> VT;
	if(r.GetCount() == 0)
		return;
	Sort__(IndexSortIterator__<I, I2, VT>(r.begin(), r2.begin()),
	       IndexSortIterator__<I, I2, VT>(r.end(), r2.end()),
		   less);
}

template <class MasterRange, class Range2>
void IndexSort(MasterRange&& r, Range2&& r2)
{
	IndexSort(r, r2, std::less<ValTypeOf<MasterRange>>());
}

template <class MasterRange, class Range2, class Less>
void StableIndexSort(MasterRange&& r, Range2&& r2, const Less& less)
{
	ASSERT(r.GetCount() == r2.GetCount());
	typedef decltype(r.begin()) I;
	typedef decltype(r2.begin()) I2;
	typedef ValTypeOf<MasterRange> VT;
	if(r.GetCount() == 0)
		return;
	StableSort(SubRange(IndexSortIterator__<I, I2, VT>(r.begin(), r2.begin()),
		                IndexSortIterator__<I, I2, VT>(r.end(), r2.end())).Write(),
	           less);
}

template <class MasterRange, class Range2>
void StableIndexSort(MasterRange&& r, Range2&& r2)
{
	StableIndexSort(r, r2, std::less<ValTypeOf<MasterRange>>());
}

template <class II, class VI, class WI, class K>
struct IndexSort2Iterator__
{
	typedef IndexSort2Iterator__<II, VI, WI, K> Iter;

	IndexSort2Iterator__(II ii, VI vi, WI wi) : ii(ii), vi(vi), wi(wi) {}

	Iter&       operator ++ ()               { ++ii; ++vi; ++wi; return *this; }
	Iter&       operator -- ()               { --ii; --vi; --wi; return *this; }
	const K&    operator *  () const         { return *ii; }
	Iter        operator +  (i32 i) const    { return Iter(ii + i, vi + i, wi + i); }
	Iter        operator -  (i32 i) const    { return Iter(ii - i, vi - i, wi - i); }
	i32         operator -  (Iter b) const   { return (i32)(ii - b.ii); }
	bool        operator == (Iter b) const   { return ii == b.ii; }
	bool        operator != (Iter b) const   { return ii != b.ii; }
	bool        operator <  (Iter b) const   { return ii <  b.ii; }
	bool        operator <= (Iter b) const   { return ii <=  b.ii; }
	friend void IterSwap    (Iter a, Iter b) { IterSwap(a.ii, b.ii); IterSwap(a.vi, b.vi); IterSwap(a.wi, b.wi); }

	II          ii;
	VI          vi;
	WI          wi;
};

template <class MasterRange, class Range2, class Range3, class Less>
void IndexSort2(MasterRange&& r, Range2&& r2, Range3&& r3, const Less& less)
{
	ASSERT(r.GetCount() == r2.GetCount());
	ASSERT(r.GetCount() == r3.GetCount());
	if(r.GetCount() == 0)
		return;
	typedef decltype(r.begin()) I;
	typedef decltype(r2.begin()) I2;
	typedef decltype(r3.begin()) I3;
	typedef ValTypeOf<MasterRange> VT;
	Sort__(IndexSort2Iterator__<I, I2, I3, VT>(r.begin(), r2.begin(), r3.begin()),
	       IndexSort2Iterator__<I, I2, I3, VT>(r.end(), r2.end(), r3.end()),
		   less);
}

template <class MasterRange, class Range2, class Range3>
void IndexSort2(MasterRange&& r, Range2&& r2, Range3&& r3)
{
	IndexSort2(r, r2, r3, std::less<ValTypeOf<MasterRange>>());
}

template <class MasterRange, class Range2, class Range3, class Less>
void StableIndexSort2(MasterRange&& r, Range2&& r2, Range3&& r3, const Less& less)
{
	ASSERT(r.GetCount() == r2.GetCount());
	ASSERT(r.GetCount() == r3.GetCount());
	if(r.GetCount() == 0)
		return;
	typedef decltype(r.begin()) I;
	typedef decltype(r2.begin()) I2;
	typedef decltype(r3.begin()) I3;
	typedef ValTypeOf<MasterRange> VT;
	StableSort(SubRange(IndexSort2Iterator__<I, I2, I3, VT>(r.begin(), r2.begin(), r3.begin()),
	                    IndexSort2Iterator__<I, I2, I3, VT>(r.end(), r2.end(), r3.end())).Write(),
		       less);
}

template <class MasterRange, class Range2, class Range3>
void StableIndexSort2(MasterRange&& r, Range2&& r2, Range3&& r3)
{
	StableIndexSort2(r, r2, r3, std::less<ValTypeOf<MasterRange>>());
}

template <class II, class VI, class WI, class XI, class K>
struct IndexSort3Iterator__
{
	typedef IndexSort3Iterator__<II, VI, WI, XI, K> Iter;

	IndexSort3Iterator__(II ii, VI vi, WI wi, XI xi) : ii(ii), vi(vi), wi(wi), xi(xi) {}

	Iter&       operator ++ ()               { ++ii; ++vi; ++wi; ++xi; return *this; }
	Iter&       operator -- ()               { --ii; --vi; --wi; --xi; return *this; }
	const K&    operator *  () const         { return *ii; }
	Iter        operator +  (i32 i) const    { return Iter(ii + i, vi + i, wi + i, xi + i); }
	Iter        operator -  (i32 i) const    { return Iter(ii - i, vi - i, wi - i, xi - i); }
	i32         operator -  (Iter b) const   { return (i32)(ii - b.ii); }
	bool        operator == (Iter b) const   { return ii == b.ii; }
	bool        operator != (Iter b) const   { return ii != b.ii; }
	bool        operator <  (Iter b) const   { return ii <  b.ii; }
	bool        operator <= (Iter b) const   { return ii <=  b.ii; }
	friend void IterSwap    (Iter a, Iter b) { IterSwap(a.ii, b.ii); IterSwap(a.vi, b.vi); IterSwap(a.wi, b.wi); IterSwap(a.xi, b.xi); }

	II          ii;
	VI          vi;
	WI          wi;
	XI          xi;
};

template <class MasterRange, class Range2, class Range3, class Range4, class Less>
void IndexSort3(MasterRange&& r, Range2&& r2, Range3&& r3, Range4&& r4, const Less& less)
{
	ASSERT(r.GetCount() == r2.GetCount());
	ASSERT(r.GetCount() == r3.GetCount());
	ASSERT(r.GetCount() == r4.GetCount());
	if(r.GetCount() == 0)
		return;
	typedef decltype(r.begin()) I;
	typedef decltype(r2.begin()) I2;
	typedef decltype(r3.begin()) I3;
	typedef decltype(r4.begin()) I4;
	typedef ValTypeOf<MasterRange> VT;
	Sort__(IndexSort3Iterator__<I, I2, I3, I4, VT>(r.begin(), r2.begin(), r3.begin(), r4.begin()),
	       IndexSort3Iterator__<I, I2, I3, I4, VT>(r.end(), r2.end(), r3.end(), r4.end()),
		   less);
}

template <class MasterRange, class Range2, class Range3, class Range4>
void IndexSort3(MasterRange&& r, Range2&& r2, Range3&& r3, Range4&& r4)
{
	IndexSort3(r, r2, r3, r4, std::less<ValTypeOf<MasterRange>>());
}

template <class MasterRange, class Range2, class Range3, class Range4, class Less>
void StableIndexSort3(MasterRange&& r, Range2&& r2, Range3&& r3, Range4&& r4, const Less& less)
{
	ASSERT(r.GetCount() == r2.GetCount());
	ASSERT(r.GetCount() == r3.GetCount());
	ASSERT(r.GetCount() == r4.GetCount());
	if(r.GetCount() == 0)
		return;
	typedef decltype(r.begin()) I;
	typedef decltype(r2.begin()) I2;
	typedef decltype(r3.begin()) I3;
	typedef decltype(r4.begin()) I4;
	typedef ValTypeOf<MasterRange> VT;
	StableSort(SubRange(IndexSort3Iterator__<I, I2, I3, I4, VT>(r.begin(), r2.begin(), r3.begin(), r4.begin()),
	                    IndexSort3Iterator__<I, I2, I3, I4, VT>(r.end(), r2.end(), r3.end(), r4.end())).Write(),
		       less);
}

template <class MasterRange, class Range2, class Range3, class Range4>
void StableIndexSort3(MasterRange&& r, Range2&& r2, Range3&& r3, Range4&& r4)
{
	StableIndexSort3(r, r2, r3, r4, std::less<ValTypeOf<MasterRange>>());
}

template <class I, class V>
struct SortOrderIterator__ : PostfixOps< SortOrderIterator__<I, V> >
{
	typedef SortOrderIterator__<I, V> Iter;

	SortOrderIterator__(i32 *ii, I vi) : ii(ii), vi(vi) {}

	Iter&       operator ++ ()               { ++ii; return *this; }
	Iter&       operator -- ()               { --ii; return *this; }
	const V&    operator *  () const         { return *(vi + *ii); }
	Iter        operator +  (i32 i) const    { return Iter(ii + i, vi); }
	Iter        operator -  (i32 i) const    { return Iter(ii - i, vi); }
	i32         operator -  (Iter b) const   { return i32(ii - b.ii); }
	bool        operator == (Iter b) const   { return ii == b.ii; }
	bool        operator != (Iter b) const   { return ii != b.ii; }
	bool        operator <  (Iter b) const   { return ii < b.ii; }
	bool        operator <= (Iter b) const   { return ii <=  b.ii; }
	friend void IterSwap    (Iter a, Iter b) { IterSwap(a.ii, b.ii); }

	i32        *ii;
	I           vi;
};

template <class Range, class Less>
Vec<i32> GetSortOrder(const Range& r, const Less& less)
{
	auto begin = r.begin();
	Vec<i32> index;
	index.SetCount(r.GetCount());
	for(i32 i = index.GetCount(); --i >= 0; index[i] = i)
		;
	typedef SortOrderIterator__<decltype(begin), ValTypeOf<Range>> It;
	Sort__(It(index.begin(), begin), It(index.end(), begin), less);
	return index;
}

template <class Range>
Vec<i32> GetSortOrder(const Range& r)
{
	return GetSortOrder(r, std::less<ValTypeOf<Range>>());
}

template <class I, class T>
struct StableSortOrderIterator__ : PostfixOps< StableSortOrderIterator__<I, T> >
{
	typedef StableSortOrderIterator__<I, T> Iter;

	StableSortOrderIterator__(i32 *ii, I vi) : ii(ii), vi(vi) {}

	Iter&       operator ++ ()               { ++ii; return *this; }
	Iter&       operator -- ()               { --ii; return *this; }
	Iter        operator +  (i32 i) const    { return Iter(ii + i, vi); }
	Iter        operator -  (i32 i) const    { return Iter(ii - i, vi); }
	i32         operator -  (Iter b) const   { return i32(ii - b.ii); }
	bool        operator == (Iter b) const   { return ii == b.ii; }
	bool        operator != (Iter b) const   { return ii != b.ii; }
	bool        operator <  (Iter b) const   { return ii < b.ii; }
	bool        operator <= (Iter b) const   { return ii <=  b.ii; }
	friend void IterSwap    (Iter a, Iter b) { IterSwap(a.ii, b.ii); }

	StableSortItem__<T> operator*() const    { return StableSortItem__<T>(*(vi + *ii), *ii); }

	i32        *ii;
	I           vi;
};

template <class Range, class Less>
Vec<i32> GetStableSortOrder(const Range& r, const Less& less)
{
	Vec<i32> index;
	index.SetCount(r.GetCount());
	for(i32 i = index.GetCount(); --i >= 0; index[i] = i)
		;
	auto begin = r.begin();
	typedef ValTypeOf<Range> VT;
	typedef StableSortOrderIterator__<decltype(begin), VT> It;
	Sort__(It(index.begin(), begin), It(index.end(), begin), StableSortLess__<VT, Less>(less));
	return index;
}

template <class Range>
Vec<i32> GetStableSortOrder(const Range& r)
{
	return GetStableSortOrder(r, std::less<ValTypeOf<Range>>());
}

template <class Map, class Less>
void SortByKey(Map& map, const Less& less)
{
	typename Map::KeyContainer k = map.PickKeys();
	typename Map::ValContainer v = map.PickVals();
	IndexSort(k, v, less);
	map = Map(pick(k), pick(v));
}

template <class Map>
void SortByKey(Map& map)
{
	SortByKey(map, std::less<typename Map::KeyType>());
}

template <class Map, class Less>
void SortByVal(Map& map, const Less& less)
{
	typename Map::KeyContainer k = map.PickKeys();
	typename Map::ValContainer v = map.PickVals();
	IndexSort(v, k, less);
	map = Map(pick(k), pick(v));
}

template <class Map>
void SortByVal(Map& map)
{
	SortByVal(map, std::less<ValTypeOf<Map>>());
}

template <class Map, class Less>
void StableSortByKey(Map& map, const Less& less)
{
	typename Map::KeyContainer k = map.PickKeys();
	typename Map::ValContainer v = map.PickVals();
	StableIndexSort(k, v, less);
	map = Map(pick(k), pick(v));
}

template <class Map>
void StableSortByKey(Map& map)
{
	StableSortByKey(map, std::less<typename Map::KeyType>());
}

template <class Map, class Less>
void StableSortByVal(Map& map, const Less& less)
{
	typename Map::KeyContainer k = map.PickKeys();
	typename Map::ValContainer v = map.PickVals();
	StableIndexSort(v, k, less);
	map = Map(pick(k), pick(v));
}

template <class Map>
void StableSortByVal(Map& map)
{
	StableSortByVal(map, std::less<ValTypeOf<Map>>());
}

template <class Index, class Less>
void SortIndex(Index& index, const Less& less)
{
	typename Index::ValContainer k = index.PickKeys();
	Sort(k, less);
	index = Index(pick(k));
}

template <class Index>
void SortIndex(Index& index)
{
	SortIndex(index, std::less<ValTypeOf<Index>>());
}

template <class Index, class Less>
void StableSortIndex(Index& index, const Less& less)
{
	typename Index::ValContainer k = index.PickKeys();
	StableSort(k, less);
	index = Index(pick(k));
}

template <class Index>
void StableSortIndex(Index& index)
{
	StableSortIndex(index, std::less<ValTypeOf<Index>>());
}
