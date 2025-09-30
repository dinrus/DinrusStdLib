template <class T>
inline i32 sgn(T a) { return a > 0 ? 1 : a < 0 ? -1 : 0; }

template <class T>
inline T tabs(T a) { return (a >= 0 ? a : -a); }

// there is a general mess about abs, this is not ideal but best we can do now... as long as we
// do not want abs(1.1) == 1 surprise in GCC
template <class T> inline T abs(T a) { return (a >= 0 ? a : -a); }

template <class T>
inline i32 cmp(const T& a, const T& b) { return a > b ? 1 : a < b ? -1 : 0; }

template <class Range>
void Reverse(Range&& r)
{
	auto start = r.begin();
	auto end = r.end();
	if(start != end && --end != start)
		do
			IterSwap(start, end);
		while(++start != end && --end != start);
}

template <class Range, class Accumulator>
void Accumulate(const Range& r, Accumulator& a)
{
	for(const auto& e : r)
		a(e);
}

template <class Range>
ValTypeOf<Range> Sum(const Range& r, const ValTypeOf<Range>& zero)
{
	ValTypeOf<Range> sum = zero;
	for(const auto& e : r)
		sum += e;
	return sum;
}

template <class T>
ValTypeOf<T> Sum(const T& c)
{
	return Sum(c, (ValTypeOf<T>)0);
}

template <class Range, class V>
i32 Count(const Range& r, const V& val)
{
	i32 count = 0;
	for(const auto& x : r)
		if(x == val)
			count++;
	return count;
}

template <class Range, class Predicate>
i32 CountIf(const Range& r, const Predicate& p)
{
	i32 count = 0;
	for(const auto& x : r)
		if(p(x))
			count++;
	return count;
}

template <class Range, class Pred>
i32 FindBest(const Range& r, const Pred& pred)
{
	i32 count = r.GetCount();
	if(count == 0)
		return -1;
	auto *m = &r[0];
	i32 mi = 0;
	for(i32 i = 1; i < count; i++)
		if(pred(r[i], *m)) {
			m = &r[i];
			mi = i;
		}
	return mi;
}

template <class Range>
i32 FindMin(const Range& r)
{
	return FindBest(r, std::less<ValTypeOf<Range>>());
}

template <class Range>
const ValTypeOf<Range>& Min(const Range& r)
{
	return r[FindMin(r)];
}

template <class Range>
const ValTypeOf<Range>& Min(const Range& r, const ValTypeOf<Range>& def)
{
	i32 q = FindMin(r);
	return q < 0 ? def : r[q];
}


template <class Range>
i32 FindMax(const Range& r)
{
	return FindBest(r, std::greater<ValTypeOf<Range>>());
}

template <class Range>
const ValTypeOf<Range>& Max(const Range& r)
{
	return r[FindMax(r)];
}

template <class Range>
const ValTypeOf<Range>& Max(const Range& r, const ValTypeOf<Range>& def)
{
	i32 q = FindMax(r);
	return q < 0 ? def : r[q];
}

template <class Range1, class Range2>
bool IsEqualRange(const Range1& a, const Range2& b)
{
	if(a.GetCount() != b.GetCount())
		return false;
	for(i32 i = 0; i < a.GetCount(); i++)
		if(!(a[i] == b[i]))
			return false;
	return true;
}

template <class Range1, class Range2, class Cmp>
i32 CompareRanges(const Range1& a, const Range2& b, Cmp cmp)
{
	i32 n = min(a.GetCount(), b.GetCount());
	for(i32 i = 0; i < n; i++) {
		i32 q = cmp(a[i], b[i]);
		if(q)
			return q;
	}
	return cmp(a.GetCount(), b.GetCount());
}

template <class Range1, class Range2>
i32 CompareRanges(const Range1& a, const Range2& b)
{
	i32 n = min(a.GetCount(), b.GetCount());
	for(i32 i = 0; i < n; i++) {
		i32 q = SgnCompare(a[i], b[i]);
		if(q)
			return q;
	}
	return SgnCompare(a.GetCount(), b.GetCount());
}

template <class Range, class C>
i32 FindMatch(const Range& r, const C& match, i32 from = 0)
{
	for(i32 i = from; i < r.GetCount(); i++)
		if(match(r[i])) return i;
	return -1;
}

template <class Range, class V>
i32 FindIndex(const Range& r, const V& value, i32 from = 0)
{
	for(i32 i = from; i < r.GetCount(); i++)
		if(r[i] == value) return i;
	return -1;
}

template <class Range, class Predicate>
Vec<i32> FindAll(const Range& r, Predicate match, i32 from = 0)
{
	Vec<i32> ndx;
	for(i32 i = from; i < r.GetCount(); i++)
		if(match(r[i]))
			ndx.Add(i);
	return ndx;
}

template <class Range, class Predicate>
Vec<i32> FindAlli(const Range& r, Predicate match, i32 from = 0)
{
	Vec<i32> ndx;
	for(i32 i = from; i < r.GetCount(); i++)
		if(match(i))
			ndx.Add(i);
	return ndx;
}

template <class Range, class T, class Less>
i32 FindLowerBound(const Range& r, const T& val, const Less& less)
{
	i32 pos = 0;
	i32 count = r.GetCount();
	while(count > 0) {
		i32 half = count >> 1;
		i32 m = pos + half;
		if(less(r[m], val)) {
			pos = m + 1;
			count = count - half - 1;
		}
		else
			count = half;
	}
	return pos;
}

template <class Range, class T>
i32 FindLowerBound(const Range& r, const T& val)
{
	return FindLowerBound(r, val, std::less<T>());
}

template <class Range, class T, class L>
i32 FindUpperBound(const Range& r, const T& val, const L& less)
{
	i32 pos = 0;
	i32 count = r.GetCount();
	while(count > 0) {
		i32 half = count >> 1;
		i32 m = pos + half;
		if(less(val, r[m]))
			count = half;
		else {
			pos = m + 1;
			count = count - half - 1;
		}
	}
	return pos;
}

template <class Range, class T>
i32 FindUpperBound(const Range& r, const T& val)
{
	return FindUpperBound(r, val, std::less<T>());
}

template <class Range, class T, class L>
i32 FindBinary(const Range& r, const T& val, const L& less)
{
	i32 i = FindLowerBound(r, val, less);
	return i < r.GetCount() && !less(val, r[i]) ? i : -1;
}

template <class Range, class T>
i32 FindBinary(const Range& r, const T& val)
{
	return FindBinary(r, val, std::less<T>());
}

template <class Container, class T>
void LruAdd(Container& lru, T value, i32 limit = 10)
{
	i32 q = FindIndex(lru, value);
	if(q >= 0)
		lru.Remove(q);
	lru.Insert(0, value);
	if(lru.GetCount() > limit)
		lru.SetCount(limit);
}

template <class C = Vec<i32>, class V>
C MakeIota(V end, V start = 0, V step = 1)
{
	C x;
	x.Reserve((end - start) / step);
	for(V i = start; i < end; i += step)
		x.Add(i);
	return x;
}