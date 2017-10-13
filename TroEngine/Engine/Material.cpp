#include "Material.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

Material::Material(uint id, uint w, uint h, const char* path) : id_material(id), width(w), height(h), path(path)
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

uint Material::GetWidth() const
{
	return width;
}

uint Material::GetHeight() const
{
	return height;
}