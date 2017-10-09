#include "Gizmo.h"

#define M_PI 3.14159
#define M_PI_2 M_PI/2

// ------------------
// CUBE Vertex Array
// ------------------
void GCubeVA::Start()
{
	glGenBuffers(1, (GLuint*)&mem_id);
	glBindBuffer(GL_ARRAY_BUFFER, mem_id);

	float ver[36 * 3] =
	{ 0.f, 0.f, 0.f,
		5.f, 0.f, 5.f,
		0.f, 0.f, 5.f,
		0.f, 0.f, 0.f,
		5.f, 0.f, 0.f,
		5.f, 0.f, 5.f,
		0.f, 5.f, 5.f,
		0.f, 0.f, 5.f,
		5.f, 0.f, 5.f,
		0.f, 5.f, 5.f,
		5.f, 0.f, 5.f,
		5.f, 5.f, 5.f,
		5.f, 5.f, 5.f,
		5.f, 0.f, 5.f,
		5.f, 0.f, 0.f,
		5.f, 5.f, 5.f,
		5.f, 0.f, 0.f,
		5.f, 5.f, 0.f,
		5.f, 5.f, 0.f,
		5.f, 0.f, 0.f,
		0.f, 0.f, 0.f,
		5.f, 5.f, 0.f,
		0.f, 0.f, 0.f,
		0.f, 5.f, 0.f,
		0.f, 5.f, 0.f,
		0.f, 0.f, 0.f,
		0.f, 0.f, 5.f,
		0.f, 5.f, 5.f,
		0.f, 5.f, 0.f,
		0.f, 0.f, 5.f,
		0.f, 5.f, 0.f,
		0.f, 5.f, 5.f,
		5.f, 5.f, 5.f,
		0.f, 5.f, 0.f,
		5.f, 5.f, 5.f,
		5.f, 5.f, 0.f };

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, ver, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void GCubeVA::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mem_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 36 * 3);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ------------------
// CUBE Indices Draw Element
// ------------------
void GCubeIE::Start()
{
	glGenBuffers(1, (GLuint*)&mem_id);
	glBindBuffer(GL_ARRAY_BUFFER, mem_id);

	float ver[8 * 3] =
	{
		15.f,5.f,5.f,
		10.f,5.f,5.f,
		10.f,0.f,5.f,
		15.f,0.f,5.f,
		15.f,0.f,0.f,
		15.f,5.f,0.f,
		10.f,5.f,0.f,
		10.f,0.f,0.f
	};

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * 3, ver, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}

void GCubeIE::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, mem_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	GLubyte indices[] = 
	  { 0,1,2, 2,3,0,
		0,3,4, 4,5,0,
		0,5,6, 6,1,0,
		1,6,7, 7,2,1,
		7,4,3, 3,2,7,
		4,7,6, 6,5,4 };

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);

	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// ------------------
//		Sphere
// ------------------
void GSphere::Start(float _radius, uint _rings, uint _sectors)
{
	radius = _radius;
	rings = _rings;
	sectors = _sectors;

	float const R = 1. / (float)(rings - 1);
	float const S = 1. / (float)(sectors - 1);
	int r, s;

	vertices.resize(rings * sectors * 3);
	normals.resize(rings * sectors * 3);
	std::vector<GLfloat>::iterator v = vertices.begin();
	std::vector<GLfloat>::iterator n = normals.begin();
	for (r = 0; r < rings; r++) 
	{
		for (s = 0; s < sectors; s++) 
		{
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			*v++ = x * radius;
			*v++ = z * radius;
			*v++ = y * radius;

			*n++ = x;
			*n++ = z;
			*n++ = y;
		}
	}

	indices.resize(rings * sectors * 4);
	std::vector<GLushort>::iterator i = indices.begin();
	for (r = 0; r < rings - 1; r++) for (s = 0; s < sectors - 1; s++) {
		*i++ = r * sectors + s;
		*i++ = r * sectors + (s + 1);
		*i++ = (r + 1) * sectors + (s + 1);
		*i++ = (r + 1) * sectors + s;
	}

}

void GSphere::Render()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
	glNormalPointer(GL_FLOAT, 0, &normals[0]);
	glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);

}

// ------------------
//		Cylinder
// ------------------
void GCylinder::Start(int slices, float r, float h)
{
	float angle = 0;
	float delta_angle = 2 * M_PI / slices;

	vertices.push_back(0.f);
	vertices.push_back(h / 2);
	vertices.push_back(0.f);

	//top circle


}

void GCylinder::Render()
{
}
