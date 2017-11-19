#ifndef __SCENE__
#define __SCENE__

#include "Globals.h"
#include "GameObject.h"
#include <vector>

class KDTree;
class JSONDoc;

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

	bool PreUpdate();
	update_status Update(float dt);

	bool CleanUp();

	bool IsActive();

	void AddGameObject(GameObject* go);
	GameObject* CreateGameObject();
	GameObject* CreateGameObject(const char* name);

	GameObject* GetGameObject(uint id) const;
	GameObject* GetGameObjectByUID(uint uid)const;
	void GetAllStaticGameObjects(std::vector<GameObject*>& vector_to_fill)const;
	void GetAllDynamicGameObjects(std::vector<GameObject*>& vector_to_fill)const;
	void GetAllGameObjects(std::vector<GameObject*>& vector_to_fill)const;

	void TestToKDTree(const Ray& ray, float p1_distance, float p2_distance, std::vector<GameObject*>& vec_to_fill) const;
	void TestToKDTree(const AABB& box, std::vector<GameObject*>& vec_to_fill)const;
	void TestToKDTree(const LineSegment& segment, std::vector<GameObject*>& vec_to_fill)const;
	void TestToKDTree(const Frustum& frustum, std::vector<GameObject*>& vec_to_fill)const;

	void DrawHierarchy() const;

	void CreateTree() const;

	void SerializeScene(JSONDoc* doc);

	void UpdateTransforms()const;

private:
	bool active;
	uint new_go_id = 0;
	KDTree* kd_tree = nullptr;

public:
	std::vector<GameObject*> game_objects;

	bool update_kd_tree = false;
};


#endif // !__SCENE__

