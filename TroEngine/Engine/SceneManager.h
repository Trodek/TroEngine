#ifndef __SCENEMANAGER__
#define __SCENEMANAGER__

#include "Module.h"
#include <list>

class Scene;

class SceneManager :public Module
{
public:
	SceneManager(bool start_enabled = true);
	~SceneManager();

	bool Awake(JSONDoc* config);
	bool Start();
	update_status PreUpdate(float dt);
	update_status Update(float dt);
	bool CleanUp();

	Scene* GetCurrentScene()const;

	void AddScene(Scene* scene);
	void DrawScenes();
	void DebugDrawScenes();

	void ConfigGUI();

private:
	std::list<Scene*> scenes;
	Scene* curr_scene = nullptr;
};

#endif // !__SCENEMANAGER__

