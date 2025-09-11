#include "GLDraw.h"

namespace drx {

GLVertexData::GLVertexData(const GLVertexData& src)
{
	if(src.data) {
		data = src.data;
		data->refcount++;
	}
}

GLVertexData& GLVertexData::operator=(const GLVertexData& src)
{
	if(data != src.data) {
		if(data) Clear();
		data = src.data;
		data->refcount++;
	}
	return *this;
}

void GLVertexData::Clear()
{
	if(data && --data->refcount == 0) {
	    glDeleteVertexArrays(1, &data->VAO);
	    glDeleteBuffers(1, &data->EBO);
	    for(auto h : data->VBO)
	        glDeleteBuffers(1, &h);
		delete data;
	}
	data = NULL;
}

void GLVertexData::Do()
{
	if(!data) {
		data = new Data;
	    glGenVertexArrays(1, &data->VAO);
	    glGenBuffers(1, &data->EBO);
	}
	ASSERT(data->refcount == 1); // Changes are only allowed before copied
}

GLVertexData& GLVertexData::Add(ukk values, i32 type, i32 ntuple, i32 count)
{
	Do();
    glBindVertexArray(data->VAO);
    i32 ii = data->VBO.GetCount();
	GLuint& vbo = data->VBO.Add();
	glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    i32 sz = (i32)decode(type, GL_FLOAT, sizeof(float),
	                           GL_BYTE, sizeof(u8),
	                           GL_UNSIGNED_BYTE, sizeof(u8),
	                           GL_SHORT, sizeof(i16),
	                           GL_UNSIGNED_SHORT, sizeof(u16),
	                           GL_INT, sizeof(i32),
	                           GL_UNSIGNED_INT, sizeof(u32),
	                           sizeof(double));
	glBufferData(GL_ARRAY_BUFFER, sz * ntuple * count, values, GL_STATIC_DRAW);
	if(type == GL_FLOAT)
		glVertexAttribPointer(ii, ntuple, type, GL_FALSE, ntuple * sz, (void*)0);
	else
		glVertexAttribIPointer(ii, ntuple, type, ntuple * sz, (void*)0);
    glEnableVertexAttribArray(ii);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	return *this;
}

GLVertexData& GLVertexData::Index(i32k *indices, i32 count)
{
	Do();
	glBindVertexArray(data->VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(i32) * count, indices, GL_STATIC_DRAW);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	data->elements = count;
	return *this;
}

extern i32 sDrawCounter;
extern i32 sElementCounter;

void GLVertexData::Draw(i32 mode) const
{
	if(data) {
		GL_TIMING("GLVertexData::Draw");
		glBindVertexArray(data->VAO);
		glDrawElements(mode, data->elements, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		sDrawCounter++;
		sElementCounter += data->elements;
	}
}

void GLVertexData::Draw(GLCode& shaders, i32 mode) const
{
	shaders.Use();
	Draw(mode);
}

GLVertexData& GLVertexData::Add(const Vec<Pointf>& pt)
{
	Buffer<float> f(2 * pt.GetCount());
	float *t = f;
	for(const Pointf& p : pt) {
		*t++ = (float)p.x;
		*t++ = (float)p.y;
	}
	return Add(f, GL_FLOAT, 2, pt.GetCount());
}

};