template <class T, class Less>
i32 SortedIndex<T, Less>::FindAdd(const T& key)
{
	i32 i = FindLowerBound(key);
	if(i == GetCount() || Less()(key, iv[i]))
		iv.Insert(i, key);
	return i;
}

template <class T, class Less>
i32 SortedIndex<T, Less>::FindNext(i32 i) const
{
	return i + 1 < iv.GetCount() && !Less()(iv[i], iv[i + 1]) ? i + 1 : -1;
}

template <class T, class Less>
i32 SortedIndex<T, Less>::FindLast(const T& x) const
{
	i32 i = iv.FindUpperBound(x, Less());
	return i > 0 && !Less()(iv[i - 1], x) ? i - 1 : -1;
}

template <class T, class Less>
i32 SortedIndex<T, Less>::FindPrev(i32 i) const
{
	return i > 0 && !Less()(iv[i - 1], iv[i]) ? i - 1 : -1;
}

template <class T, class Less>
i32 SortedIndex<T, Less>::RemoveKey(const T& x)
{
	i32 l = FindLowerBound(x);
	i32 count = FindUpperBound(x) - l;
	Remove(l, count);
	return count;
}

template <class T, class Less>
Txt SortedIndex<T, Less>::ToTxt() const
{
	return AsTxtArray(*this);
}

template <class T>
void Slaved_InVector__<T>::Insert(i32 blki, i32 pos)
{
	data.data[blki].Insert(pos);
	res = &data.data[blki][pos];
}

template <class T>
void Slaved_InVector__<T>::Split(i32 blki, i32 nextsize)
{
	Vec<T>& x = data.data.Insert(blki + 1);
	x.InsertSplit(0, data.data[blki], nextsize);
	data.data[blki].Shrink();
}

template <class T>
void Slaved_InVector__<T>::AddFirst()
{
	data.data.Add().Add();
	res = &data.data[0][0];
}

template <class K, class T, class Less, class Data>
Txt SortedAMap<K, T, Less, Data>::ToTxt() const
{
	Txt r;
	r = "{";
	for(i32 i = 0; i < GetCount(); i++) {
		if(i)
			r << ", ";
		r << GetKey(i) << ": " << (*this)[i];
	}
	r << '}';
	return r;
}

template <class K, class T, class Less>
SortedVecMap<K, T, Less>::SortedVecMap(SortedVecMap&& s)
{
	B::key = pick(s.key);
	B::value.data = pick(s.value.data);
	B::SetSlave();
}

template <class K, class T, class Less>
SortedVecMap<K, T, Less>& SortedVecMap<K, T, Less>::operator=(SortedVecMap&& s)
{
	B::key = pick(s.key);
	B::value.data = pick(s.value.data);
	B::SetSlave();
	return *this;
}

template <class K, class T, class Less>
SortedVecMap<K, T, Less>::SortedVecMap(const SortedVecMap& s, i32)
{
	B::key = clone(s.key);
	B::value.data = clone(s.value.data);
	B::SetSlave();
}

template <class K, class T, class Less>
i32 SortedVecMap<K, T, Less>::FindAdd(const K& k, const T& init)
{
	B::value.res = NULL;
	i32 q = B::key.FindAdd(k);
	if(B::value.res)
		*B::value.res = init;
	return q;
}

template <class K, class T, class Less>
void SortedVecMap<K, T, Less>::Swap(SortedVecMap& x)
{
	B::value.data.Swap(x.value.data);
	B::key.Swap(x.B::key);
	B::SetSlave();
	x.SetSlave();
}
               
template <class T>
void Slaved_InArray__<T>::Insert(i32 blki, i32 pos)
{
	if(!res)
		res = new T;
	data.iv.data[blki].Insert(pos, res);
}

template <class T>
void Slaved_InArray__<T>::Split(i32 blki, i32 nextsize)
{
	Vec< typename InArray<T>::PointerType >& x = data.iv.data.Insert(blki + 1);
	x.InsertSplit(0, data.iv.data[blki], nextsize);
}

