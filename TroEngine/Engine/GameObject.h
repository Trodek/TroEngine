#ifndef __GAMEOBJECT__
#define __GAMEOBJECT__

#include "Globals.h"
#include <list>
#include <string>
#include "Component.h"

class GameObject
{
public:
	GameObject(const char* name, bool active = true, GameObject* parent = nullptr);
	~GameObject();

	void SetActive(bool active);
	GameObject* GetParent() const;
	void SetNewParent(GameObject* new_parent);

	bool Start();
	bool Update(float dt);
	void CleanUp();

	void Draw();
	void DebugDraw();

	void DrawConfig();

	void AddComponent(Component* component);
	Component* AddComponent(Component::Type type);

	Component* GetComponent(Component::Type type)const;

public:
	bool kill_me = false;
	std::string name;

private:
	std::list<GameObject*> childs;
	std::list<Component*> components;
	bool active = false;
	GameObject* parent;
};

#endif // !__GAMEOBJECT__

