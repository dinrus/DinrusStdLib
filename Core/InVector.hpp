#define LLOG(x)   // DLOG(x)

z64 NewInVectorSerial();

template <class T>
void InVector<T>::Init()
{
	serial = NewInVectorSerial();
	slave = 0;
	Reset();
}

template <class T>
InVector<T>::InVector()
{
	Init();
}

template <class T>
void InVector<T>::Reset()
{
	hcount = count = 0;
	SetBlkPar();
}

template <class T>
void InVector<T>::Clear()
{
	if(slave)
		Slave()->Clear();
	data.Clear();
	index.Clear();
	Reset();
	ClearCache();
}

void SetInvectorCache__(z64 serial, i32 blki, i32 offset, i32 end);
void ClearInvectorCache__();
i32  FindInvectorCache__(z64 serial, i32& pos, i32& off);

template <class T>
force_inline void InVector<T>::SetCache(i32 blki, i32 offset) const
{
#ifdef flagIVTEST
	Check(0, 0);
#endif
	SetInvectorCache__(serial, blki, offset, offset + data[blki].GetCount());
}

template <class T>
force_inline void InVector<T>::ClearCache() const
{
	ClearInvectorCache__();
}

template <class T>
force_inline i32 InVector<T>::FindBlock(i32& pos, i32& off) const
{
	i32 i = FindInvectorCache__(serial, pos, off);
	return i >= 0 ? i : FindBlock0(pos, off);
}

template <class T>
i32 InVector<T>::FindBlock0(i32& pos, i32& off) const
{
	LLOG("FindBlock " << pos);
	ASSERT(pos >= 0 && pos <= count);
	if(pos == count) {
		LLOG("Found last");
		pos = data.Top().GetCount();
		off = count - pos;
		return data.GetCount() - 1;
	}
	i32 blki = 0;
	i32 offset = 0;
	for(i32 i = index.GetCount(); --i >= 0;) {
		i32 n = index[i][blki];
		if(pos >= n) {
			blki++;
			pos -= n;
			offset += n;
		}
		blki += blki;
	}
	i32 n = data[blki].GetCount();
	if(pos >= n) {
		blki++;
		pos -= n;
		offset += n;
	}

	SetCache(blki, offset);

	off = offset;
	return blki;
}

template <class T>
force_inline i32 InVector<T>::FindBlock(i32& pos) const
{
	i32 h;
	return FindBlock(pos, h);
}

template <class T>
const T& InVector<T>::operator[](i32 i) const
{
	LLOG("operator[] " << i);
	ASSERT(i >= 0 && i < count);
	i32 blki = FindBlock(i);
	return data[blki][i];
}

template <class T>
T& InVector<T>::operator[](i32 i)
{
	LLOG("operator[] " << i);
	ASSERT(i >= 0 && i < count);
	i32 blki = FindBlock(i);
	return data[blki][i];
}

template <class T>
void InVector<T>::Reindex()
{
	LLOG("--- Reindexing");
	ClearCache();
	SetBlkPar();
	index.Clear();
	if(slave)
		Slave()->Reindex();
	hcount = 0;
	Vec<T> *ds = data.Begin();
	Vec<T> *dend = data.End();
	i32 n = data.GetCount();
	if(n <= 2)
		return;
	Vec<i32>& w = index.Add();
	hcount = 2;
	w.SetCount((n + 1) >> 1);
	i32 *t = w.Begin();
	while(ds != dend) {
		*t = (ds++)->GetCount();
		if(ds == dend)
			break;
		*t++ += (ds++)->GetCount();
	}
	i32 *s = w.Begin();
	i32 *end = w.End();
	n = w.GetCount();
	while(n > 2) {
		Vec<i32>& w = index.Add();
		hcount += hcount;
		w.SetCount((n + 1) >> 1);
		t = w.Begin();
		while(s != end) {
			*t = *s++;
			if(s == end)
				break;
			*t++ += *s++;
		}
		s = w.Begin();
		end = w.End();
		n = w.GetCount();
	}
#ifdef flagIVTEST
	Check(0, 0);
#endif
}

template <class T>
void InVector<T>::SetBlkPar()
{
#if defined(_DEBUG) && defined(flagIVTEST)
	blk_high = 11;
	blk_low = 5;
#else
	i32 n = 2500 + data.GetCount() / 4;
	blk_high = minmax(n / (i32)sizeof(T), 16, 65000);
	blk_low = minmax(n / 3 / (i32)sizeof(T), 16, 65000);
#endif
}

