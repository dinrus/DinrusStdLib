#define LDUMP(x)  // DDUMP(x)
#define LLOG(x)   // DLOG(x)

namespace drx {
namespace bz2 {
	static uk bzalloc_new(uk opaque, i32 items, i32 size)
	{
		return new u8[items * size];
	}
	
	static void bzfree_new(uk opaque, uk addr)
	{
		delete[] (u8*)addr;
	}
	
	
	void Lib::Begin()
	{
		Free();
		SetError(false);
		out.Clear();
	}
	
	void Lib::Compress()
	{
		Begin();
		if(BZ2_bzCompressInit(&z, compression_level, 0, 30) != BZ_OK)
			Panic("Неудачное BZ2_bzCompressInit");
		mode = DEFLATE;
	}
	
	void Lib::Decompress(bool all)
	{
		Begin();
		rdall = all;
		eos = false;
		if(BZ2_bzDecompressInit(&z, 0, 0) != BZ_OK)
			Panic("Неудачное BZ2_bzDecompressInit");
		mode = INFLATE;
	}
	
	void Lib::Pump(bool finish)
	{
		if(error)
			return;
		ASSERT(mode);
		if(!output)
			output.Alloc(chunk);
		if (mode == INFLATE) {
			z.avail_out = chunk;
			z.next_out = output;
			while (z.avail_in && !IsEOS()) {
				i32k code = BZ2_bzDecompress(&z);
				i32k count = chunk - z.avail_out;
				if(count) {
					WhenOut((tukk)~output, count);
					if (error) {
						Free();
						break;
					}
					total += count;
					z.avail_out = chunk;
					z.next_out = output;
				}
				if(code == BZ_STREAM_END) {
					if (rdall) {
						BZ2_bzDecompressEnd(&z);
						if(BZ2_bzDecompressInit(&z, 0, 0) != BZ_OK)
							Panic("Неудачное BZ2_bzDecompressInit");
					}
					else
						eos = true;
				}
				if(code != BZ_OK && code != BZ_STREAM_END) {
					LLOG("BZLIB ERROR " << code);
					Free();
					SetError(true);
					break;
				}
			}
		} else {
			i32k flush = finish ? BZ_FINISH : BZ_RUN;
			z.avail_out = chunk;
			z.next_out = output;
			while (z.avail_in) {
				i32k code = BZ2_bzCompress(&z, flush);
				i32k count = chunk - z.avail_out;
				if(count) {
					WhenOut((tukk)~output, count);
					if (error) {
						Free();
						break;
					}
					z.avail_out = chunk;
					z.next_out = output;
				}
				if(code != BZ_RUN_OK && code != BZ_FINISH_OK) {
					LLOG("BZLIB ERROR " << code);
					Free();
					SetError(true);
					break;
				}
			}
		}
	}
	
	void Lib::Put(ukk ptr, i32 size)
	{
		if(error)
			return;
//			return false;
		LLOG("BZLIB Put " << size);
		tukk p = reinterpret_cast<tukk>(ptr);
		while(size && !IsEOS()) {
			i32 psz = (i32) min(size, INT_MAX / 4);
			if(mode == DEFLATE)
				total += size;
			z.next_in = const_cast<tuk >(p);
			z.avail_in = size;
			Pump(false);
			size -= psz;
			p += psz;
		}
//		return !IsEOS();
	}
	
	void Lib::PutOut(ukk ptr, i32 size)
	{
		LLOG("BZLIB PutOut " << out.GetCount());
		out.Cat((tukk)ptr, (i32)size);
	}
	
	void Lib::End()
	{
		LLOG("BZLIB End");
		if(mode != INFLATE)
			Pump(true);
		Free();
	}
	
	void Lib::Free()
	{
		if(mode == INFLATE)
			BZ2_bzDecompressEnd(&z);
		else if(mode == DEFLATE)
			BZ2_bzCompressEnd(&z);
		mode = NONE;
		total = 0;
	}
	
	void Lib::Init()
	{
		mode = NONE;
		SetError(false);
		rdall = true;
		eos = false;
		Zero(z);
	}
	
	void Lib::Clear()
	{
		Free();
		Init();
	}
	
	Lib& Lib::ChunkSize(i32 n)
	{
		ASSERT(n < INT_MAX / 4);
		output.Clear();
		chunk = n;
		return *this;
	}
	
	Lib::Lib()
	{
		Init();
		z.bzalloc = bzalloc_new;
		z.bzfree = bzfree_new;
		z.opaque = 0;
		chunk = 4096;
		WhenOut = callback(this, &Lib::PutOut);
		compression_level = 9;
	}
	
