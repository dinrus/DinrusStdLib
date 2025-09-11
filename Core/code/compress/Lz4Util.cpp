//#include "../Compress.h"

namespace drx {
	
void sCompressStreamCopy_(Stream& out, Stream& in, Gate<i64, i64> progress, Stream& orig_in, i64 insz);

static i64 sLZ4Compress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress, bool co)
{
	LZ4CompressStream outs(out);
	if(co)
		outs.Co();
	sCompressStreamCopy_(outs, in, progress, in, size);
	outs.Close();
	if(!out.IsError() && !outs.IsError())
		return out.GetSize();
	return -1;
}

static i64 sLZ4Decompress(Stream& out, Stream& in, i64 size, Gate<i64, i64> progress, bool co)
{
	LZ4DecompressStream ins(in);
	if(co)
		ins.Co();
	sCompressStreamCopy_(out, ins, progress, in, size);
	ins.Close();
	if(!out.IsError() && !ins.IsError())
		return out.GetSize();
	return -1;
}

i64 LZ4Compress(Stream& out, Stream& in, Gate<i64, i64> progress)
{
	return sLZ4Compress(out, in, in.GetLeft(), progress, false);
}

i64 LZ4Decompress(Stream& out, Stream& in, Gate<i64, i64> progress)
{
	return sLZ4Decompress(out, in, in.GetLeft(), progress, false);
}

Txt LZ4Compress(Stream& in, Gate<i64, i64> progress)
{
	TxtStream out;
	return LZ4Compress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt LZ4Decompress(Stream& in, Gate<i64, i64> progress)
{
	TxtStream out;
	return LZ4Decompress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt LZ4Compress(ukk data, i64 len, Gate<i64, i64> progress)
{
	MemReadStream in(data, len);
	return LZ4Compress(in, progress);
}

Txt LZ4Compress(const Txt& s, Gate<i64, i64> progress)
{
	return LZ4Compress(~s, s.GetLength(), progress);
}

Txt LZ4Decompress(ukk data, i64 len, Gate<i64, i64> progress)
{
	MemReadStream in(data, len);
	return LZ4Decompress(in, progress);
}

Txt LZ4Decompress(const Txt& s, Gate<i64, i64> progress)
{
	return LZ4Decompress(~s, s.GetLength(), progress);
}

i64 CoLZ4Compress(Stream& out, Stream& in, Gate<i64, i64> progress)
{
	return sLZ4Compress(out, in, in.GetLeft(), progress, true);
}

i64 CoLZ4Decompress(Stream& out, Stream& in, Gate<i64, i64> progress)
{
	return sLZ4Decompress(out, in, in.GetLeft(), progress, true);
}

Txt CoLZ4Compress(Stream& in, Gate<i64, i64> progress)
{
	TxtStream out;
	return CoLZ4Compress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt CoLZ4Decompress(Stream& in, Gate<i64, i64> progress)
{
	TxtStream out;
	return CoLZ4Decompress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt CoLZ4Compress(ukk data, i64 len, Gate<i64, i64> progress)
{
	TxtStream out;
	MemReadStream in(data, len);
	return CoLZ4Compress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt CoLZ4Compress(const Txt& s, Gate<i64, i64> progress)
{
	return CoLZ4Compress(~s, s.GetLength(), progress);
}

Txt CoLZ4Decompress(ukk data, i64 len, Gate<i64, i64> progress)
{
	TxtStream out;
	MemReadStream in(data, len);
	return CoLZ4Decompress(out, in, progress) < 0 ? Txt::GetVoid() : out.GetResult();
}

Txt CoLZ4Decompress(const Txt& s, Gate<i64, i64> progress)
{
	return CoLZ4Decompress(~s, s.GetLength(), progress);
}

};