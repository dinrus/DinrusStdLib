#ifndef S_PAINTER_FILLERS_H
#define S_PAINTER_FILLERS_H

namespace drx {

struct SolidFiller : Rasterizer::Filler {
	RGBA *t;
	RGBA  c;
	bool  invert;
	
	void Start(i32 minx, i32 maxx);
	void Render(i32 val);
	void Render(i32 val, i32 len);
};

struct SpanFiller : Rasterizer::Filler {
	RGBA       *t;
	const RGBA *s;
	i32         y;
	RGBA       *buffer;
	SpanSource *ss;
	i32         alpha;
	
	void Start(i32 minx, i32 maxx);
	void Render(i32 val);
	void Render(i32 val, i32 len);
};

struct SubpixelFiller : Rasterizer::Filler {
	i16        *sbuffer;
	i16        *begin;
	RGBA         *t, *end;
	i16        *v;
	RGBA         *s;
	RGBA          color;
	SpanSource   *ss;
	i32           alpha;
	RGBA         *buffer;
	i32           y;
	bool          invert;

	void Write(i32 len);
	void RenderN(i32 val, i32 h, i32 n);

	void Start(i32 minx, i32 maxx);
	void Render(i32 val);
	void Render(i32 val, i32 len);
	void End();
};

struct ClipFiller : Rasterizer::Filler {
	Buffer<u8> buffer;
	u8        *t;
	i32          x;
	i32          cx;
	i32          last;
	u8        *lastn;
	bool         empty;
	bool         full;
	
	void Span(i32 c, i32 len);

	virtual void Render(i32 val);
	virtual void Render(i32 val, i32 len);
	virtual void Start(i32 x, i32 len);

	void   Clear();
	void   Finish(ClippingLine& cl);
	
	void   Init(i32 cx);
};

struct MaskFillerFilter : Rasterizer::Filler {
	Rasterizer::Filler *t;
	u8k         *mask;
	i32                 empty;
	i32                 full;

	void Start(i32 minx, i32 maxx);
	void Render(i32 val, i32 len);
	void Render(i32 val);
	void End() { t->End(); }
	
	void Set(Rasterizer::Filler *f, u8k *m) { t = f; mask = m; empty = full = 0; }
};

struct NoAAFillerFilter : Rasterizer::Filler {
	Rasterizer::Filler *t;

	void Start(i32 minx, i32 maxx);
	void Render(i32 val, i32 len);
	void Render(i32 val);
	void End() { t->End(); }
	
	void Set(Rasterizer::Filler *f)                 { t = f; }
};

}

#endif
