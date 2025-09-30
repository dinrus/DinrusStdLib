class Nuller;

inline i32 strlen8(tukk s) { return s ? (i32)strlen(s) : 0; }
i32 strlen16(const char16 *s);
i32 strlen32(wtukk s);

#ifdef TARGET_POSIX
inline i32 stricmp(tukk a, tukk b)         { return strcasecmp(a, b); }
inline i32 strnicmp(tukk a, tukk b, i32 n) { return strncasecmp(a, b, n); }
#endif

#ifdef TARGET_WINCE
inline i32 stricmp(tukk a, tukk b)         { return _stricmp(a, b); }
inline i32 strnicmp(tukk a, tukk b, i32 n) { return _strnicmp(a, b, n); }
#endif

force_inline i32 strlen__(tukk s)  { return strlen8(s); }
inline i32 strlen__(wtukk s)       { return strlen32(s); }

inline i32 cmpval__(char x)               { return (u8)x; }
inline i32 cmpval__(wchar x)              { return x; }

i32 find(tukk text, i32 len, tukk needle, i32 nlen, i32 from);
i32 find(wtukk text, i32 len, wtukk needle, i32 nlen, i32 from);

class Txt;
class WTxt;
class TxtBuf;
class WTxtBuf;
class Stream;

template <class B>
class ATxt : public B {
    typedef typename B::tchar  tchar;
    typedef typename B::bchar  bchar;
    typedef typename B::Buffer buffer;
    typedef typename B::Txt Txt;

public:
    void Clear()                                              { B::Free(); B::Zero(); }
    i32  GetLength() const                                    { return B::GetCount(); }
    bool IsEmpty() const                                      { return B::GetCount() == 0; }

    const tchar *End() const                                  { return B::Begin() + GetLength(); }
    const tchar *end() const                                  { return End(); }
    const tchar *Last() const                                 { return End() - !!B::GetCount(); }
    const tchar *GetIter(i32 i) const                         { ASSERT(i >= 0 && i <= B::GetCount()); return B::Begin() + i; }

    i32 operator*() const                                     { return *B::Begin(); }
    i32 operator[](i32 i) const                               { ASSERT(i >= 0 && i <= B::GetCount()); return B::Begin()[i]; }

    operator const tchar *() const                            { return B::Begin(); }
    const tchar *operator~() const                            { return B::Begin(); }
    operator const bchar *() const                            { return (bchar *)B::Begin(); }
    operator ukk () const                                     { return B::Begin(); }

    void Insert(i32 pos, i32 c)                               { *B::Insert(pos, 1, NULL) = c; }
    void Insert(i32 pos, const tchar *s, i32 count)           { B::Insert(pos, count, s); }
    void Insert(i32 pos, const Txt& s)                        { Insert(pos, s, s.GetCount()); }
    void Insert(i32 pos, tukk s);

    void TrimLast(i32 count = 1)                              { B::Trim(B::GetCount() - count); }

    void  Cat(i32 c)                                          { B::Cat(c); }
    void  Cat(const tchar *s, i32 len)                        { B::Cat(s, len); }
    void  Cat(const tchar *s);
    void  Cat(const Txt& s)                                   { Cat(~s, s.GetLength()); }
    void  Cat(i32 c, i32 count);
    void  Cat(const tchar *s, const tchar *lim)               { ASSERT(s <= lim); Cat(s, i32(lim - s)); }
    void  Cat(const Txt& s, i32 len)                          { B::Cat(~s, len); }
    void  Cat(const bchar *s, i32 len)                        { Cat((const tchar *) s, len); }

    Txt&   Cat()                                              { return *(Txt *)this; }

    i32    Compare(const Txt& s) const                        { return B::Compare(s); }
    i32    Compare(const tchar *s) const;

    bool   IsEqual(const Txt& s) const                        { return B::IsEqual(s); }
    bool   IsEqual(const tchar *s) const                      { return B::IsEqual(s); }

    Txt    Mid(i32 pos, i32 length) const;
    Txt    Mid(i32 pos) const                                 { return Mid(pos, GetLength() - pos); }
    Txt    Right(i32 count) const                             { return Mid(GetLength() - count); }
    Txt    Left(i32 count) const                              { return Mid(0, count); }

    i32    Find(i32 chr, i32 from = 0) const;
    i32    ReverseFind(i32 chr, i32 from) const;
    i32    ReverseFind(i32 chr) const;

    i32    Find(i32 len, const tchar *s, i32 from) const   { return find(B::Begin(), B::GetCount(), s, len, from); }
    i32    Find(const tchar *s, i32 from = 0) const        { return Find(strlen__(s), s, from); }
    i32    Find(const Txt& s, i32 from = 0) const          { return Find(s.GetCount(), ~s, from); }

    i32    FindAfter(const tchar *s, i32 from = 0) const   { i32 n = strlen__(s); i32 q = Find(n, s, from); return q < 0 ? -1 : q + n; }
    i32    FindAfter(const Txt& s, i32 from = 0) const     { i32 n = s.GetCount(); i32 q = Find(n, ~s, from); return q < 0 ? -1 : q + n; }

