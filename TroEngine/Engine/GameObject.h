#ifndef __GAMEOBJECT__
#define __GAMEOBJECT__

#include "Globals.h"
#include <vector>
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

	void RemoveComponent(Component* comp);
	void RemoveComponentsByType(Component::Type type);
	void RemoveAllComponents();

	GameObject* CreateChild(const char* name);
	GameObject* GetChild(uint id) const;
	uint GetNumChilds() const;

	void DrawHierarchy();

public:
	bool kill_me = false;
	std::string name;

private:
	std::vector<GameObject*> childs;
	std::vector<Component*> components;
	bool active = false;
	GameObject* parent = nullptr;
};

#endif // !__GAMEOBJECT__

