#pragma once

#include <list>
#include "Globals.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "Module.h"
#include <time.h>
#include <vector>
#include "MathGeoLibFwd.h"
#include "MathGeoLib.h"

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class SceneManager;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModuleGUI;
class JSONManager;
class MeshImporter;
class MaterialManager;
class SceneImporter;
class ResourceManager;
class ShaderManager;

class Application
{
public:
	ModuleWindow* window;
	ModuleInput* input;
	ModuleAudio* audio;
	SceneManager* scene_manager;
	ModuleRenderer3D* renderer3D;
	ModuleCamera3D* camera;
	ModuleGUI* gui;
	JSONManager* json;
	MeshImporter* mesh;
	MaterialManager* materials;
	SceneImporter* scene_importer;
	ResourceManager* resources;
	ShaderManager* shader_manager;

	bool debug_mode = false;

private:

	std::list<Module*>	list_modules;

	bool				close_app = false;

	LCG					random_generator;

	std::string organization;

	//Congiguration
	char*						new_title;
	char*						new_org;
	int							new_fps = 0;
	std::vector<float>			fps_log;
	std::vector<float>			ms_log;
	std::vector<float>			mem_log;
	PerfTimer					logic_timer;

	// Engine debug info
	int							capped_ms = -1;
	PerfTimer					ptimer;
	Uint64						frame_count = 0;
	Timer						startup_time;
	Timer						frame_time;
	Timer						last_sec_frame_time;
	Uint32						last_sec_frame_count = 0;
	Uint32						prev_last_sec_frame_count = 0;
	float						dt = 0.0f;

	//Performance
	std::vector<float>			modules_update_time;
	std::vector<std::vector<float>> modules_perf_times;
	PerfTimer					module_time;

public:

	Application();
	~Application();

	bool Init();
	update_status Update();
	bool CleanUp();

	void CloseApp();

	float RandFloat();
	int RandRange(int min, int max);

	void ConfigGUI();
	void HardwareConfig();

	void OpenWebPage(const char* url);

	void DrawModulesConfig();
	void DrawPerformanceWindow();

	//Try to create path folder. Return true if created or already exist, false if not created or don't exist
	bool CreateFolder(const char* path) const;

	//Copy file to target folder
	bool CopyFileTo(const char* file, const char* target, std::string* new_path = nullptr);

	//fills paths vector with files found on path. if a extension is specified, returns files with that extension
	void GetFilesInPath(std::vector<std::string>& paths, const char * path, const char* extension = nullptr);


private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();

	void CapFPS(float fps);
	void FrameRateCalculations();

	JSONDoc* LoadConfig();
	JSONDoc* CreateDefaultConfig();
	void SaveConfig();
};

extern Application* App;