    i32    ReverseFind(i32 len, const tchar *s, i32 from) const;
    i32    ReverseFind(const tchar *s, i32 from) const;
    i32    ReverseFind(const Txt& s, i32 from) const       { return ReverseFind(s.GetCount(), ~s, from); }
    i32    ReverseFind(const tchar *s) const               { return GetLength() ? ReverseFind(s, GetLength()-1) : -1;}
    i32    ReverseFind(const Txt& s) const                 { return GetLength() ? ReverseFind(s, GetLength()-1) : -1;}

    i32    ReverseFindAfter(i32 len, const tchar *s, i32 from) const;
    i32    ReverseFindAfter(const tchar *s, i32 from) const;
    i32    ReverseFindAfter(const Txt& s, i32 from) const  { return ReverseFindAfter(s.GetCount(), ~s, from); }
    i32    ReverseFindAfter(const tchar *s) const          { return GetLength() ? ReverseFindAfter(s, GetLength()-1) : -1;}
    i32    ReverseFindAfter(const Txt& s) const            { return GetLength() ? ReverseFindAfter(s, GetLength()-1) : -1;}

    void   Replace(const tchar *find, i32 findlen, const tchar *replace, i32 replacelen);
    void   Replace(const Txt& find, const Txt& replace);
    void   Replace(const tchar *find, const tchar *replace);
    void   Replace(const Txt& find, const tchar *replace);
    void   Replace(const tchar *find, const Txt& replace);

    bool   StartsWith(const tchar *s, i32 len) const;
    bool   StartsWith(const tchar *s) const;
    bool   StartsWith(const Txt& s) const                  { return StartsWith(~s, s.GetLength()); }

    bool   TrimStart(const tchar *s, i32 len)              { if(!StartsWith(s, len)) return false; B::Remove(0, len); return true; }
    bool   TrimStart(const tchar *s)                       { return TrimStart(s, strlen__(s)); }
    bool   TrimStart(const Txt& s)                         { return TrimStart(~s, s.GetLength()); }

    bool   EndsWith(const tchar *s, i32 len) const;
    bool   EndsWith(const tchar *s) const;
    bool   EndsWith(const Txt& s) const                    { return EndsWith(~s, s.GetLength()); }

    bool   TrimEnd(const tchar *s, i32 len)                { if(!EndsWith(s, len)) return false; TrimLast(len); return true; }
    bool   TrimEnd(const tchar *s)                         { return TrimEnd(s, strlen__(s)); }
    bool   TrimEnd(const Txt& s)                           { return TrimEnd(~s, s.GetLength()); }

    i32    FindFirstOf(i32 len, const tchar *set, i32 from = 0) const;
    i32    FindFirstOf(const tchar *set, i32 from = 0) const  { return FindFirstOf(strlen__(set), set, from); }
    i32    FindFirstOf(const Txt& set, i32 from = 0) const { return FindFirstOf(set.GetCount(), ~set, from); }

    friend bool operator<(const Txt& a, const Txt& b)      { return a.Compare(b) < 0; }
    friend bool operator<(const Txt& a, const tchar *b)    { return a.Compare(b) < 0; }
    friend bool operator<(const tchar *a, const Txt& b)    { return b.Compare(a) > 0; }

    friend bool operator<=(const Txt& a, const Txt& b)     { return a.Compare(b) <= 0; }
    friend bool operator<=(const Txt& a, const tchar *b)   { return a.Compare(b) <= 0; }
    friend bool operator<=(const tchar *a, const Txt& b)   { return b.Compare(a) >= 0; }

    friend bool operator>(const Txt& a, const Txt& b)      { return a.Compare(b) > 0; }
    friend bool operator>(const Txt& a, const tchar *b)    { return a.Compare(b) > 0; }
    friend bool operator>(const tchar *a, const Txt& b)    { return b.Compare(a) < 0; }

    friend bool operator>=(const Txt& a, const Txt& b)      { return a.Compare(b) >= 0; }
    friend bool operator>=(const Txt& a, const tchar *b)    { return a.Compare(b) >= 0; }
    friend bool operator>=(const tchar *a, const Txt& b)    { return b.Compare(a) <= 0; }

    friend bool operator==(const Txt& a, const Txt& b)      { return a.IsEqual(b); }
    friend bool operator!=(const Txt& a, const Txt& b)      { return !a.IsEqual(b); }
    friend bool operator==(const Txt& a, const tchar *b)    { return a.IsEqual(b); }
    friend bool operator==(const tchar *a, const Txt& b)    { return b.IsEqual(a); }
    friend bool operator!=(const Txt& a, const tchar *b)    { return !a.IsEqual(b); }
    friend bool operator!=(const tchar *a, const Txt& b)    { return !b.IsEqual(a); }

