#ifndef __MODULEGUI__
#define __MODULEGUI__

#include "Module.h"
#include "Globals.h"
#include <list>

class GUIElement;
class ConsoleGUI;
class Inspector;

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

private:
	std::list<GUIElement*> gui_elements;
};

#endif // !__MODULEGUI__

