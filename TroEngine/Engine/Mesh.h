#ifndef __MESH__
#define __MESH__

#include "Globals.h"
#include "MathGeoLib.h"

class Mesh
{
public:
	Mesh(uint num_ver, float* ver, uint num_ind, uint* ind, uint num_uv = 0, float* uv = nullptr, uint num_norm = 0, float* norm = nullptr);

	uint GetIndicesID() const;
	uint GetIndicesNum() const;
	uint* GetIndices() const;

	uint GetVerticesID() const;
	uint GetVerticesNum() const;
	float* GetVertices() const;

	uint GetUVNum();
	uint GetUVID();
	float* GetUV();

	AABB GetAABB() const;
	void UpdateAABB(const float4x4& trans);

	uint GetUID() const;

	void Render(bool wireframe = false);

	bool TestSegmentToTriangles(const LineSegment& segment, float& distance, float3& hit);

	void CleanUp();

private:
	uint id_indices = 0; // id in VRAM	
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // id in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint id_uv = 0; // id in VRAM
	uint num_uv = 0;
	float* uv = nullptr;

	uint id_normals = 0; //id in VRAM
	uint num_normals = 0;
	float* normals;

	AABB bounding_box;

	uint UID = 0;
};


#endif // !__GEOMETRY__