    friend Txt operator+(const Txt& a, const Txt& b)     { Txt c(a); c += b; return c; }
    friend Txt operator+(const Txt& a, const tchar *b)   { Txt c(a); c += b; return c; }
    friend Txt operator+(const Txt& a, tchar b)          { Txt c(a); c += b; return c; }
    friend Txt operator+(Txt&& a, const Txt& b)          { Txt c(pick(a)); c += b; return c; }
    friend Txt operator+(Txt&& a, const tchar *b)        { Txt c(pick(a)); c += b; return c; }
    friend Txt operator+(Txt&& a, tchar b)               { Txt c(pick(a)); c += b; return c; }
    friend Txt operator+(const tchar *a, const Txt& b)   { Txt c(a); c += b; return c; }
    friend Txt operator+(tchar a, const Txt& b)          { Txt c(a, 1); c += b; return c; }
};

class Txt0 : Moveable<Txt0> {
    enum { //
        KIND = 14,    // chr[KIND] is Txt tier flag, 0 - small, 31 - medium, 32..254 ref alloc, 255 - read alloc from Ref
        SLEN = 15,    // chr[SLEN] stores the length of small tier strings (up to 14 bytes)
        LLEN = 2,     // chr[LLEN] stores the length of medium (< 32) and large tier strings
        SPECIAL = 13, // chr[SPECIAL]: stores Val type
    };
    enum { // chr[KIND] predefined values, if > MEDIUM, it is Rc type; if <255, also stores alloc
        SMALL = 0, // SVO ending zero, also has to be 0 because of GetSpecial, also flag of small
        MEDIUM = 31 // medium, ptr points to 32 bytes block on heap
    };

    struct Rc {
        Atomic refcount;
        i32    alloc;

        char *GetPtr() const  { return (char*)(this + 1); }
    };

    union {
        char   chr[16];
        char  *ptr; // medium and large tier, in large tier (Rc *)ptr - 1 points to Rc
        dword *wptr;
        qword *qptr;
        word   v[8];
        dword  w[4];
        qword  q[2];
    };


#if defined(_DEBUG) && defined(COMPILER_GCC)
    i32          len;
    const char  *s;
#endif

#ifdef _DEBUG
    void Dsyn();
#else
    void Dsyn() {}
#endif

    char&  SLen()                { return chr[SLEN]; }
    char   SLen() const          { return chr[SLEN]; }
    dword& LLen()                { return w[LLEN]; }
    dword  LLen() const          { return w[LLEN]; }
    bool   IsSmall() const       { return chr[KIND] == SMALL; }
    bool   IsLarge() const       { return chr[KIND] != SMALL; }
    bool   IsMedium() const      { return chr[KIND] == MEDIUM; }
    bool   IsRef() const         { return (u8)chr[KIND] > MEDIUM; }
    Rc    *Ref() const           { return (Rc *)ptr - 1; }
    bool   IsShared() const      { return Ref()->refcount != 1; }
    bool   IsSharedRef() const   { return IsRef() && IsShared(); }
    i32    LAlloc() const        { i32 b = (u8)chr[KIND]; return b == 255 ? Ref()->alloc : b; }
    dword  LEqual(const Txt0& s) const;

    void LSet(const Txt0& s);
    void LFree();
    void LCat(i32 c);
    hash_t LHashVal() const;

    void UnShare();
    void SetSLen(i32 l);

    char *Ptr()                   { return IsSmall() ? chr : ptr; }
    char *Alloc_(i32 count, char& kind);
    char *Alloc(i32 count, char& kind);

    static Txt0::Rc voidptr[2];

    void Swap(Txt0& b);

    // interface for Val
    static dword StW(u8 st)      { return MAKE4B(0, st, 0, 0); }
    void  SetSpecial0(u8 st)     { w[3] = StW(st); }
    void  SetSpecial(u8 st)      { ASSERT(IsSmall() && GetCount() == 0); SetSpecial0(st); }
    u8    GetSpecial() const     { return (chr[SLEN] | chr[KIND]) == 0 ? chr[SPECIAL] : 0; }
    u8    GetSt() const          { return chr[SPECIAL]; }
    dword GetStW() const         { return w[3]; }
    bool  IsSpecial() const      { return !v[7] && v[6]; }
    bool  IsSpecial(u8 st) const { return w[3] == StW(st); }

    friend class Txt;
    friend class TxtBuf;
    friend class Val;
    friend class TextCtrl;

protected:
    void Zero()                     { q[0] = q[1] = 0; Dsyn(); }
    void SetSmall(const Txt0& s)    { q[0] = s.q[0]; q[1] = s.q[1]; }
    void Free()                     { if(IsLarge()) LFree(); }
    void Pick0(Txt0&& s) {
        SetSmall(s);
        s.Zero();
    }
    void Set0(const Txt0& s) {
        if(s.IsSmall()) SetSmall(s); else LSet(s);
        Dsyn();
    }
    void Assign(const Txt0& s) {
        if(s.IsSmall()) {
            Free();
            SetSmall(s);
        }
        else
            if(this != &s) {
                Free();
                LSet(s);
            }
        Dsyn();
    }
    void  Set0(tukk s, i32 len);
    void  SetL(tukk s, i32 len);
    char *Insert(i32 pos, i32 count, tukk str);

