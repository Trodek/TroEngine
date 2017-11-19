#include "Mesh.h"
#include "GLInclude.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "UID.h"

Mesh::Mesh(uint num_ver, float * ver, uint num_ind, uint * ind, uint num_uv, float* uv, uint num_norm, float* norm) :
	 num_indices(num_ind), indices(ind), num_vertices(num_ver), vertices(ver), num_uv(num_uv), uv(uv), num_normals(num_norm), normals(norm)
{
	//Load vertices to vram
	id_vertices = App->renderer3D->GenBuffer();
	App->renderer3D->BindArrayBuffer(id_vertices);
	App->renderer3D->LoadArrayToVRAM(sizeof(float) * num_vertices * 3, ver, GL_STATIC_DRAW);
	App->renderer3D->UnbindArraybuffer();

	//Load indices to vram
	id_indices = App->renderer3D->GenBuffer();
	App->renderer3D->BindArrayBuffer(id_indices);
	App->renderer3D->LoadArrayToVRAM(sizeof(uint) * num_indices, ind, GL_STATIC_DRAW);
	App->renderer3D->UnbindArraybuffer();


	//Load uv to vram
	if (uv != nullptr)
	{
		id_uv = App->renderer3D->GenBuffer();
		App->renderer3D->BindArrayBuffer(id_uv);
		App->renderer3D->LoadArrayToVRAM(sizeof(float) * num_uv * 3, uv, GL_STATIC_DRAW);
		App->renderer3D->UnbindArraybuffer();
	}

	//Load normals to vram
	if (norm != nullptr)
	{
		id_normals = App->renderer3D->GenBuffer();
		App->renderer3D->BindArrayBuffer(id_normals);
		App->renderer3D->LoadArrayToVRAM(sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);
		App->renderer3D->UnbindArraybuffer();
	}

	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);

	uint* uid = md5(data, size);
	UID = *uid;
	RELEASE_ARRAY(data);
}

Mesh::Mesh(uint uid, uint num_ver, float * ver, uint num_ind, uint * ind, uint num_uv, float * uv, uint num_norm, float * norm) :
	num_indices(num_ind), indices(ind), num_vertices(num_ver), vertices(ver), num_uv(num_uv), uv(uv), num_normals(num_norm), normals(norm), UID(uid)
{
	//Load vertices to vram
	id_vertices = App->renderer3D->GenBuffer();
	App->renderer3D->BindArrayBuffer(id_vertices);
	App->renderer3D->LoadArrayToVRAM(sizeof(float) * num_vertices * 3, ver, GL_STATIC_DRAW);
	App->renderer3D->UnbindArraybuffer();

	//Load indices to vram
	id_indices = App->renderer3D->GenBuffer();
	App->renderer3D->BindArrayBuffer(id_indices);
	App->renderer3D->LoadArrayToVRAM(sizeof(uint) * num_indices, ind, GL_STATIC_DRAW);
	App->renderer3D->UnbindArraybuffer();


	//Load uv to vram
	if (uv != nullptr)
	{
		id_uv = App->renderer3D->GenBuffer();
		App->renderer3D->BindArrayBuffer(id_uv);
		App->renderer3D->LoadArrayToVRAM(sizeof(float) * num_uv * 3, uv, GL_STATIC_DRAW);
		App->renderer3D->UnbindArraybuffer();
	}

	//Load normals to vram
	if (norm != nullptr)
	{
		id_normals = App->renderer3D->GenBuffer();
		App->renderer3D->BindArrayBuffer(id_normals);
		App->renderer3D->LoadArrayToVRAM(sizeof(float) * num_normals * 3, normals, GL_STATIC_DRAW);
		App->renderer3D->UnbindArraybuffer();
	}
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

uint Mesh::GetUVNum()
{
	return num_uv;
}

uint Mesh::GetUVID()
{
	return id_uv;
}

float * Mesh::GetUV()
{
	return uv;
}

uint Mesh::GetUID() const
{
	return UID;
}

void Mesh::Render(bool wireframe)
{
	App->renderer3D->EnableState(GL_VERTEX_ARRAY);

	App->renderer3D->BindArrayBuffer(id_vertices);
	App->renderer3D->SetVertexPointer();
	App->renderer3D->BindElementArrayBuffer(id_indices);

	//Apply UV if exist
	if (num_uv != 0)
	{
		App->renderer3D->EnableState(GL_TEXTURE_COORD_ARRAY);
		App->renderer3D->BindArrayBuffer(id_uv);
		App->renderer3D->SetTexCoordPointer();
	}

	//Apply Normals if exist
	if (num_normals != 0)
	{
		App->renderer3D->EnableState(GL_NORMAL_ARRAY);
		App->renderer3D->BindArrayBuffer(id_normals);
		App->renderer3D->SetNormalsPointer();
	}

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

	App->renderer3D->UnbindArraybuffer();
	App->renderer3D->UnbindElementArrayBuffer();

	App->renderer3D->DisableState(GL_VERTEX_ARRAY);
	App->renderer3D->DisableState(GL_TEXTURE_COORD_ARRAY);
	
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
	RELEASE_ARRAY(uv);
	RELEASE_ARRAY(normals);
}