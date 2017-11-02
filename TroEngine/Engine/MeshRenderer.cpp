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
	if(mesh != nullptr)
		mesh->Render(wireframe);
}

void MeshRenderer::DebugDraw()
{
}

void MeshRenderer::CleanUp()
{
}

void MeshRenderer::SetMesh(Mesh * mesh)
{
	this->mesh = mesh;
}

void MeshRenderer::DrawConfig()
{
	if (ImGui::CollapsingHeader("MeshRenderer"))
	{
		ImGui::Checkbox("Wireframe##meshrenderer", &wireframe);

		if (mesh != nullptr)
		{
			ImGui::LabelText("Vertices##mesh", "%d", mesh->GetVerticesNum());
			ImGui::LabelText("Indices##mesh", "%d", mesh->GetIndicesNum());
		}
	}
}

void MeshRenderer::OnUpdateTransform()
{
	mesh->UpdateAABB(GetOwner()->GetTransform());
}

void MeshRenderer::RemoveMesh()
{
	mesh = nullptr;
}

AABB MeshRenderer::GetMeshAABB()
{
	return mesh->GetAABB();
}
