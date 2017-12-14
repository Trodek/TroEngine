#include "Shader.h"
#include "imgui.h"
#include "ResourceManager.h"
#include "UID.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Scene.h"
#include "SceneManager.h"
#include "ShaderManager.h"

Shader::Shader(ShaderType type) : type(type)
{
	shader_code = new char[2048];

	switch (type)
	{
	case ST_VERTEX:
	{
		char vert_shader_template[512] =
			"#version 330 core\n\
		layout(location = 0) in vec3 position;\n\
		layout(location = 1) in vec3 texCoord;\n\
		layout(location = 2) in vec3 normals;\n\
		layout(location = 3) in vec4 color;\n\n\
		out vec4 ourColor;\n\
		out vec3 Normal;\n\
		out vec2 TexCoord;\n\
		out vec3 pos;\n\n\
		uniform mat4 Model;\n\
		uniform mat4 view;\n\
		uniform mat4 projection;\n\n\
		void main()\n\
		{ \n\n\
		}";

		SetShaderCode(vert_shader_template);

	}
		break;
	case ST_FRAGMENT:
	{
		char frag_shader_template[512] =
			"#version 330 core\n\
		in vec4 ourColor;\n\
		in vec3 Normal;\n\
		in vec3 pos;\n\
		in vec2 TexCoord;\n\n\
		out vec4 color;\n\n\
		uniform sampler2D ourTexture;\n\n\
		void main()\n\
		{\n\n\
		}";

		SetShaderCode(frag_shader_template);
	}
		break;
	default:
		break;
	}


	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);

	uint* uid = md5(data, size);
	UID = *uid;
	RELEASE_ARRAY(data);
}

Shader::Shader(ShaderType type, Resource * res) : type(type)
{
	UID = res->UID;
	shader_code = new char[2048];
	LoadFromFile(App->resources->BuildAssetPath(res).c_str());
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
	//shader code is changed, recompile it
	App->renderer3D->DeleteShader(id);

	CompileShader();
}

void Shader::SaveToAssets()
{
	App->shader_manager->SaveToAssets(this);
}

void Shader::SetShaderCode(const char * code)
{
	strcpy_s(shader_code, 2048,code);
	UpdateShader();
}

const char * Shader::GetCodeText() const
{
	return shader_code;
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

void Shader::CompileShader()
{
	switch (type)
	{
	case ST_VERTEX:
		id = App->renderer3D->CreateVertexShader(shader_code);
		break;
	case ST_FRAGMENT:
		id = App->renderer3D->CreateFragmentShader(shader_code);
		break;
	}
	if (id != 0)
	{
		App->scene_manager->GetCurrentScene()->OnShaderEdit(this);
		EDITOR_LOG("Shader compilation Success :)");
	}
}

void Shader::LoadFromFile(const char * path)
{
	//Open file path and get size
	FILE* file = fopen(path, "r");
	if (file != nullptr)
	{
		fseek(file, 0L, SEEK_END);
		uint total_size = ftell(file);
		rewind(file); //go back to file begining

		fread(shader_code, sizeof(char), total_size, file);
		fclose(file);
	}
}