	Lib::~Lib()
	{
		Free();
	}
}

void BZ2Decompress(Stream& out, Stream& in, Gate<i32, i32> progress)
{
	enum { BUF_SIZE = 65536 };
	Buffer<char> input(BUF_SIZE), output(BUF_SIZE);
	i32 avail = in.Get(input, BUF_SIZE);
	if(avail == 0)
		return;
	bz_stream z;
	Zero(z);
	z.bzalloc = bz2::bzalloc_new;
	z.bzfree = bz2::bzfree_new;
	z.opaque = 0;
	if(BZ2_bzDecompressInit(&z, 0, 0) != BZ_OK)
	{
		out.SetError();
		return;
	}
	z.next_in = input;
	z.avail_in = avail;
	z.next_out = output;
	z.avail_out = BUF_SIZE;
	i32 code;
	bool running = true;
	i64 total = in.GetLeft();
	i32 done = 0;
	do
	{
		if(z.avail_in == 0 && running)
		{
			if((z.avail_in = in.Get(z.next_in = input, BUF_SIZE)) == 0)
				running = false;
			done += z.avail_in;
			if(progress(done, (i32)total) || in.IsError())
			{
				BZ2_bzDecompressEnd(&z);
				out.SetError();
				return;
			}
		}
		code = BZ2_bzDecompress(&z);
		if(z.avail_out == 0)
		{
			out.Put(z.next_out = output, z.avail_out = BUF_SIZE);
			if(out.IsError())
			{
				BZ2_bzDecompressEnd(&z);
				return;
			}
		}
	}
	while(code == BZ_OK);
	if(z.avail_out < BUF_SIZE)
		out.Put(output, BUF_SIZE - z.avail_out);
	BZ2_bzDecompressEnd(&z);
}

void BZ2Compress(Stream& out, Stream& in, Gate<i32, i32> progress)
{
	enum { BUF_SIZE = 65536 };
	Buffer<char> input(BUF_SIZE), output(BUF_SIZE);
	bz_stream z;
	z.bzalloc = bz2::bzalloc_new;
	z.bzfree = bz2::bzfree_new;
	z.opaque = 0;
	if(BZ2_bzCompressInit(&z, 9, 0, 30) != BZ_OK)
	{
		out.SetError();
		return;
	}
	z.avail_in = 0;
	z.avail_out = BUF_SIZE;
	z.next_out = output;
	i32 code;
	i32 flush = BZ_RUN;
	i64 total = in.GetLeft();
	i32 done = 0;
	do
	{
		if(z.avail_in == 0 && flush == BZ_RUN)
		{
			z.next_in = input;
			if((z.avail_in = in.Get(z.next_in = input, BUF_SIZE)) == 0)
				flush = BZ_FINISH;
			done += z.avail_in;
			if(progress(done, (i32)total) || in.IsError())
			{
				BZ2_bzCompressEnd(&z);
				out.SetError();
				return;
			}
		}
		code = BZ2_bzCompress(&z, flush);
		if(z.avail_out == 0)
		{
			out.Put(z.next_out = output, z.avail_out = BUF_SIZE);
			if(out.IsError())
			{
				BZ2_bzCompressEnd(&z);
				return;
			}
		}
	}
	while(code == BZ_RUN_OK || code == BZ_FINISH_OK);
	if(z.avail_out < BUF_SIZE)
		out.Put(output, BUF_SIZE - z.avail_out);
	BZ2_bzCompressEnd(&z);
	if(code != BZ_STREAM_END)
		out.SetError();
}

Txt BZ2Compress(Stream& in, Gate<i32, i32> progress)
{
	TxtStream out;
	BZ2Compress(out, in, progress);
	return out;
}

Txt BZ2Decompress(Stream& in, Gate<i32, i32> progress)
{
	TxtStream out;
	BZ2Decompress(out, in, progress);
	return out;
}

Txt BZ2Compress(ukk data, i64 len, Gate<i32, i32> progress)
{
	MemReadStream in(data, len);
	return BZ2Compress(in, progress);
}

Txt BZ2Decompress(ukk data, i64 len, Gate<i32, i32> progress)
{
	MemReadStream in(data, len);
	return BZ2Decompress(in, progress);
}

Txt BZ2Compress(const Txt& data, Gate<i32, i32> progress)
{
	return BZ2Compress(~data, data.GetLength(), progress);
}

Txt BZ2Decompress(const Txt& data, Gate<i32, i32> progress)
{
	return BZ2Decompress(~data, data.GetLength(), progress);
}

}
