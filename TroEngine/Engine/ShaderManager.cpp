#include "ShaderManager.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Application.h"
#include "ResourceManager.h"

ShaderManager::ShaderManager()
{
}

ShaderManager::~ShaderManager()
{
}

uint ShaderManager::AddShader(Shader * s)
{
	shaders.push_back(s);
	return shaders.size() - 1;
}

uint ShaderManager::AddShaderProgram(ShaderProgram * sp)
{
	shader_programs.push_back(sp);
	return shader_programs.size() - 1;
}

bool ShaderManager::Awake(JSONDoc * config)
{
	App->CreateFolder("Assets\\Shaders");
	App->CreateFolder("Library\\Shaders");

	return true;
}

bool ShaderManager::Start()
{
	CreateDefaultShaders();

	CreateDefaultProgram();
	return true;
}

bool ShaderManager::CleanUp()
{
	bool ret = true;

	for (std::vector<Shader*>::iterator it = shaders.begin(); it != shaders.end(); )
	{
		(*it)->CleanUp();
		RELEASE(*it);

		it = shaders.erase(it);
	}

	for (std::vector<ShaderProgram*>::iterator it = shader_programs.begin(); it != shader_programs.end(); )
	{
		RELEASE(*it);

		it = shader_programs.erase(it);
	}

	return ret;
}

Shader * ShaderManager::GetShader(uint index)
{
	return (index < shaders.size()) ? shaders[index] : nullptr;
}

Shader * ShaderManager::GetShaderByUID(uint uid)
{
	for (int i = 0; i < shaders.size(); ++i)
	{
		if (shaders[i]->GetUID() == uid)
			return shaders[i];
	}
	return nullptr;
}

Shader * ShaderManager::GetVertexDefaultShader() const
{
	return vertex_default;
}

Shader * ShaderManager::GetFragmentDefaultShader() const
{
	return fragment_default;
}

ShaderProgram * ShaderManager::GetDefaultShaderProgram() const
{
	return default_program;
}

ShaderProgram * ShaderManager::GetProgram(Shader * vert, Shader * frag) const
{
	for (int i = 0; i < shader_programs.size(); ++i)
	{
		if (shader_programs[i]->GetFragmentShader() == frag && shader_programs[i]->GetVertexShader() == vert)
		{
			return shader_programs[i];
		}
	}
	return nullptr;
}

void ShaderManager::OnShaderUpdate(Shader * shader) const
{
	//update all programs that contain that shader
	for (int i = 0; i < shader_programs.size(); ++i)
	{
		if (shader == shader_programs[i]->GetFragmentShader() || shader == shader_programs[i]->GetVertexShader())
			shader_programs[i]->LinkShaderProgram();
	}
}

void ShaderManager::SaveToAssets(ShaderProgram * shader)
{
	shader->SaveToLibray();
}

void ShaderManager::CreateShader(ShaderType type)
{
	Shader* shader = new Shader(type);

	AddShader(shader);
}

void ShaderManager::CreateShader(Resource * res)
{
	ShaderType t = ST_NULL;
	if (res->extension == "vert")
		t = ST_VERTEX;
	else if (res->extension == "frag")
		t = ST_FRAGMENT;

	uint uid = res->UID;

	Shader* shader = nullptr;
	if (uid != 0)
		shader = new Shader(t, uid);
	else
	{
		shader = new Shader(t);
		res->UID = shader->GetUID();
	}

	std::string path = App->resources->BuildAssetPath(res);
	FILE* f = fopen(path.c_str(), "r");
	fseek(f, 0L, SEEK_END);
	uint total_size = ftell(f);
	rewind(f); //go back to file begining

	char* code = new char[total_size];

	fread(code, sizeof(char), total_size, f);
	fclose(f);
	shader->SetShaderCode(code);
	RELEASE_ARRAY(code);

	AddShader(shader);
}

void ShaderManager::CreateShaderFromFile(const char * path, Resource* r)
{
	Resource* res = nullptr;

	if (r == nullptr) //no meta exist
	{
		res = new Resource();
		res->extension = App->resources->GetExtensionFromPath(path);
		res->rel_path = App->resources->GetRelativePathFromPath(path);
		res->type = R_SHADER;

		Shader* s = nullptr;
		if (res->extension == "frag")
		{
			s = new Shader(ST_FRAGMENT);
			res->sh_type = ST_FRAGMENT;
		}
		else if (res->extension == "vert")
		{
			s = new Shader(ST_VERTEX);
			res->sh_type = ST_VERTEX;
		}

		s->LoadFromFile(path);

		res->UID = s->GetUID();
		AddShader(s);
		res->manager_id = shaders.size() - 1;
		res->name = App->resources->GetNameFromPath(path);

		uint key = App->resources->CreateResource(path);

		App->resources->resources[key].push_back(res);
		s->UpdateShader();
	}
	else // meta exist
	{
		res = r;
		Shader* s = new Shader(res->sh_type, res->UID);
		s->LoadFromFile(path);
		AddShader(s);
		res->manager_id = shaders.size() - 1;
		s->UpdateShader();
	}

	CheckSaveID(path);

}

void ShaderManager::CreateShaderProgram()
{
}

int ShaderManager::ShadersCount() const
{
	return shaders.size();
}

uint ShaderManager::GetSaveID()
{
	save_id++;
	return save_id - 1;
}

void ShaderManager::CreateDefaultShaders()
{
	//Default Vertex Shader
	char vert_shader[512] =
		"#version 330 core\n \
		layout(location = 0) in vec3 position;\n \
		layout(location = 1) in vec3 texCoord;\n \
		layout(location = 2) in vec3 normals;\n \
		layout(location = 3) in vec4 color;\n\n \
		out vec4 ourColor;\n \
		out vec3 Normal;\n \
		out vec2 TexCoord;\n\n \
		uniform mat4 Model;\n \
		uniform mat4 view;\n \
		uniform mat4 projection;\n\n \
		void main()\n \
		{ \n \
			gl_Position = projection * view * Model * vec4(position, 1.0f);\n \
			ourColor = color;\n \
			TexCoord = texCoord.xy;\n \
		}";

	vertex_default = new Shader(ST_VERTEX);
	vertex_default->SetShaderCode(vert_shader);
	AddShader(vertex_default);
	
	//Default Fragment Shader
	char frag_shader[512] =
		"#version 330 core\n \
		in vec4 ourColor;\n \
		in vec3 Normal;\n \
		in vec2 TexCoord;\n\n \
		out vec4 color;\n\n \
		uniform sampler2D ourTexture;\n\n \
		void main()\n \
		{\n \
			color = texture(ourTexture, TexCoord);\n \
		}";

	fragment_default = new Shader(ST_FRAGMENT);
	fragment_default->SetShaderCode(frag_shader);
	AddShader(fragment_default);

}

void ShaderManager::CreateDefaultProgram()
{
	default_program = new ShaderProgram(vertex_default, fragment_default);
	AddShaderProgram(default_program);
}

void ShaderManager::CheckSaveID(const char * file)
{
	std::string f = file;
	uint cut = f.find_last_of("_");
	std::string num = f.substr(cut + 1);

	int id = atoi(num.c_str());
	if (id > save_id)
		save_id = id + 1;
}
