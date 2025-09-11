#include "../Core.h"

namespace drx {

InFilterStream::InFilterStream()
{
	Init();
}

void InFilterStream::Init()
{
	pos = 0;
	in = NULL;
	eof = false;
	style = STRM_READ|STRM_LOADING;
	SetLoading();
	buffer.Clear();
	ptr = rdlim = Stream::buffer = NULL;
	todo = 0;
	t = NULL;
}

bool InFilterStream::IsOpen() const
{
	return in->IsOpen();
}

i32 InFilterStream::_Term()
{
	while(ptr == rdlim && !eof)
		Fetch();
	return ptr == rdlim ? -1 : *ptr;
}

i32 InFilterStream::_Get()
{
	while(ptr == rdlim && !eof)
		Fetch();
	return ptr == rdlim ? -1 : *ptr++;
}

dword InFilterStream::_Get(uk data, dword size)
{
	t = (u8 *)data;
	dword sz0 = min(dword(rdlim - ptr), size);
	memcpy8(t, ptr, sz0);
	t += sz0;
	ptr += sz0;
	todo = size - sz0;
	while(todo && !eof)
		Fetch();
	return size - todo;
}

void InFilterStream::SetRd()
{
	Stream::buffer = ptr = buffer.begin();
	rdlim = buffer.end();
}

void InFilterStream::Out(ukk p, i32 size)
{
	u8k *s = (u8 *)p;
	if(todo) {
		dword sz = min(todo, (dword)size);
		memcpy8(t, s, sz);
		t += sz;
		s += sz;
		todo -= sz;
		size -= sz;
		pos += sz;
	}
	if(size) {
		i32 l = buffer.GetCount();
		buffer.SetCountR(l + size);
		memcpy8(buffer.begin() + l, s, size);
		SetRd();
	}
	WhenOut();
}

void InFilterStream::Fetch()
{
	ASSERT(ptr == rdlim);
	pos += buffer.GetCount();
	buffer.SetCount(0); // SetCount instead of Clear to maintain capacity
	if(!eof) {
		if(More)
			eof = !More();
		else {
			if(!inbuffer)
				inbuffer.Alloc(buffersize);
			i32 n = in->Get(~inbuffer, buffersize);
			if(n == 0) {
				End();
				eof = true;
			}
			else {
				Filter(~inbuffer, n);
				eof = FilterEof();
			}
		}
	}
	SetRd();
}

/////////////////////////////////////////////////////////////////////////////////////////////

OutFilterStream::OutFilterStream()
{
	Init();
}

void OutFilterStream::Init()
{
	buffer.Alloc(4096);
	wrlim = ~buffer + 4096;
	ptr = ~buffer;
	out = NULL;
	count = pos = 0;
	style = STRM_WRITE;
	pos = 0;
	Stream::buffer = ~buffer;
}

OutFilterStream::~OutFilterStream()
{
	Close();
}

void OutFilterStream::Close()
{
	if(buffer) {
		FlushOut();
		End();
		buffer.Clear();
	}
}

void OutFilterStream::FlushOut()
{
	if(ptr != ~buffer) {
		i32 sz = (i32)(ptr - ~buffer);
		pos += sz;
		WhenPos(pos);
		Filter(~buffer, sz);
		ptr = ~buffer;
	}
}

void OutFilterStream::_Put(i32 w)
{
	FlushOut();
	*ptr++ = w;
}

void OutFilterStream::_Put(ukk data, dword size)
{
	u8k *p = (u8k *)data;
	for(;;) {
		i32 n = min(Avail(), size);
		memcpy8(ptr, p, n);
		size -= n;
		p += n;
		ptr += n;
		if(size == 0)
			return;
		FlushOut();
	}
}

bool OutFilterStream::IsOpen() const
{
	return buffer;
}

void OutFilterStream::Out(ukk ptr, i32 size)
{
	count += size;
	out->Put(ptr, size);
}

}