template <class T>
void InVector<T>::Index(i32 q, i32 n)
{
	for(i32 i = 0; i < index.GetCount(); i++)
		index[i].At(q >>= 1, 0) += n;
}

template <class T>
T *InVector<T>::Insert0(i32 ii, i32 blki, i32 pos, i32 off, const T *val)
{
	if(data[blki].GetCount() > blk_high) {
		if(slave)
			Slave()->Split(blki, data[blki].GetCount() / 2);
		Vec<T>& x = data.Insert(blki + 1);
		x.InsertSplit(0, data[blki], data[blki].GetCount() / 2);
		data[blki].Shrink();
		Reindex();
		pos = ii;
		blki = FindBlock(pos, off);
	}
	LLOG("blki: " << blki << ", pos: " << pos);
	count++;
	if(slave) {
		Slave()->Count(1);
		Slave()->Index(blki, 1);
		Slave()->Insert(blki, pos);
	}
	Index(blki, 1);
	if(val)
		data[blki].Insert(pos, *val);
	else
		data[blki].Insert(pos);
	SetCache(blki, off);
	return &data[blki][pos];
}

template <class T>
T *InVector<T>::Insert0(i32 ii, const T *val)
{
	ASSERT(ii >= 0 && ii <= GetCount());
	if(data.GetCount() == 0) {
		count++;
		ClearCache();
		if(slave) {
			Slave()->Count(1);
			Slave()->AddFirst();
		}
		if(val) {
			data.Add().Add(*val);
			return &data[0][0];
		}
		return &data.Add().Add();
	}
	i32 pos = ii;
	i32 off;
	i32 blki = FindBlock(pos, off);
	return Insert0(ii, blki, pos, off, val);
}

template <class T>
template <class Range>
void InVector<T>::Insert_(i32 ii, const Range& r, bool def)
{
	i32 n = r.GetCount();

	ASSERT(ii >= 0 && ii <= GetCount() && n >= 0 && !slave);

	if(n <= 0)
		return;
	
	auto s = r.begin();

	if(data.GetCount() == 0) {
		i32 m = (blk_high + blk_low) / 2;
		count = n;
		while(n > 0) {
			i32 n1 = min(m, n);
			if(def)
				data.Add().SetCount(n1);
			else
				data.Add().AppendRange(SubRange(s, n1));
			s += n1;
			n -= n1;
		}
		Reindex();
		return;
	}

	i32 pos = ii;
	i32 off;
	i32 blki = FindBlock(pos, off);
	i32 bc = data[blki].GetCount();
	
	count += n;

	if(bc + n < blk_high) { // block will not be bigger than threshold after insert
		if(def)
			data[blki].InsertN(pos, n);
		else
			data[blki].InsertRange(pos, SubRange(s, n));
		Index(blki, n);
		SetCache(blki, off);
	}
	else
	if(bc - pos + n < blk_high) { // splitting into 2 blocks is enough
		Vec<T>& t = data.Insert(blki + 1);
		if(def)
			t.InsertN(0, n);
		else
			t.InsertRange(0, SubRange(s, n));
		t.InsertSplit(n, data[blki], pos);
		data[blki].Shrink();
		Reindex();
	}
	else { // need to insert several blocks
		i32 m = (blk_high + blk_low) / 2;
		i32 bn = (n + m - 1) / m;
		i32 ti;
		if(pos) { // need to split first block
			ti = blki + 1; // TODO should add some of data to splitted blocks
			data.InsertN(ti, bn + 1);
			data[ti + bn].InsertSplit(0, data[blki], pos);
			data[blki].Shrink();
		}
		else {
			ti = blki;
			data.InsertN(ti, bn);
		}
		for(i32 i = 0; i < bn; i++) {
			i32 q = min(m, n);
			if(def)
				data[ti + i].SetCount(q);
			else
				data[ti + i].AppendRange(SubRange(s, q));
			s += q;
			n -= q;
		}
		ASSERT(n == 0);
		Reindex();
	}
#ifdef flagIVTEST
	Check(0, 0);
#endif
}

template <class T>
void InVector<T>::Join(i32 blki)
{
	data[blki].AppendPick(pick(data[blki + 1]));
	data.Remove(blki + 1);
}

