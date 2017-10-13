#ifndef __SCENE__
#define __SCENE__

#include "Globals.h"
#include "GameObject.h"

class Scene
{
public:
	Scene(bool active = true) : active(active)
	{	}
	~Scene()
	{	}

	virtual bool Start()
	{
		return true;
	}

	virtual void Draw()
	{	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	bool IsActive()
	{
		return active;
	}

	void AddGameObject(GameObject* go)
	{
		game_objects.push_back(go);
	}

	GameObject* GetGameObject(uint id) const
	{
		GameObject* ret = nullptr;
		uint i = 0;
		for (std::list<GameObject*>::const_iterator go = game_objects.begin(); go != game_objects.end(); ++go)
		{
			if (i == id)
			{
				ret = (*go);
				break;
			}
			++i;
		}
		return ret;
	}

private:
	bool active;

public:
	std::list<GameObject*> game_objects;
};


#endif // !__SCENE__

