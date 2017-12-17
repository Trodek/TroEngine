#ifndef __MESH__
#define __MESH__

#include "Globals.h"
#include "MathGeoLib.h"

enum Primitive
{
	P_NULL,
	P_BOX,
	P_PLANE,
	P_HDPLANE,
};

class Mesh
{
public:
	Mesh(uint num_ver, float* ver, uint num_ind, uint* ind);
	Mesh(uint uid, uint num_ver, float* ver, uint num_ind, uint* ind);

	uint GetIndicesID() const;
	uint GetIndicesNum() const;
	uint* GetIndices() const;

	uint GetVerticesID() const;
	uint GetVerticesNum() const;
	float* GetVertices() const;

	uint GetUID() const;

	void Render(bool wireframe = false);

	bool TestSegmentToTriangles(const LineSegment& segment, float& distance, float3& hit);

	void CleanUp();

private:
	void InitializeMesh();

private:
	uint id_indices = 0; // id in VRAM	
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // id in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;

	uint UID = 0;

	uint vao = 0;
};


#endif // !__GEOMETRY__

