#ifndef __MODULEGUI__
#define __MODULEGUI__

#include "Module.h"
#include "Globals.h"

class ModuleGUI : public Module
{
public:
	ModuleGUI(Application* app, bool start_enabled = true);
	~ModuleGUI();

	bool Init();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	bool CleanUp();
};

#endif // !__MODULEGUI__

