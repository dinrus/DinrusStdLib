#include "../Core.h"

namespace drx {

PackedData::~PackedData()
{
	if(ptr) MemoryFree(ptr);
}

void PackedData::Clear()
{
	if(ptr) MemoryFree(ptr);
	ptr = nullptr;
}

Vec<Txt> PackedData::Unpack() const
{
	Vec<Txt> r;
	u8k *s = (u8k *)ptr;
	for(;;) {
		i32 len = *s++;
		if(len == 255)
			break;
		if(len == 254) {
			memcpy(&len, s, 4);
			s += 4;
		}
		r << Txt(s, len);
		s += len;
	}
	return r;
}

void PackedData::SetData(i32 ii, ukk data, i32 datalen)
{
	size_t alloc = 32;
	tuk result = (tuk )MemoryAllocSz(alloc);
	tuk t = result;
	auto Reserve = [&](i32 n) {
		size_t needs = t + n - result;
		if(needs > alloc) {
			alloc = 3 * needs / 2;
			tuk r2 = (tuk )MemoryAllocSz(alloc);
			memcpy(r2, result, t - result);
			MemoryFree(result);
			t = t - result + r2;
			result = r2;
		}
	};
	auto Out1 = [&](i32 c) {
		Reserve(1);
		*t++ = c;
	};
	auto Out = [&](ukk s, i32 len) {
		Reserve(len);
		memcpy(t, s, len);
		t += len;
	};
	i32 i = 0;
	u8k *p = (u8k *)ptr;
	u8k *s = p;
	u8k *b = s; // before last control code
	u8k *rb = NULL; // start of replaced area
	u8k *re = NULL; // end of replaced area
	if(s)
		for(;;) {
			b = s;
			i32 len = *s++;
			if(len == 255)
				break;
			if(len == 254) {
				memcpy(&len, s, sizeof(i32));
				s += sizeof(i32);
			}
			if(i == ii) {
				rb = b;
				s += len;
				re = s;
			}
			else
				s += len;
			i++;
		}

	auto Put = [&]() {
		if(datalen < 254)
			Out1(datalen);
		else {
			Out1(254);
			u8 h[sizeof(i32)];
			memcpy(h, &datalen, sizeof(i32));
			Out(h, sizeof(i32));
		}
		Out(data, datalen);
	};
	if(rb) { // we have found an area to replace
		Out(p, i32(rb - p));
		Put();
		Out(re, i32((s - re)));
	}
	else { // we need to add new entries
		if(p) // copy existing entries
			Out(p, i32(b - p));
		while(i < ii) {
			Out1(0);
			i++;
		}
		Put();
		Out1(255);
	}
	if(ptr)
		MemoryFree(ptr);
	ptr = result;
}

size_t PackedData::GetPackedSize() const
{
	if(!ptr)
		return 0;
	u8k *s = (u8k *)ptr;
	for(;;) {
		i32 len = *s++;
		if(len == 255)
			break;
		if(len == 254) {
			memcpy(&len, s, 4);
			s += 4;
		}
		s += len;
	}
	return s - (u8k *)ptr;
}

};