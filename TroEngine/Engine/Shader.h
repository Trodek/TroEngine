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
	Shader(ShaderType type);
	Shader(ShaderType type, Resource* res);
	~Shader();

	void CleanUp();

	void OnEdit();

	void UpdateShader(); //called when the shader is modified

	void SaveToAssets();

	void SetShaderCode(const char* code);
	const char* GetCodeText() const;

	uint GetShaderID();
	ShaderType GetType()const;
	uint GetUID()const;

private:
	void CompileShader();

	void LoadFromFile(const char* path);

private:
	char* shader_code = nullptr;
	char* compiled_code = nullptr; // ask ric
	uint id = 0;
	bool compiles = false;

	ShaderType type = ST_NULL;

	uint UID = 0;
};
#endif // !__SHADER__

