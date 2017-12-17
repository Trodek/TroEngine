#include "Mesh.h"
#include "GLInclude.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "UID.h"

Mesh::Mesh(uint num_ver, float * ver, uint num_ind, uint * ind) :
	 num_indices(num_ind), indices(ind), num_vertices(num_ver), vertices(ver)
{
	//Load vertices to vram
	id_vertices = App->renderer3D->GenBuffer();
	App->renderer3D->BindArrayBuffer(id_vertices);
	App->renderer3D->LoadArrayToVRAM(sizeof(float) * num_vertices * 13, ver, GL_STATIC_DRAW);
	App->renderer3D->UnbindArraybuffer();

	//Load indices to vram
	id_indices = App->renderer3D->GenBuffer();
	App->renderer3D->BindArrayBuffer(id_indices);
	App->renderer3D->LoadArrayToVRAM(sizeof(uint) * num_indices, ind, GL_STATIC_DRAW);
	App->renderer3D->UnbindArraybuffer();

	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);

	uint* uid = md5(data, size);
	UID = *uid;
	RELEASE_ARRAY(data);

	InitializeMesh();
}

Mesh::Mesh(uint uid, uint num_ver, float * ver, uint num_ind, uint * ind) :
	num_indices(num_ind), indices(ind), num_vertices(num_ver), vertices(ver), UID(uid)
{
	//Load vertices to vram
	id_vertices = App->renderer3D->GenBuffer();
	App->renderer3D->BindArrayBuffer(id_vertices);
	App->renderer3D->LoadArrayToVRAM(sizeof(float) * num_vertices * 13, ver, GL_STATIC_DRAW);
	App->renderer3D->UnbindArraybuffer();

	//Load indices to vram
	id_indices = App->renderer3D->GenBuffer();
	App->renderer3D->BindArrayBuffer(id_indices);
	App->renderer3D->LoadArrayToVRAM(sizeof(uint) * num_indices, ind, GL_STATIC_DRAW);
	App->renderer3D->UnbindArraybuffer();

	InitializeMesh();
}

uint Mesh::GetIndicesID() const
{
	return id_indices;
}

uint Mesh::GetIndicesNum() const
{
	return num_indices;
}

uint * Mesh::GetIndices() const
{
	return indices;
}

uint Mesh::GetVerticesID() const
{
	return id_vertices;
}

uint Mesh::GetVerticesNum() const
{
	return num_vertices;
}

float * Mesh::GetVertices() const
{
	return vertices;
}

uint Mesh::GetUID() const
{
	return UID;
}

void Mesh::Render(bool wireframe)
{
	//use this mesh vertex array object
	App->renderer3D->BindVertexArrayBuffer(vao);
	
	//Set Wireframe if needed
	GLenum curr_mode = App->renderer3D->GetPolyMode();
	if (wireframe && curr_mode != GL_LINE)
	{
		App->renderer3D->PolygonModeWireframe();
	}

	App->renderer3D->RenderElement(num_indices);

	if (wireframe)
	{
		//Set curr_mode again
		if (curr_mode == GL_FILL)
			App->renderer3D->PolygonModeFill();
		else if (curr_mode == GL_POINT)
			App->renderer3D->PolygonModePoints();
	}
	
	App->renderer3D->UnbindVertexArrayBuffer();
}

bool Mesh::TestSegmentToTriangles(const LineSegment & segment, float & distance, float3 & hit)
{
	bool ret = false;
	distance = 9999999999999.f;

	for (int i = 0; i < num_indices; i += 3)
	{
		Triangle tri;
		tri.a.Set(vertices[(3 * indices[i])], vertices[(3 * indices[i] + 1)], vertices[(3 * indices[i] + 2)]);
		tri.b.Set(vertices[(3 * indices[i + 1])], vertices[(3 * indices[i + 1] + 1)], vertices[(3 * indices[i + 1] + 2)]);
		tri.c.Set(vertices[(3 * indices[i + 2])], vertices[(3 * indices[i + 2] + 1)], vertices[(3 * indices[i + 2] + 2)]);

		float dist;
		float3 hit_point;
		if (segment.Intersects(tri, &dist, &hit_point))
		{
			ret = true;
			if (dist < distance)
			{
				distance = dist;
				hit = hit_point;
			}
		}
	}

	return ret;
}

void Mesh::CleanUp()
{
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);
}

void Mesh::InitializeMesh()
{
	vao = App->renderer3D->GenVertexArrayBuffer();

	App->renderer3D->BindVertexArrayBuffer(vao);

	App->renderer3D->BindArrayBuffer(id_vertices);

	//vertices
	App->renderer3D->SetVertexAttributePointer(0, 3, 13, 0);
	App->renderer3D->EnableVertexAttributeArray(0);
	//texture coords
	App->renderer3D->SetVertexAttributePointer(1, 3, 13, 3);
	App->renderer3D->EnableVertexAttributeArray(1);
	//normals
	App->renderer3D->SetVertexAttributePointer(2, 3, 13, 6);
	App->renderer3D->EnableVertexAttributeArray(2);
	//colors
	App->renderer3D->SetVertexAttributePointer(3, 4, 13, 9);
	App->renderer3D->EnableVertexAttributeArray(3);

	App->renderer3D->BindElementArrayBuffer(id_indices);

	App->renderer3D->UnbindVertexArrayBuffer();

}
