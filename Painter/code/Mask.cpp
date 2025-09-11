#include <drx/Painter/Painter.h>

namespace drx {

void BufferPainter::BeginMaskOp()
{
	attr.mask = true;
	Size sz = ip->GetSize();
	mask.Add() = *ip;
	ip->Create(sz);
	Clear(RGBAZero());
	Begin();
}

static inline u8 *sSpan(u8 *t, i32 c, i32& len)
{
	while(len > 128) {
		i32 n = min(len, 128);
		*t++ = 0;
		*t++ = c + n - 1;
		len -= n;
	}
	if(len) {
		*t++ = 0;
		*t++ = c + len - 1;
		len = 0;
	}
	return t;
}

void BufferPainter::FinishMask()
{
	Buffer<u8> wb(mode == MODE_SUBPIXEL ? 6 * ip->GetWidth() : 2 * ip->GetWidth());
	bool creating = false;
	if(!attr.hasclip) {
		clip.Add().Alloc(ip->GetHeight());
		attr.hasclip = true;
		attr.cliplevel = clip.GetCount();
		creating = true;
	}
	Buffer<ClippingLine>& cl = clip.Top();
	for(i32 y = 0; y < ip->GetHeight(); y++)
		if(creating || !cl[y].IsEmpty()) {
			i32  c0 = 0;
			i32  c256 = 0;
			const RGBA *s = (*ip)[y];
			const RGBA *e = (*ip)[y] + ip->GetWidth();
			u8  *t = wb;
			while(s < e) {
				i32 val = s->a * (56 * s->r + 183 * s->g + 20 * s->b) >> 16;
				if(val == 0) {
					if(c256) t = sSpan(t, 128, c256);
					c0++;
					if(mode == MODE_SUBPIXEL)
						c0 += 2;
				}
				else
				if(val == 256) {
					if(c0) t = sSpan(t, 0, c0);
					c256++;
					if(mode == MODE_SUBPIXEL)
						c256 += 2;
				}
				else {
					if(c256) t = sSpan(t, 128, c256);
					if(c0) t = sSpan(t, 0, c0);
					*t++ = val;
					if(mode == MODE_SUBPIXEL) {
						*t++ = val;
						*t++ = val;
					}
				}
				s++;
			}
			if(c256) t = sSpan(t, 128, c256);
			if(c0) t = sSpan(t, 0, c0);
			cl[y].Clear();
			cl[y].Set(~wb, i32(t - ~wb));
		}
	*ip = mask.Top();
	mask.Drop();
	attr.mask = false;
}

}
