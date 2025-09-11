force_inline
void IndexCommon::Link(i32& m, Hash& hh, i32 ii)
{
	if(m < 0)
		m = hh.prev = hh.next = ii;
	else {
		hh.next = m;
		hh.prev = hash[m].prev;
		hash[hh.prev].next = ii;
		hash[m].prev = ii;
	}
}

force_inline
void IndexCommon::Link(i32 ii, dword sh)
{
	Link(map[sh & mask], hash[ii], ii);
}

force_inline
void IndexCommon::Del(i32& m, Hash& hh, i32 ii)
{ // unlink from m
	if(ii == m) { // this is item pointed by map
		if(hh.next == ii) { // this is the only item in the bucket
			m = -1; // bucket is now empty
			return;
		}
		m = hh.next; // move bucket pointer to the next item
	}
	hash[hh.next].prev = hh.prev; // unlink
	hash[hh.prev].next = hh.next;
}

template <typename T>
never_inline
void Index<T>::ReallocHash(i32 n)
{ // realloc hash to have the same capacity as key, copy n elements from previous alloc
	if(key.GetAlloc()) {
		size_t sz = key.GetAlloc() * sizeof(Hash);
		if(!MemoryTryRealloc(hash, sz)) {
			Hash *h = (Hash *)MemoryAlloc(sz);
			if(hash) {
				if(n)
					memcpy_t(h, hash, n);
				MemoryFree(hash);
			}
			hash = h;
		}
	}
	else {
		MemoryFree(hash);
		hash = NULL;
	}
}

template <typename T>
never_inline
void Index<T>::FixHash(bool makemap)
{
	ReallocHash(0);
	unlinked = -1;
	for(i32 i = 0; i < key.GetCount(); i++)
		hash[i].hash = Smear(key[i]);
	if(makemap)
		MakeMap(key.GetCount());
	else
		Remap(key.GetCount());
}

template <typename T>
template <typename U>
never_inline
void Index<T>::GrowAdd(U&& k, dword sh)
{
	i32 n = key.GetCount();
	key.GrowAdd(std::forward<U>(k));
	ReallocHash(n);
}

template <typename T>
template <typename U>
void Index<T>::AddS(i32& m, U&& k, dword sh)
{
	i32 ii = key.GetCount();
	if(ii >= key.GetAlloc())
		GrowAdd(std::forward<U>(k), sh);
	else
		new(key.Rdd()) T(std::forward<U>(k));
	Hash& hh = hash[ii];
	hh.hash = sh;
	if(ii >= (i32)mask)
		GrowMap(key.GetCount());
	else
		Link(m, hh, ii);
}

template <typename T>
template <typename U>
void Index<T>::AddS(U&& k, dword sh)
{
	AddS(map[sh & mask], std::forward<U>(k), sh);
}

template <typename T>
force_inline
i32 Index<T>::FindFrom(i32 i, dword sh, const T& k, i32 end) const
{
	if(i >= 0)
		do {
			if(key[i] == k)
				return i;
			i = hash[i].next;
		}
		while(i != end);
	return -1;
}

template <class T>
i32 Index<T>::Find(const T& k) const
{
	dword sh = Smear(k);
	i32& m = map[sh & mask];
	return FindFrom(m, sh, k, m);
}

template <class T>
i32 Index<T>::FindNext(i32 i) const
{
	const Hash& hh = hash[i];
	i32 end = map[hash[i].hash & mask];
	return hh.next == end ? -1 : FindFrom(hh.next, hh.hash, key[i], end);
}

template <typename T>
force_inline
i32 Index<T>::FindBack(i32 i, dword sh, const T& k, i32 end) const
{
	do {
		const Hash& ih = hash[i];
		if(key[i] == k)
			return i;
		i = ih.prev;
	}
	while(i != end);
	return -1;
}

template <class T>
i32 Index<T>::FindLast(const T& k) const
{
	dword sh = Smear(k);
	i32& m = map[sh & mask];
	return m < 0 ? -1 : FindBack(hash[m].prev, sh, k, hash[m].prev);
}

template <class T>
i32 Index<T>::FindPrev(i32 i) const
{
	const Hash& hh = hash[i];
	i32 end = map[hash[i].hash & mask];
	return hh.prev == hash[end].prev ? -1 : FindBack(hh.prev, hh.hash, key[i], hash[end].prev);
}

template <class T>
template <class OP, class U>
force_inline
i32 Index<T>::FindAdd(U&& k, OP op) {
	dword sh = Smear(k);
	i32& m = map[sh & mask];
	i32 i = m;
	if(i >= 0)
		do {
			if(key[i] == k)
				return i;
			i = hash[i].next;
		}
		while(i != m);
	i = key.GetCount();
	AddS(m, std::forward<U>(k), sh);
	op();
	return i;
}

template <typename T>
void Index<T>::Unlink(i32 ii)
{
	Hash& hh = hash[ii];
	Del(map[hh.hash & mask], hh, ii);
	Link(unlinked, hh, ii);
	hh.hash = 0;
}

