#include "../Core.h"

namespace drx {

#ifdef CPU_SIMD

void memset8__(uk p, i16x8 data, size_t len)
{
	ASSERT(len >= 16);
	u8 *t = (u8 *)p;
	auto Set4 = [&](size_t at) { data.Store(t + at); };
	Set4(len - 16); // fill tail
	Set4(0); // align up on the next 16 bytes boundary
	if(len <= 32)
		return;
	u8k *e = t + len;
	t = (u8 *)(((uintptr_t)t | 15) + 1);
	len = e - t;
	e -= 128;
#ifdef CPU_SSE2
	if(len >= 1024*1024) { // for really huge data, bypass the cache
		auto Set4S = [&](i32 at) { data.Stream(t + at); };
		while(len >= 64) {
			Set4S(0*16); Set4S(1*16); Set4S(2*16); Set4S(3*16);
			t += 64;
			len -= 64;
		}
		_mm_sfence();
		e = t - 1;
	}
#endif
	while(t <= e) {
		Set4(0*16); Set4(1*16); Set4(2*16); Set4(3*16);
		Set4(4*16); Set4(5*16); Set4(6*16); Set4(7*16);
		t += 128;
	}
	if(len & 64) {
		Set4(0*16); Set4(1*16); Set4(2*16); Set4(3*16);
		t += 64;
	}
	if(len & 32) {
		Set4(0*16); Set4(1*16);
		t += 32;
	}
	if(len & 16)
		Set4(0);
}

never_inline
void memcpy8__(uk p, ukk q, size_t len)
{
	ASSERT(len >= 16);

	u8 *t = (u8 *)p;
	u8k *s = (u8k *)q;

	if(len > 4*1024*1024) { // for really huge data, call memcpy to bypass the cache
		memcpy(t, s, len);
		return;
	}

	auto Copy128 = [&](size_t at) { i16x8(s + at).Store(t + at); };
	Copy128(len - 16); // copy tail
	Copy128(0); // align target data up on the next 16 bytes boundary
	if(len <= 32)
		return;
	u8k *e = t + len;
	u8 *t1 = (u8 *)(((uintptr_t)t | 15) + 1);
	s += t1 - t;
	t = t1;
	len = e - t;
	e -= 64;
	while(t <= e) {
		Copy128(0*16); Copy128(1*16); Copy128(2*16), Copy128(3*16);
		t += 64;
		s += 64;
	}
	if(len & 32) {
		Copy128(0*16); Copy128(1*16);
		t += 32;
		s += 32;
	}
	if(len & 16)
		Copy128(0*16);
}

bool memeq8(ukk p, ukk q, size_t len) { return inline_memeq8_aligned(p, q, len); }
bool memeq16(ukk p, ukk q, size_t len) { return inline_memeq16_aligned(p, q, len); }
bool memeq32(ukk p, ukk q, size_t len) { return inline_memeq32_aligned(p, q, len); }
bool memeq64(ukk p, ukk q, size_t len) { return inline_memeq64_aligned(p, q, len); }
bool memeq128(ukk p, ukk q, size_t len) { return inline_memeq128_aligned(p, q, len); }

#endif

#ifdef CPU_UNALIGNED

#ifdef HASH64

hash_t memhash(ukk ptr, size_t len)
{
	u8k *s = (u8 *)ptr;
	zu64 val = HASH_CONST1;
	if(len >= 8) {
		if(len >= 32) {
			zu64 val1, val2, val3, val4;
			val1 = val2 = val3 = val4 = HASH_CONST1;
			while(len >= 32) {
				val1 = HASH_CONST2 * val1 + *(qword *)(s);
				val2 = HASH_CONST2 * val2 + *(qword *)(s + 8);
				val3 = HASH_CONST2 * val3 + *(qword *)(s + 16);
				val4 = HASH_CONST2 * val4 + *(qword *)(s + 24);
				s += 32;
				len -= 32;
			}
			val = HASH_CONST2 * val + val1;
			val = HASH_CONST2 * val + val2;
			val = HASH_CONST2 * val + val3;
			val = HASH_CONST2 * val + val4;
		}
		u8k *e = s + len - 8;
		while(s < e) {
			val = HASH_CONST2 * val + *(qword *)(s);
			s += 8;
		}
		return HASH_CONST2 * val + *(qword *)(e);
	}
	if(len > 4) {
		val = HASH_CONST2 * val + *(dword *)(s);
		val = HASH_CONST2 * val + *(dword *)(s + len - 4);
		return val;
	}
	if(len >= 2) {
		val = HASH_CONST2 * val + *(word *)(s);
		val = HASH_CONST2 * val + *(word *)(s + len - 2);
		return val;
	}
	return len ? HASH_CONST2 * val + *s : val;
}

#else

never_inline
hash_t memhash(ukk ptr, size_t len)
{
	u8k *s = (u8 *)ptr;
	dword val = HASH_CONST1;
	if(len >= 4) {
		if(len >= 48) {
			dword val1, val2, val3, val4;
			val1 = val2 = val3 = val4 = HASH_CONST1;
			while(len >= 16) {
				val1 = HASH_CONST2 * val1 + *(dword *)(s);
				val2 = HASH_CONST2 * val2 + *(dword *)(s + 4);
				val3 = HASH_CONST2 * val3 + *(dword *)(s + 8);
				val4 = HASH_CONST2 * val4 + *(dword *)(s + 12);
				s += 16;
				len -= 16;
			}
			val = HASH_CONST2 * val + val1;
			val = HASH_CONST2 * val + val2;
			val = HASH_CONST2 * val + val3;
			val = HASH_CONST2 * val + val4;
		}
		u8k *e = s + len - 4;
		while(s < e) {
			val = HASH_CONST2 * val + *(dword *)(s);
			s += 4;
		}
		return HASH_CONST2 * val + *(dword *)(e);
	}
	if(len >= 2) {
		val = HASH_CONST2 * val + *(word *)(s);
		val = HASH_CONST2 * val + *(word *)(s + len - 2);
		return val;
	}
	return len ? HASH_CONST2 * val + *s : val;
}

#endif

#else

hash_t memhash(ukk ptr, size_t count)
{
	unsigned hash = 1234567890U;

	u8k *s = (u8 *)ptr;
	u8k *e = s + count;
	while(s < e)
		hash = ((hash << 5) - hash) ^ *s++;

	return hash;
}

#endif

};