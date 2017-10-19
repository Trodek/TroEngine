#ifndef __SCENE__
#define __SCENE__

#include "Globals.h"
#include "GameObject.h"
#include <vector>

class Scene
{
public:
	Scene(bool active = true) : active(active)
	{	}
	~Scene()
	{	}

	bool Start();

	void Draw();

	update_status Update(float dt);

	bool CleanUp();

	bool IsActive();

	void AddGameObject(GameObject* go);
	GameObject* CreateGameObject();

	GameObject* GetGameObject(uint id) const;

	void DrawHierarchy() const;

private:
	bool active;
	uint new_go_id = 0;

public:
	std::vector<GameObject*> game_objects;
};


#endif // !__SCENE__

