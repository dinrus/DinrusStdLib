#if defined(CPU_UNALIGNED) && defined(CPU_LE)
NOUBSAN inline i32    Peek16le(ukk ptr)  { return *(const word *)ptr; }
NOUBSAN inline i32    Peek32le(ukk ptr)  { return *(const dword *)ptr; }
NOUBSAN inline z64  Peek64le(ukk ptr)  { return *(const z64 *)ptr; }

NOUBSAN inline void   Poke16le(ukk ptr, i32 val)    { *(word *)ptr = val; }
NOUBSAN inline void   Poke32le(ukk ptr, i32 val)    { *(dword *)ptr = val; }
NOUBSAN inline void   Poke64le(ukk ptr, z64 val)  { *(z64 *)ptr = val; }
#else
inline i32    Peek16le(ukk ptr)  { return MAKEWORD(((u8 *)ptr)[0], ((u8 *)ptr)[1]); }
inline i32    Peek32le(ukk ptr)  { return MAKELONG(Peek16le(ptr), Peek16le((u8 *)ptr + 2)); }
inline z64  Peek64le(ukk ptr)  { return MAKEQWORD(Peek32le(ptr), Peek32le((u8 *)ptr + 4)); }

inline void   Poke16le(ukk ptr, i32 val)    { ((u8 *)ptr)[0] = LOBYTE(val); ((u8 *)ptr)[1] = HIBYTE(val); }
inline void   Poke32le(ukk ptr, i32 val)    { Poke16le(ptr, LOWORD(val)); Poke16le((u8 *)ptr + 2, HIWORD(val)); }
inline void   Poke64le(ukk ptr, z64 val)  { Poke32le(ptr, LODWORD(val)); Poke32le((u8 *)ptr + 4, HIDWORD(val)); }
#endif

inline i32    Peek16be(ukk ptr)  { return MAKEWORD(((u8 *)ptr)[1], ((u8 *)ptr)[0]); }
inline i32    Peek32be(ukk ptr)  { return MAKELONG(Peek16be((u8 *)ptr + 2), Peek16be(ptr)); }
inline z64  Peek64be(ukk ptr)  { return MAKEQWORD(Peek32be((u8 *)ptr + 4), Peek32be(ptr)); }

inline void   Poke16be(ukk ptr, i32 val)    { ((u8 *)ptr)[1] = LOBYTE(val); ((u8 *)ptr)[0] = HIBYTE(val); }
inline void   Poke32be(ukk ptr, i32 val)    { Poke16be(ptr, HIWORD(val)); Poke16be((u8 *)ptr + 2, LOWORD(val)); }
inline void   Poke64be(ukk ptr, z64 val)  { Poke32be(ptr, HIDWORD(val)); Poke32be((u8 *)ptr + 4, LODWORD(val)); }

#ifdef CPU_LITTLE_ENDIAN

#define MAKE2B(b0, b1)                            MAKEWORD(b0, b1)
#define MAKE4B(b0, b1, b2, b3)                    MAKELONG(MAKEWORD(b0, b1), MAKEWORD(b2, b3))
#define MAKE8B(b0, b1, b2, b3, b4, b5, b6, b7)    MAKEQWORD(MAKE4B(b0, b1, b2, b3), MAKE4B(b4, b5, b6, b7))

inline i32    Peek16(ukk ptr)  { return Peek16le(ptr); }
inline i32    Peek32(ukk ptr)  { return Peek32le(ptr); }
inline z64  Peek64(ukk ptr)  { return Peek64le(ptr); }

inline void   Poke16(ukk ptr, i32 val)    { Poke16le(ptr, val); }
inline void   Poke32(ukk ptr, i32 val)    { Poke32le(ptr, val); }
inline void   Poke64(ukk ptr, z64 val)  { Poke64le(ptr, val); }

#else

#define MAKE2B(b0, b1)                            MAKEWORD(b1, b0);
#define MAKE4B(b0, b1, b2, b3)                    MAKELONG(MAKEWORD(b2, b3), MAKEWORD(b0, b1))
#define MAKE8B(b0, b1, b2, b3, b4, b5, b6, b7)    MAKEQWORD(MAKE4B(b7, b6, b5, b4), MAKE4B(b3, b2, b1, b0))

inline i32    Peek16(ukk ptr)  { return Peek16be(ptr); }
inline i32    Peek32(ukk ptr)  { return Peek32be(ptr); }
inline z64  Peek64(ukk ptr)  { return Peek64be(ptr); }

inline void   Poke16(ukk ptr, i32 val)    { Poke16be(ptr, val); }
inline void   Poke32(ukk ptr, i32 val)    { Poke32be(ptr, val); }
inline void   Poke64(ukk ptr, z64 val)  { Poke64be(ptr, val); }

