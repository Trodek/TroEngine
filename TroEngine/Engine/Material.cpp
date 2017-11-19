#include "Material.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "UID.h"

Material::Material(uint id, uint w, uint h, const char* path) : id_material(id), width(w), height(h), path(path)
{
	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);

	uint* uid = md5(data, size);
	UID = *uid;
	RELEASE_ARRAY(data);
}

Material::Material(uint uid, uint id, uint w, uint h, const char * path) : id_material(id), width(w), height(h), path(path), UID(uid)
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

uint Material::GetUID() const
{
	return UID;
}