    typedef char         tchar;
    typedef u8           bchar;
    typedef TxtBuf    Buffer;
    typedef drx::Txt     Txt;

    i32    CompareL(const Txt0& s) const;

public:
    bool LEq(const Txt0& s) const;
    bool IsEqual(const Txt0& s) const {
        zu64 q1 = q[1];
        zu64 sq1 = s.q[1];
        return (q1 == sq1 && q[0] == s.q[0]) || (((q1 | sq1) & MAKE8B(0,0,0,0,0,0,255,0)) && LEq(s));
    }
    bool IsEqual(tukk s) const;

    i32    Compare(const Txt0& s) const;

    hash_t GetHashVal() const {
#ifdef HASH64
        return chr[KIND] ? LHashVal() : (hash_t)CombineHash(q[0], q[1]);
#else
        return chr[KIND] ? LHashVal() : (hash_t)CombineHash(w[0], w[1], w[2], w[3]);
#endif
    }

    void Cat(i32 c) {
        if(SLen() < 14)
            chr[i32(SLen()++)] = c;
        else
            LCat(c);
        Dsyn();
    }

    void Cat(tukk s, i32 len);
    void Set(tukk s, i32 len);

    void Set(i32 i, i32 chr);
    void Trim(i32 pos);

    tukk Begin() const   { return IsSmall() ? chr : ptr; }
    tukk begin() const   { return Begin(); }
    tukk End() const     { return Begin() + GetLength(); }
    tukk end() const     { return End(); }

    i32 operator[](i32 i) const { ASSERT(i >= 0 && i <= GetCount()); return Begin()[i]; }

    operator tukk () const   { return Begin(); }
    tukk operator~() const   { return Begin(); }

    void Remove(i32 pos, i32 count = 1);
    void Clear()                { Free(); Zero(); }

    i32 GetCount() const        { return IsSmall() ? chr[SLEN] : w[LLEN]; }
    i32 GetLength() const       { return GetCount(); }
    i32 GetAlloc() const        { return IsSmall() ? 14 : LAlloc(); }

    void Reserve(i32 r);

    Txt0()                   {}
    ~Txt0()                  { Free(); }
};

class Txt : public Moveable<Txt, ATxt<Txt0> > {
    void Swap(Txt& b)                                   { Txt0::Swap(b); }

#ifdef _DEBUG
#ifndef COMPILER_GCC
    i32          len;
    const char  *s;
#endif
    friend class Txt0;
#endif

    void AssignLen(tukk s, i32 slen);

    enum SSPECIAL { SPECIAL };

    template <class T>
    Txt(const T& x, u8 st, SSPECIAL) {
        *(T*)chr = x;
        SetSpecial0(st);
    }
    Txt(SSPECIAL) {}

    friend class Val;

public:
    const Txt& operator+=(char c)                { Cat(c); return *this; }
    const Txt& operator+=(tukk s)                { Cat(s); return *this; }
    const Txt& operator+=(const Txt& s)          { Cat(s); return *this; }

    Txt& operator=(tukk s);
    Txt& operator=(const Txt& s)                 { Txt0::Assign(s); return *this; }
    Txt& operator=(Txt&& s)                      { if(this != &s) { Free(); Pick0(pick(s)); } return *this; }
    Txt& operator=(TxtBuf& b)                    { *this = Txt(b); return *this; }
 
    void   Shrink()                              { *this = Txt(Begin(), GetLength()); }
    i32    GetCharCount() const;

    Txt()                                        { Zero(); }
    Txt(const Nuller&)                           { Zero(); }
    Txt(const Txt& s)                            { Txt0::Set0(s); }
    Txt(Txt&& s)                                 { Txt0::Pick0(pick(s)); }
    Txt(tukk s);
    Txt(const Txt& s, i32 n)                     { ASSERT(n >= 0 && n <= s.GetLength()); Txt0::Set0(~s, n); }
    Txt(tukk s, i32 n)                           { Txt0::Set0(s, n); }
    Txt(u8k *s, i32 n)                           { Txt0::Set0((tukk )s, n); }
    Txt(tukk s, tukk lim)                        { Txt0::Set0(s, (i32)(lim - s)); }
    Txt(i32 chr, i32 count)                      { Txt0::Zero(); Cat(chr, count); }
    Txt(TxtBuf& b);


    Txt(char16 *s);
    Txt(wtuk s);

    WTxt       ToWTxt() const;
    const Txt& ToTxt() const                     { return *this; }

    static Txt GetVoid();
    bool       IsVoid() const;

    friend void Swap(Txt& a, Txt& b)             { a.Swap(b); }

    Txt(const std::string& s)                    { Txt0::Set0(s.c_str(), (i32)s.length()); }
    std::string ToStd() const                    { return std::string(Begin(), End()); }

