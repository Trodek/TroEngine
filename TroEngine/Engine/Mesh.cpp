#include "Mesh.h"
#include "GLInclude.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

Mesh::Mesh(uint num_ver, float * ver, uint num_ind, uint * ind, uint num_uv, float* uv, uint tex_id) :
	 num_indices(num_ind), indices(ind), num_vertices(num_ver), vertices(ver), num_uv(num_uv), uv(uv), texture_id(tex_id)
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

void Mesh::CleanUp()
{
	RELEASE_ARRAY(vertices);
	RELEASE_ARRAY(indices);
	RELEASE_ARRAY(uv);
}