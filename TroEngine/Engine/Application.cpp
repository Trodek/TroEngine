#include "Application.h"
#include "Brofiler/Brofiler.h"

#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "EditorScene.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleGUI.h"
#include "EditorConsole.h"
#include "EditorGUI.h"

#include "Algorithm\Random\LCG.h"
#include "imgui.h"

Application::Application()
{
	window = new ModuleWindow();
	input = new ModuleInput();
	audio = new ModuleAudio();
	scene_intro = new EditorScene();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	gui = new ModuleGUI();
	console = new EditorConsole();
	editor_gui = new EditorGUI();

	// The order of calls is very important!
	// Modules will Awake() Start() and Update() in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(window);
	AddModule(gui);
	AddModule(editor_gui);
	AddModule(console);
	AddModule(camera);
	AddModule(input);
	AddModule(audio);
	
	// Scenes
	AddModule(scene_intro);

	// Renderer last!
	AddModule(renderer3D);

	organization = "UPC CITM";

	new_title = new char[150];
	std::strcpy(new_title, window->title.c_str());

	new_org = new char[150];
	std::strcpy(new_org, organization.c_str());
}

Application::~Application()
{
	std::list<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end())
	{
		delete *item;
		++item;
	}

	list_modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	PERF_START(ptimer);

	// Call Awake() in all modules
	std::list<Module*>::iterator item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Awake();
		++item;
	}

	// After all Awake calls we call Start() in all modules
	EDITOR_LOG("Application Start --------------");
	item = list_modules.begin();

	while(item != list_modules.end() && ret == true)
	{
		ret = (*item)->Start();
		++item;
	}
	
	PERF_PEEK(ptimer);

	return ret;
}

// ---------------------------------------------
void Application::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void Application::FinishUpdate()
{
	FrameRateCalculations();
}

void Application::CapFPS(float fps)
{
	if (fps > 0)
	{
		capped_ms = (1000 / fps);
		EDITOR_LOG("FPS capped to %.2f", fps);
	}
	else if (fps == 0)
	{
		capped_ms = 0;
	}
}

void Application::FrameRateCalculations()
{
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	Uint32 last_frame_ms = frame_time.Read();
	Uint32 frames_on_last_update = prev_last_sec_frame_count;

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		SDL_Delay(capped_ms - last_frame_ms);
	}

	fps_log.push_back(frames_on_last_update);

	if (fps_log.size() > 75)
		fps_log.erase(fps_log.begin());

	if (ms_log.size() > 75)
		ms_log.erase(ms_log.begin());
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{
	BROFILER_CATEGORY("UpdateLogic", Profiler::Color::Azure);

	update_status ret = UPDATE_CONTINUE;

	logic_timer.Start();

	PrepareUpdate();

	std::list<Module*>::iterator item = list_modules.begin();
	
	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PreUpdate(dt);
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->Update(dt);
		++item;
	}

	item = list_modules.begin();

	while(item != list_modules.end() && ret == UPDATE_CONTINUE)
	{
		ret = (*item)->PostUpdate(dt);
		++item;
	}

	ms_log.push_back(logic_timer.ReadMs());

	FinishUpdate();

	if (close_app)
	{
		ret = UPDATE_STOP;
	}

	return ret;
}

bool Application::CleanUp()
{
	PERF_START(ptimer);

	bool ret = true;

	for (std::list<Module*>::iterator it = list_modules.begin(); it != list_modules.end() && ret == true; ++it)
	{
		ret = (*it)->CleanUp();
	}

	return ret;

	PERF_PEEK(ptimer);
}

void Application::CloseApp()
{
	close_app = true;
}

float Application::RandFloat()
{ 
	return random_generator.FloatIncl(0.0f, 1.0f);
}

int Application::RandRange(int min, int max)
{
	return random_generator.Int(min, max);
}

void Application::ConfigGUI()
{
	if (ImGui::CollapsingHeader("Application"))
	{
		if (ImGui::InputText("App Name", new_title, 150))
		{
			if (App->window->title != new_title)
			{
				window->SetTitle(new_title);
			}
		}

		if (ImGui::InputText("Organization", new_org, 150))
		{
			if (App->organization != new_org)
			{
				organization = new_org;
			}
		}

		if (ImGui::SliderInt("Max FPS", &new_fps, 0, 200))
		{
			CapFPS(new_fps);
		}

		char title[25];
		sprintf_s(title, 25, "FPS: %.1f", fps_log[fps_log.size() - 1]);
		ImGui::PlotHistogram("##fps", &fps_log[0], fps_log.size(), 0, title, 0.0f, 100.0f, ImVec2(310, 100));
		sprintf_s(title, 25, "Logic ms: %.1f", ms_log[ms_log.size() - 1]);
		ImGui::PlotHistogram("##logicms", &ms_log[0], ms_log.size(), 0, title, 0.0f, 40.0f, ImVec2(310, 100));
	}
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}