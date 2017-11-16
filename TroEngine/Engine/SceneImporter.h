#ifndef __SCENEIMPORTER__
#define __SCENEIMPORTER__

#include "Module.h"

class Scene;
class GameObject;
class JSONDoc;

class SceneImporter : public Module
{
public:
	SceneImporter(bool start_enabled = true);
	~SceneImporter();

	void SaveScene(Scene* scene, const char* path);
	void SaveScene(Scene* scene, JSONDoc* doc);
	void SaveScene(GameObject* go, const char* path);
	void SaveScene(GameObject* go, JSONDoc* doc);
	void SaveSceneToBuffer(Scene* scene);

	GameObject* LoadScene(const char* path);
	void LoadScene(const char* path, Scene* scene);
	void LoadSceneFromBuffer(Scene* scene);

private:
	JSONDoc* buffer = nullptr;
};

#endif