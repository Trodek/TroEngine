#ifndef __SHADER_MANAGER__
#define __SHADER_MANAGER__

#include "Globals.h"
#include "Module.h"
#include "Shader.h"
#include <vector>

class Shader;
class ShaderProgram;
class Resource;

class ShaderManager : public Module
{
public:
	ShaderManager();
	~ShaderManager();

	uint AddShader(Shader* s);
	uint AddShaderProgram(ShaderProgram* sp);

	bool Awake(JSONDoc* config);
	bool Start();
	bool CleanUp();

	Shader* GetShader(uint index);
	Shader* GetShaderByUID(uint uid);
	Shader* GetVertexDefaultShader()const;
	Shader* GetFragmentDefaultShader()const;
	ShaderProgram* GetDefaultShaderProgram()const;

	ShaderProgram* GetProgram(Shader* vert, Shader* frag) const;

	void OnShaderUpdate(Shader* shader)const;

	void SaveToAssets(ShaderProgram* shader);

	void CreateShader(ShaderType type);
	void CreateShader(Resource* res);
	void CreateShaderFromFile(const char* path, Resource* r = nullptr);

	void CreateShaderProgram();

	int ShadersCount()const;

	uint GetSaveID();

private:
	void CreateDefaultShaders();
	void CreateDefaultProgram();

	void CheckSaveID(const char* file);

private:
	std::vector<Shader*> shaders;
	std::vector<ShaderProgram*> shader_programs;
	Shader* vertex_default = nullptr;
	Shader* fragment_default = nullptr;
	ShaderProgram* default_program = nullptr;

	uint save_id = 0;
};


#endif // !__SHADER_MANAGER__