#endif

#if defined(CPU_X86) && defined(COMPILER_MSC)
#ifdef COMPILER_GCC
#ifdef CPU_64
inline word   SwapEndian16(word v)    { __asm__("xchgb %b0,%h0" : "=Q" (v) :  "0" (v)); return v; }
inline i16  SwapEndian16(i16 v)   { __asm__("xchgb %b0,%h0" : "=Q" (v) :  "0" (v)); return v; }
#else
inline word   SwapEndian16(word v)    { __asm__("xchgb %b0,%h0" : "=q" (v) :  "0" (v)); return v; }
inline i16  SwapEndian16(i16 v)   { __asm__("xchgb %b0,%h0" : "=q" (v) :  "0" (v)); return v; }
#endif
inline dword  SwapEndian32(dword v)   { __asm__("bswap %0" : "=r" (v) : "0" (v)); return v; }
inline i32    SwapEndian32(i32 v)     { __asm__("bswap %0" : "=r" (v) : "0" (v)); return v; }
#endif

#ifdef COMPILER_MSC
#pragma intrinsic (_byteswap_ushort, _byteswap_ulong, _byteswap_uint64, strlen)

inline word   SwapEndian16(word v)    { return _byteswap_ushort(v); }
inline i16  SwapEndian16(i16 v)   { return _byteswap_ushort(v); }
inline dword  SwapEndian32(dword v)   { return _byteswap_ulong(v); }
inline i32    SwapEndian32(i32 v)     { return _byteswap_ulong(v); }
#endif

inline void   EndianSwap(word& v)     { v = SwapEndian16(v); }
inline void   EndianSwap(i16& v)    { v = SwapEndian16(v); }
inline void   EndianSwap(dword& v)    { v = SwapEndian32(v); }
inline void   EndianSwap(i32& v)      { v = SwapEndian32(v); }

#else

#ifdef COMPILER_GCC

inline dword  SwapEndian32(dword v)   { return __builtin_bswap32(v); }
inline i32    SwapEndian32(i32 v)     { return __builtin_bswap32(v); }

inline word   SwapEndian16(word v)    { return SwapEndian32(v) >> 16; } // GCC bug workaround
inline i16  SwapEndian16(i16 v)   { return SwapEndian32(v) >> 16; }

inline void   EndianSwap(word& v)     { v = SwapEndian16(v); }
inline void   EndianSwap(i16& v)    { v = SwapEndian16(v); }
inline void   EndianSwap(dword& v)    { v = SwapEndian32(v); }
inline void   EndianSwap(i32& v)      { v = SwapEndian32(v); }

#else

inline void   EndianSwap(word& v)     { u8 *x = (u8 *)(&v); Swap(x[0], x[1]); }
inline void   EndianSwap(i16& v)    { EndianSwap(*(word *)&v); }
inline void   EndianSwap(dword& v)    { u8 *x = (u8 *)&v; Swap(x[0], x[3]); Swap(x[1], x[2]); }
inline void   EndianSwap(i32& v)      { EndianSwap(*(dword *)&v); }
inline word   SwapEndian16(word v)    { EndianSwap(v); return v; }
inline i16  SwapEndian16(i16 v)   { EndianSwap(v); return v; }
inline dword  SwapEndian32(dword v)   { EndianSwap(v); return v; }
inline i32    SwapEndian32(i32 v)     { EndianSwap(v); return v; }

#endif

#endif

#if defined(CPU_AMD64) && (defined(COMPILER_GCC) || defined(COMPILER_MSC))
#ifdef COMPILER_GCC
inline zu64  SwapEndian64(zu64 v) { __asm__("bswap %0" : "=r" (v) : "0" (v)); return v; }
inline z64   SwapEndian64(z64 v)  { __asm__("bswap %0" : "=r" (v) : "0" (v)); return v; }
#endif
#ifdef COMPILER_MSC
inline zu64  SwapEndian64(zu64 v) { return _byteswap_uint64(v); }
inline z64   SwapEndian64(z64 v)  { return _byteswap_uint64(v); }
#endif

inline void   EndianSwap(z64& v)    { v = SwapEndian64(v); }
inline void   EndianSwap(zu64& v)   { v = SwapEndian64(v); }

#else

#ifdef COMPILER_GCC

inline zu64  SwapEndian64(zu64 v) { return __builtin_bswap64(v); }
inline z64   SwapEndian64(z64 v)  { return __builtin_bswap64(v); }

inline void    EndianSwap(z64& v)   { v = SwapEndian64(v); }
inline void    EndianSwap(zu64& v)  { v = SwapEndian64(v); }

