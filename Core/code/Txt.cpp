#include "../Core.h"

namespace drx {

#ifdef _DEBUG
void Txt0::Dsyn()
{
    Txt *d_str = static_cast<Txt *>(this);
    d_str->s = Begin();
    d_str->len = GetCount();
}
#endif

Txt0::Rc Txt0::voidptr[2];

#ifndef DRX_HEAP

inline void  *MemoryAlloc32_i()              { return new u8[32]; }
inline void   MemoryFree32_i(uk ptr)      { delete[] (u8 *)ptr; }

#include "../StringMem.i"

#endif

bool Txt0::LEq(const Txt0& s) const
{
    i32 l = GetCount();
    return l == s.GetCount() && inline_memeq8_aligned(begin(), s.begin(), l);
}

hash_t Txt0::LHashVal() const
{
    i32 l = LLen();
    if(l < 15) { // must be the same as small hash
#ifdef HASH64
        qword m[2];
        m[0] = m[1] = 0;
        memcpy8((tuk )m, ptr, l);
        ((u8 *)m)[SLEN] = l;
        return CombineHash(m[0], m[1]);
#else
        dword m[4];
        m[0] = m[1] = m[2] = m[3] = 0;
        memcpy8((tuk )m, ptr, l);
        ((u8 *)m)[SLEN] = l;
        return CombineHash(m[0], m[1], m[2], m[3]);
#endif
    }
    return memhash(ptr, l);
}

i32 Txt0::CompareL(const Txt0& s) const
{
    tukk a = Begin();
    i32 la = GetLength();
    tukk b = s.Begin();
    i32 lb = s.GetLength();
    i32 q = inline_memcmp_aligned(a, b, min(la, lb));
    return q ? q : SgnCompare(la, lb);
}

tuk Txt0::Insert(i32 pos, i32 count, tukk s)
{
    ASSERT(pos >= 0 && count >= 0 && pos <= GetCount());
    i32 len = GetCount();
    i32 newlen = len + count;
    if(newlen < len) // overflow, string >2GB
        Panic("Строка слишком большая!");
    tuk str = (tuk )Begin();
    if(newlen < GetAlloc() && !IsSharedRef() && (!s || s < str || s > str + len)) {
        if(pos < len)
            memmove(str + pos + count, str + pos, len - pos);
        if(IsSmall())
            SLen() = newlen;
        else
            LLen() = newlen;
        str[newlen] = 0;
        if(s)
            memcpy8(str + pos, s, count);
        Dsyn();
        return str + pos;
    }
    char kind;
    tuk p = Alloc(max(len >= i32((z64)2 * INT_MAX / 3) ? INT_MAX : len + (len >> 1), newlen),
                    kind);
    if(pos > 0)
        memcpy8(p, str, pos);
    if(pos < len)
        memcpy8(p + pos + count, str + pos, len - pos);
    if(s)
        memcpy8(p + pos, s, count);
    p[newlen] = 0;
    Free();
    ptr = p;
    LLen() = newlen;
    SLen() = 15;
    chr[KIND] = kind;
    Dsyn();
    return ptr + pos;
}

void Txt0::UnShare()
{
    if(IsSharedRef()) {
        i32 len = LLen();
        char kind;
        tuk p = Alloc(len, kind);
        memcpy8(p, ptr, len + 1);
        Free();
        chr[KIND] = kind;
        ptr = p;
    }
}

void Txt0::SetSLen(i32 l)
{
    SLen() = l;
    memset(chr + l, 0, 15 - l);
}

void Txt0::Remove(i32 pos, i32 count)
{
    ASSERT(pos >= 0 && count >= 0 && pos + count <= GetCount());
    UnShare();
    tuk s = (tuk )Begin();
    memmove(s + pos, s + pos + count, GetCount() - pos - count + 1);
    if(IsSmall())
        SetSLen(SLen() - count);
    else
        LLen() -= count;
    Dsyn();
}

void Txt0::Set(i32 pos, i32 chr)
{
    ASSERT(pos >= 0 && pos < GetCount());
    UnShare();
    Ptr()[pos] = chr;
}

void Txt0::Trim(i32 pos)
{
    ASSERT(pos >= 0 && pos <= GetCount());
    if(IsSmall()) {
        chr[pos] = 0;
        SetSLen(pos);
    }
    else {
        UnShare();
        ptr[pos] = 0;
        LLen() = pos;
    }
    Dsyn();
}

void Txt0::Cat(tukk s, i32 len)
{
    if(IsSmall()) {
        if(SLen() + len < 14) {
            memcpy8(chr + SLen(), s, len);
            SLen() += len;
            chr[(i32)SLen()] = 0;
            Dsyn();
            return;
        }
    }
    else
        if((i32)LLen() + len < LAlloc() && !IsSharedRef()) {
            memcpy8(ptr + LLen(), s, len);
            LLen() += len;
            ptr[LLen()] = 0;
            Dsyn();
            return;
        }
    Insert(GetCount(), len, s);
}

void Txt0::Reserve(i32 r)
{
    i32 l = GetCount();
    Insert(GetCount(), r, NULL);
    Trim(l);
}

void Txt::AssignLen(tukk s, i32 slen)
{
    i32  len = GetCount();
    tuk str = (tuk )Begin();
    if(s >= str && s <= str + len)
        *this = Txt(s, slen);
    else {
        Txt0::Free();
        Txt0::Set0(s, slen);
    }
}

Txt Txt::GetVoid()
{
    Txt s;
    s.ptr = (tuk )(voidptr + 1);
    s.LLen() = 0;
    s.SLen() = 15;
    s.chr[KIND] = 50;
    return s;
}

bool Txt::IsVoid() const
{
    return IsRef() && ptr == (tuk )(voidptr + 1);
}

WTxt Txt::ToWTxt() const
{
    return WTxt(Begin(), GetCount());
}

i32 Txt::GetCharCount() const
{
    return GetDefaultCharset() == CHARSET_UTF8 ? Utf32Len(Begin(), GetCount()) : GetCount();
}

Txt::Txt(char16 *s) : Txt(ToUtf8(s)) {}
Txt::Txt(wtuk s) : Txt(ToUtf8(s)) {}

Txt::Txt(TxtBuf& b)
{
    Zero();
    if(b.pbegin == b.buffer) {
        Txt0::Set0(b.pbegin, (i32)(uintptr_t)(b.pend - b.pbegin));
        return;
    }
    i32 l = b.GetLength();
    if(l <= 14) {
        memcpy8(chr, b.pbegin, l);
        SLen() = l;
        b.Free();
    }
    else {
        ptr = b.pbegin;
        ptr[l] = 0;
        SLen() = 15;
        LLen() = l;
        chr[KIND] = min(b.GetAlloc(), 255);
        if(GetAlloc() > 4 * GetLength() / 3)
            Shrink();
    }
    b.Zero();

//  char h[100];
//  DLOG(sprintf(h, "Txt(TxtBuf) end %p (%p)", ptr, this));
    Dsyn();
//  DLOG(sprintf(h, "Txt(TxtBuf) end2 %p (%p)", ptr, this));
}

void TxtBuf::Realloc(dword n, tukk cat, i32 l)
{
    i32 al;
    size_t ep = pend - pbegin;
    if(n > INT_MAX)
        n = INT_MAX;
    bool realloced = false;
    tuk p;
    if((i32)(limit - pbegin) > 800) {
        size_t sz = sizeof(Rc) + n + 1;
        Rc *rc = (Rc *)pbegin - 1;
        if(MemoryTryRealloc(rc, sz)) {
            realloced = true;
            al = rc->alloc = (i32)min((size_t)INT_MAX, sz - sizeof(Rc) - 1);
            p = pbegin;
        }
    }
    if(!realloced) {
        p = Alloc(n, al);
        memcpy8(p, pbegin, min((dword)GetLength(), n));
    }
    if(cat) {
        if(ep + l > INT_MAX)
            Panic("TxtBuf слишком большой (>2GB)!");
        memcpy8(p + ep, cat, l);
        ep += l;
    }
    if(!realloced) {
        Free();
        pbegin = p;
    }
    pend = pbegin + ep;
    limit = pbegin + al;
}

void TxtBuf::Expand()
{
    Realloc(GetLength() * 3 / 2);
    if(pend == limit)
        Panic("TxtBuf слишком большой!");
}

void TxtBuf::SetLength(i32 l)
{
    if(l > GetAlloc())
        Realloc(l);
    pend = pbegin + l;
}

void TxtBuf::Shrink()
{
    i32 l = GetLength();
    if(l < GetAlloc() && l > 14)
        Realloc(l);
    pend = pbegin + l;
}

void TxtBuf::ReallocL(tukk s, i32 l)
{
    Realloc(max(GetLength(), l) + GetLength(), s, l);
}

Txt TrimLeft(const Txt& str)
{
    tukk s = str;
    if(!IsSpace(*s))
        return str;
    while(IsSpace((u8)*s)) s++;
    return Txt(s, str.End());
}

Txt TrimRight(const Txt& str)
{
    if(str.IsEmpty())
        return str;
    tukk s = str.Last();
    if(!IsSpace(*s))
        return str;
    while(s >= ~str && IsSpace((u8)*s)) s--;
    return Txt(~str, s + 1);
}

Txt TrimBoth(const Txt& str)
{
    return TrimLeft(TrimRight(str));
}

Txt TrimLeft(tukk sw, i32 len, const Txt& s)
{
    return s.StartsWith(sw, len) ? s.Mid(len) : s;
}

Txt TrimRight(tukk sw, i32 len, const Txt& s)
{
    return s.EndsWith(sw, len) ? s.Mid(0, s.GetCount() - len) : s;
}

struct TxtICompare__
{
    i32 encoding;
    i32 operator()(char a, char b) const { return ToUpper(a, encoding) - ToUpper(b, encoding); }

    TxtICompare__(i32 e) : encoding(e) {}
};

i32 CompareNoCase(const Txt& a, const Txt& b)
{
    return CompareNoCase(ToUtf32(a), ToUtf32(b));
}

i32 CompareNoCase(const Txt& a, tukk b)
{
    return CompareNoCase(ToUtf32(a), ToUtf32(b, (i32)strlen(b)));
}

}
