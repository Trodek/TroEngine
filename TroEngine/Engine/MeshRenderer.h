#ifndef __MESHRENDERER__
#define __MESHRENDERER__

#include "Globals.h"
#include "Component.h"
#include <list>
#include "MathGeoLib.h"
#include "Mesh.h"

class MeshRenderer : public Component
{
public:
	MeshRenderer(GameObject* owner);
	~MeshRenderer();

	void Draw();
	void DebugDraw();
	void CleanUp();

	void SetMesh(Mesh* mesh);
	void SetMesh(uint uid);

	void DrawConfig();
	void OnUpdateTransform();

	void RemoveMesh();

	AABB GetMeshAABB();

	void Serialize(JSONDoc* doc);

	bool TestSegmentToMesh(LineSegment segment, float& distance, float3& hit) const;

private:
	void UpdateAABB(const float4x4 & trans);

private:
	Mesh* mesh = nullptr;
	bool wireframe = false;
	AABB bounding_box;

public:
	bool primitive = false;
	Primitive p_type = P_NULL;

};

#endif // !__MESHRENDERER__

