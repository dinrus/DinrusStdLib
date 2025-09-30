template <class T>
T *DeclPtr__();

template <class Range>
using ValTypeOf = typename std::remove_reference<decltype(*DeclPtr__<typename std::remove_reference<Range>::type>()->begin())>::type;

template <class Range>
using IteratorOf = decltype(DeclPtr__<typename std::remove_reference<Range>::type>()->begin());

template <class Range>
using ConstIteratorOf = decltype(DeclPtr__<const typename std::remove_reference<Range>::type>()->begin());

template <class I>
class SubRangeClass {
	I   l;
	i32 count;

public:
	typedef typename std::remove_reference<decltype(*l)>::type value_type;

	i32 GetCount() const { return count; }

	SubRangeClass& Write()                   { return *this; }

	value_type& operator[](i32 i) const { ASSERT(i >= 0 && i < count); return l[i]; }
	I  begin() const { return l; }
	I  end() const { return l + count; }

	Txt   ToTxt() const                            { return AsTxtArray(*this); }
	template <class B> bool operator==(const B& b) const { return IsEqualRange(*this, b); }
#ifndef CPP_20
	template <class B> bool operator!=(const B& b) const { return !operator==(b); }
#endif
	template <class B> i32  Compare(const B& b) const    { return CompareRanges(*this, b); }
	template <class B> bool operator<=(const B& x) const { return Compare(x) <= 0; }
	template <class B> bool operator>=(const B& x) const { return Compare(x) >= 0; }
	template <class B> bool operator<(const B& x) const  { return Compare(x) < 0; }
	template <class B> bool operator>(const B& x) const  { return Compare(x) > 0; }

	SubRangeClass(I begin, i32 count) : l(begin), count(count)   {}
	SubRangeClass(I begin, I end) : l(begin), count(i32(end - begin)) {}
	SubRangeClass() {} // MSC bug workaround
};

template <class I>
SubRangeClass<I> SubRange(I l, I h)
{
	return SubRangeClass<I>(l, h);
}

template <class I>
SubRangeClass<I> SubRange(I l, i32 count)
{
	return SubRangeClass<I>(l, count);
}

template <class C>
auto SubRange(C&& c, i32 pos, i32 count) -> decltype(SubRange(c.begin() + pos, count))
{
	return SubRange(c.begin() + pos, count);
}

template <class C>
auto SubRangeFrom(C&& c, i32 pos) -> decltype(SubRange(c.begin() + pos, c.GetCount() - pos))
{
	return SubRange(c.begin() + pos, c.GetCount() - pos);
}

template <class C> using SubRangeOf = decltype(SubRange(((C *)0)->begin(), ((C *)0)->end()));

template <class T>
struct ConstRangeClass {
	T   value;
	i32 count;

	typedef T value_type;
	typedef value_type ValType;

	const value_type& operator[](i32 i) const { return value; }
	i32 GetCount() const                      { return count; }

	typedef ConstIIterator<ConstRangeClass> Iterator;

	Iterator begin() const { return Iterator(*this, 0); }
	Iterator end() const { return Iterator(*this, count); }

	Txt   ToTxt() const                            { return AsTxtArray(*this); }
	template <class B> bool operator==(const B& b) const { return IsEqualRange(*this, b); }
#ifndef CPP_20
	template <class B> bool operator!=(const B& b) const { return !operator==(b); }
#endif
	template <class B> i32  Compare(const B& b) const    { return CompareRanges(*this, b); }
	template <class B> bool operator<=(const B& x) const { return Compare(x) <= 0; }
	template <class B> bool operator>=(const B& x) const { return Compare(x) >= 0; }
	template <class B> bool operator<(const B& x) const  { return Compare(x) < 0; }
	template <class B> bool operator>(const B& x) const  { return Compare(x) > 0; }

	ConstRangeClass(const T& value, i32 count) : value(value), count(count) {}
	ConstRangeClass(i32 count) : count(count) {}
	ConstRangeClass() {} // MSC bug workaround
};

template <class T>
ConstRangeClass<T> ConstRange(const T& value, i32 count)
{
	return ConstRangeClass<T>(value, count);
}

template <class T>
ConstRangeClass<T> ConstRange(i32 count)
{
	return ConstRangeClass<T>(count);
}

template <class BaseRange>
struct ReverseRangeClass {
	typename std::remove_reference<BaseRange>::type& r;

	typedef ValTypeOf<BaseRange>                       value_type;
	typedef value_type                                   ValType;

