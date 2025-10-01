enum LookOp {
	LOOK_PAINT,
	LOOK_MARGINS,
	LOOK_PAINTEDGE,
};

enum {
	CH_SCROLLBAR_IMAGE = -1000, // special Image hotspot x value for ChPaint of scrollbar
	CH_EDITFIELD_IMAGE = -1001, // special Image hotspot x2 value, y2 is then x and y for the purposes of painting
};

void  ChLookFn(Val (*fn)(Draw& w, const Rect& r, const Val& look, i32 lookop, Color ink));

Image AdjustColors(const Image& img);

void   Override(Iml& target, Iml& source, bool colored = false);
void   ColoredOverride(Iml& target, Iml& source);

void   ChReset();
void   ChFinish();

void   ChPaint(Draw& w, const Rect& r, const Val& look, Color ink = Null);
void   ChPaint(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Val& look, Color ink = Null);
void   ChPaintEdge(Draw& w, const Rect& r, const Val& look, Color ink = Null);
void   ChPaintEdge(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Val& look, Color ink = Null);
void   ChPaintBody(Draw& w, const Rect& r, const Val& look, Color ink = Null);
void   ChPaintBody(Draw& w, i32 x, i32 y, i32 cx, i32 cy, const Val& look, Color ink = Null);
Rect   ChMargins(const Val& look);

void   DeflateMargins(Rect& r, const Rect& margin);
void   ChDeflateMargins(Rect& r, const Val& look);
void   DeflateMargins(Size& sz, const Rect& m);
void   ChDeflateMargins(Size& sz, const Val& look);
void   InflateMargins(Rect& r, const Rect& m);
void   ChInflateMargins(Rect& r, const Val& look);
void   InflateMargins(Size& sz, const Rect& m);
void   ChInflateMargins(Size& sz, const Val& look);

void   ChInvalidate();
bool   ChIsInvalidated();
bool   IsLabelTextColorMismatch();
bool   IsDarkColorFace();

template <class T>
struct ChStyle {
	u8 status;
	u8 registered;
	T   *standard;

	const T& Standard() const      { return *standard; }
	T&       Write() const         { T& x = *(T *)this; x.status = 2; ChInvalidate(); return x; }
	void     Assign(const T& src)  { *(T *)this = src; }

	ChStyle()                      { status = 0; registered = 0; standard = NULL; }
};

#define CH_STYLE(klass, type, style) \
struct COMBINE5(klass, __, type, __, style) : klass::type { \
	void Init(); \
	static void InitIt(); \
}; \
\
void COMBINE5(klass, __, type, __, style)::InitIt() { \
	klass::style(); \
} \
\
const klass::type& klass::style() \
{ \
	static COMBINE5(klass, __, type, __, style) b, standard; \
	if(b.status == 0) { \
		ChRegisterStyle__(b.status, b.registered, COMBINE5(klass, __, type, __, style)::InitIt); \
		b.Init(); \
		b.status = 1; \
		standard = b; \
		standard.standard = b.standard = &standard; \
	} \
	return b; \
} \
\
void COMBINE5(klass, __, type, __, style)::Init()


// CH_VAR0 allows inserting action into _Write (missing ending '}')
#define CH_VAR0(chtype, type, name, init) \
chtype& COMBINE(ch_var__, name)(); \
void COMBINE(ch_init__, name)() { \
	COMBINE(ch_var__, name)(); \
} \
\
chtype& COMBINE(ch_var__, name)() { \
	static chtype b; \
	if(b.status == 0) { \
		ChRegisterStyle__(b.status, b.registered, COMBINE(ch_init__, name)); \
		b.value = init; \
		b.status = 1; \
	} \
	return b; \
} \
\
type name() { return COMBINE(ch_var__, name)().value; } \
void COMBINE(name, _Write)(type v) { COMBINE(ch_var__, name)().Write().value = v;

#define CH_VAR(chtype, type, name, init) CH_VAR0(chtype, type, name, init) }

struct ChColor : ChStyle<ChColor> { Color value; };
#define CH_COLOR(name, init) CH_VAR(ChColor, Color, name, init)

struct ChInt : ChStyle<ChInt> { i32 value; };
#define CH_INT(name, init) CH_VAR(ChInt, i32, name, init)

struct ChVal : ChStyle<ChVal> { Val value; };
#define CH_VALUE(name, init) CH_VAR(ChVal, Val, name, init)

struct ChImage : ChStyle<ChImage> { Image value; };
#define CH_IMAGE(name, init) CH_VAR(ChImage, Image, name, init)

Val ChLookWith(const Val& look, const Image& img, Point offset = Point(0, 0));
Val ChLookWith(const Val& look, const Image& img, Color color, Point offset = Point(0, 0));
Val ChLookWith(const Val& look, const Image& img, Color (*color)(i32 i), i32 i, Point offset = Point(0, 0));

void  ChLookWith(Val *look, const Image& image, const Color *color, i32 n = 4);

//private:
void ChRegisterStyle__(u8& state, u8& registered, void (*init)());

Val ChBorder(const ColorF *colors, const Val& face = SColorFace());
