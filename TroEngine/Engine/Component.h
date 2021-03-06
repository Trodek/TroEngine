#ifndef __COMPONENT__
#define __COMPONENT__

#include "MathGeoLib.h"

class GameObject;
class JSONDoc;

class Component
{
public:
	enum Type
	{
		Null,
		C_Transform,
		C_MeshRenderer,
		C_Material,
		Camera,
	};

public:
	Component(Type type, GameObject* owner);
	~Component();

	GameObject* GetOwner() const;
	Type GetType() const;
	void SetActive(bool active);
	bool IsActive() const;

	virtual void OnActivate() {}
	virtual void OnDeactivate() {}

	virtual bool Start() { return true; }
	virtual bool Update(float dt) { return true; }
	virtual void CleanUp() {}

	virtual void OnUpdateTransform() {}

	virtual void DebugDraw() {}

	virtual void DrawConfig() {}

	virtual void Serialize(JSONDoc* doc){}

public:
	bool kill_me = false;

private:
	Type type = Null;
	bool active = false;
	GameObject* owner = nullptr;

};

#endif // !__COMPONENT__
