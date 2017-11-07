#ifndef __SCENE__
#define __SCENE__

#include "Globals.h"
#include "GameObject.h"
#include <vector>

class KDTree;

class Scene
{
public:
	Scene(bool active = true) : active(active)
	{	}
	~Scene()
	{	}

	bool Start();

	void Draw();
	void DebugDraw();

	update_status Update(float dt);

	bool CleanUp();

	bool IsActive();

	void AddGameObject(GameObject* go);
	GameObject* CreateGameObject();
	GameObject* CreateGameObject(const char* name);

	GameObject* GetGameObject(uint id) const;

	void DrawHierarchy() const;

	void CreateTree() const;

private:
	bool active;
	uint new_go_id = 0;
	KDTree* kd_tree = nullptr;

public:
	std::vector<GameObject*> game_objects;
};


#endif // !__SCENE__

