#ifndef __SHADER_EDITOR__
#define __SHADER_EDITOR__

#include "GUIElement.h"

class Shader;

class ShaderEditor : public GUIElement
{
public:
	ShaderEditor();
	~ShaderEditor();

	update_status UpdateGUI(float dt);

	void CreateShaderEditor();

public:
	bool active = false;
	Shader* curr_shader = nullptr;
};

#endif