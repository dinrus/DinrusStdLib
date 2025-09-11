template <class B>
force_inline
void ATxt<B>::Insert(i32 pos, tukk s)
{
	Insert(pos, s, strlen__(s));
}

template <class B>
void ATxt<B>::Cat(i32 c, i32 count)
{
	tchar* s = B::Insert(GetLength(), count, NULL);
	while(count--)
		*s++ = c;
}

template <class B>
force_inline
void ATxt<B>::Cat(const tchar* s)
{
	Cat(s, strlen__(s));
}

template <class B>
i32 ATxt<B>::Compare(const tchar* b) const
{
	const tchar* a = B::Begin();
	const tchar* ae = End();
	for(;;) {
		if(a >= ae)
			return *b == 0 ? 0 : -1;
		if(*b == 0)
			return 1;
		i32 q = cmpval__(*a++) - cmpval__(*b++);
		if(q)
			return q;
	}
}

template <class B>
typename ATxt<B>::Txt ATxt<B>::Mid(i32 from, i32 count) const
{
	i32 l = GetLength();
	if(from > l) from = l;
	if(from < 0) from = 0;
	if(count < 0)
		count = 0;
	if(from + count > l)
		count = l - from;
	return Txt(B::Begin() + from, count);
}

template <class B>
i32 ATxt<B>::Find(i32 chr, i32 from) const
{
	ASSERT(from >= 0 && from <= GetLength());
	const tchar* e = End();
	const tchar* ptr = B::Begin();
	for(const tchar* s = ptr + from; s < e; s++)
		if(*s == chr)
			return (i32)(s - ptr);
	return -1;
}

template <class B>
i32 ATxt<B>::ReverseFind(i32 chr, i32 from) const
{
	ASSERT(from >= 0 && from <= GetLength());
	if(from < GetLength()) {
		const tchar* ptr = B::Begin();
		for(const tchar* s = ptr + from; s >= ptr; s--)
			if(*s == chr)
				return (i32)(s - ptr);
	}
	return -1;
}

template <class B>
i32 ATxt<B>::ReverseFind(i32 len, const tchar* s, i32 from) const
{
	ASSERT(from >= 0 && from <= GetLength());
	if(from < GetLength()) {
		const tchar* ptr = B::Begin();
		const tchar* p = ptr + from - len + 1;
		len *= sizeof(tchar);
		while(p >= ptr) {
			if(*s == *p && memcmp(s, p, len) == 0)
				return (i32)(p - ptr);
			p--;
		}
	}
	return -1;
}

template <class B>
i32 ATxt<B>::ReverseFindAfter(i32 len, const tchar* s, i32 from) const
{
	i32 q = ReverseFind(len, s, from);
	return q >= 0 ? q + len : -1;
}

template <class B>
void ATxt<B>::Replace(const tchar* find, i32 findlen, const tchar* replace, i32 replacelen)
{
	if(findlen == 0)
		return;
	Txt r;
	i32 i = 0;
	const tchar* p = B::Begin();
	for(;;) {
		i32 j = Find(findlen, find, i);
		if(j < 0)
			break;
		r.Cat(p + i, j - i);
		r.Cat(replace, replacelen);
		i = j + findlen;
	}
	r.Cat(p + i, B::GetCount() - i);
	B::Free();
	B::Set0(r);
}

template <class B>
i32 ATxt<B>::ReverseFind(const tchar* s, i32 from) const
{
	return ReverseFind(strlen__(s), s, from);
}

template <class B>
i32 ATxt<B>::ReverseFindAfter(const tchar* s, i32 from) const
{
	return ReverseFindAfter(strlen__(s), s, from);
}

template <class B>
i32 ATxt<B>::ReverseFind(i32 chr) const
{
	return B::GetCount() ? ReverseFind(chr, B::GetCount() - 1) : -1;
}

template <class B>
void ATxt<B>::Replace(const Txt& find, const Txt& replace)
{
	Replace(~find, find.GetCount(), ~replace, replace.GetCount());
}

template <class B>
force_inline
void ATxt<B>::Replace(const tchar* find, const tchar* replace)
{
	Replace(find, (i32)strlen__(find), replace, (i32)strlen__(replace));
}

template <class B>
force_inline
void ATxt<B>::Replace(const Txt& find, const tchar* replace)
{
	Replace(~find, find.GetCount(), replace, (i32)strlen__(replace));
}

template <class B>
force_inline
void ATxt<B>::Replace(const tchar* find, const Txt& replace)
{
	Replace(find, (i32)strlen__(find), ~replace, replace.GetCount());
}

template <class B>
bool ATxt<B>::StartsWith(const tchar* s, i32 len) const
{
	if(len > GetLength()) return false;
	return memcmp(s, B::Begin(), len * sizeof(tchar)) == 0;
}

template <class B>
force_inline
bool ATxt<B>::StartsWith(const tchar* s) const
{
	return StartsWith(s, strlen__(s));
}

template <class B>
bool ATxt<B>::EndsWith(const tchar* s, i32 len) const
{
	i32 l = GetLength();
	if(len > l) return false;
	return memcmp(s, B::Begin() + l - len, len * sizeof(tchar)) == 0;
}

template <class B>
force_inline
bool ATxt<B>::EndsWith(const tchar* s) const
{
	return EndsWith(s, strlen__(s));
}

