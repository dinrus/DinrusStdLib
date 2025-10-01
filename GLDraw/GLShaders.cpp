#include "GLDraw.h"

#ifdef GL_USE_SHADERS

namespace drx {

static GLuint LoadShader(tukk src, GLenum type) {
	GLuint shader;
	GLint compiled;

	shader = glCreateShader(type);
	if(!shader)
		return 0;

	glShaderSource(shader, 1, &src, NULL);
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if(!compiled) {
		Txt error = type == GL_VERTEX_SHADER ? "Шейдер вершины" : "Шейдер фрагмента";
		error << " не удалось скомпилировать ";
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1) {
			Buffer<char> infoLog(infoLen);
			glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
			error << ~infoLog;
		}
		Panic(error);
	}

#ifdef _DEBUG
	GLint infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
	if(infoLen > 1) {
		Buffer<char> infoLog(infoLen);
		glGetShaderInfoLog(shader, infoLen, NULL, infoLog);
	}
#endif

	return shader;
}

void GLProgram::Compile(tukk vertex_shader_, tukk fragment_shader_)
{
	Clear();

	program = glCreateProgram();

	vertex_shader = LoadShader(vertex_shader_, GL_VERTEX_SHADER);
	fragment_shader = LoadShader(fragment_shader_, GL_FRAGMENT_SHADER);
	
	glAttachShader(program, vertex_shader);
	glAttachShader(program, fragment_shader);
}

void GLProgram::Link()
{
	glLinkProgram(program);

	GLint linked;

	glGetProgramiv(program, GL_LINK_STATUS, &linked);
	if(!linked){
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if(infoLen > 1) {
			Buffer<char> infoLog(infoLen);
			glGetProgramInfoLog(program, infoLen, NULL, infoLog);
			Panic(infoLog);
		}
		Panic("Компоновка не удалась");
		Clear();
	}
}

void GLProgram::Create(tukk vertex_shader_, tukk fragment_shader_,
                       Tuple2<i32, tukk > *bind_attr, i32 bind_count)
{
	Compile(vertex_shader_, fragment_shader_);
	
	for(i32 i = 0; i < bind_count; i++)
		glBindAttribLocation(program, bind_attr[i].a, bind_attr[i].b);

	Link();
	
	Use();
}

void GLProgram::Create(tukk vertex_shader, tukk fragment_shader,
                       i32 attr1, tukk attr1name)
{
	Tuple2<i32, tukk > bind[] = {
		{ attr1, attr1name },
	};
	Create(vertex_shader, fragment_shader, bind, __countof(bind));
}

void GLProgram::Create(tukk vertex_shader, tukk fragment_shader,
                       i32 attr1, tukk attr1name,
                       i32 attr2, tukk attr2name)
{
	Tuple2<i32, tukk > bind[] = {
		{ attr1, attr1name },
		{ attr2, attr2name },
	};
	Create(vertex_shader, fragment_shader, bind, __countof(bind));
}

void GLProgram::Create(tukk vertex_shader, tukk fragment_shader,
                       i32 attr1, tukk attr1name,
                       i32 attr2, tukk attr2name,
                       i32 attr3, tukk attr3name)
{
	Tuple2<i32, tukk > bind[] = {
		{ attr1, attr1name },
		{ attr2, attr2name },
		{ attr3, attr3name },
	};
	Create(vertex_shader, fragment_shader, bind, __countof(bind));
}

void GLProgram::Clear()
{
	if(program)
		glDeleteProgram(program);
	if(vertex_shader)
		glDeleteShader(vertex_shader);
	if(fragment_shader)
		glDeleteShader(fragment_shader);
}

extern i32 sProgramCounter;

void GLProgram::Use()
{
	static z64 currentid;
	if(currentid != serialid) {
		currentid = serialid;
		glUseProgram(program);
		sProgramCounter++;
	}
}

GLProgram::GLProgram()
{
	static z64 h;
	serialid = ++h;
	vertex_shader = fragment_shader = program = 0;
}

GLProgram::~GLProgram()
{
	if(program)
		glDeleteProgram(program);
	if(vertex_shader)
		glDeleteShader(vertex_shader);
	if(fragment_shader)
		glDeleteShader(fragment_shader);
}

};

#endif