template <class T>
force_inline bool InVector<T>::JoinSmall(i32 blki)
{
	if(blki < data.GetCount()) {
		i32 n = data[blki].GetCount();
		if(n == 0) {
			if(slave)
				Slave()->RemoveBlk(blki, 1);
			data.Remove(blki);
			return true;
		}
		if(n < blk_low) {
			if(blki > 0 && data[blki - 1].GetCount() + n <= blk_high) {
				if(slave)
					Slave()->Join(blki - 1);
				Join(blki - 1);
				return true;
			}
			if(blki + 1 < data.GetCount() && n + data[blki + 1].GetCount() <= blk_high) {
				if(slave)
					Slave()->Join(blki);
				Join(blki);
				return true;
			}
		}
	}
	return false;
}

template <class T>
void InVector<T>::Remove(i32 pos, i32 n)
{
	ASSERT(pos >= 0 && pos + n <= GetCount());
	if(n == 0)
		return;
	i32 off;
	i32 blki = FindBlock(pos, off);
	count -= n;
	if(slave)
		Slave()->Count(-n);
	if(pos + n < data[blki].GetCount()) {
		if(slave)
			Slave()->Remove(blki, pos, n);
		data[blki].Remove(pos, n);
		if(JoinSmall(blki))
			Reindex();
		else {
			if(slave)
				Slave()->Index(blki, -n);
			Index(blki, -n);
			SetCache(blki, off);
		}
	}
	else {
		i32 b1 = blki;
		i32 nn = min(n, data[b1].GetCount() - pos);
		if(slave)
			Slave()->Remove(b1, pos, nn);
		data[b1++].Remove(pos, nn);
		n -= nn;
		i32 b2 = b1;
		while(b2 < data.GetCount() && n >= data[b2].GetCount()) {
			n -= min(n, data[b2].GetCount());
			b2++;
		}
		if(slave)
			Slave()->RemoveBlk(b1, b2 - b1);
		data.Remove(b1, b2 - b1);
		if(b1 < data.GetCount()) {
			if(slave)
				Slave()->Remove(b1, 0, n);
			data[b1].Remove(0, n);
		}
		JoinSmall(blki + 1);
		JoinSmall(blki);
		Reindex();
	}
#ifdef flagIVTEST
	Check(0, 0);
#endif
}

template <class T>
void InVector<T>::SetCount(i32 n)
{
	if(n < GetCount())
		Trim(n);
	else
		InsertN(GetCount(), n - GetCount());
}

template <class T>
void InVector<T>::Shrink()
{
	for(i32 i = 0; i < data.GetCount(); i++)
		data[i].Shrink();
	data.Shrink();
	for(i32 i = 0; i < index.GetCount(); i++)
		index[i].Shrink();
	index.Shrink();
}

template <class T>
void InVector<T>::Set(i32 i, const T& x, i32 count)
{
	Iterator it = GetIter(i);
	while(count-- > 0)
		*it++ = x;
}

template <class T>
InVector<T>::InVector(const InVector<T>& v, i32)
{
	data <<= v.data;
	index <<= v.index;
	count = v.count;
	hcount = v.hcount;
	blk_high = v.blk_high;
	blk_low = v.blk_low;
	serial = NewInVectorSerial();
	slave = v.slave;
}

template <class T>
void InVector<T>::Pick(InVector&& v)
{
	data = pick(v.data);
	index = pick(v.index);
	count = v.count;
	hcount = v.hcount;
	blk_high = v.blk_high;
	blk_low = v.blk_low;
	serial = v.serial;
	slave = v.slave;

	v.Init();
}


template <class T>
template <class L>
i32 InVector<T>::FindUpperBound(const T& val, const L& less, i32& off, i32& pos) const
{
	if(data.GetCount() == 0) {
		pos = off = 0;
		return 0;
	}
	i32 blki = 0;
	i32 ii = 0;
	i32 offset = 0;
	i32 half = hcount;
	for(i32 i = index.GetCount(); --i >= 0;) {
		i32 m = blki + half;
		if(m - 1 < data.GetCount() && !less(val, data[m - 1].Top())) {
			blki = m;
			offset += index[i][ii];
			ii++;
		}
		ii += ii;
		half >>= 1;
	}
#ifdef flagIVTEST
	Check(blki, offset);
#endif
	if(blki < data.GetCount()) {
		if(!less(val, data[blki].Top()))
			offset += data[blki++].GetCount();
		if(blki < data.GetCount()) {
			pos = drx::FindUpperBound(data[blki], val, less);
			off = offset;
			SetCache(blki, offset);
			return blki;
		}
	}
	pos = data.Top().GetCount();
	off = count - pos;
	blki--;
	SetCache(blki, off);
	return blki;
}

