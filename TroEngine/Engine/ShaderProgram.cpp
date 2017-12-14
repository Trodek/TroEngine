#include "ShaderProgram.h"
#include "UID.h"

ShaderProgram::ShaderProgram()
{
	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);

	uint* uid = md5(data, size);
	UID = *uid;
	RELEASE_ARRAY(data);
}

ShaderProgram::ShaderProgram(Shader * vert, Shader * frag): vertex_shader(vert),fragment_shader(frag)
{
	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);

	uint* uid = md5(data, size);
	UID = *uid;
	RELEASE_ARRAY(data);
}

ShaderProgram::ShaderProgram(uint uid, Shader * vert, Shader * frag) : vertex_shader(vert), fragment_shader(frag), UID(uid)
{
}

ShaderProgram::~ShaderProgram()
{
}

void ShaderProgram::SetVertexShader(Shader * shader)
{
	if (shader != nullptr && shader != vertex_shader)
		vertex_shader = shader;
}

void ShaderProgram::SetFragmentShader(Shader * shader)
{
	if (shader != nullptr && shader != fragment_shader)
		vertex_shader = shader;
}

void ShaderProgram::SetShaders(Shader * vert, Shader * frag)
{
	if (vert != nullptr && vert != vertex_shader)
		vertex_shader = vert;

	if (frag != nullptr && frag != fragment_shader)
		vertex_shader = frag;
}

uint ShaderProgram::GetProgramID() const
{
	return program_id;
}