    template <class Maker>
    static Txt Make(i32 alloc, Maker m);
};

inline std::string to_string(const Txt& s)       { return std::string(s.Begin(), s.End()); }

class TxtBuf : NoCopy {
    char   *pbegin;
    char   *pend;
    char   *limit;
    char    buffer[256];

    friend class Txt;

    typedef Txt0::Rc Rc;

    char *Alloc(i32 len, i32& alloc);
    void  Realloc(dword n, tukk cat = NULL, i32 l = 0);
    void  ReallocL(tukk s, i32 l);
    void  Expand();
    void  Zero()                    { pbegin = pend = buffer; limit = pbegin + 255; }
    void  Free();
    void  Set(Txt& s);

public:
    char *Begin()                   { *pend = '\0'; return pbegin; }
    char *begin()                   { return Begin(); }
    char *End()                     { *pend = '\0'; return pend; }
    char *end()                     { return End(); }

    char& operator*()               { return *Begin(); }
    char& operator[](i32 i)         { return Begin()[i]; }
    operator char*()                { return Begin(); }
    operator u8*()                  { return (u8 *)Begin(); }
    operator void*()                { return Begin(); }
    char *operator~()               { return Begin(); }

    void SetLength(i32 l);
    void SetCount(i32 l)            { SetLength(l); }
    i32  GetLength() const          { return (i32)(pend - pbegin); }
    i32  GetCount() const           { return GetLength(); }
    void Strlen();
    void Clear()                    { Free(); Zero(); }
    void Reserve(i32 r)             { i32 l = GetLength(); SetLength(l + r); SetLength(l); }
    void Shrink();

    void Cat(i32 c)                 { if(pend >= limit) Expand(); *pend++ = c; }
    void Cat(i32 c, i32 count);
    void Cat(tukk s, i32 l);
    void Cat(tukk s, tukk e) { Cat(s, i32(e - s)); }
    void Cat(tukk s);
    void Cat(const Txt& s)    { Cat(s, s.GetLength()); }

    i32  GetAlloc() const     { return (i32)(limit - pbegin); }

    void operator=(Txt& s)    { Free(); Set(s); }

    TxtBuf()                  { Zero(); }
    TxtBuf(Txt& s)            { Set(s); }
    TxtBuf(i32 len)           { Zero(); SetLength(len); }
    ~TxtBuf()                 { if(pbegin != buffer) Free(); }
};

inline bool  IsEmpty(const Txt& s)      { return s.IsEmpty(); }

Txt FormatPtr(ukk p);

template <class T>
inline Txt AsTxt(const T& x)
{
    return x.ToTxt();
}

template <class T>
inline Txt AsTxt(T *x)
{
    return FormatPtr(x);
}

force_inline Txt& operator<<(Txt& s, tukk x)
{
    s.Cat(x, strlen__(x));
    return s;
}

force_inline Txt& operator<<(Txt& s, char *x)
{
    s.Cat(x);
    return s;
}

inline Txt& operator<<(Txt& s, const Txt &x)
{
    s.Cat(x);
    return s;
}

inline Txt& operator<<(Txt& s, char x)
{
    s.Cat((i32) x);
    return s;
}

inline Txt& operator<<(Txt& s, ukk x)
{
    s << FormatPtr(x);
    return s;
}

inline Txt& operator<<(Txt& s, uk x)
{
    s << FormatPtr(x);
    return s;
}

template <class T>
inline Txt& operator<<(Txt& s, const T& x)
{
    s.Cat(AsTxt(x));
    return s;
}

template<>
inline Txt& operator<<(Txt& s, tukk  const &x)
{
    s.Cat(x);
    return s;
}

template<>
inline Txt& operator<<(Txt& s, const Txt &x)
{
    s.Cat(x);
    return s;
}

template<>
inline Txt& operator<<(Txt& s, const char& x)
{
    s.Cat(x);
    return s;
}

force_inline Txt& operator<<(Txt&& s, tukk x)
{
    s.Cat(x, strlen__(x));
    return static_cast<Txt&>(s);
}

force_inline Txt& operator<<(Txt&& s, char *x)
{
    s.Cat(x);
    return static_cast<Txt&>(s);
}

inline Txt& operator<<(Txt&& s, const Txt &x)
{
    s.Cat(x);
    return static_cast<Txt&>(s);
}

inline Txt& operator<<(Txt&& s, char x)
{
    s.Cat((i32) x);
    return static_cast<Txt&>(s);
}

inline Txt& operator<<(Txt&& s, ukk x)
{
    s << FormatPtr(x);
    return static_cast<Txt&>(s);
}

inline Txt& operator<<(Txt&& s, uk x)
{
    s << FormatPtr(x);
    return static_cast<Txt&>(s);
}

template <class T>
inline Txt& operator<<(Txt&& s, const T& x)
{
    s.Cat(AsTxt(x));
    return static_cast<Txt&>(s);
}

