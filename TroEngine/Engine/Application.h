#pragma once

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "Module.h"

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class EditorScene;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGUI;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	EditorScene* scene_intro;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGUI* gui;

private:

	Timer	ms_timer;
	float	dt;
	std::list<Module*> list_modules;

	bool	close_app = false;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void CloseApp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};

extern Application* App;