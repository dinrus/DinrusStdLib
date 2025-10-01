#ifndef _GLDrawDemo_Ugl_h_
#define _GLDrawDemo_Ugl_h_

namespace drx {
	
#define GL_TIMING(x)

#ifdef _DEBUG
#define GLCHK(x) do { \
	x; \
	i32 err = glGetError(); \
	if(err) LOG("ERROR " << err << " (" << __LINE__ << "): " << #x); \
	LOG((tukk )gluErrorTxt(err)); \
} while(0)
#endif

struct GLContext2D { // СДЕЛАТЬ: This should be changed to regular matrix (later)
	Sizef  vs;
	Sizef  off = Sizef(-1, 1);
	double alpha = 1;
	
	void Set(Size sz)    { vs = Sizef(2.0 / sz.cx, -2.0 / sz.cy); }
	
	GLContext2D(Size sz) { Set(sz); }
	GLContext2D()        {}
};

struct GLCode : GLProgram {
	z64 serialid;

	GLCode(tukk vertex_shader, tukk pixel_shader);

	i32 operator[](tukk id)                                             { return GetUniform(id); }
	
	GLCode& Uniform(i32 i, double a);
	GLCode& Uniform(i32 i, double a, double b);
	GLCode& Uniform(i32 i, double a, double b, double c);
	GLCode& Uniform(i32 i, double a, double b, double c, double d);

	GLCode& Uniform(tukk id, double a);
	GLCode& Uniform(tukk id, double a, double b);
	GLCode& Uniform(tukk id, double a, double b, double c);
	GLCode& Uniform(tukk id, double a, double b, double c, double d);

	GLCode& operator()(tukk id, double a)                               { return Uniform(id, a); }
	GLCode& operator()(tukk id, double a, double b)                     { return Uniform(id, a, b); }
	GLCode& operator()(tukk id, double a, double b, double c)           { return Uniform(id, a, b, c); }
	GLCode& operator()(tukk id, double a, double b, double c, double d) { return Uniform(id, a, b, c, d); }

	GLCode& operator()(i32 i, double a)                                        { return Uniform(i, a); }
	GLCode& operator()(i32 i, double a, double b)                              { return Uniform(i, a, b); }
	GLCode& operator()(i32 i, double a, double b, double c)                    { return Uniform(i, a, b, c); }
	GLCode& operator()(i32 i, double a, double b, double c, double d)          { return Uniform(i, a, b, c, d); }

	GLCode& operator()(tukk id, Pointf p)                               { return Uniform(id, p.x, p.y); }
	GLCode& operator()(i32 i, Pointf p)                                        { return Uniform(i, p.x, p.y); }
	GLCode& operator()(tukk id, Sizef sz)                               { return Uniform(id, sz.cx, sz.cy); }
	GLCode& operator()(i32 i, Sizef sz)                                        { return Uniform(i, sz.cx, sz.cy); }
	GLCode& operator()(tukk id, Color c, double alpha = 1);
	GLCode& operator()(i32 i, Color c, double alpha = 1);
	
	GLCode& Mat4(i32 i, float *mat4);
	GLCode& Mat4(tukk id, float *mat4);
};

class GLTexture {
	struct Data {
		i32     refcount = 1;
		GLuint  textureid = 0;
		Point   hotspot = Point(0, 0);
		Size    sz = Size(0, 0);
	};
	
	Data    *data = NULL;
	
	void Set(GLuint texture, Size sz, Point hotspot = Point(0, 0));
	
	friend class GLTextureDraw;
	
public:
	void     Clear();
	void     Set(const Image& img, dword flags = TEXTURE_LINEAR|TEXTURE_MIPMAP);
	
	void     Bind(i32 ii = 0) const;
	i32      GetID() const      { return data ? data->textureid : 0; }
	operator GLuint() const     { return GetID(); }
	Size     GetSize() const    { return data ? data->sz : Size(0, 0); }
	Point    GetHotSpot() const { return data ? data->hotspot : Point(0, 0); }

	GLTexture()                 {}
	GLTexture(const Image& img, dword flags = TEXTURE_LINEAR|TEXTURE_MIPMAP) { Set(img, flags); }
	~GLTexture()                { Clear(); }

	GLTexture(const GLTexture& src);
	GLTexture& operator=(const GLTexture& src);
};

void GLBind(i32 ii, const Image& img, dword style = TEXTURE_LINEAR|TEXTURE_MIPMAP);
void GLBind(const Image& img, dword style = TEXTURE_LINEAR|TEXTURE_MIPMAP);

void GLDrawTexture(const GLContext2D& dd, const Rectf& rect, i32 textureid, Size tsz, const Rect& src);
void GLDrawTexture(const GLContext2D& dd, const Rectf& rect, const GLTexture& img, const Rect& src);
void GLDrawImage(const GLContext2D& dd, const Rectf& rect, const Image& img, const Rect& src);
void GLDrawTexture(const GLContext2D& dd, const Rectf& rect, const GLTexture& img);
void GLDrawImage(const GLContext2D& dd, const Rectf& rect, const Image& img);

class GLVertexData {
	struct Data {
		i32            refcount = 1;
		GLuint         VAO = 0;
		GLuint         EBO = 0;
		i32            elements = 0;
	    
