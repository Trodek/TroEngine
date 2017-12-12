#include "Shader.h"
#include "imgui.h"
#include "ResourceManager.h"
#include "UID.h"

Shader::Shader()
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

Shader::Shader(Resource * res)
{
	UID = res->UID;
}

Shader::~Shader()
{
}

void Shader::CleanUp()
{
}

void Shader::OnEdit()
{

	ImGui::InputTextMultiline("##shader_code", shader_code, 2048);

}

void Shader::UpdateShader()
{
}

uint Shader::GetShaderID()
{
	return id;
}

ShaderType Shader::GetType() const
{
	return type;
}

uint Shader::GetUID() const
{
	return UID;
}