template<>
inline Txt& operator<<(Txt&& s, tukk  const &x)
{
    s.Cat(x);
    return static_cast<Txt&>(s);
}

template<>
inline Txt& operator<<(Txt&& s, const Txt &x)
{
    s.Cat(x);
    return static_cast<Txt&>(s);
}

template<>
inline Txt& operator<<(Txt&& s, const char& x)
{
    s.Cat(x);
    return static_cast<Txt&>(s);
}

template<>
inline bool  IsNull(const Txt& s)       { return s.IsEmpty(); }

inline
bool IsVoid(const Txt& s)               { return s.IsVoid(); }

template<>
inline Txt AsTxt(const Txt& s)     { return s; }

template<>
inline hash_t GetHashVal(const Txt& s) { return s.GetHashVal(); }

i32 CompareNoCase(const Txt& a, const Txt& b);
i32 CompareNoCase(const Txt& a, tukk b);

/*
inline
i32 CompareNoCase(tukk a, const Txt& b) {
    return -CompareNoCase(b, a);
}
*/

Txt TrimLeft(const Txt& s);
Txt TrimRight(const Txt& s);
Txt TrimBoth(const Txt& s);

Txt TrimLeft(tukk prefix, i32 len, const Txt& s);
force_inline Txt TrimLeft(tukk prefix, const Txt& s)    { return TrimLeft(prefix, (i32)strlen(prefix), s); }
force_inline Txt TrimLeft(const Txt& prefix, const Txt& s)  { return TrimLeft(~prefix, prefix.GetCount(), s); }

Txt TrimRight(tukk suffix, i32 len, const Txt& s);
force_inline Txt TrimRight(tukk suffix, const Txt& s)   { return TrimRight(suffix, (i32)strlen(suffix), s); }
force_inline Txt TrimRight(const Txt& suffix, const Txt& s) { return TrimRight(~suffix, suffix.GetCount(), s); }

inline TxtBuf& operator<<(TxtBuf& s, tukk x)
{
    s.Cat(x);
    return s;
}

inline TxtBuf& operator<<(TxtBuf& s, char *x)
{
    s.Cat(x);
    return s;
}

inline TxtBuf& operator<<(TxtBuf& s, const Txt &x)
{
    s.Cat(x);
    return s;
}

inline TxtBuf& operator<<(TxtBuf& s, char x)
{
    s.Cat((i32) x);
    return s;
}

inline TxtBuf& operator<<(TxtBuf& s, ukk x)
{
    s << FormatPtr(x);
    return s;
}

inline TxtBuf& operator<<(TxtBuf& s, uk x)
{
    s << FormatPtr(x);
    return s;
}

template <class T>
inline TxtBuf& operator<<(TxtBuf& s, const T& x)
{
    s.Cat(AsTxt(x));
    return s;
}

template<>
inline TxtBuf& operator<<(TxtBuf& s, tukk  const &x)
{
    s.Cat(x);
    return s;
}

template<>
inline TxtBuf& operator<<(TxtBuf& s, const Txt &x)
{
    s.Cat(x);
    return s;
}

template<>
inline TxtBuf& operator<<(TxtBuf& s, const char& x)
{
    s.Cat(x);
    return s;
}

template <class T>
void RawCat(Txt& s, const T& x)
{
    s.Cat((tukk )&x, sizeof(x));
}

template <class T>
void RawCat(TxtBuf& s, const T& x)
{
    s.Cat((tukk )&x, sizeof(x));
}

class WTxt0 {
    enum { SMALL = 23 };

    wtuk ptr;
    i32    length;
    i32    alloc;

#ifdef _DEBUG
    void Dsyn();
#else
    void Dsyn() {}
#endif

    static Atomic voidptr[2];

    bool    IsRc() const  { return alloc > SMALL; }
    Atomic& Rc()          { return *((Atomic *)ptr - 1); }
    bool    IsShared()    { return IsRc() && Rc() > 1; }

    wchar  *Alloc(i32& count);
    void    LCat(i32 c);
    void    UnShare();

    friend class WTxtBuf;
    friend class WTxt;

protected:
    typedef wchar         tchar;
    typedef i16         bchar;
    typedef WTxtBuf Buffer;
    typedef WTxt       Txt;

    void    Zero()                       { static wchar e[2]; length = alloc = 0; ptr = e; Dsyn(); ASSERT(*ptr == 0); }
    void    Set0(wtukk s, i32 length);
    void    Set0(const WTxt0& s);
    void    Pick0(WTxt0&& s)          { ptr = s.ptr; length = s.length; alloc = s.alloc; s.Zero(); Dsyn(); }
    void    Free();
    void    FFree()                      { if(alloc > 0) Free(); }
    void    Swap(WTxt0& b)            { drx::Swap(ptr, b.ptr); drx::Swap(length, b.length); drx::Swap(alloc, b.alloc); Dsyn(); b.Dsyn(); }
    wchar  *Insert(i32 pos, i32 count, wtukk data);

public:
    wtukk Begin() const           { return ptr; }
    wtukk End() const             { return Begin() + GetLength(); }
    wtukk begin() const           { return Begin(); }
    wtukk end() const             { return End(); }
    i32   operator[](i32 i) const        { return ptr[i]; }

