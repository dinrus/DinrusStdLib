//#include "../Compress.h"

#define  _7ZIP_ST

//#include <drx/Core/lib/lzma/LzmaDec.h>
//#include <drx/Core/lib/lzma/LzmaEnc.h>

namespace drx {

static voidpf lzma_alloc_new(ISzAllocPtr, size_t size) {
    voidpf t = new u8[size];
    return t;
}

static void lzma_free_new(ISzAllocPtr, uk address) {
    if(!address)
        return;
    delete[] (u8*)address;
}

static ISzAlloc g_Alloc = { &lzma_alloc_new, &lzma_free_new };

struct IUppInStream {
    ISeqInStream s;
    Stream& in;

    static SRes Read(const ISeqInStream *pp, uk buf, size_t *size) {
        IUppInStream *p = (IUppInStream*)pp;
        i64 h = (size_t)p->in.Get64(buf, *size);
        if(p->in.IsError() || h < 0) {
            *size = 0;
            return SZ_ERROR_READ;
        }
        *size = (size_t)h;
        return SZ_OK;
    }

    IUppInStream(Stream& in) : in(in) {
        s.Read = Read;
    }
};

struct IUppOutStream {
    ISeqOutStream s;
    Stream& out;
    i64   len;

    static size_t Write(const ISeqOutStream *pp, ukk buf, size_t size) {
        IUppOutStream *p = (IUppOutStream*)pp;
        p->out.Put64(buf, size);
        p->len += size;
        return p->out.IsError() ? -1 : size;
    }

    IUppOutStream(Stream& out) : out(out) {
        s.Write = Write;
        len = 0;
    }
};

#define IN_BUF_SIZE (1 << 16)
#define OUT_BUF_SIZE (1 << 16)

static SRes Decode2(CLzmaDec *state, ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 unpackSize, Gate2<i64, i64> progress)
{
    i32 thereIsSize = (unpackSize != (UInt64)(Int64)-1);
    Byte inBuf[IN_BUF_SIZE];
    Byte outBuf[OUT_BUF_SIZE];
    size_t inPos = 0, inSize = 0, outPos = 0;
    LzmaDec_Init(state);
    u64 todo = unpackSize;
    for(;;) {
        if(inPos == inSize) {
            inSize = IN_BUF_SIZE;
            RINOK(inStream->Read(inStream, inBuf, &inSize));
            inPos = 0;
        }
        SRes res;
        SizeT inProcessed = inSize - inPos;
        SizeT outProcessed = OUT_BUF_SIZE - outPos;
        ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
        ELzmaStatus status;
        if(thereIsSize && outProcessed > unpackSize)
        {
            outProcessed = (SizeT)unpackSize;
            finishMode = LZMA_FINISH_END;
        }

        res = LzmaDec_DecodeToBuf(state, outBuf + outPos, &outProcessed,
                                  inBuf + inPos, &inProcessed, finishMode, &status);
        inPos += inProcessed;
        outPos += outProcessed;
        unpackSize -= outProcessed;

        if(progress(inPos, todo))
            return SZ_ERROR_WRITE;

        if(outStream)
            if(outStream->Write(outStream, outBuf, outPos) != outPos)
                return SZ_ERROR_WRITE;

        outPos = 0;
        if(res != SZ_OK || (thereIsSize && unpackSize == 0))
            return res;

        if(inProcessed == 0 && outProcessed == 0) {
            if(thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK)
                return SZ_ERROR_DATA;
            return res;
        }
    }
}

static SRes Decode(ISeqOutStream *outStream, ISeqInStream *inStream, Gate2<i64, i64> progress)
{
    UInt64 unpackSize;
    i32 i;
    SRes res = 0;

    CLzmaDec state;

    /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
    u8 header[LZMA_PROPS_SIZE + 8];

    /* Read and parse header */
    IUppInStream *is = (IUppInStream*)inStream;
    if(is->in.Get(header, sizeof(header)) != sizeof(header))
        return SZ_ERROR_READ;

    unpackSize = 0;
    for(i = 0; i < 8; i++)
        unpackSize += (UInt64)header[LZMA_PROPS_SIZE + i] << (i * 8);

    LzmaDec_Construct(&state);
    RINOK(LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc));
    res = Decode2(&state, outStream, inStream, unpackSize, progress);
    LzmaDec_Free(&state, &g_Alloc);
    return res;
}

struct LUppProgress {
    ICompressProgress   s;
    Gate2<i64, i64> progress;
    i64               total;

    static SRes Progress(const ICompressProgress *pp, UInt64 inSize, UInt64)
    {
        LUppProgress *p = (LUppProgress*)pp;
        return p->progress(inSize, p->total);
    }

