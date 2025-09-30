#include "../Core.h"

namespace drx {

wtuk WTxt0::Alloc(i32& count)
{
    if(count <= SMALL) {
        count = SMALL;
        wtuk p = (wtuk )MemoryAlloc((SMALL + 1) * sizeof(wchar));
        return p;
    }
    size_t sz = sizeof(Atomic) + ((size_t)count + 1) * sizeof(wchar);
    Atomic *rc = (Atomic *)MemoryAllocSz(sz);
    if(count != INT_MAX)
        count = i32(((sz - sizeof(Atomic)) / sizeof(wchar)) - 1);
    *rc = 1;
    return (wtuk )(rc + 1);
}

void WTxt0::Free()
{
    if(alloc > 0) {
        if(IsRc()) {
            Atomic& rc = Rc();
            if(AtomicDec(rc) == 0)
                MemoryFree(&rc);
        }
        else
            MemoryFree(ptr);
    }
}

#ifdef _DEBUG
void WTxt0::Dsyn()
{
    WTxt *d_str = static_cast<WTxt *>(this);
    d_str->s = Begin();
    d_str->len = GetCount();
}
#endif

wtuk WTxt0::Insert(i32 pos, i32 count, wtukk s)
{
    ASSERT(pos >= 0 && count >= 0 && pos <= GetCount());
    i32 newlen = length + count;
    if(newlen < length)
        Panic("WTxt слишком большой!");
    if(newlen < alloc && !IsShared() && (!s || s < ptr || s > ptr + length)) {
        if(pos < length)
            memmove(ptr + pos + count, ptr + pos, (length - pos) * sizeof(wchar));
        length = newlen;
        ptr[newlen] = 0;
        if(s)
            memcpy_t(ptr + pos, s, count);
        Dsyn();
        return ptr + pos;
    }
    i32 all = max(length >= i32((z64)2 * INT_MAX / 3) ? INT_MAX : length + (length >> 1), newlen);
    wtuk p = Alloc(all);
    if(pos > 0)
        memcpy_t(p, ptr, pos);
    if(pos < length)
        memcpy_t(p + pos + count, ptr + pos, length - pos);
    if(s)
        memcpy_t(p + pos, s, count);
    ASSERT(pos + count <= all);
    p[newlen] = 0;
    Free();
    ptr = p;
    length = newlen;
    alloc = all;
    Dsyn();
    return ptr + pos;
}

void WTxt0::Set0(const WTxt0& src)
{
    if(src.alloc <= 0) {
        static wchar h[2];
        ptr = h;
        length = 0;
        alloc = src.alloc;
        Dsyn();
        return;
    }
    if(src.length == 0) {
        Zero();
        return;
    }
    length = src.length;
    alloc = src.alloc;
    if(src.IsRc()) {
        ptr = src.ptr;
        AtomicInc(Rc());
    }
    else {
        ptr = (wtuk)MemoryAlloc((SMALL + 1) * sizeof(wchar));
        memcpy(ptr, src.ptr, sizeof(wchar) * (SMALL + 1));
    }
    Dsyn();
}

void WTxt0::Cat(wtukk s, i32 l)
{
    if(length + l >= alloc || IsShared())
        Insert(length, l, s);
    else {
        memcpy_t(ptr + length, s, l);
        ptr[length += l] = 0;
    }
    Dsyn();
}

void WTxt0::Set(wtukk s, i32 length)
{
    Free();
    Set0(s, length);
}

void WTxt0::LCat(i32 c)
{
    *Insert(length, 1, NULL) = c;
}

void WTxt0::Set0(wtukk s, i32 l)
{
    alloc = length = l;
    memcpy_t(ptr = Alloc(alloc), s, l);
    ptr[l] = 0;
    Dsyn();
}

void WTxt0::UnShare()
{
    if(!IsShared()) return;
    i32 al = length;
    wtuk p = Alloc(al);
    memcpy_t(p, ptr, length + 1);
    Free();
    ptr = p;
    alloc = al;
}

void WTxt0::Set(i32 pos, i32 ch)
{
    ASSERT(pos >= 0 && pos < GetCount());
    UnShare();
    ptr[pos] = ch;
}

void WTxt0::Trim(i32 pos)
{
    ASSERT(pos >= 0 && pos <= GetCount());
    UnShare();
    length = pos;
    ptr[pos] = 0;
    Dsyn();
}

void WTxt0::Remove(i32 pos, i32 count)
{
    ASSERT(pos >= 0 && count >= 0 && pos + count <= GetCount());
    UnShare();
    memmove(ptr + pos, ptr + pos + count, (GetCount() - pos - count + 1) * sizeof(wchar));
    length -= count;
    Dsyn();
}

void WTxt0::Insert(i32 pos, wtukk s, i32 count)
{
    Insert(pos, count, s);
    Dsyn();
}

i32 WTxt0::Compare(const WTxt0& s) const
{
    wtukk a = Begin();
    wtukk ae = a + GetLength();
    wtukk b = s.Begin();
    wtukk be = b + s.GetLength();
    for(;;) {
        if(a >= ae)
            return b >= be ? 0 : -1;
        if(b >= be)
            return 1;
        i32 q = cmpval__(*a++) - cmpval__(*b++);
        if(q)
            return q;
    }
}

WTxt& WTxt::operator=(wtukk s)
{
    i32  len = GetCount();
    wtuk str = (wtuk )Begin();
    if(s >= str && s <= str + len)
        return *this = WTxt(s, strlen__(s));
    WTxt0::Free();
    WTxt0::Set0(s, strlen__(s));
    return *this;
}

WTxt::WTxt(WTxtBuf& b)
{
    length = b.GetLength();
    ptr = b.pbegin;
    ptr[length] = 0;
    alloc = b.GetAlloc();
    if(GetAlloc() > 4 * GetLength() / 3)
        Shrink();
    b.Zero();
    Dsyn();
}

WTxt::WTxt(tukk s) {
    Zero();
    *this = ToUnicode(s, s ? (i32)strlen(s) : 0, CHARSET_DEFAULT);
}

WTxt::WTxt(const char16 *s) {
    Zero();
    *this = ToUtf32(s);
}

WTxt::WTxt(tukk s, i32 n) {
    Zero();
    *this = ToUnicode(s, n, CHARSET_DEFAULT);
}

WTxt::WTxt(tukk s, tukk lim) {
    Zero();
    *this = ToUnicode(s, s ? (i32)(lim - s) : 0, CHARSET_DEFAULT);
}

Txt WTxt::ToTxt() const
{
    return FromUnicode(*this, CHARSET_DEFAULT);
}

Atomic WTxt0::voidptr[2];

WTxt WTxt::GetVoid()
{
    WTxt b;
    b.alloc = -1;
    return b;
}

#ifndef _HAVE_NO_STDWTXT
static_assert(sizeof(wchar_t) == 4 || sizeof(wchar_t) == 2, "Неверный размер wchar_t");

WTxt::WTxt(const std::wstring& s)
{
    WTxt0::Zero();
    if(sizeof(wchar_t) == 4)
        *this = WTxt((wtukk)s.c_str(), (i32)s.size());
    if(sizeof(wchar_t) == 2)
        *this = ToUtf32((char16 *)s.c_str(), (i32)s.size());
}

std::wstring WTxt::ToStd() const
{
    if(sizeof(wchar_t) == 4)
        return std::wstring((const wchar_t *)begin(), GetCount());
    if(sizeof(wchar_t) == 2) {
        Vec<char16> h = ToUtf16(*this);
        return std::wstring((const wchar_t *)h.begin(), h.GetCount());
    }
}
#endif

void WTxtBuf::Zero()
{
    static wchar h[2];
    pbegin = pend = limit = h;
}

wtuk WTxtBuf::Alloc(i32 count, i32& alloc)
{
    if(count <= 23) {
        wtuk s = (wtuk)MemoryAlloc(24 * sizeof(wchar));
        alloc = WTxt0::SMALL;
        return s;
    }
    else {
        size_t sz = sizeof(Atomic) + ((size_t)count + 1) * sizeof(wchar);
        Atomic *rc = (Atomic *)MemoryAlloc(sz);
        alloc = (i32)min((size_t)INT_MAX, ((sz - sizeof(Atomic)) / sizeof(wchar)) - 1);
        ASSERT(alloc >= 0);
        *rc = 1;
        return (wtuk )(rc + 1);
    }
}

void WTxtBuf::Free()
{
    i32 all = (i32)(limit - pbegin);
    if(all == WTxt0::SMALL)
        MemoryFree(pbegin);
    if(all > WTxt0::SMALL)
        MemoryFree((Atomic *)pbegin - 1);
}

void WTxtBuf::Expand(dword n, wtukk cat, i32 l)
{
    i32 al;
    size_t ep = pend - pbegin;
    if(n > INT_MAX)
        n = INT_MAX;
    wtuk p = Alloc(n, al);
    memcpy_t(p, pbegin, GetLength());
    if(cat) {
        if(ep + l > INT_MAX)
            Panic("WTxtBuf слишком большой!");
        memcpy_t(p + ep, cat, l);
        ep += l;
    }
    Free();
    pbegin = p;
    pend = pbegin + ep;
    limit = pbegin + al;
}

void WTxtBuf::Expand()
{
    Expand(GetLength() * 2);
    if(pend == limit)
        Panic("WTxtBuf слишком большой!");
}

void WTxtBuf::SetLength(i32 l)
{
    if(l > (limit - pbegin))
        Expand(l);
    pend = pbegin + l;
}

void WTxtBuf::Cat(wtukk s, i32 l)
{
    if(pend + l > limit)
        Expand(max(GetLength(), l) + GetLength(), s, l);
    else {
        memcpy_t(pend, s, l);
        pend += l;
    }
}

void WTxtBuf::Cat(i32 c, i32 l)
{
    if(pend + l > limit)
        Expand(max(GetLength(), l) + GetLength(), NULL, l);
    memset32(pend, c, l);
    pend += l;
}

void WTxtBuf::Set(WTxt& s)
{
    s.UnShare();
    i32 l = s.GetLength();
    pbegin = s.ptr;
    pend = pbegin + l;
    limit = pbegin + s.GetAlloc();
    s.Zero();
}

WTxt TrimLeft(const WTxt& str)
{
    wtukk s = str;
    if(!IsSpace(*s))
        return s;
    while(IsSpace(*s)) s++;
    return WTxt(s, str.End());
}

WTxt TrimRight(const WTxt& str)
{
    if(str.IsEmpty())
        return str;
    wtukk s = str.Last();
    if(!IsSpace(*s))
        return str;
    while(s >= ~str && IsSpace(*s)) s--;
    return WTxt(~str, s + 1);
}

struct WTxtICompare__
{
    i32 operator()(wchar a, wchar b) const { return ToUpper(a) - ToUpper(b); }
};

i32 CompareNoCase(const WTxt& a, const WTxt& b)
{
#ifdef DEPRECATED
    return IterCompare(a.Begin(), a.End(), b.Begin(), b.End(), WTxtICompare__());
#else
    return CompareRanges(a, b, WTxtICompare__());
#endif
}

i32 CompareNoCase(const WTxt& a, wtukk b)
{
#ifdef DEPRECATED
    return IterCompare(a.Begin(), a.End(), b, b + strlen__(b), WTxtICompare__());
#else
    return CompareRanges(a, SubRange(b, b + strlen__(b)), WTxtICompare__());
#endif
}

}