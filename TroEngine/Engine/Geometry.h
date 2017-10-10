#ifndef __GEOMETRY__
#define __GEOMETRY__

#include "Globals.h"

class Geometry
{
public:
	Geometry(uint num_ver, float* ver, uint num_ind, uint* ind);

	uint GetIndicesID() const;
	uint GetIndicesNum() const;
	uint* GetIndices() const;

	uint GetVerticesID() const;
	uint GetVerticesNum() const;
	float* GetVertices() const;

	void Render();

	void CleanUp();

private:
	uint id_indices = 0; // id in VRAM	
	uint num_indices = 0;
	uint* indices = nullptr;

	uint id_vertices = 0; // id in VRAM
	uint num_vertices = 0;
	float* vertices = nullptr;
};


#endif // !__GEOMETRY__