    LUppProgress() { s.Progress = Progress; }
};

static SRes Encode(ISeqOutStream *outStream, ISeqInStream *inStream, UInt64 fileSize, ICompressProgress *pg, i32 lvl)
{
    CLzmaEncHandle enc;
    SRes res;
    CLzmaEncProps props;

    enc = LzmaEnc_Create(&g_Alloc);
    if (enc == 0)
        return SZ_ERROR_MEM;

    LzmaEncProps_Init(&props);
    props.level = lvl;
    res = LzmaEnc_SetProps(enc, &props);

    if(res == SZ_OK) {
        Byte header[LZMA_PROPS_SIZE + 8];
        size_t headerSize = LZMA_PROPS_SIZE;
        i32 i;
        res = LzmaEnc_WriteProperties(enc, header, &headerSize);
        for(i = 0; i < 8; i++)
            header[headerSize++] = (Byte)(fileSize >> (8 * i));
        if(outStream->Write(outStream, header, headerSize) != headerSize)
            res = SZ_ERROR_WRITE;
        else {
            if(res == SZ_OK)
                res = LzmaEnc_Encode(enc, outStream, inStream, pg, &g_Alloc, &g_Alloc);
        }
    }
    LzmaEnc_Destroy(enc, &g_Alloc, &g_Alloc);
    return res;
}

i64 LZMACompress(Stream& out, Stream& in, i64 size, Gate2<i64, i64> progress, i32 lvl)
{
    IUppInStream   lin(in);
    IUppOutStream  lout(out);
    LUppProgress   pg;
    pg.progress = progress;
    pg.total = size;
    return Encode(&lout.s, &lin.s, size, &pg.s, lvl) == SZ_OK
           && !in.IsError() && !out.IsError() ? lout.len : -1;
}

i64 LZMACompress(Stream& out, Stream& in, Gate2<i64, i64> progress, i32 lvl)
{
    return LZMACompress(out, in, in.GetLeft(), progress, lvl);
}

Txt LZMACompress(ukk data, i64 len, Gate2<i64, i64> progress, i32 lvl)
{
    TxtStream  out;
    MemReadStream in(data, len);
    if(LZMACompress(out, in, len, progress, lvl) >= 0)
        return out;
    return Txt::GetVoid();
}

Txt LZMACompress(const Txt& s, Gate2<i64, i64> progress, i32 lvl)
{
    return LZMACompress(~s, s.GetLength(), progress, lvl);
}

i64 LZMADecompress(Stream& out, Stream& in, Gate2<i64, i64> progress)
{
    IUppInStream   lin(in);
    IUppOutStream  lout(out);
    return Decode(&lout.s, &lin.s, progress) == SZ_OK && !in.IsError() && !out.IsError() ? lout.len : -1;
}

Txt LZMADecompress(ukk data, i64 len, Gate2<i64, i64> progress)
{
    TxtStream  out;
    MemReadStream in(data, len);
    if(LZMADecompress(out, in, progress) >= 0)
        return out;
    return Txt::GetVoid();
}

Txt LZMADecompress(const Txt& s, Gate2<i64, i64> progress)
{
    return LZMADecompress(~s, s.GetLength(), progress);
}

bool LZMACompressFile(tukk dstfile, tukk srcfile, Gate2<i64, i64> progress, i32 lvl)
{
    FileIn in(srcfile);
    if(!in)
        return false;
    FileOut out(dstfile);
    if(!out)
        return false;
    if(LZMACompress(out, in, (i32)in.GetLeft(), progress, lvl) < 0)
        return false;
    out.Close();
    return !out.IsError();
}

bool LZMACompressFile(tukk srcfile, Gate2<i64, i64> progress, i32 lvl)
{
    Txt dstfile = Txt(srcfile) + ".lzma";
    return LZMACompressFile(dstfile, srcfile, progress, lvl);
}

bool LZMADecompressFile(tukk dstfile, tukk srcfile, Gate2<i64, i64> progress)
{
    FileIn in(srcfile);
    if(!in)
        return false;
    FileOut out(dstfile);
    if(!out)
        return false;
    if(LZMADecompress(out, in, progress) < 0)
        return false;
    out.Close();
    return !out.IsError();
}

bool LZMADecompressFile(tukk srcfile, Gate2<i64, i64> progress)
{
    Txt dstfile = srcfile;
    if(dstfile.EndsWith(".lzma"))
        dstfile.Trim(dstfile.GetLength() - 5);
    else
        return false;
    return LZMADecompressFile(dstfile, srcfile, progress);
}

}
