#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Material.h"

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(C_Material, owner)
{
}

ComponentMaterial::ComponentMaterial(GameObject * owner, Material * mat) : Component(C_Material, owner), material(mat)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::SetMaterial(Material * new_mat)
{
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
}

void ComponentMaterial::UseChecker()
{
	use_checker = true;
}
