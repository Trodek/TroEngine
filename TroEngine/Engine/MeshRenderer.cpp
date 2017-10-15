#include "MeshRenderer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Application.h"
#include "MeshImporter.h"
#include "imgui.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(Component::Type::MeshRenderer,owner)
{
}

MeshRenderer::~MeshRenderer()
{
}

void MeshRenderer::Draw()
{
	for (std::list<Mesh*>::iterator m = meshes.begin(); m != meshes.end(); ++m)
	{
		(*m)->Render(wireframe);
	}
}

void MeshRenderer::DebugDraw()
{
}

void MeshRenderer::CleanUp()
{
}

void MeshRenderer::AddMesh(Mesh * mesh)
{
	meshes.push_back(mesh);
}

void MeshRenderer::DrawConfig()
{
	if (ImGui::CollapsingHeader("MeshRenderer"))
	{
		ImGui::Checkbox("Wireframe##meshrenderer", &wireframe);

		int i = 1;
		for (std::list<Mesh*>::iterator m = meshes.begin(); m != meshes.end(); ++m)
		{
			char title[25];
			sprintf_s(title, 25, "Mesh %d##meshrenderer", i);
			if (ImGui::CollapsingHeader(title))
			{
				char name[25];
				sprintf_s(name, 25, "Vertices##mesh%d", i);
				ImGui::LabelText(name, "%d", (*m)->GetVerticesNum());
				sprintf_s(name, 25, "Indices##mesh%d", i);
				ImGui::LabelText(name, "%d", (*m)->GetIndicesNum());
			}
			++i;
		}
	}
}

void MeshRenderer::RemoveAllMeshes()
{
	for (std::list<Mesh*>::iterator m = meshes.begin(); m != meshes.end();)
	{
		App->mesh->RemoveMesh(*m);

		m = meshes.erase(m);
	}
}

AABB MeshRenderer::GetMeshAABB(uint mesh_id)
{
	int i = 1;
	for (std::list<Mesh*>::const_iterator m = meshes.begin(); m != meshes.end(); ++m)
	{
		if (i == mesh_id)
		{
			return (*m)->GetAABB();
		}
		++i;
	}
}
