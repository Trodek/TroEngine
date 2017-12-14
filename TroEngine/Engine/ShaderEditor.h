#ifndef __SHADER_EDITOR__
#define __SHADER_EDITOR__

#include "GUIElement.h"
#include "TextEditor.h"

class Shader;

class ShaderEditor : public GUIElement
{
public:
	ShaderEditor();
	~ShaderEditor();

	update_status UpdateGUI(float dt);

	void CreateShaderEditor();
	void SetShader(Shader* shader);

public:
	bool active = false;

private:
	TextEditor editor;
	Shader* curr_shader = nullptr;
	bool shader_changed = false;
};

#endif