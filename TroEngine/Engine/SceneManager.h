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
	update_status Update(float dt);
	bool CleanUp();

	void AddScene(Scene* scene);
	void DrawScenes();

	void ConfigGUI();

private:
	std::list<Scene*> scenes;
};

#endif // !__SCENEMANAGER__

