#include "MaterialManager.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Material.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "ResourceManager.h"
#include <cstdio>

#pragma comment (lib, "engine/Devil/libx86/DevIL.lib")
#pragma comment (lib, "engine/Devil/libx86/ILU.lib")
#pragma comment (lib, "engine/Devil/libx86/ILUT.lib")

MaterialManager::MaterialManager()
{
	SetName("materialmanager");
}

MaterialManager::~MaterialManager()
{
	ilShutDown();
}

bool MaterialManager::Awake(JSONDoc * config)
{
	bool ret = true;

	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	App->CreateFolder("Assets\\Materials");
	App->CreateFolder("Library\\Materials");

	return ret;
}

bool MaterialManager::Start()
{
	bool ret = true;

	return ret;
}

bool MaterialManager::CleanUp()
{
	bool ret = true;

	for (std::vector<Material*>::iterator m = materials.begin(); m != materials.end(); )
	{
		RELEASE(*m);

		m = materials.erase(m);
	}

	return ret;
}

Material* MaterialManager::ImportImage(const char* path)
{
	uint id = 0;
	ILenum error;
	Material* m = nullptr;

	//check if material already exist
	Resource* res = App->resources->ExistResource(std::string(path));
	if(res != nullptr) // already exist
	{
		m = materials[res->manager_id];
	}
	else
	{
		//Data will be handled by renderer. Devil is only used to load the image.
		if (ilLoad(IL_TYPE_UNKNOWN, path))
		{
			ILinfo info;
			iluGetImageInfo(&info);
			if (info.Origin == IL_ORIGIN_UPPER_LEFT)
			{
				iluFlipImage();
			}

			ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);
			int w = ilGetInteger(IL_IMAGE_WIDTH);
			int h = ilGetInteger(IL_IMAGE_HEIGHT);
			id = App->renderer3D->LoadTextureToVRAM(w, h, ilGetData(), ilGetInteger(IL_IMAGE_FORMAT));

			res = new Resource();
			res->extension = App->resources->GetExtensionFromPath(path);
			res->file_id = 0; //there's just 1 texture on each file
			res->name = App->resources->GetNameFromPath(path);
			res->rel_path = App->resources->GetRelativePathFromPath(path);

			m = new Material(id, w, h, path);
			materials.push_back(m);
			res->manager_id = materials.size() - 1;
			res->type = ResourceType::R_TEXTURE;
			res->UID = m->GetUID();

			SaveAsDDS(res);
			App->resources->resources[App->resources->CreateResource(path)].push_back(res);
			App->resources->CreateMeta(path);

			ilDeleteImage(ilGetInteger(IL_ACTIVE_IMAGE));
		}
		else
		{
			error = ilGetError();
			EDITOR_LOG("Error loading image %s. Error %d.", path, error);
		}
	}

	return m;
}

void MaterialManager::SaveAsDDS(Resource* res)
{
	ILuint		size;
	ILubyte*	data;

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);

	size = ilSaveL(IL_DDS, NULL, 0);
	if (size > 0)
	{
		data = new ILubyte[size];
		if (ilSaveL(IL_DDS, data, size) > 0)
		{
			char file[69];
			sprintf_s(file, "Library\\Materials\\texture_%d.dds", save_id++);
			FILE* tex_file = fopen(file, "wb");
			fwrite(data, sizeof(ILubyte), size, tex_file);
			fclose(tex_file);

			res->lib_name = App->resources->GetNameFromPath(file);
			res->lib_path = App->resources->GetRelativePathFromPath(file);

			EDITOR_LOG("New material texture saved: %s.", file);
		}
	}

}

void MaterialManager::RemoveMaterial(Material * mat)
{
	for (std::vector<Material*>::iterator m = materials.begin(); m != materials.end(); ++m)
	{
		if (*m == mat)
		{
			RELEASE(*m);

			materials.erase(m);
			break;
		}
	}
}
