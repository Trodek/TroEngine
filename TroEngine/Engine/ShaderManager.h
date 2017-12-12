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
	bool CleanUp();

	Shader* GetShader(uint index);

private:
	std::vector<Shader*> shaders;
};


#endif // !__SHADER_MANAGER__
