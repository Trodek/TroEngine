#include "SceneManager.h"
#include "Scene.h"
#include "Primitive.h"
#include "Application.h"
#include "MeshImporter.h"
#include "SceneImporter.h"

SceneManager::SceneManager(bool start_enabled)
{
	SetName("scenemanager");
}

SceneManager::~SceneManager()
{
}

bool SceneManager::Awake(JSONDoc * config)
{
	bool ret = true;

	//Create test scene 
	Scene* s = new Scene();
	curr_scene = s;
	AddScene(s);

	game_clock.Stop();

	return ret;
}

bool SceneManager::Start()
{
	bool ret = true;

	//Call all active scenes Start()
	for (std::list<Scene*>::iterator s = scenes.begin(); s != scenes.end(); ++s)
	{
		if ((*s)->IsActive()) 
			ret = (*s)->Start();
	}

	return ret;
}

update_status SceneManager::PreUpdate(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	
	if (load_scene)
	{
		App->scene_importer->LoadSceneFromBuffer(curr_scene);
		curr_scene->update_kd_tree = true;
		curr_scene->UpdateTransforms();
		load_scene = false;
	}

	bool scene_ret = true;
	for (std::list<Scene*>::iterator s = scenes.begin(); s != scenes.end(); ++s)
	{
		if ((*s)->IsActive())
			scene_ret = (*s)->PreUpdate();
	}

	if (scene_ret == false)
		ret = UPDATE_ERROR;

	return ret;
}

update_status SceneManager::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	if (game_dt == 0)
		game_dt = dt;

	if (play || do_tick)
	{
		if (paused)
			game_dt = 0.f;
		//Call all active scenes Start()
		for (std::list<Scene*>::iterator s = scenes.begin(); s != scenes.end(); ++s)
		{
			if ((*s)->IsActive())
				ret = (*s)->Update(game_dt);
		}

		//calculate game_dt
		game_dt = game_clock.ReadSec() - last_game_frame_time;
		last_game_frame_time = game_clock.ReadSec();
		
		if (do_tick)
		{
			do_tick = false;
			EDITOR_LOG("Game Logic has done 1 tick.");
		}
	}

	return ret;
}

bool SceneManager::CleanUp()
{
	bool ret = true;

	for (std::list<Scene*>::iterator s = scenes.begin(); s != scenes.end();)
	{
		ret = (*s)->CleanUp();
		RELEASE(*s);

		s = scenes.erase(s);
	}

	return ret;
}

Scene * SceneManager::GetCurrentScene() const
{
	return curr_scene;
}

void SceneManager::AddScene(Scene * scene)
{
	scenes.push_back(scene);
}

void SceneManager::DrawScenes()
{
	//For now, render a generic grid
	PPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	//Draw all active scenes
	for (std::list<Scene*>::iterator s = scenes.begin(); s != scenes.end(); ++s)
	{
		if ((*s)->IsActive())
			(*s)->Draw();
	}
}

void SceneManager::DebugDrawScenes()
{
	//Debug draw all active scenes
	for (std::list<Scene*>::iterator s = scenes.begin(); s != scenes.end(); ++s)
	{
		if ((*s)->IsActive())
			(*s)->DebugDraw();
	}
}

void SceneManager::ConfigGUI()
{
}

float SceneManager::ReadGameTimer() const
{
	return game_clock.ReadSec();
}

float SceneManager::GetGameTimerSpeed() const
{
	return game_clock.GetSpeed();
}

void SceneManager::SetGameTimerSpeed(float s)
{
	game_clock.SetSpeed(s);
}

void SceneManager::Play()
{
	if (!play)
	{
		play = true;
		game_clock.Start();
		App->scene_importer->SaveSceneToBuffer(GetCurrentScene());
	}
	else
	{
		play = false;
		paused = false;
		game_clock.Stop();
		load_scene = true;
	}
}

void SceneManager::Pause()
{
	if (play)
	{
		if (!paused)
		{
			paused = true;
			game_clock.PauseOn();
		}
		else
		{
			paused = false;
			game_clock.PauseOff();
		}
	}
}

void SceneManager::Tick()
{
	do_tick = true;
}
