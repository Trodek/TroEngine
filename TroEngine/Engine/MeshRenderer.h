#ifndef __MESHRENDERER__
#define __MESHRENDERER__

#include "Globals.h"
#include "Component.h"
#include <list>
#include "MathGeoLib.h"

class Mesh;

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* owner);
	~MeshRenderer();

	void Draw();
	void DebugDraw();
	void CleanUp();

	void SetMesh(Mesh* mesh);

	void DrawConfig();
	void OnUpdateTransform();

	void RemoveMesh();

	AABB GetMeshAABB();

	void Serialize(JSONDoc* doc);

private:
	Mesh* mesh = nullptr;
	bool wireframe = false;

};

#endif // !__MESHRENDERER__

