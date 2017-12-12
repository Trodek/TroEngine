#include "ShaderManager.h"
#include "Shader.h"
#include "Application.h"

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

bool ShaderManager::Awake(JSONDoc * config)
{
	App->CreateFolder("Assets\\Shaders");
	App->CreateFolder("Library\\Shaders");

	return true;
}

bool ShaderManager::Start()
{
	CreateDefaultShaders();
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

	return ret;
}

Shader * ShaderManager::GetShader(uint index)
{
	return (index < shaders.size()) ? shaders[index] : nullptr;
}

Shader * ShaderManager::GetVertexDefaultShader() const
{
	return vertex_default;
}

Shader * ShaderManager::GetFragmentDefaultShader() const
{
	return fragment_default;
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
		uniform mat4 model;\n \
		uniform mat4 view;\n \
		uniform mat4 projection;\n\n \
		void main()\n \
		{ \n \
			gl_Position = projection * view * model * vec4(position, 1.0f);\n \
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
