//#include "../Compress.h"

namespace drx {
	
void sCompressStreamCopy_(Stream& out, Stream& in, Gate<i64, i64> progress, Stream& orig_in, i64 insz);

static i64 sZstdCompress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress, bool co)
{
	ZstdCompressStream outs(out);
	if(co)
		outs.Co();
	sCompressStreamCopy_(outs, in, progress, in, size);
	outs.Close();
	if(!out.IsError() && !outs.IsError())
		return out.GetSize();
	return -1;
}

static i64 sZstdDecompress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress, bool co)
{
	ZstdDecompressStream ins(in);
	if(co)
		ins.Co();
	sCompressStreamCopy_(out, ins, progress, in, size);
	ins.Close();
	if(!out.IsError() && !ins.IsError())
		return out.GetSize();
	return -1;
}

i64 ZstdCompress(Stream& out, Stream& in, Gate<i64, i64> progress)
{
	return sZstdCompress(out, in, in.GetLeft(), progress, false);
}

i64 ZstdDecompress(Stream& out, Stream& in, Gate<i64, i64> progress)
{
	return sZstdDecompress(out, in, in.GetLeft(), progress, false);
}

Txt ZstdCompress(ukk data, i64 len, Gate<i64, i64> progress)
{
	TxtStream out;
	MemReadStream in(data, len);
	return ZstdCompress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt ZstdCompress(const Txt& s, Gate<i64, i64> progress)
{
	return ZstdCompress(~s, s.GetLength(), progress);
}

Txt ZstdDecompress(ukk data, i64 len, Gate<i64, i64> progress)
{
	TxtStream out;
	MemReadStream in(data, len);
	return ZstdDecompress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt ZstdDecompress(const Txt& s, Gate<i64, i64> progress)
{
	return ZstdDecompress(~s, s.GetLength(), progress);
}

i64 CoZstdCompress(Stream& out, Stream& in, Gate<i64, i64> progress)
{
	return sZstdCompress(out, in, in.GetLeft(), progress, true);
}

i64 CoZstdDecompress(Stream& out, Stream& in, Gate<i64, i64> progress)
{
	return sZstdDecompress(out, in, in.GetLeft(), progress, true);
}

Txt CoZstdCompress(ukk data, i64 len, Gate<i64, i64> progress)
{
	TxtStream out;
	MemReadStream in(data, len);
	return CoZstdCompress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt CoZstdCompress(const Txt& s, Gate<i64, i64> progress)
{
	return CoZstdCompress(~s, s.GetLength(), progress);
}

Txt CoZstdDecompress(ukk data, i64 len, Gate<i64, i64> progress)
{
	TxtStream out;
	MemReadStream in(data, len);
	return CoZstdDecompress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt CoZstdDecompress(const Txt& s, Gate<i64, i64> progress)
{
	return CoZstdDecompress(~s, s.GetLength(), progress);
}

};