#ifndef _GlDraw_GlDraw_h_
#define _GlDraw_GlDraw_h_

#include <drx/Draw/Draw.h>
#include <drx/Painter/Painter.h>

#define GLEW_STATIC

#include <X/glew/glew.h>

#include <X/tess2/tess2.h>

#ifdef TARGET_WIN32
#include <X/glew/wglew.h>
#endif

#define GL_USE_SHADERS

#define GL_COMB_OPT

#include <GL/gl.h>

#ifdef Complex
#undef Complex
#endif

namespace drx {

enum {
	TEXTURE_LINEAR     = 0x01,
	TEXTURE_MIPMAP     = 0x02,
	TEXTURE_COMPRESSED = 0x04,
};

GLuint CreateGLTexture(const Image& img, dword flags);

GLuint GetTextureForImage(dword flags, const Image& img, zu64 context = 0); // cached
inline GLuint GetTextureForImage(const Image& img, zu64 context = 0) { return GetTextureForImage(TEXTURE_LINEAR|TEXTURE_MIPMAP, img, context); }

#ifdef GL_USE_SHADERS

enum {
	ATTRIB_VERTEX = 1,
	ATTRIB_COLOR,
	ATTRIB_TEXPOS,
	ATTRIB_ALPHA,
};

class GLProgram {
protected:
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program;
	z64  serialid;

	void Compile(tukk vertex_shader_, tukk fragment_shader_);
	void Link();

public:
	void Create(tukk vertex_shader, tukk fragment_shader,
	            Tuple2<i32, tukk > *bind_attr = NULL, i32 bind_count = 0);
	void Create(tukk vertex_shader, tukk fragment_shader,
	            i32 attr1, tukk attr1name);
	void Create(tukk vertex_shader, tukk fragment_shader,
	            i32 attr1, tukk attr1name,
	            i32 attr2, tukk attr2name);
	void Create(tukk vertex_shader, tukk fragment_shader,
	            i32 attr1, tukk attr1name,
	            i32 attr2, tukk attr2name,
	            i32 attr3, tukk attr3name);
	void Clear();
	
	i32  GetAttrib(tukk name)           { return glGetAttribLocation(program, name); }
	i32  GetUniform(tukk name)          { return glGetUniformLocation(program, name); }
	
	void Use();

	GLProgram();
	~GLProgram();
};

extern GLProgram gl_image, gl_image_colored, gl_rect;

#endif

class GLDraw : public SDraw {
	void SetColor(Color c);

	zu64   context;
	
#ifdef GL_COMB_OPT
	struct RectColor : Moveable<RectColor> {
		Rect  rect;
		Color color;
	};
	Vec<RectColor> put_rect;
#endif

	void FlushPutRect();

public:
	void    Flush()                   { FlushPutRect(); }

	virtual void  PutImage(Point p, const Image& img, const Rect& src);
#ifdef GL_USE_SHADERS
	virtual void  PutImage(Point p, const Image& img, const Rect& src, Color color);
#endif
	virtual void  PutRect(const Rect& r, Color color);
	
	void Init(Size sz, zu64 context = 0);
	void Finish();
	
	static void ClearCache();
	static void ResetCache();
	
	GLDraw()        { context = 0; }
	GLDraw(Size sz) { Init(sz); }

	~GLDraw();
};

void GLOrtho(float left, float right, float bottom, float top, float near_, float far_, GLuint u_projection);

};

#include "GLPainter.h"

#endif