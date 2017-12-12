#ifndef __SHADER_MANAGER__
#define __SHADER_MANAGER__

#include "Globals.h"
#include "Module.h"
#include <vector>

class Shader;

class ShaderManager : public Module
{
public:
	ShaderManager();
	~ShaderManager();

	uint AddShader(Shader* s);

	bool Awake(JSONDoc* config);
	bool Start();
	bool CleanUp();

	Shader* GetShader(uint index);
	Shader* GetVertexDefaultShader()const;
	Shader* GetFragmentDefaultShader()const;

private:
	void CreateDefaultShaders();

private:
	std::vector<Shader*> shaders;
	Shader* vertex_default = nullptr;
	Shader* fragment_default = nullptr;
};


#endif // !__SHADER_MANAGER__
