#ifndef __MODULEGUI__
#define __MODULEGUI__

#include "Module.h"
#include "Globals.h"

class ModuleGUI : public Module
{
public:
	ModuleGUI(bool start_enabled = true);
	~ModuleGUI();

	bool Awake();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();

	void SetGUIColors();
};

#endif // !__MODULEGUI__

