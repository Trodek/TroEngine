#include "Material.h"

Material::Material()
{
}

Material::~Material()
{
}

uint Material::GetMaterialID() const
{
	return id_material;
}

std::string Material::GetPath() const
{
	return path;
}
