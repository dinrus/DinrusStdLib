#include "GLDraw.h"

namespace drx {

GLCode::GLCode(tukk vertex_shader, tukk pixel_shader)
{
	Compile(vertex_shader, pixel_shader);
	Vec<Tuple2<i32, tukk >> ins;
	CParser p(vertex_shader);
	auto readID = [&](i32& n) {
		Txt id;
		n = 0;
		while(!p.IsEof() && !p.IsChar(';'))
			if(p.IsId()) {
				id = p.ReadId();
				if(p.Char('[') && p.IsInt()) {
					n = p.ReadInt();
					p.Char(']');
				}
			}
			else {
				p.SkipTerm();
				n = 0;
			}
		return id;
	};
	i32 ii = 0;
	while(!p.IsEof() && !p.Char('{'))
		if(p.Id("attribute") || p.Id("in")) {
			i32 n;
			Txt id = readID(n);
			if(id.GetCount())
				glBindAttribLocation(program, ii++, id);
		}
		else
			p.SkipTerm();
	
	Link();
	Use();

	p.Set(pixel_shader);
	i32 ti = 0;
	while(!p.IsEof() && !p.Char('{'))
		if(p.Id("sampler2D") || p.Id("sampler3D")) {
			i32 n;
			Txt id = readID(n);
			if(id.GetCount()) {
				if(n) {
					for(i32 i = 0; i < n; i++) {
						ASSERT(GetUniform(id + '[' + AsTxt(i) + ']') >= 0);
						glUniform1i(GetUniform(id + '[' + AsTxt(i) + ']'), ti++);
					}
				}
				else
					glUniform1i(GetUniform(id), ti++);
			}
		}
		else
			p.SkipTerm();
}

GLCode& GLCode::Uniform(i32 i, double a)
{
	Use();
	glUniform1f(i, (float)a);
	return *this;
}

GLCode& GLCode::Uniform(i32 i, double a, double b)
{
	Use();
	glUniform2f(i, (float)a, (float)b);
	return *this;
}

GLCode& GLCode::Uniform(i32 i, double a, double b, double c)
{
	Use();
	glUniform3f(i, (float)a, (float)b, (float)c);
	return *this;
}

GLCode& GLCode::Uniform(i32 i, double a, double b, double c, double d)
{
	Use();
	glUniform4f(i, (float)a, (float)b, (float)c, (float)d);
	return *this;
}

GLCode& GLCode::Uniform(tukk id, double a)
{
	Use();
	glUniform1f(GetUniform(id), (float)a);
	return *this;
}

GLCode& GLCode::Uniform(tukk id, double a, double b)
{
	Use();
	glUniform2f(GetUniform(id), (float)a, (float)b);
	return *this;
}

GLCode& GLCode::Uniform(tukk id, double a, double b, double c)
{
	Use();
	glUniform3f(GetUniform(id), (float)a, (float)b, (float)c);
	return *this;
}

GLCode& GLCode::Uniform(tukk id, double a, double b, double c, double d)
{
	Use();
	glUniform4f(GetUniform(id), (float)a, (float)b, (float)c, (float)d);
	return *this;
}

GLCode& GLCode::operator()(tukk id, Color c, double alpha)
{
	return Uniform(GetUniform(id), c, alpha);
}

GLCode& GLCode::operator()(i32 i, Color c, double alpha)
{
	return Uniform(i, c.GetR() / 255.0f, c.GetG() / 255.0f, c.GetB() / 255.0f, alpha);
}

GLCode& GLCode::Mat4(i32 i, float *mat4)
{
	glUniformMatrix4fv(i, 1, GL_FALSE, mat4);
	return *this;
}

GLCode& GLCode::Mat4(tukk id, float *mat4)
{
	return Mat4(GetUniform(id), mat4);
}

};
