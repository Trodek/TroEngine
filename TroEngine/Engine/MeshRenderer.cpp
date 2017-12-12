#include "MeshRenderer.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Application.h"
#include "MeshImporter.h"
#include "imgui.h"
#include "ModuleRenderer3D.h"
#include "JSONManager.h"
#include "ResourceManager.h"

MeshRenderer::MeshRenderer(GameObject* owner) : Component(Component::Type::C_MeshRenderer, owner)
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
	if (mesh != nullptr)
	{
		//float3 translation = GetMeshAABB().CenterPoint();
		//float3 scale = GetMeshAABB().Size();
		//Quat rot = Quat::identity;
		//float4x4 transform = float4x4::FromTRS(translation, rot, scale);
		//
		//App->renderer3D->PushMatrix();
		//App->renderer3D->MultMatrix(transform.Transposed().ptr());
		//
		//GLenum poly_mode = App->renderer3D->GetPolyMode();
		//App->renderer3D->PolygonModeWireframe();
		//
		//App->renderer3D->SetLineWidth(2.5f);
		//
		//App->mesh->cube->Render();
		//
		//App->renderer3D->SetLineWidth(1.0f);
		//
		//App->renderer3D->SetPolyMode(poly_mode);
		//
		//App->renderer3D->PopMatrix();
	}
}

void MeshRenderer::CleanUp()
{
}

void MeshRenderer::SetMesh(Mesh * mesh)
{
	this->mesh = mesh;
	bounding_box.SetNegativeInfinity();
	bounding_box.Enclose((float3*)mesh->GetVertices(), mesh->GetVerticesNum());
}

void MeshRenderer::SetMesh(uint uid)
{
	Resource* res = App->resources->GetResource(uid);
	mesh = App->mesh->GetMesh(res->manager_id);
}

void MeshRenderer::DrawConfig()
{
	if (ImGui::CollapsingHeader("MeshRenderer"))
	{
		if (ImGui::Button("Delete"))
			GetOwner()->RemoveComponent(this);

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
	UpdateAABB(GetOwner()->GetTransform());
}

void MeshRenderer::UpdateAABB(const float4x4 & trans)
{
	bounding_box.SetNegativeInfinity();
	bounding_box.Enclose((float3*)mesh->GetVertices(), mesh->GetVerticesNum());
	bounding_box.TransformAsAABB(trans);
}

void MeshRenderer::RemoveMesh()
{
	mesh = nullptr;
}

AABB MeshRenderer::GetMeshAABB()
{
	return bounding_box;
}

void MeshRenderer::Serialize(JSONDoc * doc)
{
	doc->AddSectionToArray("Components");
	doc->MoveToSectionFromArray("Components", doc->GetArraySize("Components") - 1);

	doc->SetNumber("type", GetType());
	doc->SetNumber("owner", GetOwner()->GetUID());
	if (mesh != nullptr)
		doc->SetNumber("mesh", mesh->GetUID());
	else
		doc->SetNumber("mesh", 0);
}

bool MeshRenderer::TestSegmentToMesh(LineSegment segment, float & distance, float3 & hit) const
{
	if (mesh != nullptr)
	{	
		//convert the segment to local space
		LineSegment segment_local(segment);
		segment_local.Transform(GetOwner()->GetTransform().Inverted());

		return mesh->TestSegmentToTriangles(segment_local, distance, hit);
	}
	else
		return false;
}
