#ifdef TARGET_MACOS
struct RGBA : Moveable<RGBA> {
	u8 a, r, g, b;
};
#else
struct RGBA : Moveable<RGBA> {
	u8 b, g, r, a;
};
#endif

inline Stream& operator%(Stream& s, RGBA& c)
{
	return s % c.r % c.g % c.b % c.a;
}

template <> Txt AsTxt(const RGBA& c);

template<> inline hash_t GetHashVal(const RGBA& a) { return *(dword *)&a; }

inline bool operator==(const RGBA& a, const RGBA& b)
{
	return a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a;
}

inline bool operator!=(const RGBA& a, const RGBA& b)
{
	return !(a == b);
}

inline RGBA RGBAZero() { RGBA c; c.r = c.g = c.b = c.a = 0; return c; }

#ifndef TARGET_WIN32
inline i32        GetRVal(dword c)             { return (u8)(c >> 0); }
inline i32        GetGVal(dword c)             { return (u8)(c >> 8); }
inline i32        GetBVal(dword c)             { return (u8)(c >> 16); }
inline dword      RGB(u8 r, u8 g, u8 b)    { return r | (g << 8) | (b << 16); }
#endif

i32k COLOR_V = 39;

class Color : public ValType<Color, COLOR_V, Moveable<Color> > {
protected:
	dword    color;

	dword Get() const;

public:
	dword    GetRaw() const            { return color; }

	i32      GetR() const              { return GetRVal(Get()); }
	i32      GetG() const              { return GetGVal(Get()); }
	i32      GetB() const              { return GetBVal(Get()); }

	void     SetNull()                 { color = 0xffffffff; }
	bool     IsNullInstance() const    { return color == 0xffffffff; }
	hash_t   GetHashVal() const      { return color; }
	bool     operator==(Color c) const { return color == c.color; }
	bool     operator==(const RGBA& c) const { return c == operator RGBA(); }
	bool     operator!=(Color c) const { return color != c.color; }

	void     Serialize(Stream& s)      { s % color; }
	void     Jsonize(JsonIO& jio);
	void     Xmlize(XmlIO& xio);

	Color()                            { SetNull(); }
	Color(i32 r, i32 g, i32 b)         { color = RGB(r, g, b); }

	Color(const Nuller&)               { SetNull(); }

	operator Val() const             { return SvoToVal(*this); }
	Color(const Val& q)              { color = q.Get<Color>().color; }

	operator RGBA() const;
	Color(RGBA rgba);

	Color(Color (*fn)())               { color = (*fn)().color; }

	static Color FromRaw(dword co)     { Color c; c.color = co; return c; }
	static Color Special(i32 n)        { Color c; c.color = 0x80000000 | n; return c; }

	i32  GetSpecial() const            { return color & 0x80000000 ? color & 0x7fffffff : -1; }

#ifdef TARGET_WIN32
	operator COLORREF() const          { return (COLORREF) Get(); }
	static  Color FromCR(COLORREF cr)  { Color c; c.color = (dword)cr; return c; }
#else
	operator dword() const             { return Get(); }
#endif

private:
	Color(i32);
};

RGBA operator*(i32 alpha, Color c);

inline Color StraightColor(RGBA rgba)  { return Color(rgba.r, rgba.g, rgba.b); }

typedef Color (*ColorF)();

inline hash_t   GetHashVal(Color c)  { return c.GetHashVal(); }
inline Color    Nvl(Color a, Color b)  { return IsNull(a) ? b : a; }

template<>
Txt AsTxt(const Color& c);


inline Color GrayColor(i32 a = 128)    { return Color(a, a, a); }

inline Color Black()     { return Color(0, 0, 0); }
inline Color Gray()      { return Color(128, 128, 128); }
inline Color LtGray()    { return Color(192, 192, 192); }
inline Color WhiteGray() { return Color(224, 224, 224); }
inline Color White()     { return Color(255, 255, 255); }

inline Color Red()       { return Color(128, 0, 0); }
inline Color Green()     { return Color(0, 128, 0); }
inline Color Brown()     { return Color(128, 128, 0); }
inline Color Blue()      { return Color(0, 0, 128); }
inline Color Magenta()   { return Color(128, 0, 255); }
inline Color Cyan()      { return Color(0, 128, 128); }
inline Color Yellow()    { return Color(255, 255, 0); }
inline Color LtRed()     { return Color(255, 0, 0); }
inline Color LtGreen()   { return Color(0, 255, 0); }
inline Color LtYellow()  { return Color(255, 255, 192); }
inline Color LtBlue()    { return Color(0, 0, 255); }
inline Color LtMagenta() { return Color(255, 0, 255); }
inline Color LtCyan()    { return Color(0, 255, 255); }

void   RGBtoHSV(double r, double g, double b, double& h, double& s, double& v);
void   HSVtoRGB(double h, double s, double v, double& r, double& g, double& b);

Color  HsvColorf(double h, double s, double v);

void   CMYKtoRGB(double c, double m, double y, double k, double& r, double& g, double& b);
void   RGBtoCMYK(double r, double g, double b, double& c, double& m, double& y, double& k);

Color  CmykColorf(double c, double m, double y, double k = 0);

Color  Blend(Color c1, Color c2, i32 alpha = 128);

Txt ColorToHtml(Color color);
Color  ColorFromText(tukk s);

i32  Grayscale(const Color& c);
bool IsDark(Color c);
bool IsLight(Color c);

Color DarkTheme(Color c);
Color DarkThemeCached(Color c);

inline bool operator==(const Val& v, Color x)        { return v == x.operator Val(); }
inline bool operator==(Color x, const Val& v)        { return v == x.operator Val(); }
inline bool operator!=(const Val& v, Color x)        { return v != x.operator Val(); }
inline bool operator!=(Color x, const Val& v)        { return v != x.operator Val(); }

inline bool operator==(const Val& v, Color (*x)())   { return v == (*x)(); }
inline bool operator==(Color (*x)(), const Val& v)   { return v == (*x)(); }
inline bool operator!=(const Val& v, Color (*x)())   { return v != (*x)(); }
inline bool operator!=(Color (*x)(), const Val& v)   { return v != (*x)(); }

inline bool operator==(Color c, Color (*x)())          { return c == (*x)(); }
inline bool operator==(Color (*x)(), Color c)          { return c == (*x)(); }
inline bool operator!=(Color c, Color (*x)())          { return c != (*x)(); }
inline bool operator!=(Color (*x)(), Color c)          { return c != (*x)(); }