    operator wtukk () const       { return Begin(); }
    wtukk operator~() const       { return Begin(); }

    void Cat(i32 c)                      { if(!IsRc() && length < alloc) { ptr[length++] = c; ptr[length] = 0; } else LCat(c); Dsyn(); }
    void Cat(wtukk s, i32 length);
    void Set(wtukk s, i32 length);

    i32  GetCount() const                { return length; }
    i32  GetLength() const               { return length; }
    i32  GetAlloc() const                { return alloc; }

    hash_t   GetHashVal() const             { return memhash(ptr, length * sizeof(wchar)); }
    bool     IsEqual(const WTxt0& s) const { return s.length == length && memeq_t(ptr, s.ptr, length); }
    bool     IsEqual(wtukk s) const    { i32 l = strlen__(s); return l == GetCount() && memeq_t(begin(), s, l); }
    i32      Compare(const WTxt0& s) const;

    void Remove(i32 pos, i32 count = 1);
    void Insert(i32 pos, wtukk s, i32 count);
    void Clear()                         { Free(); Zero(); }

    void Set(i32 pos, i32 ch);
    void Trim(i32 pos);

    WTxt0()                           { Zero(); }
    ~WTxt0()                          { Free(); }

//  WTxt0& operator=(const WTxt0& s) { Free(); Set0(s); return *this; }
};

class WTxt : public Moveable<WTxt, ATxt<WTxt0> >
{
    void Swap(WTxt& b)                                   { WTxt0::Swap(b); }

#ifdef _DEBUG
    i32          len;
    wtukk s;
    friend class WTxt0;
#endif

public:
    DRX::Txt ToTxt() const;

    const WTxt& operator+=(wchar c)                      { Cat(c); return *this; }
    const WTxt& operator+=(wtukk s)               { Cat(s); return *this; }
    const WTxt& operator+=(const WTxt& s)             { Cat(s); return *this; }

    WTxt& operator<<(wchar c)                            { Cat(c); return *this; }
    WTxt& operator<<(const WTxt& s)                   { Cat(s); return *this; }
    WTxt& operator<<(wtukk s)                     { Cat(s); return *this; }

    WTxt& operator=(wtukk s);
    WTxt& operator=(const WTxt& s)                    { if(this != &s) { WTxt0::FFree(); WTxt0::Set0(s); } return *this; }
    WTxt& operator=(WTxt&& s)                         { if(this != &s) { WTxt0::FFree(); WTxt0::Pick0(pick(s)); } return *this; }
    WTxt& operator=(WTxtBuf& b)                    { *this = WTxt(b); return *this; }
//  WTxt& operator<<=(const WTxt& s)                  { if(this != &s) { WTxt0::Free(); WTxt0::Set0(s, s.GetCount()); } return *this; }

    void   Shrink()                                         { *this = WTxt(Begin(), GetLength()); }

    WTxt()                                               {}
    WTxt(const Nuller&)                                  {}
    WTxt(const WTxt& s)                               { WTxt0::Set0(s); }
    WTxt(WTxt&& s)                                    { WTxt0::Pick0(pick(s)); }
    WTxt(wtukk s)                                 { WTxt0::Set0(s, strlen__(s)); }
    WTxt(const WTxt& s, i32 n)                        { ASSERT(n >= 0 && n <= s.GetLength()); WTxt0::Set0(~s, n); }
    WTxt(wtukk s, i32 n)                          { WTxt0::Set0(s, n); }
    WTxt(wtukk s, wtukk lim)               { WTxt0::Set0(s, (i32)(lim - s)); }
    WTxt(i32 chr, i32 count)                             { WTxt0::Zero(); Cat(chr, count); }
    WTxt(WTxtBuf& b);

    WTxt(tukk s);
    WTxt(tukk s, i32 n);
    WTxt(tukk s, tukk lim);

    WTxt(const char16 *s);

    static WTxt GetVoid();
    bool   IsVoid() const                                   { return alloc < 0; }

    friend void Swap(WTxt& a, WTxt& b)                { a.Swap(b); }
    friend WTxt operator+(const WTxt& a, char b)      { WTxt c(a); c += b; return c; }
    friend WTxt operator+(WTxt&& a, char b)           { WTxt c(pick(a)); c += b; return c; }
    friend WTxt operator+(char a, const WTxt& b)      { WTxt c(a, 1); c += b; return c; }

#ifndef _HAVE_NO_STDWTXT
    WTxt(const std::wstring& s);
    operator std::wstring() const                           { return ToStd(); }
    std::wstring ToStd() const;
#endif
};

#ifndef _HAVE_NO_STDWTXT
inline std::wstring to_string(const WTxt& s)             { return s.ToStd(); }
#endif

class WTxtBuf : NoCopy {
    wchar   *pbegin;
    wchar   *pend;
    wchar   *limit;