		Vec<GLuint> VBO;
	};
	
	Data    *data = NULL;
	
	void     Do();

public:
	void    Clear();

	GLVertexData& Add(ukk data, i32 type, i32 ntuple, i32 count);
	GLVertexData& Add(const float *data, i32 ntuple, i32 count) { return Add(data, GL_FLOAT, ntuple, count); }
	GLVertexData& Add(u8k *data, i32 ntuple, i32 count)  { return Add(data, GL_UNSIGNED_BYTE, ntuple, count); }
	GLVertexData& Add(const dword *data, i32 ntuple, i32 count) { return Add(data, GL_UNSIGNED_INT, ntuple, count); }
	GLVertexData& Add(const Vec<float>& data, i32 ntuple)    { return Add(data, ntuple, data.GetCount() / ntuple); }
	GLVertexData& Add(const Vec<u8>& data, i32 ntuple)     { return Add(data, ntuple, data.GetCount() / ntuple); }
	GLVertexData& Add(const Vec<dword>& data, i32 ntuple)    { return Add(data, ntuple, data.GetCount() / ntuple); }
	GLVertexData& Add(const Vec<Pointf>& pt);
	GLVertexData& Index(i32k *indices, i32 count);
	GLVertexData& Index(const Vec<i32>& indices)             { return Index(indices, indices.GetCount()); }
	
	void    Draw(i32 mode = GL_TRIANGLES) const;

	void    Draw(GLCode& shaders, i32 mode = GL_TRIANGLES) const;
	
	operator bool() const                                       { return data; }
	bool    IsEmpty() const                                     { return !data; }

	GLVertexData()                                              {}
	~GLVertexData()                                             { Clear(); }

	GLVertexData(const GLVertexData& src);
	GLVertexData& operator=(const GLVertexData& src);
};

const GLVertexData& GLRectMesh();

template <typename Src>
void GLPolygons(GLVertexData& mesh, const Src& polygon);

void GLDrawPolygons(const GLContext2D& dd, Pointf at, const GLVertexData& mesh, Sizef scale, Color color);
void GLDrawConvexPolygons(const GLContext2D& dd, Pointf at, const GLVertexData& mesh, Sizef scale, Color color);

template <typename Src>
void GLPolylines(GLVertexData& data, const Src& polygon, bool close_loops = false);

void DashPolyline(Vec<Vec<Pointf>>& polyline, const Vec<Pointf>& line,
                  const Vec<double>& pattern, double distance = 0);

void GLDrawPolylines(const GLContext2D& dd, Pointf at, const GLVertexData& mesh, Sizef scale, double width, Color color);

void GLDrawStencil(Color color, double alpha);

void GLDrawEllipse(const GLContext2D& dd, Pointf center, Sizef radius, Color fill_color,
                   double width, Color line_color, const Vec<double>& dash, double distance);
void GLDrawEllipse(const GLContext2D& dd, Pointf center, Sizef radius, Color fill_color,
                   double width, Color line_color);

GLTexture GetGlyphGLTextureCached(double angle, i32 chr, Font font, Color color);

void GLDrawText(const GLContext2D& dd, Pointf pos, double angle, wtukk text, Font font,
                Color ink, i32 n = -1, i32k *dx = NULL);

void GLArc(Vec<Vec<Pointf>>& line, const Rectf& rc, Pointf start, Pointf end);

class GLTriangles {
	Vec<float>  pos;
	Vec<u8>   color;
	Vec<GLint>  elements;
	i32            ii = 0;

public:
	i32  Vertex(float x, float y, u8 r, u8 g, u8 b, double alpha) {
		pos << x << y << (float)alpha;
		color << r << g << b;
		return ii++;
	}

	i32  Vertex(float x, float y, Color c, double alpha)  { return Vertex(x, y, c.GetR(), c.GetG(), c.GetB(), alpha); }
	i32  Vertex(Pointf p, Color c, double alpha)          { return Vertex((float)p.x, (float)p.y, c, alpha); }
	i32  Vertex(i32 x, i32 y, Color c, double alpha)      { return Vertex((float)x, (float)y, c, alpha); }

	void Triangle(i32 a, i32 b, i32 c)                    { elements << a << b << c; }
	
	void Clear()                                          { elements.Clear(); pos.Clear(); color.Clear(); ii = 0; }

