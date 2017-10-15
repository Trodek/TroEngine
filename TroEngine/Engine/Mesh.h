#ifndef __MESH__
#define __MESH__

#include "Globals.h"
#include "MathGeoLib.h"

class Mesh
{
public:
	Mesh(uint num_ver, float* ver, uint num_ind, uint* ind, uint num_uv = 0, float* uv = nullptr, uint tex_id = 0);

	uint GetIndicesID() const;
	uint GetIndicesNum() const;
	uint* GetIndices() const;

	uint GetVerticesID() const;
	uint GetVerticesNum() const;
	float* GetVertices() const;

	AABB GetAABB();

	void Render(bool wireframe = false);

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

	uint texture_id = 0;

	AABB bounding_box;
};


#endif // !__GEOMETRY__

