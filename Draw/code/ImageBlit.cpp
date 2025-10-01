#include <drx/Draw/Draw.h>

namespace drx {

u8k *UnpackRLE(RGBA *t, u8k *s, i32 len)
{
	RGBA *e = t + len;
	while(t < e)
		if(*s & 0x80) {
			if(*s == 0x80)
				break;
			i32 count = min<i32>((i32)(*s & 0x3F), (i32)(e - t));
			RGBA h;
			if(*s++ & 0x40)
				Zero(h);
			else {
				h.b = s[0];
				h.g = s[1];
				h.r = s[2];
				h.a = 255;
				s += 3;
			}
			count = min<i32>(count, (i32)(e - t));
			Fill(t, h, count);
			t += count;
		}
		else {
			if(*s == 0)
				break;
			i32 count = *s++;
			while(count-- && t < e) {
				RGBA h;
				h.b = s[0];
				h.g = s[1];
				h.r = s[2];
				h.a = 255;
				*t++ = h;
				s += 3;
			}
		}
	while(t < e)
		Zero(*t++);
	return s;
}

Txt PackRLE(const RGBA *s, i32 len)
{
	TxtBuf r;
	const RGBA *e = s + len;
	while(s < e) {
		const RGBA *q = s;
		if(s->a == 0) {
			s++;
			while(s < e && s->a == 0 && s - q < 0x3f)
				s++;
			r.Cat((0x80|0x40) + (i32)(s - q));
		}
		else
		if(s + 1 < e && s[0] == s[1]) {
			s++;
			while(s + 1 < e && s[0] == s[1] && s - q < 0x3e)
				s++;
			s++;
			r.Cat(0x80 + (i32)(s - q));
			r.Cat(q->b);
			r.Cat(q->g);
			r.Cat(q->r);
		}
		else {
			s++;
			while(s + 1 < e && s->a && s[0] != s[1] && s - q < 0x3f)
				s++;
			r.Cat((i32)(s - q));
			while(q < s) {
				r.Cat(q->b);
				r.Cat(q->g);
				r.Cat(q->r);
				q++;
			}
		}
	}
	return Txt(r);
}

i32 Premultiply(RGBA *t, const RGBA *s, size_t len)
{
	const RGBA *e = s + len;
	while(s < e) {
		if(s->a != 255) {
			while(s < e) {
				u8 a = s->a;
				if(s->a != 0 && s->a != 255) {
					while(s < e) {
						i32 alpha = s->a + (s->a >> 7);
						t->r = alpha * (s->r) >> 8;
						t->g = alpha * (s->g) >> 8;
						t->b = alpha * (s->b) >> 8;
						t->a = s->a;
						s++;
						t++;
					}
					return IMAGE_ALPHA;
				}
				t->r = a & s->r;
				t->g = a & s->g;
				t->b = a & s->b;
				t->a = s->a;
				s++;
				t++;
			}
			return IMAGE_MASK;
		}
		*t++ = *s++;
	}
	return IMAGE_OPAQUE;
}

i32 um_table__[256];

void sInitUmTable__()
{
	ONCELOCK {
		for(i32 i = 1; i < 256; i++)
			um_table__[i] = 65536 / i;
	}
}

i32 Unmultiply(RGBA *t, const RGBA *s, size_t len)
{
	sInitUmTable__();
	const RGBA *e = s + len;
	while(s < e) {
		if(s->a != 255) {
			while(s < e) {
				u8 a = s->a;
				if(s->a != 0 && s->a != 255) {
					while(s < e) {
						i32 alpha = um_table__[s->a];
						t->r = (alpha * s->r) >> 8;
						t->g = (alpha * s->g) >> 8;
						t->b = (alpha * s->b) >> 8;
						t->a = s->a;
						s++;
						t++;
					}
					return IMAGE_ALPHA;
				}
				t->r = a & s->r;
				t->g = a & s->g;
				t->b = a & s->b;
				t->a = s->a;
				s++;
				t++;
			}
			return IMAGE_MASK;
		}
		*t++ = *s++;
	}
	return IMAGE_OPAQUE;
}

void AlphaBlend(RGBA *t, const RGBA *s, i32 len)
{
	const RGBA *e = s + len;
	while(s < e) {
		i32 alpha = 256 - (s->a + (s->a >> 7));
		t->r = s->r + (alpha * t->r >> 8);
		t->g = s->g + (alpha * t->g >> 8);
		t->b = s->b + (alpha * t->b >> 8);
		t->a = s->a + (alpha * t->a >> 8);
		s++;
		t++;
	}
}

void AlphaBlendOpaque(RGBA *t, const RGBA *s, i32 len)
{
	const RGBA *e = s + len;
	while(s < e) {
		i32 alpha = 256 - (s->a + (s->a >> 7));
		t->r = s->r + (alpha * t->r >> 8);
		t->g = s->g + (alpha * t->g >> 8);
		t->b = s->b + (alpha * t->b >> 8);
		t->a = 255;
		s++;
		t++;
	}
}

void AlphaBlend(RGBA *t, const RGBA *s, i32 len, Color color)
{
	const RGBA *e = s + len;
	i32 r = color.GetR();
	i32 g = color.GetG();
	i32 b = color.GetB();
	while(s < e) {
		i32 alpha = s->a + (s->a >> 7);
		t->r += alpha * (r - t->r) >> 8;
		t->g += alpha * (g - t->g) >> 8;
		t->b += alpha * (b - t->b) >> 8;
		t->a = s->a + ((256 - alpha) * t->a >> 8);
		s++;
		t++;
	}
}

void AlphaBlendOpaque(RGBA *t, const RGBA *s, i32 len, Color color)
{
	const RGBA *e = s + len;
	i32 r = color.GetR();
	i32 g = color.GetG();
	i32 b = color.GetB();
	while(s < e) {
		i32 alpha = s->a + (s->a >> 7);
		t->r += alpha * (r - t->r) >> 8;
		t->g += alpha * (g - t->g) >> 8;
		t->b += alpha * (b - t->b) >> 8;
		t->a = 255;
		s++;
		t++;
	}
}

void AlphaBlendStraightOpaque(RGBA *t, const RGBA *s, i32 len)
{
	const RGBA *e = s + len;
	while(s < e) {
		i32 alpha = s->a + (s->a >> 7);
		t->r += alpha * (s->r - t->r) >> 8;
		t->g += alpha * (s->g - t->g) >> 8;
		t->b += alpha * (s->b - t->b) >> 8;
		t->a = 255;
		s++;
		t++;
	}
}

void AlphaBlendStraightOpaque(RGBA *t, const RGBA *s, i32 len, i32 alpha)
{
	if(alpha >= 256) {
		AlphaBlendStraightOpaque(t, s, len);
		return;
	}
	const RGBA *e = s + len;
	alpha *= 0x102;
	while(s < e) {
		i32 a = (s->a * alpha) >> 16;
		t->r += a * (s->r - t->r) >> 8;
		t->g += a * (s->g - t->g) >> 8;
		t->b += a * (s->b - t->b) >> 8;
		t->a = 255;
		s++;
		t++;
	}
}

struct sBlends {
	i16 m;
	u8  a;
};

sBlends *sblends;

void sOnceInitBlends()
{
	ONCELOCK {
		sblends = (sBlends *)MemoryAllocPermanent(256 * 256 * sizeof(sBlends));
		for(i32 Fa = 0; Fa <= 255; Fa++)
			for(i32 Ba = 0; Ba <= 255; Ba++) {
				double A = (Fa / 255.0 + Ba / 255.0 - Fa / 255.0 * Ba / 255.0);
				sblends[(Ba << 8) + Fa].a = minmax((i32)(255 * A + 0.5), 0, 255);
				sblends[(Ba << 8) + Fa].m = A > 0.001 ? i32(256 * (Fa / 255.0) / A + 0.5) : 0;
			}
	}
}

inline void sInitBlends()
{
	if(!sblends)
		sOnceInitBlends();
}

void AlphaBlendStraight(RGBA *b, const RGBA *f, i32 len)
{
	sInitBlends();
	const RGBA *e = f + len;
	while(f < e) {
		sBlends& x = sblends[(b->a << 8) + f->a];
		i32 m = x.m;
		b->a = x.a;
		b->r += m * (f->r - b->r) >> 8;
		b->g += m * (f->g - b->g) >> 8;
		b->b += m * (f->b - b->b) >> 8;
		b++;
		f++;
	}
}

void AlphaBlendOverBgStraight(RGBA *b, RGBA bg, i32 len)
{
	sInitBlends();
	const RGBA *e = b + len;
	while(b < e) {
		sBlends& x = sblends[(bg.a << 8) + b->a];
		i32 m = x.m;
		b->a = x.a;
		b->r = bg.r + (m * (b->r - bg.r) >> 8);
		b->g = bg.g + (m * (b->g - bg.g) >> 8);
		b->b = bg.b + (m * (b->b - bg.b) >> 8);
		b++;
	}
}

void AlphaBlendStraight(RGBA *b, const RGBA *f, i32 len, Color color)
{
	sInitBlends();
	const RGBA *e = f + len;
	i32 cr = color.GetR();
	i32 cg = color.GetG();
	i32 cb = color.GetB();
	while(f < e) {
		sBlends& x = sblends[(b->a << 8) + f->a];
		i32 m = x.m;
		b->a = x.a;
		b->r += m * (cr - b->r) >> 8;
		b->g += m * (cg - b->g) >> 8;
		b->b += m * (cb - b->b) >> 8;
		b++;
		f++;
	}
}

i32 GetChMaskPos32(dword mask)
{
	if(mask == 0xff000000)
		return 3;
	if(mask == 0xff0000)
		return 2;
	if(mask == 0xff00)
		return 1;
	return 0;
}

void TransformComponents(RGBA *t, const RGBA *s, i32 len,
	u8k r[], u8k g[], u8k b[], u8k a[])
{
	while(--len >= 0) {
		t->r = r[s->r];
		t->g = g[s->g];
		t->b = b[s->b];
		t->a = a[s->a];
		s++;
		t++;
	}
}

void MultiplyComponents(RGBA *t, const RGBA *s, i32 len, i32 num, i32 den)
{
	u8 trans[256];
	for(i32 i = 0; i < 256; i++)
		trans[i] = (u8)minmax((2 * i + 1) * num / (2 * den), 0, 255);
	TransformComponents(t, s, len, trans, trans, trans, trans);
}

}
