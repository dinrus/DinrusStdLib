//#BLITZ_APPROVE

#define IMAGE_META(k, v)
#define IMAGE_SCAN(s)
#define IMAGE_PACKED(n, d)

#define PREMULTIPLIED
#define IMAGE_BEGIN_DATA
#define IMAGE_DATA(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,aa,ab,ac,ad,ae,af,b0,b1,b2,b3,b4,b5,b6,b7,b8,b9,ba,bb,bc,bd,be,bf)
#define IMAGE_END_DATA(n, c)

class IMAGECLASS {
public:
#define IMAGE_BEGIN(n) I_##n,
#define IMAGE_ID(n) I_##n,
	enum {
#include IMAGEFILE
		COUNT
	};
#undef  IMAGE_BEGIN
#undef  IMAGE_ID

public:
	static DRX::Iml&   Iml();

	static void        Register__()                { Register(ASTXT(IMAGECLASS), Iml()); }

	static i32         Find(const DRX::Txt& s);
	static i32         Find(tukk s);
	static i32         GetCount()                  { return Iml().GetCount(); }
	static DRX::Txt GetId(i32 i)                { return Iml().GetId(i); }

	static DRX::Image  Get(i32 i);
	static DRX::Image  Get(tukk s);
	static DRX::Image  Get(const DRX::Txt& s);

	static void   Set(i32 i, const DRX::Image& m);
	static void   Set(tukk s, const DRX::Image& m);

	static void   Reset()                     { Iml().Reset(); }

#define IMAGE_BEGIN(n) static DRX::Image n() { return Get(I_##n); }
#define IMAGE_ID(n) static DRX::Image n() { return Get(I_##n); }
#include IMAGEFILE
#undef  IMAGE_BEGIN
#undef  IMAGE_ID

};

#undef  IMAGE_SCAN
#undef  IMAGE_PACKED
#undef  IMAGE_META

#undef  IMAGE_BEGIN_DATA
#undef  IMAGE_END_DATA
#undef  IMAGE_DATA

#ifndef IMAGE_KEEP
#undef  IMAGECLASS
#undef  IMAGEFILE
#endif