	void Draw(const GLContext2D& dd);
};

void Ellipse(GLTriangles& tr, Pointf center, Sizef radius, Color color, double width, Color line_color, double alpha);
void Polyline(GLTriangles& tr, const Vec<Pointf>& p, double width, Color color, double alpha, bool close);

#include "GLPainter.hpp"

class DrawGL : public NilPainter, GLTriangles {
public:
	virtual dword GetInfo() const;

	virtual void BeginOp();
	virtual bool ClipOp(const Rect& r);
	virtual bool ClipoffOp(const Rect& r);
	virtual bool IntersectClipOp(const Rect& r);
	virtual void OffsetOp(Point p);
	virtual bool ExcludeClipOp(const Rect& r);
	virtual void EndOp();
	virtual bool IsPaintingOp(const Rect& r) const;

	virtual void DrawRectOp(i32 x, i32 y, i32 cx, i32 cy, Color color);
	virtual void DrawImageOp(i32 x, i32 y, i32 cx, i32 cy, const Image& image, const Rect& src, Color color);
	virtual void DrawTextOp(i32 x, i32 y, i32 angle, wtukk text, Font font, Color ink, i32 n, i32k *dx);

	virtual void DrawArcOp(const Rect& rc, Point start, Point end, i32 width, Color color);
	virtual void DrawEllipseOp(const Rect& r, Color color, i32 pen, Color pencolor);
	virtual void DrawLineOp(i32 x1, i32 y1, i32 x2, i32 y2, i32 width, Color color);
	virtual void DrawPolyPolyPolygonOp(const Point *vertices, i32 vertex_count, i32k *subpolygon_counts, i32 scc, i32k *disjunct_polygon_counts, i32 dpcc, Color color, i32 width, Color outline, zu64 pattern, Color doxor);
	virtual void DrawPolyPolylineOp(const Point *vertices, i32 vertex_count, i32k *counts, i32 count_count, i32 width, Color color, Color doxor);
	
	virtual void MoveOp(const Pointf& p, bool rel);
	virtual void LineOp(const Pointf& p, bool rel);
	virtual void OpacityOp(double o);
	virtual void CloseOp();
	virtual void StrokeOp(double width, const RGBA& rgba);
	virtual void FillOp(const RGBA& color);
	virtual void DashOp(const Vec<double>& dash, double start);

private:
	struct Cloff : Moveable<Cloff> {
		Rect   clip;
		Pointf offset;
	};
	
	struct State {
		double alpha;
		double dash_start;
		Vec<double> dash;
	};
	
	Vec<Cloff> cloff;
	Array<State>  state;
	GLContext2D   dd;
	Size          view_size;
	Rect          scissor;

	Pointf prev;
	Vec<Vec<Pointf>> path;
	bool   path_done;
	bool   close_path;
	double dash_start;
	Vec<double> dash, path_dash;

	void   Push();
	Pointf Off(i32 x, i32 y);
	Pointf Off(Point p)                      { return Off(p.x, p.y); }
	Rectf  Off(i32 x, i32 y, i32 cx, i32 cy);
	Rectf  Off(i32 x, i32 y, Size sz);
	void   SyncScissor();
	void   DoPath(Vec<Vec<Pointf>>& poly, const Point *pp, const Point *end);
	static const Vec<double>& GetDash(i32& width);
	void   ApplyDash(Vec<Vec<Pointf>>& polyline, i32& width);
	void   DoDrawPolylines(Vec<Vec<Pointf>>& poly, i32 width, Color color, bool close = false);
	void   DoDrawPolygons(const Vec<Vec<Pointf>>& path, Color color);
	
	friend class GLTextureDraw;

public:
	using Draw::Clip;

	void Init(Size sz, double alpha = 1);
	
	void Flush();
	
	operator const GLContext2D&()           { Flush(); return dd; }

	DrawGL() {}
	DrawGL(Size sz, double alpha = 1)       { Init(sz, alpha); }
	~DrawGL();
};

class GLTextureDraw : public DrawGL {
	GLuint framebuffer = 0;
    GLuint texture = 0;
	GLuint rbo = 0;
	Size   sz;
	i32    msaa = 0;

public:
	void      Clear();
	void      Create(Size sz, i32 msaa = 0);
	GLTexture GetResult();
	operator  GLTexture()                { return GetResult(); }

	GLTextureDraw()                      {}
	GLTextureDraw(Size sz, i32 msaa = 0) { Create(sz, msaa); }
	~GLTextureDraw()                     { Clear(); }
};

void GLClearCounters();
i32  GLElementCounter();
i32  GLTextureCounter();
i32  GLProgramCounter();
i32  GLDrawCounter();
i32  GLTesselateCounter();

};

#endif
