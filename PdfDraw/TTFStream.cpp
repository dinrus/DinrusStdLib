#include "PdfDraw.h"

namespace drx {

TTFReader::TTFStream& TTFReader::TTFStream::operator%(u8& q) {
	Raw(&q, 1);
	return *this;
}

#ifdef CPU_LE
TTFReader::TTFStream& TTFReader::TTFStream::operator%(i32& q) {
	u8 *h = (u8 *)&q;
	*this % h[3] % h[2] % h[1] % h[0];
	return *this;
}

TTFReader::TTFStream& TTFReader::TTFStream::operator%(u32& q) {
	u8 *h = (u8 *)&q;
	*this % h[3] % h[2] % h[1] % h[0];
	return *this;
}

TTFReader::TTFStream& TTFReader::TTFStream::operator%(i16& q) {
	u8 *h = (u8 *)&q;
	*this % h[1] % h[0];
	return *this;
}

TTFReader::TTFStream& TTFReader::TTFStream::operator%(u16& q) {
	u8 *h = (u8 *)&q;
	*this % h[1] % h[0];
	return *this;
}
#else
TTFReader::TTFStream& TTFReader::TTFStream::operator%(i32& q) {
	u8 *h = (u8 *)&q;
	*this % h[0] % h[1] % h[2] % h[3];
	return *this;
}

TTFReader::TTFStream& TTFReader::TTFStream::operator%(u32& q) {
	u8 *h = (u8 *)&q;
	*this % h[0] % h[1] % h[2] % h[3];
	return *this;
}

TTFReader::TTFStream& TTFReader::TTFStream::operator%(i16& q) {
	u8 *h = (u8 *)&q;
	*this % h[0] % h[1];
	return *this;
}

TTFReader::TTFStream& TTFReader::TTFStream::operator%(u16& q) {
	u8 *h = (u8 *)&q;
	*this % h[0] % h[1];
	return *this;
}
#endif

void TTFReader::TTFStreamIn::Seek(i32 offset)
{
	ASSERT(offset >= 0 && offset < lim - beg);
	s = beg + offset;
}

i32 TTFReader::TTFStreamIn::Get8()
{
	if(s + 1 > lim)
		throw Fail();
	i32 q = (u8)*s;
	s++;
	return q;
}

i32 TTFReader::TTFStreamIn::Get16()
{
	if(s + 2 > lim)
		throw Fail();
	i32 q = ((u8)s[0] << 8) | (u8)s[1];
	s += 2;
	return q;

}

i32 TTFReader::TTFStreamIn::Get32()
{
	if(s + 4 > lim)
		throw Fail();
	i32 q = ((u8)s[0] << 24) | ((u8)s[1] << 16) | ((u8)s[2] << 8) | (u8)s[3];
	s += 2;
	return q;
}

Txt TTFReader::TTFStreamIn::Get(i32 n)
{
	if(s + n > lim)
		throw Fail();
	Txt r(s, n);
	s += n;
	return r;
}

void TTFReader::TTFStreamIn::Raw(uk ptr, i32 count) {
	if(s + count > lim)
		throw Fail();
	memcpy(ptr, s, count);
	s += count;
}

}
