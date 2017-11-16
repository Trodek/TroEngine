#ifndef __SCENEMANAGER__
#define __SCENEMANAGER__

#include "Module.h"
#include <list>
#include "Timer.h"

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

	float ReadGameTimer()const;

	void Play();
	void Pause();
	void Tick();

private:
	std::list<Scene*> scenes;
	Scene* curr_scene = nullptr;

	bool play = false;
	bool paused = false;
	bool do_tick = false;
	bool load_scene = false;

	Timer game_clock;
	float last_game_frame_time = 0.f;
	float game_dt = 0.f;
};

#endif // !__SCENEMANAGER__