template <class T>
void Slaved_InArray__<T>::Remove(i32 blki, i32 pos, i32 n)
{
	Vec< typename InArray<T>::PointerType >& b = data.iv.data[blki];
	for(i32 i = 0; i < n; i++)
		if(b[i + pos])
			delete (T *)b[i + pos];
	b.Remove(pos, n);
}

template <class T>
void Slaved_InArray__<T>::AddFirst()
{
	if(!res)
		res = new T;
	data.iv.data.Add().Add(res);
}

template <class K, class T, class Less>
i32 SortedArrayMap<K, T, Less>::FindAdd(const K& k, const T& init)
{
	B::value.res = NULL;
	i32 q = B::key.FindAdd(k);
	if(B::value.res)
		*B::value.res = init;
	return q;
}

template <class K, class T, class Less>
SortedArrayMap<K, T, Less>::SortedArrayMap(SortedArrayMap&& s)
{
	B::key = pick(s.key);
	B::value.data = pick(s.value.data);
	B::SetSlave();
}

template <class K, class T, class Less>
SortedArrayMap<K, T, Less>& SortedArrayMap<K, T, Less>::operator=(SortedArrayMap&& s)
{
	B::key = pick(s.key);
	B::value.data = pick(s.value.data);
	B::SetSlave();
	return *this;
}

template <class K, class T, class Less>
SortedArrayMap<K, T, Less>::SortedArrayMap(const SortedArrayMap& s, i32)
{
	B::key = clone(s.key);
	B::value.data = clone(s.value.data);
	B::SetSlave();
}

template <class K, class T, class Less>
void SortedArrayMap<K, T, Less>::Swap(SortedArrayMap& x)
{
	B::value.data.Swap(x.value.data);
	B::key.Swap(x.B::key);
	B::SetSlave();
	x.SetSlave();
}

#ifdef DRX
template <class K, class T>
void StreamSortedMap(Stream& s, T& cont)
{
	i32 n = cont.GetCount();
	s / n;
	if(n < 0) {
		s.LoadError();
		return;
	}
	if(s.IsLoading()) {
		cont.Clear();
		while(n--) {
			K key;
			s % key;
			s % cont.Add(key);
		}
	}
	else
		for(i32 i = 0; i < cont.GetCount(); i++) {
			K key = cont.GetKey(i);
			s % key;
			s % cont[i];
		}
}

template <class K, class T, class Less>
void SortedVecMap<K, T, Less>::Serialize(Stream& s) {
	StreamSortedMap<K, SortedVecMap<K, T, Less> >(s, *this);
}

template <class K, class T, class Less>
void SortedVecMap<K, T, Less>::Xmlize(XmlIO& xio)
{
	XmlizeSortedMap<K, T, SortedVecMap<K, T, Less> >(xio, "key", "value", *this);
}

template <class K, class T, class Less>
void SortedVecMap<K, T, Less>::Jsonize(JsonIO& jio)
{
	JsonizeSortedMap<SortedVecMap<K, T, Less>, K, T>(jio, *this, "key", "value");
}

template <class K, class T, class Less>
void SortedArrayMap<K, T, Less>::Serialize(Stream& s) {
	StreamSortedMap<K, SortedArrayMap<K, T, Less> >(s, *this);
}

template <class K, class T, class Less>
void SortedArrayMap<K, T, Less>::Xmlize(XmlIO& xio)
{
	XmlizeSortedMap<K, T, SortedArrayMap<K, T, Less> >(xio, "key", "value", *this);
}

template <class K, class T, class Less>
void SortedArrayMap<K, T, Less>::Jsonize(JsonIO& jio)
{
	JsonizeSortedMap<SortedArrayMap<K, T, Less>, K, T>(jio, *this, "key", "value");
}

#endif