template <class B>
i32 ATxt<B>::FindFirstOf(i32 len, const tchar* s, i32 from) const
{
	ASSERT(from >= 0 && from <= GetLength());
	const tchar* ptr = B::Begin();
	const tchar* e = B::End();
	const tchar* se = s + len;
	if(len == 1) {
		tchar c1 = s[0];
		for(const tchar* bs = ptr + from; bs < e; bs++) {
			if(*bs == c1)
				return (i32)(bs - ptr);
		}
		return -1;
	}
	if(len == 2) {
		tchar c1 = s[0];
		tchar c2 = s[1];
		for(const tchar* bs = ptr + from; bs < e; bs++) {
			tchar ch = *bs;
			if(ch == c1 || ch == c2)
				return (i32)(bs - ptr);
		}
		return -1;
	}
	if(len == 3) {
		tchar c1 = s[0];
		tchar c2 = s[1];
		tchar c3 = s[2];
		for(const tchar* bs = ptr + from; bs < e; bs++) {
			tchar ch = *bs;
			if(ch == c1 || ch == c2 || ch == c3)
				return (i32)(bs - ptr);
		}
		return -1;
	}
	if(len == 4) {
		tchar c1 = s[0];
		tchar c2 = s[1];
		tchar c3 = s[2];
		tchar c4 = s[3];
		for(const tchar* bs = ptr + from; bs < e; bs++) {
			tchar ch = *bs;
			if(ch == c1 || ch == c2 || ch == c3 || ch == c4)
				return (i32)(bs - ptr);
		}
		return -1;
	}
	for(const tchar* bs = ptr + from; bs < e; bs++)
		for(const tchar* ss = s; ss < se; ss++)
			if(*bs == *ss)
				return (i32)(bs - ptr);
	return -1;
}

force_inline
void Txt0::Set(tukk s, i32 len)
{
	Clear();
	if(len <= 14) {
		memcpy8(chr, s, len);
		SLen() = len;
		Dsyn();
		return;
	}
	SetL(s, len);
	Dsyn();
}

force_inline
void Txt0::Set0(tukk s, i32 len)
{
	Zero();
	if(len <= 14) {
		SLen() = len;
		memcpy8(chr, s, len);
		Dsyn();
		return;
	}
	SetL(s, len);
	Dsyn();
}

inline
bool Txt0::IsEqual(tukk s) const
{ // This is optimized for comparison with string literals...
	size_t len = strlen(s);
	ukk p;
	if(IsSmall()) {
		if(len > 14 || len != (size_t)chr[SLEN]) return false; // len > 14 fixes issue with GCC warning, might improves performance too
		p = chr;
	}
	else {
		if(len != (size_t)w[LLEN]) return false;
		p = ptr;
	}
	return memcmp(p, s, len) == 0; // compiler is happy to optimize memcmp out with up to 64bit comparisons for literals...
}

inline
i32 Txt0::Compare(const Txt0& s) const
{
#ifdef CPU_LE
	if((chr[KIND] | s.chr[KIND]) == 0) { // both strings are small, len is the MSB
	#ifdef CPU_64
		zu64 a = SwapEndian64((zu64)q[0]);
		zu64 b = SwapEndian64((zu64)s.q[0]);
		if(a != b)
			return a < b ? -1 : 1;
		a = SwapEndian64((zu64)q[1]);
		b = SwapEndian64((zu64)s.q[1]);
		return a == b ? 0 : a < b ? -1 : 1;
	#else
		zu64 a = SwapEndian32(w[0]);
		zu64 b = SwapEndian32(s.w[0]);
		if(a != b)
			return a < b ? -1 : 1;
		a = SwapEndian32(w[1]);
		b = SwapEndian32(s.w[1]);
		if(a != b)
			return a < b ? -1 : 1;
		a = SwapEndian32(w[2]);
		b = SwapEndian32(s.w[2]);
		if(a != b)
			return a < b ? -1 : 1;
		a = SwapEndian32(w[3]);
		b = SwapEndian32(s.w[3]);
		return a == b ? 0 : a < b ? -1 : 1;
	#endif
	}
#endif
	return CompareL(s);
}

force_inline
Txt& Txt::operator=(tukk s)
{
	AssignLen(s, strlen__(s));
	return *this;
}

force_inline
Txt::Txt(tukk s)
{
	Txt0::Set0(s, strlen__(s));
}

force_inline
void Txt0::Swap(Txt0& b)
{
	qword a0 = q[0]; // Explicit code (instead of Swap) seems to emit some SSE2 code with CLANG
	qword a1 = q[1];
	qword b0 = b.q[0];
	qword b1 = b.q[1];
	q[0] = b0;
	q[1] = b1;
	b.q[0] = a0;
	b.q[1] = a1;
	Dsyn(); b.Dsyn();
}

template <class Maker>
inline
Txt Txt::Make(i32 alloc, Maker m)
{
	Txt s;
	if(alloc <= 14) {
		i32 len = m(s.chr);
		ASSERT(len <= alloc);
		s.SLen() = len;
		s.Dsyn();
	}
	else {
		if(alloc < 32) {
			s.chr[KIND] = MEDIUM;
			s.ptr = (tuk )MemoryAlloc32();
		}
		else
			s.ptr = s.Alloc(alloc, s.chr[KIND]);
		i32 len = m(s.ptr);
		ASSERT(len <= alloc);
		s.ptr[len] = 0;
		s.LLen() = len;
		s.SLen() = 15;
		if(alloc >= 32 && alloc > 2 * len)
			s.Shrink();
	}
	return s;
}

force_inline
void TxtBuf::Strlen()
{
	SetLength((i32)strlen__(pbegin));
}

inline
void TxtBuf::Cat(tukk s, i32 l)
{
	if(pend + l > limit)
		ReallocL(s, l);
	else {
		memcpy8(pend, s, l);
		pend += l;
	}
}

inline
void TxtBuf::Cat(i32 c, i32 l)
{
	if(pend + l > limit)
		ReallocL(NULL, l);
	memset8(pend, c, l);
	pend += l;
}

force_inline
void TxtBuf::Cat(tukk s)
{
	Cat(s, (i32)strlen__(s));
}
