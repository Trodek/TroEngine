#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Material.h"
#include "MaterialManager.h"
#include "imgui.h"

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
	App->materials->RemoveMaterial(material);
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
	if (material != nullptr)
	{
		if (ImGui::CollapsingHeader("Material"))
		{
			ImGui::Checkbox("Use Checker Material", &use_checker);

			ImGui::LabelText("ID##material", "%d", material->GetMaterialID());
			ImGui::LabelText("Width##material", "%d", material->GetWidth());
			ImGui::LabelText("Height##material", "%d", material->GetHeight());
			ImGui::LabelText("Path##material", "%s", material->GetPath().c_str());
		}
	}
}

void ComponentMaterial::UseChecker()
{
	use_checker = true;
}