enum {
	CO_PARTITION_MIN = 128,
	CO_PARTITION_MAX = 1024*1024,
};

template <class C, class MC>
inline size_t CoChunk__(C count, MC min_chunk = CO_PARTITION_MIN, MC max_chunk = CO_PARTITION_MAX)
{
	i32 n = min(count / CPU_Cores(), (C)max_chunk);
	return n < (C)min_chunk ? 0 : n;
}

template <class Iter, class Lambda>
void CoPartition(Iter begin, Iter end, const Lambda& lambda, i32 min_chunk = CO_PARTITION_MIN, i32 max_chunk = CO_PARTITION_MAX)
{
	size_t chunk = CoChunk__(end - begin, min_chunk, max_chunk);
	if(chunk) {
		CoWork co;
		while(begin < end) {
			Iter e = Iter(begin + min(chunk, size_t(end - begin)));
			co & [=] {
				lambda(begin, e);
			};
			begin = e;
		}
	}
	else
		lambda(begin, end);
}

template <class Range, class Lambda>
void CoPartition(Range&& r, const Lambda& lambda)
{
	size_t chunk = CoChunk__(r.GetCount(), CO_PARTITION_MIN);
	if(chunk) {
		CoWork co;
		auto begin = r.begin();
		auto end = r.end();
		while(begin < end) {
			auto e = begin + min(chunk, size_t(end - begin));
			co & [=] {
				auto sr = SubRange(begin, e); // we need l-value
				lambda(sr);
			};
			begin = e;
		}
	}
	else
		lambda(SubRange(r.begin(), r.end()));
}

template <class Range, class Accumulator>
void CoAccumulate(const Range& r, Accumulator& result)
{
	typedef ConstIteratorOf<Range> I;
	CoPartition(r.begin(), r.end(),
		[=, &result](I i, I e) {
			Accumulator h;
			while(i < e)
				h(*i++);
			CoWork::FinLock();
			result(h);
		}
	);
}

template <class Range>
ValTypeOf<Range> CoSum(const Range& r, const ValTypeOf<Range>& zero)
{
	typedef ValTypeOf<Range> VT;
	typedef ConstIteratorOf<Range> I;
	VT sum = zero;
	CoPartition(r.begin(), r.end(),
		[=, &sum](I i, I e) {
			VT h = zero;
			while(i < e)
				h += *i++;
			CoWork::FinLock();
			sum += h;
		}
	);
	return sum;
}

template <class T>
ValTypeOf<T> CoSum(const T& c)
{
	return CoSum(c, (ValTypeOf<T>)0);
}

template <class Range, class V>
i32 CoCount(const Range& r, const V& val)
{
	i32 count = 0;
	CoPartition(r, [=, &val, &count](const SubRangeOf<const Range>& r) {
		i32 n = Count(r, val);
		CoWork::FinLock();
		count += n;
	});
	return count;
}

template <class Range, class Predicate>
i32 CoCountIf(const Range& r, const Predicate& p)
{
	i32 count = 0;
	CoPartition(r, [=, &p, &count](const SubRangeOf<const Range>& r) {
		i32 n = CountIf(r, p);
		CoWork::FinLock();
		count += n;
	});
	return count;
}

template <class Range, class Better>
i32 CoFindBest(const Range& r, const Better& better)
{
	if(r.GetCount() <= 0)
		return -1;
	typedef ConstIteratorOf<Range> I;
	I best = r.begin();
	CoPartition(r.begin() + 1, r.end(),
		[=, &best](I i, I e) {
			I b = i++;
			while(i < e) {
				if(better(*i, *b))
					b = i;
				i++;
			}
			CoWork::FinLock();
			if(better(*b, *best) || (!better(*best, *b) && b < best))
				best = b;
		}
	);
	return best - r.begin();
}

template <class Range>
i32 CoFindMin(const Range& r)
{
	return CoFindBest(r, std::less<ValTypeOf<Range>>());
}

template <class Range>
const ValTypeOf<Range>& CoMin(const Range& r)
{
	return r[CoFindMin(r)];
}

template <class Range>
const ValTypeOf<Range>& CoMin(const Range& r, const ValTypeOf<Range>& def)
{
	i32 q = CoFindMin(r);
	return q < 0 ? def : r[q];
}


template <class Range>
i32 CoFindMax(const Range& r)
{
	return CoFindBest(r, std::greater<ValTypeOf<Range>>());
}

template <class Range>
const ValTypeOf<Range>& CoMax(const Range& r)
{
	return r[CoFindMax(r)];
}

template <class Range>
const ValTypeOf<Range>& CoMax(const Range& r, const ValTypeOf<Range>& def)
{
	i32 q = CoFindMax(r);
	return q < 0 ? def : r[q];
}

template <class Range, class Match>
i32 CoFindMatch(const Range& r, const Match& eq, i32 from = 0)
{
	i32 count = r.GetCount();
	std::atomic<i32> found;
	found = count;
	CoPartition(from, count,
		[=, &r, &found](i32 i, i32 e) {
			for(; i < e; i++) {
				if(found < i)
					break;
				if(eq(r[i])) {
					CoWork::FinLock();
					if(i < found)
						found = i;
					return;
				}
			}
		}
	);
	return found < (i32)count ? (i32)found : -1;
}

template <class Range, class V>
i32 CoFindIndex(const Range& r, const V& value, i32 from = 0)
{
	return CoFindMatch(r, [=, &value](const ValTypeOf<Range>& m) { return m == value; }, from);
}

template <class Range1, class Range2>
i32 CoIsEqualRange(const Range1& r1, const Range2& r2)
{
	if(r1.GetCount() != r2.GetCount())
		return false;
	std::atomic<bool> equal(true);
	CoPartition(0, r1.GetCount(),
		[=, &equal, &r1, &r2](i32 i, i32 e) {
			while(i < e && equal) {
				if(r1[i] != r2[i]) {
					CoWork::FinLock();
					equal = false;
				}
				i++;
			}
		}
	);
	return equal;
}

template <class Range, class Predicate>
Vec<i32> CoFindAll(const Range& r, Predicate match, i32 from = 0)
{
	Vec<Vec<i32>> rs;
	i32 total = 0;
	CoPartition(from, r.GetCount(), [=, &r, &rs, &total](i32 begin, i32 end) {
		Vec<i32> v = FindAll(SubRange(r, 0, end), match, begin);
		CoWork::FinLock();
		if(v.GetCount()) {
			total += v.GetCount();
			rs.Add(pick(v));
		}
	});
	CoSort(rs, [](const Vec<i32>& a, const Vec<i32>& b) { return a[0] < b[0]; });
	Vec<i32> result;
	for(const auto& s : rs)
		result.Append(s);
	return result;
}
