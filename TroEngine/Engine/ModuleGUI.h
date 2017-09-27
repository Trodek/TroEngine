#ifndef __MODULEGUI__
#define __MODULEGUI__

#include "Module.h"
#include "Globals.h"

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Awake(JSONDoc* config);
	update_status PreUpdate(float dt);
	bool CleanUp();

	void RenderGUI();
	void SetGUIColors(float _alpha = 1.0f);
};

#endif // !__MODULEGUI__

