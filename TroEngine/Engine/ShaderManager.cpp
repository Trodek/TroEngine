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