	const value_type& operator[](i32 i) const            { return r[r.GetCount() - i - 1]; }
	value_type& operator[](i32 i)                        { return r[r.GetCount() - i - 1]; }
	i32 GetCount() const                                 { return r.GetCount(); }

	typedef IIterator<ReverseRangeClass>                  Iterator;
	typedef ConstIIterator<ReverseRangeClass>             ConstIterator;

	ReverseRangeClass& Write()                           { return *this; }

	ConstIterator begin() const                          { return ConstIterator(*this, 0); }
	ConstIterator end() const                            { return ConstIterator(*this, r.GetCount()); }

	Iterator begin()                                     { return Iterator(*this, 0); }
	Iterator end()                                       { return Iterator(*this, r.GetCount()); }

	Txt   ToTxt() const                            { return AsTxtArray(*this); }
	template <class B> bool operator==(const B& b) const { return IsEqualRange(*this, b); }
#ifndef CPP_20
	template <class B> bool operator!=(const B& b) const { return !operator==(b); }
#endif
	template <class B> i32  Compare(const B& b) const    { return CompareRanges(*this, b); }
	template <class B> bool operator<=(const B& x) const { return Compare(x) <= 0; }
	template <class B> bool operator>=(const B& x) const { return Compare(x) >= 0; }
	template <class B> bool operator<(const B& x) const  { return Compare(x) < 0; }
	template <class B> bool operator>(const B& x) const  { return Compare(x) > 0; }

	ReverseRangeClass(BaseRange& r) : r(r) {}
	ReverseRangeClass() {} // MSC bug workaround
};

template <class BaseRange>
ReverseRangeClass<BaseRange> ReverseRange(BaseRange&& r)
{
	return ReverseRangeClass<BaseRange>(r);
}

template <class BaseRange>
struct ViewRangeClass {
	typename std::remove_reference<BaseRange>::type *r;
	Vec<i32> ndx;

	typedef ValTypeOf<BaseRange> value_type;
	typedef value_type ValType;

	const value_type& operator[](i32 i) const { return (*r)[ndx[i]]; }
	value_type& operator[](i32 i)             { return (*r)[ndx[i]]; }
	i32 GetCount() const                      { return ndx.GetCount(); }

	typedef IIterator<ViewRangeClass> Iterator;
	typedef ConstIIterator<ViewRangeClass> ConstIterator;

	ViewRangeClass& Write()                   { return *this; }

	ConstIterator begin() const { return ConstIterator(*this, 0); }
	ConstIterator end() const  { return ConstIterator(*this, ndx.GetCount()); }

	Iterator begin() { return Iterator(*this, 0); }
	Iterator end()   { return Iterator(*this, ndx.GetCount()); }

	Txt   ToTxt() const                            { return AsTxtArray(*this); }
	template <class B> bool operator==(const B& b) const { return IsEqualRange(*this, b); }
#ifndef CPP_20
	template <class B> bool operator!=(const B& b) const { return !operator==(b); }
#endif
	template <class B> i32  Compare(const B& b) const    { return CompareRanges(*this, b); }
	template <class B> bool operator<=(const B& x) const { return Compare(x) <= 0; }
	template <class B> bool operator>=(const B& x) const { return Compare(x) >= 0; }
	template <class B> bool operator<(const B& x) const  { return Compare(x) < 0; }
	template <class B> bool operator>(const B& x) const  { return Compare(x) > 0; }

	ViewRangeClass(BaseRange& r, Vec<i32>&& ndx) : r(&r), ndx(pick(ndx)) {}
	ViewRangeClass() {} // MSC bug workaround
};

template <class BaseRange>
ViewRangeClass<BaseRange> ViewRange(BaseRange&& r, Vec<i32>&& ndx)
{
	return ViewRangeClass<BaseRange>(r, pick(ndx));
}

template <class BaseRange, class Predicate>
ViewRangeClass<BaseRange> FilterRange(BaseRange&& r, Predicate p)
{
	return ViewRangeClass<BaseRange>(r, FindAll(r, p));
}

template <class BaseRange, class Predicate>
ViewRangeClass<BaseRange> SortedRange(BaseRange&& r, Predicate p)
{
	return ViewRangeClass<BaseRange>(r, GetSortOrder(r, p));
}

template <class BaseRange>
ViewRangeClass<BaseRange> SortedRange(BaseRange&& r)
{
	return ViewRangeClass<BaseRange>(r, GetSortOrder(r));
}