#include "MeshRenderer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Application.h"
#include "MeshImporter.h"
#include "imgui.h"
#include "ModuleRenderer3D.h"

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
	float3 translation = GetMeshAABB().CenterPoint();
	float3 scale = GetMeshAABB().Size();
	Quat rot = Quat::identity;
	float4x4 transform = float4x4::FromTRS(translation, rot, scale);

	App->renderer3D->PushMatrix();
	App->renderer3D->MultMatrix(transform.Transposed().ptr());

	GLenum poly_mode = App->renderer3D->GetPolyMode();
	App->renderer3D->PolygonModeWireframe();

	App->renderer3D->SetLineWidth(2.5f);

	App->mesh->cube->Render();

	App->renderer3D->SetLineWidth(1.0f);

	App->renderer3D->SetPolyMode(poly_mode);

	App->renderer3D->PopMatrix();
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
