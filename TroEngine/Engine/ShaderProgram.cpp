#include "ShaderProgram.h"
#include "UID.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Shader.h"
#include "ResourceManager.h"

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

	LinkShaderProgram();
}

ShaderProgram::ShaderProgram(uint uid, Shader * vert, Shader * frag) : vertex_shader(vert), fragment_shader(frag), UID(uid)
{
	LinkShaderProgram();
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

Shader * ShaderProgram::GetVertexShader() const
{
	return vertex_shader;
}

Shader * ShaderProgram::GetFragmentShader() const
{
	return fragment_shader;
}

void ShaderProgram::UseProgram()
{
	App->renderer3D->UseShaderProgram(program_id);
}

void ShaderProgram::SaveToLibray() const
{
	Resource* res = App->resources->GetResource(UID);
	std::string path = App->resources->BuildLibraryPath(res);

	int prog_size = App->renderer3D->GetProgramSize(program_id);

	char* binary = new char[prog_size];

	uint bin_size = App->renderer3D->GetProgramBinary(program_id, prog_size, binary);

	FILE* file = fopen(path.c_str(), "wb");
	fwrite(binary, sizeof(char), bin_size, file);
	fclose(file);

	RELEASE_ARRAY(binary);

}

void ShaderProgram::LinkShaderProgram()
{
	if (program_id != 0)
	{
		App->renderer3D->DeleteProgram(program_id);
		program_id = 0;
	}

	program_id = App->renderer3D->CreateShaderProgram();
	App->renderer3D->AttachShaderToProgram(program_id, vertex_shader->GetShaderID());
	App->renderer3D->AttachShaderToProgram(program_id, fragment_shader->GetShaderID());
	if (App->renderer3D->LinkProgram(program_id) == false)
	{
		EDITOR_LOG("Error while linking shader program, check errors above.");
		App->renderer3D->DeleteProgram(program_id);
		program_id = 0;
	}
	else EDITOR_LOG("Shader Program %d created :)", program_id);
}

uint ShaderProgram::GetProgramID() const
{
	return program_id;
}

uint ShaderProgram::GetUID() const
{
	return UID;
}
