#ifndef __MESHRENDERER__
#define __MESHRENDERER__

#include "Component.h"
#include <list>

class Mesh;

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* owner);
	~MeshRenderer();

	void Draw();
	void DebugDraw();
	void CleanUp();

	void AddMesh(Mesh* mesh);

	void DrawConfig();

	void RemoveAllMeshes();

private:
	std::list<Mesh*> meshes;
	bool wireframe = false;

};

#endif // !__MESHRENDERER__