template <class T>
template <class L>
i32 InVector<T>::FindLowerBound(const T& val, const L& less, i32& off, i32& pos) const
{
	if(data.GetCount() == 0) {
		pos = off = 0;
		return 0;
	}
	i32 blki = 0;
	i32 ii = 0;
	i32 offset = 0;
	i32 half = hcount;
	for(i32 i = index.GetCount(); --i >= 0;) {
		i32 m = blki + half;
		if(m < data.GetCount() && less(data[m][0], val)) {
			blki = m;
			offset += index[i][ii];
			ii++;
		}
		ii += ii;
		half >>= 1;
	}
#ifdef flagIVTEST
	Check(blki, offset);
#endif
	if(blki < data.GetCount()) {
		if(blki + 1 < data.GetCount() && less(data[blki + 1][0], val))
			offset += data[blki++].GetCount();
		if(blki < data.GetCount()) {
			pos = drx::FindLowerBound(data[blki], val, less);
			off = offset;
			SetCache(blki, offset);
			return blki;
		}
	}
	pos = data.Top().GetCount();
	off = count - pos;
	blki--;
	SetCache(blki, off);
	return blki;
}

template <class T>
template <class L>
i32 InVector<T>::InsertUpperBound(const T& val, const L& less)
{
	if(data.GetCount() == 0) {
		count++;
		ClearCache();
		if(slave) {
			Slave()->Count(1);
			Slave()->AddFirst();
		}
		data.Add().Insert(0) = val;
		return 0;
	}
	i32 off;
	i32 pos;
	i32 blki = FindUpperBound(val, less, off, pos);
	Insert0(off + pos, blki, pos, off, &val);
	return off + pos;
}

template <class T>
template <class L>
i32 InVector<T>::Find(const T& val, const L& less) const
{
	i32 i = FindLowerBound(val, less);
	return i < GetCount() && !less(val, (*this)[i]) ? i : -1;
}


template <class T>
void InVector<T>::SetIter(ConstIterator& it, i32 ii) const
{
	if(count) {
		it.v = this;
		it.blki = FindBlock(ii, it.offset);
		it.begin = data[it.blki].Begin();
		it.end = data[it.blki].End();
		it.ptr = it.begin + ii;
	}
	else
		SetEnd(it);
}

template <class T>
void InVector<T>::SetBegin(ConstIterator& it) const
{
	if(count) {
		it.v = this;
		it.blki = 0;
		it.ptr = it.begin = data[0].Begin();
		it.end = data[0].End();
		it.offset = 0;
	}
	else
		SetEnd(it);
}

template <class T>
void InVector<T>::SetEnd(ConstIterator& it) const
{
	if(count) {
		it.v = this;
		it.blki = data.GetCount() - 1;
		it.begin = data.Top().Begin();
		it.ptr = it.end = data.Top().End();
		it.offset = count - data.Top().GetCount();
	}
	else {
		it.v = this;
		it.blki = 0;
		it.ptr = it.begin = it.end = NULL;
		it.offset = 0;		
	}
}

template <typename T>
force_inline typename InVector<T>::ConstIterator& InVector<T>::ConstIterator::operator+=(i32 d)
{
	if(d >= 0 ? d < end - ptr : -d < ptr - begin)
		ptr += d;
	else
		v->SetIter(*this, GetIndex() + d);
	ASSERT(end - begin == v->data[blki].GetCount());
	return *this;
}

template <typename T>
void InVector<T>::ConstIterator::NextBlk()
{
	ASSERT(end - begin == v->data[blki].GetCount());
	if(blki + 1 < v->data.GetCount()) {
		offset += v->data[blki].GetCount();
		++blki;
		ptr = begin = v->data[blki].Begin();
		end = v->data[blki].End();
	}
}

template <typename T>
void InVector<T>::ConstIterator::PrevBlk()
{
	--blki;
	begin = v->data[blki].Begin();
	ptr = end = v->data[blki].End();
	offset -= v->data[blki].GetCount();
}

template <typename T>
void InVector<T>::Swap(InVector& b)
{
	drx::Swap(data, b.data);
	drx::Swap(index, b.index);
	drx::Swap(count, b.count);
	drx::Swap(hcount, b.hcount);
	drx::Swap(serial, b.serial);
	drx::Swap(blk_high, b.blk_high);
	drx::Swap(blk_low, b.blk_low);
	drx::Swap(slave, b.slave);
}

