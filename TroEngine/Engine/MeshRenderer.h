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

	void RemoveMesh();

	AABB GetMeshAABB();

private:
	Mesh* mesh;
	bool wireframe = false;

};

#endif // !__MESHRENDERER__

