#ifndef __MODULEGUI__
#define __MODULEGUI__

#include "Module.h"
#include "Globals.h"
#include <list>
#include "ImGuizmo.h"

class GUIElement;
class ConsoleGUI;
class Inspector;
class Explorer;
class ShaderEditor;

class Shader;

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Awake(JSONDoc* config);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	void RenderGUI();
	void SetGUIColors(float _alpha = 1.0f);

	void AddElement(GUIElement* ele);

public:
	ConsoleGUI* console = nullptr;
	Inspector* inspector = nullptr;
	Explorer* explorer = nullptr;
	ShaderEditor* shader_editor = nullptr;

private:
	std::list<GUIElement*> gui_elements;
	ImGuizmo::OPERATION gizmo_op = ImGuizmo::OPERATION::TRANSLATE;

	Shader* test;
};

#endif // !__MODULEGUI__