template <class T>
void InVector<T>::Xmlize(XmlIO& xio, tukk itemtag)
{
	XmlizeContainer(xio, itemtag, *this);
}

template <class T>
void InVector<T>::Jsonize(JsonIO& jio)
{
	JsonizeArray<InVector<T>>(jio, *this);
}

template <class T>
Txt InVector<T>::ToTxt() const
{
	return AsTxtArray(*this);
}

template <class T>
void InVector<T>::DumpIndex() const
{
	Txt h;
	RLOG("------- InVector dump, count: " << GetCount() << ", index depth: " << index.GetCount());
	z64 alloc = 0;
	for(i32 i = 0; i < data.GetCount(); i++) {
		if(i)
			h << ", ";
		h << data[i].GetCount() << " (" << data[i].GetAlloc() << ")";
		alloc += data[i].GetAlloc();
	}
	RLOG("Data blocks: " << data.GetCount() << ", sizeof: " << data.GetCount() * sizeof(Vec<T>));
	RLOG("Total alloc: " << alloc);
	RLOG(h);
	for(i32 j = 0; j < index.GetCount(); j++) {
		h.Clear();
		h << index[j].GetCount() << ": ";
		for(i32 k = 0; k < index[j].GetCount(); k++) {
			if(k)
				h << ", ";
			h << index[j][k];
		}
		RLOG(h);
	}
	RLOG(".");
}

#ifdef flagIVTEST
template <class T>
void InVector<T>::Check(i32 blki, i32 offset) const
{
	i32 off = 0;
	i32 all = 0;
	for(i32 i = 0; i < data.GetCount(); i++) {
		if(i < blki)
			off += data[i].GetCount();
		all += data[i].GetCount();
	}
	ASSERT(off == offset);
	ASSERT(all == count);
}
#endif

template <class T>
void InArray<T>::Delete(IVIter it, i32 count)
{
	ASSERT(count >= 0);
	while(count--)
		delete (T *)*it++;
}

template <class T>
void InArray<T>::Delete(i32 i, i32 count)
{
	Delete(iv.GetIter(i), count);
}

template <class T>
void InArray<T>::Init(i32 i, i32 count)
{
	ASSERT(count >= 0);
	IVIter it = iv.GetIter(i);
	while(count--)
		*it++ = new T;
}

template <class T>
void InArray<T>::InsertN(i32 i, i32 count)
{
	iv.InsertN(i, count);
	Init(i, count);
}

template <class T>
template <class Range>
void InArray<T>::InsertRange(i32 i, const Range& r)
{
	i32 count = r.GetCount();
	iv.InsertN(i, count);
	IVIter it = iv.begin() + i;
	auto s = r.begin();
	while(count--)
		*it++ = new T(*s++);
}

template <class T>
void InArray<T>::Remove(i32 i, i32 count)
{
	Delete(i, count);
	iv.Remove(i, count);
}

template <class T>
void InArray<T>::SetCount(i32 n)
{
	if(n < GetCount())
		Trim(n);
	else
		InsertN(GetCount(), n - GetCount());
}

template <class T>
void InArray<T>::Clear()
{
	Free();
	iv.Clear();
}

template <class T>
void InArray<T>::Set(i32 i, const T& x, i32 count)
{
	Iterator it = GetIter(i);
	while(count-- > 0)
		*it++ = x;
}

template <class T>
void InArray<T>::SetIter(ConstIterator& it, i32 ii) const
{
	it.it = iv.GetIter(ii);
}

template <class T>
void InArray<T>::SetBegin(ConstIterator& it) const
{
	it.it = iv.Begin();
}

template <class T>
void InArray<T>::SetEnd(ConstIterator& it) const
{
	it.it = iv.End();
}

template <class T>
InArray<T>::InArray(const InArray& v, i32)
{
	i32 n = v.GetCount();
	iv.SetCount(v.GetCount());
	ConstIterator s = v.Begin();
	IVIter it = iv.Begin();
	while(n--)
		*it++ = new T(clone(*s++));
}

#ifdef DRX
template <class T>
void InArray<T>::Xmlize(XmlIO& xio, tukk itemtag)
{
	XmlizeContainer(xio, itemtag, *this);
}

template <class T>
void InArray<T>::Jsonize(JsonIO& jio)
{
	JsonizeArray<InArray<T>>(jio, *this);
}

template <class T>
Txt InArray<T>::ToTxt() const
{
	return AsTxtArray(*this);
}

#endif

#ifdef LLOG
#undef  LLOG
#endif