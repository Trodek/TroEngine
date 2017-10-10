#include "Geometry.h"
#include "GLInclude.h"

Geometry::Geometry(uint num_ver, float * ver, uint num_ind, uint * ind) :
	 num_indices(num_ind), indices(ind), num_vertices(num_ver), vertices(ver)
{
	//Load vertices to vram
	glGenBuffers(1, (GLuint*)&id_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_vertices * 3, ver, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Load indices to vram
	glGenBuffers(1, (GLuint*)&id_indices);
	glBindBuffer(GL_ARRAY_BUFFER, id_indices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * num_indices, ind, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

uint Geometry::GetIndicesID() const
{
	return id_indices;
}

uint Geometry::GetIndicesNum() const
{
	return num_indices;
}

uint * Geometry::GetIndices() const
{
	return indices;
}

uint Geometry::GetVerticesID() const
{
	return id_vertices;
}

uint Geometry::GetVerticesNum() const
{
	return num_vertices;
}

float * Geometry::GetVertices() const
{
	return vertices;
}

void Geometry::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, id_vertices);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_indices);
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void Geometry::CleanUp()
{
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);
}


