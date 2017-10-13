#include "SceneManager.h"
#include "Scene.h"
#include "Primitive.h"
#include "Application.h"
#include "MeshImporter.h"

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

update_status SceneManager::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	//Call all active scenes Start()
	for (std::list<Scene*>::iterator s = scenes.begin(); s != scenes.end(); ++s)
	{
		if ((*s)->IsActive())
			ret = (*s)->Update(dt);
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

void SceneManager::ConfigGUI()
{
}