template <typename T>
i32 Index<T>::UnlinkKey(const T& k)
{
	dword sh = Smear(k);
	i32& m = map[sh & mask];
	i32 i = m;
	i32 n = 0;
	if(i >= 0)
		for(;;) {
			Hash& hh = hash[i];
			i32 ni = hh.next;
			if(key[i] == k) {
				Del(m, hh, i);
				Link(unlinked, hh, i);
				n++;
				hh.hash = 0;
				if(ni == i) // last item removed
					break;
				i = ni;
			}
			else {
				i = ni;
				if(i == m)
					break;
			}
		}
	return n;
}

template <typename T>
template <typename U>
i32 Index<T>::Put0(U&& k, dword sh)
{
	i32 i;
	if(HasUnlinked()) {
		i = hash[unlinked].prev;
		Hash& hh = hash[i];
		Del(unlinked, hh, i);
		Link(map[sh & mask], hh, i);
		hh.hash = sh;
		key[i] = std::forward<U>(k);
	}
	else {
		i = GetCount();
		AddS(std::forward<U>(k), sh);
	}
	return i;
}

template <class T>
template <class U>
force_inline
i32 Index<T>::FindPut0(U&& k, bool& put)
{
	dword sh = Smear(k);
	i32& m = map[sh & mask];
	i32 i = m;
	put = false;
	if(i >= 0)
		do {
			if(key[i] == k)
				return i;
			i = hash[i].next;
		}
		while(i != m);
	put = true;
	return Put0(std::forward<U>(k), sh);
}

template <typename T>
template <typename U>
void Index<T>::Set0(i32 ii, U&& k)
{
	Hash& hh = hash[ii];
	if(IsUnlinked(ii))
		Del(unlinked, hh, ii);
	else
		Del(map[hh.hash & mask], hh, ii);

	dword sh = Smear(k);
	hh.hash = sh;
	Link(map[sh & mask], hh, ii);
	key[ii] = std::forward<U>(k);
}

template <typename T>
never_inline
void Index<T>::Sweep()
{
	if(unlinked >= 0) {
		i32 n = key.GetCount();
		key.RemoveIf([&](i32 i) { return hash[i].hash == 0; });
		IndexCommon::Sweep(n);
	}
}

template <typename T>
never_inline
void Index<T>::Reserve(i32 n)
{
	i32 a = key.GetAlloc();
	key.Reserve(n);
	if(a != key.GetAlloc()) {
		ReallocHash(key.GetCount());
		AdjustMap(key.GetCount(), n);
	}
}

template <typename T>
never_inline
void Index<T>::Shrink()
{
	i32 a = key.GetAlloc();
	key.Shrink();
	if(a != key.GetAlloc()) {
		ReallocHash(key.GetCount());
		AdjustMap(key.GetCount(), key.GetCount());
	}
}

template <typename T>
void Index<T>::Remove(i32k *sorted_list, i32 count)
{
	if(HasUnlinked()) {
		Vec<bool> u;
		u.SetCount(GetCount());
		for(i32 i = 0; i < GetCount(); i++)
			u[i] = IsUnlinked(i);
		key.Remove(sorted_list, count);
		u.Remove(sorted_list, count);
		FixHash(false);
		for(i32 i = 0; i < GetCount(); i++)
			if(u[i])
				Unlink(i);
	}
	else {
		key.Remove(sorted_list, count);
		FixHash(false);
	}
}

template <typename T>
never_inline
void Index<T>::Serialize(Stream& s)
{
	key.Serialize(s);
	if(s.IsLoading())
		FixHash();

	i32 version = 1;
	s / version;
	if(version == 0) { // support previous version
		Vec<unsigned> h;
		h.Serialize(s);
		if(s.IsLoading())
			for(i32 i = 0; i < h.GetCount(); i++)
				if(i < GetCount() && h[i] & 0x80000000)
					Unlink(i);
	}
	else {
		Vec<i32> u = GetUnlinked();
		u.Serialize(s);
		if(s.IsLoading())
			for(i32 i : ReverseRange(u)) { // Reverse range to ensure the correct order of Put
				if(i >= 0 && i < GetCount())
					Unlink(i);
				else
					s.LoadError();
			}
	}
}

template <class T>
void Index<T>::Xmlize(XmlIO& xio, tukk itemtag)
{
	XmlizeIndex<T, Index<T> >(xio, itemtag, *this);
}

template <class T>
void Index<T>::Jsonize(JsonIO& jio)
{
	JsonizeIndex<Index<T>, T>(jio, *this);
}

template <class T>
Txt Index<T>::ToTxt() const
{
	return AsTxtArray(*this);
}

#ifdef _DEBUG
template <typename T>
Txt Index<T>::Dump() const
{
	Txt h;
	for(i32 i = 0; i < key.GetCount(); i++) {
		if(i)
			h << "; ";
		if(IsUnlinked(i))
			h << "#";
		h << i << ": " << key[i] << '/' << (hash[i].hash & mask) << " -> " << hash[i].prev << ":" << hash[i].next;
	}
	return h;
}
#endif