    friend class WTxt;

    wtuk Alloc(i32 len, i32& alloc);
    void   Expand(dword n, wtukk cat = NULL, i32 l = 0);
    void   Expand();
    void   Zero();
    void   Free();
    void   Set(WTxt& s);

public:
    wtuk Begin()                   { *pend = '\0'; return pbegin; }
    wtuk begin()                   { return Begin(); }
    wtuk End()                     { *pend = '\0'; return pend; }
    wtuk end()                     { return End(); }

    wchar& operator*()               { return *Begin(); }
    wchar& operator[](i32 i)         { return Begin()[i]; }
    operator wchar*()                { return Begin(); }
    operator i16*()                { return (i16 *)Begin(); }
    operator void*()                 { return Begin(); }
    wtuk operator~()               { return Begin(); }

    void  SetLength(i32 l);
    void  SetCount(i32 l)            { SetLength(l); }
    i32   GetLength() const          { return (i32)(pend - pbegin); }
    i32   GetCount() const           { return GetLength(); }
    void  Strlen()                   { SetLength(strlen__(pbegin)); }
    void  Clear()                    { Free(); Zero(); }
    void  Reserve(i32 r)             { i32 l = GetLength(); SetLength(l + r); SetLength(l); }

    void  Cat(wchar c)                          { if(pend >= limit) Expand(); *pend++ = c; }
    void  Cat(wchar c, i32 count);
    void  Cat(wtukk s, i32 l);
    void  Cat(wtukk s, wtukk e) { Cat(s, i32(e - s)); }
    void  Cat(wtukk s)                 { Cat(s, strlen__(s)); }
    void  Cat(const WTxt& s)               { Cat(s, s.GetLength()); }
    void  Cat(tukk s)                  { Cat(WTxt(s)); }

    i32   GetAlloc() const           { return (i32)(limit - pbegin); }

    void operator=(WTxt& s)       { Free(); Set(s); }

    WTxtBuf()                  { Zero(); }
    WTxtBuf(WTxt& s)        { Set(s); }
    WTxtBuf(i32 len)           { Zero(); SetLength(len); }
    ~WTxtBuf()                 { Free(); }
};

inline bool  IsEmpty(const WTxt& s)      { return s.IsEmpty(); }

template<>
inline bool  IsNull(const WTxt& s)       { return s.IsEmpty(); }

//template<>
//inline Txt AsTxt(const WTxt& s)     { return s; }

template<>
inline hash_t GetHashVal(const WTxt& s) { return memhash(~s, 2 * s.GetLength()); }

WTxt TrimLeft(const WTxt& str);
WTxt TrimRight(const WTxt& s);

i32 CompareNoCase(const WTxt& a, const WTxt& b);
i32 CompareNoCase(const WTxt& a, wtukk b);

inline
i32 CompareNoCase(wtukk a, const WTxt& b) {
    return -CompareNoCase(b, a);
}

template<> inline Txt AsTxt(tukk  const &s)    { return s; }
template<> inline Txt AsTxt(char * const &s)          { return s; }
template<> inline Txt AsTxt(tukk s)            { return s; }
template<> inline Txt AsTxt(char *s)                  { return s; }
template<> inline Txt AsTxt(const char& a)            { return Txt(a, 1); }
template<> inline Txt AsTxt(i8k& a)     { return Txt(a, 1); }
template<> inline Txt AsTxt(u8k& a)   { return Txt(a, 1); }
template<> inline Txt AsTxt(const bool& a)            { return a ? "true" : "false"; }
template<> inline Txt AsTxt(const std::string& s)     { return Txt(s); }
template<> inline Txt AsTxt(const std::wstring& s)    { return WTxt(s).ToTxt(); }

unsigned ctoi(wchar c);

typedef wchar (*CharFilter)(wchar);

wchar CharFilterAscii(wchar c);
wchar CharFilterAscii128(wchar c);
wchar CharFilterUnicode(wchar c);
wchar CharFilterDigit(wchar c);
wchar CharFilterWhitespace(wchar c);
wchar CharFilterNotWhitespace(wchar c);
wchar CharFilterAlpha(wchar c);
wchar CharFilterToUpper(wchar c);
wchar CharFilterToLower(wchar c);
wchar CharFilterToUpperAscii(wchar c);
wchar CharFilterAlphaToUpper(wchar c);
wchar CharFilterAlphaToLower(wchar c);
wchar CharFilterInt(wchar c);
wchar CharFilterDouble(wchar c);
wchar CharFilterDefaultToUpperAscii(wchar c);
wchar CharFilterCrLf(wchar c);
wchar CharFilterNoCrLf(wchar c);

Txt Filter(tukk s, wchar (*filter)(wchar));
Txt FilterWhile(tukk s, wchar (*filter)(wchar));

WTxt Filter(wtukk s, wchar (*filter)(wchar));
WTxt FilterWhile(wtukk s, wchar (*filter)(wchar));

#include "ATxt.hpp"