#else
inline void   EndianSwap(z64& v)    { u8 *x = (u8 *)&v; Swap(x[0], x[7]); Swap(x[1], x[6]); Swap(x[2], x[5]); Swap(x[3], x[4]); }
inline void   EndianSwap(zu64& v)   { EndianSwap(*(z64 *)&v); }
inline z64  SwapEndian64(z64 v)   { EndianSwap(v); return v; }
inline zu64 SwapEndian64(zu64 v)  { EndianSwap(v); return v; }
#endif
#endif

inline word   SwapEndian16(i32 w)     { return SwapEndian16((word)w); }
inline word   SwapEndian16(dword w)   { return SwapEndian16((word)w); }

void EndianSwap(word *v, size_t count);
void EndianSwap(i16 *v, size_t count);
void EndianSwap(dword *v, size_t count);
void EndianSwap(i32 *v, size_t count);
void EndianSwap(z64 *v, size_t count);
void EndianSwap(zu64 *v, size_t count);

#ifdef CPU_64

#define HASH64

#define HASH_CONST1 I64(0xf7c21089bee7c0a5)
#define HASH_CONST2 I64(0xc85abc8da7534a4d)
#define HASH_CONST3 I64(0x8642b0fe3e86671b)

typedef qword hash_t;

inline dword FoldHash(qword h)
{
	return (dword)SwapEndian64(HASH_CONST3 * h);
}

#else

#define HASH_CONST1 0xbee7c0a5
#define HASH_CONST2 0xa7534a4d
#define HASH_CONST3 0x8e86671b

typedef dword hash_t;

inline dword FoldHash(dword h)
{
	return SwapEndian32(HASH_CONST3 * h);
}

#endif

force_inline
i32 SignificantBits(dword x)
{ // basically log2(x) + 1 except that for 0 this is 0, number of significant bits of x
#ifdef COMPILER_MSC
	DWORD index;
	return _BitScanReverse(&index, x) ? index + 1 : 0;
#else
	return x ? 32 - __builtin_clz(x) : 0;
#endif
}

force_inline
i32 SignificantBits64(zu64 x)
{ // basically log2(x) + 1 except that for 0 this is 0, number of significant bits of x
#ifdef COMPILER_MSC
#ifdef CPU_64
	DWORD index;
	return _BitScanReverse64(&index, x) ? index + 1 : 0;
#else
	if(x & 0xffffffff00000000)
		return SignificantBits(HIDWORD(x)) + 32;
	else
		return SignificantBits((DWORD)x);
#endif
#else
	return x ? 64 - __builtin_clzll(x) : 0;
#endif
}

inline bool FitsInInt64(double x)
{
	return x >= -9223372036854775808.0 && x < 9223372036854775808.0;
}

#if defined(__SIZEOF_INT128__) && (__GNUC__ > 5 || __clang_major__ >= 5)

#ifdef CPU_X86

inline
u8 addc64(zu64& result, zu64k& value, u8 carry) {
	return _addcarry_u64(carry, result, value, &result);
}

#else

force_inline
u8 addc64(zu64& r, zu64 a, u8 carry)
{
	r += a + carry;
	return carry ? r <= a : r < a;
}

#endif

inline
zu64 mul64(zu64 a, zu64 b, zu64& hi)
{
	unsigned __int128 prod =  (unsigned __int128)a * b;
	hi = prod >> 64;
	return prod;
}

#elif defined(COMPILER_MSC) && defined(CPU_64)

inline
zu64 mul64(zu64 a, zu64 b, zu64& hi)
{
	return _umul128(a, b, &hi);
}

inline
u8 addc64(zu64& result, zu64k& value, u8 carry) {
	return _addcarry_u64(carry, result, value, &result);
}

#else

force_inline
u8 addc64(zu64& r, zu64 a, u8 carry)
{
	r += a + carry;
	return carry ? r <= a : r < a;
}

force_inline
zu64 mul64(zu64 a, zu64 b, zu64& hi)
{
	zu64 lo_lo = (a & 0xFFFFFFFF) * (b & 0xFFFFFFFF);
	zu64 hi_lo = (a >> 32)        * (b & 0xFFFFFFFF);
	zu64 lo_hi = (a & 0xFFFFFFFF) * (b >> 32);
	zu64 hi_hi = (a >> 32)        * (b >> 32);

	zu64 cross = (lo_lo >> 32) + (hi_lo & 0xFFFFFFFF) + lo_hi;
	hi = (hi_lo >> 32) + (cross >> 32)        + hi_hi;

	return (cross << 32) | (lo_lo & 0xFFFFFFFF);
}

#endif