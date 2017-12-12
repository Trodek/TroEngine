#ifndef __SHADER__
#define __SHADER__

#include "Globals.h"

class Resource;

enum ShaderType
{
	ST_NULL,
	ST_VERTEX,
	ST_FRAGMENT,
};

class Shader
{
public:
	Shader();
	Shader(Resource* res);
	~Shader();

	void CleanUp();

	void OnEdit();

	void UpdateShader(); //called when the shader is modified

	uint GetShaderID();
	ShaderType GetType()const;
	uint GetUID()const;

private:
	char* shader_code = nullptr;
	char* compiled_code = nullptr;
	uint id;
	bool compiles = false;

	ShaderType type = ST_NULL;

	uint UID;
};
#endif // !__SHADER__

