#ifndef __GAMEOBJECT__
#define __GAMEOBJECT__

#include "Globals.h"
#include <vector>
#include <string>
#include "Component.h"

class JSONDoc;

class GameObject
{
public:
	GameObject(const char* name, bool active = true, GameObject* parent = nullptr);
	GameObject(const char* name, uint UID, bool active = true, GameObject* parent = nullptr);
	~GameObject();

	void SetActive(bool active);
	bool IsActive()const;
	GameObject* GetParent() const;
	uint GetUID() const;
	void SetNewParent(GameObject* new_parent);
	void SetNewChildId(uint id);

	bool Start();
	bool PreUpdate();
	bool Update(float dt);
	void CleanUp();

	void Draw();
	void DebugDraw();

	void DrawConfig();

	void AddComponent(Component* component);
	Component* AddComponent(Component::Type type);

	Component* GetComponent(Component::Type type)const;
	bool HasComponent(Component::Type type)const;

	void RemoveComponent(Component* comp);
	void RemoveComponentsByType(Component::Type type);
	void RemoveAllComponents();

	void AddChild(GameObject* go);
	GameObject* CreateChild(const char* name);
	GameObject* CreateChild();
	GameObject* GetChild(uint id) const;
	void GetChildByUID(uint UID, GameObject* go)const;
	void GetAllChilds(std::vector<GameObject*>& go)const;
	void GetAllStaticChilds(std::vector<GameObject*>& go)const;
	void GetAllDynamicChilds(std::vector<GameObject*>& go)const;
	uint GetNumChilds() const;

	void RemoveChild(GameObject* child);

	void TransformUpdate();

	float4x4 GetTransform() const;

	void DrawHierarchy();

	bool IsStatic()const;
	void SetStatic(bool value);

	void Delete();

	void Serialize(JSONDoc* doc);

private:
	void SetChildrenStatic(bool is_static);
	void RemoveComponents();
	void RemoveChilds();

public:
	bool kill_me = false;
	std::string name;

private:
	std::vector<GameObject*> childs;
	std::vector<Component*> components;
	bool active = false;
	bool is_static = false;
	GameObject* parent = nullptr;

	bool change_static = false;

	uint new_child_id = 0;

	uint UID = 0;
};

#endif // !__GAMEOBJECT__

