// These are Txt methods which are best inlined in heap allocator

void Txt0::LSet(const Txt0& s)
{
    w[2] = s.w[2];
    w[3] = s.w[3];
    if(s.IsRef()) {
        ptr = s.ptr;
        if(ptr != (char *)(voidptr + 1))
            AtomicInc(s.Ref()->refcount);
    }
    else {
        ptr = (char *)MemoryAlloc32_i();
        memcpy(qptr, s.qptr, 32); // optimizes to movups
    }
}

void Txt0::LFree()
{
    if(IsRef()) {
        if(ptr != (tuk)(voidptr + 1)) {
            Rc *rc = Ref();
            ASSERT(rc->refcount > 0);
            if(AtomicDec(rc->refcount) == 0) MemoryFree(rc);
        }
    }
    else
        MemoryFree32_i(ptr);
}


force_inline
tuk Txt0::Alloc_(i32 count, char& kind)
{
    if(count < 32) {
        kind = MEDIUM;
        return (char *)MemoryAlloc32_i();
    }
    size_t sz = sizeof(Rc) + count + 1;
    Rc *rc = (Rc *)MemoryAllocSz(sz);
    rc->alloc = count == INT_MAX ? INT_MAX : (i32)sz - sizeof(Rc) - 1;
    rc->refcount = 1;
    kind = min(rc->alloc, 255);
    return rc->GetPtr();
}

tuk Txt0::Alloc(i32 count, char& kind)
{
    return Alloc_(count, kind);
}

void Txt0::SetL(const char *s, i32 len)
{
    ptr = Alloc_(len, chr[KIND]);
    memcpy8(ptr, s, len);
    ptr[len] = 0;
    LLen() = len;
    SLen() = 15;
}

void Txt0::LCat(i32 c)
{
    if(IsSmall()) {
        qword *x = (qword *)MemoryAlloc32_i();
        x[0] = q[0];
        x[1] = q[1];
        LLen() = SLen();
        SLen() = 15;
        chr[KIND] = MEDIUM;
        qptr = x;
    }
    i32 l = LLen();
    if(IsRef() ? !IsShared() && l < (i32)Ref()->alloc : l < 31) {
        ptr[l] = c;
        ptr[LLen() = l + 1] = 0;
    }
    else {
        char *s = Insert(l, 1, NULL);
        s[0] = c;
        s[1] = 0;
    }
}

tuk TxtBuf::Alloc(i32 count, i32& alloc)
{
    if(count <= 31) {
        char *s = (char *)MemoryAlloc32_i();
        alloc = 31;
        return s;
    }
    else {
        size_t sz = sizeof(Rc) + count + 1;
        Rc *rc = (Rc *)MemoryAlloc(sz);
        alloc = rc->alloc = (i32)min((size_t)INT_MAX, sz - sizeof(Rc) - 1);
        rc->refcount = 1;
        return (char *)(rc + 1);
    }
}

void TxtBuf::Set(Txt& s)
{
    s.UnShare();
    i32 l = s.GetLength();
    if(s.GetAlloc() == 14) {
        pbegin = (char *)MemoryAlloc32_i();
        limit = pbegin + 31;
        memcpy8(pbegin, s.Begin(), l);
        pend = pbegin + l;
    }
    else {
        pbegin = s.ptr;
        pend = pbegin + l;
        limit = pbegin + s.GetAlloc();
    }
    s.Zero();
}

void TxtBuf::Free()
{
    if(pbegin == buffer)
        return;
    i32 all = (i32)(limit - pbegin);
    if(all == 31)
        MemoryFree32_i(pbegin);
    if(all > 31)
        MemoryFree((Rc *)pbegin - 1);
}
