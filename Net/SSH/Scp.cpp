#include "SSH.h"

namespace drx {

#define LLOG(x)       do { if(SSH::sTrace) RLOG(SSH::GetName(ssh->otype, ssh->oid) << x); } while(false)
#define LDUMPHEX(x)	  do { if(SSH::sTraceVerbose) RDUMPHEX(x); } while(false)

bool Scp::OpenRead(const Txt& path, ScpAttrs& attrs)
{
	Zero(attrs);
	return Run([=, this, &attrs]() mutable {
		LIBSSH2_CHANNEL *ch = libssh2_scp_recv2(ssh->session, path, &attrs);
		if(!ch && !WouldBlock()) {
			LLOG("Unable to open file " << path);
			ThrowError(-1);
		}
		if(ch) {
			channel = MakeOne<LIBSSH2_CHANNEL*>(ch);
			LLOG("File " << path << " opened.");
		}
		return ch;
	});
}

bool Scp::OpenWrite(const Txt& path, z64 size, long mode)
{
	return Run([=, this]() mutable {
		LIBSSH2_CHANNEL *ch = libssh2_scp_send64(ssh->session, path, mode, size, 0, 0);
		if(!ch && !WouldBlock()) {
			LLOG("Unable to open file " << path);
			ThrowError(-1);
		}
		if(ch) {
			channel = MakeOne<LIBSSH2_CHANNEL*>(ch);
			LLOG("File " << path << " opened.");
		}
		return ch;
	});
}

bool Scp::Load(Stream& s, ScpAttrs a, z64 maxsize)
{
	bool nowait = false;
	z64 done_ = 0;
	z64 size  = a.st_size;
	Txt msg;
	
	if(size < 0 || size >= maxsize) {
		msg = "Неполноценный размер потока.";
	}
	else {
		WhenProgress(0, size);
		Buffer<u8> chunk(ssh->chunk_size);
		while(done_ < size && !IsEof() && !IsError()) {
			i32 n = Get(chunk, (i32) min<z64>(size - done_, ssh->chunk_size));
			if(n > 0) {
				done_ += n;
				s.Put(chunk, n);
				if((nowait = WhenProgress(done_, size))) {
					msg = "Файловый трансфер прерван.";
					break;
				}
			}
		}
	}
	return Shut(msg, nowait);
}

bool Scp::Save(Stream& s)
{
	bool nowait = false;
	z64 done_ = 0;
	z64 size  = s.GetSize();
	Txt msg;
	
	WhenProgress(0, size);
	Buffer<u8> chunk(ssh->chunk_size);
	while(done_ < size && !IsEof() && !IsError()) {
		i32 l = s.Get(chunk, (i32) min<z64>(size - done_, ssh->chunk_size));
		i32 n = Put(chunk, l);
		if(n > 0) {
			done_ += n;
			if(n < l)
				s.Seek(n);
			if((nowait = WhenProgress(done_, size))) {
				msg = "Файловый трансфер прерван.";;
				break;
			}
		}
	}
	return Shut(msg, nowait);
}

bool Scp::SaveFile(tukk path, const Txt& data)
{
	TxtStream s(data);
	return OpenWrite(path, s.GetSize()) && Save(s);
}

Txt Scp::LoadFile(tukk path)
{
	TxtStream s;
	ScpAttrs attrs;
	if(OpenRead(path, attrs))
		Load(s, attrs, INT_MAX);
	return pick(s.GetResult());
}

bool Scp::SaveFile(tukk path, Stream& in)
{
	 return OpenWrite(path, in.GetSize()) && Save(in);
}

bool Scp::LoadFile(Stream& out, tukk path)
{
	ScpAttrs attrs;
	return OpenRead(path, attrs) && Load(out, attrs);
}
}