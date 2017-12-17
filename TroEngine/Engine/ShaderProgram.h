#ifndef __SHADERPROGRAM__
#define __SHADERPROGRAM__

#include "Globals.h"

class Shader;

class ShaderProgram
{
public:
	ShaderProgram();
	ShaderProgram(Shader* vert, Shader* frag);
	ShaderProgram(uint uid, Shader* vert, Shader* frag);
	~ShaderProgram();

	void SetVertexShader(Shader* shader);
	void SetFragmentShader(Shader* shader);
	void SetShaders(Shader* vert, Shader* frag);

	Shader* GetVertexShader()const;
	Shader* GetFragmentShader()const;

	void UseProgram();

	void SaveToLibray() const;

	void LinkShaderProgram();

	uint GetProgramID() const;
	uint GetUID() const;

private:
	Shader* vertex_shader = nullptr;
	Shader* fragment_shader = nullptr;

	uint program_id = 0;

	uint UID = 0;

};


#endif // !__SHADERPROGRAM__

