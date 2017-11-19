#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Material.h"
#include "MaterialManager.h"
#include "GameObject.h"
#include "imgui.h"
#include "JSONManager.h"
#include "ResourceManager.h"

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(C_Material, owner)
{
}

ComponentMaterial::ComponentMaterial(GameObject * owner, Material * mat) : Component(C_Material, owner), material(mat)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::CleanUp()
{
	material = nullptr;
}

void ComponentMaterial::SetMaterial(Material * new_mat)
{
	if (material != nullptr) //just for now
		CleanUp();
	material = new_mat;
}

void ComponentMaterial::SetMaterial(uint mat_id)
{
	if (mat_id != 0) //invalid id
	{
		Resource* res = App->resources->GetResource(mat_id);
		if(res!=nullptr)
			material = App->materials->GetMaterial(res->manager_id);
	}
}

void ComponentMaterial::ApplyMaterial() const
{
	if (use_checker)
		App->renderer3D->SetCheckerTexture();
	else if (material != nullptr)
		App->renderer3D->BindTexure(material->GetMaterialID());
}

void ComponentMaterial::DrawConfig()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		if (ImGui::Button("Delete"))
			GetOwner()->RemoveComponent(this);
		if (material != nullptr)
		{
			ImGui::Checkbox("Use Checker Material", &use_checker);

			ImGui::LabelText("ID##material", "%d", material->GetMaterialID());
			ImGui::LabelText("Width##material", "%d", material->GetWidth());
			ImGui::LabelText("Height##material", "%d", material->GetHeight());
			ImGui::LabelText("Path##material", "%s", material->GetPath().c_str());
		}
		else
			ImGui::Text("No material assigned");
	}
}

void ComponentMaterial::UseChecker()
{
	use_checker = true;
}

void ComponentMaterial::Serialize(JSONDoc * doc)
{
	doc->AddSectionToArray("Components");
	doc->MoveToSectionFromArray("Components", doc->GetArraySize("Components") - 1);

	doc->SetNumber("type", GetType());
	doc->SetNumber("owner", GetOwner()->GetUID());
	if (material != nullptr)
		doc->SetNumber("material", material->GetUID());
	else
		doc->SetNumber("material", 0);
}
