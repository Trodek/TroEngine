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

	for (std::list<Material*>::iterator m = materials.begin(); m != materials.end(); )
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
		id = App->renderer3D->LoadTextureToVRAM(w, h, ilGetData(),ilGetInteger(IL_IMAGE_FORMAT));

		m = new Material(id, w, h, path);
		materials.push_back(m);
		
		SaveAsDDS();

		ilDeleteImage(ilGetInteger(IL_ACTIVE_IMAGE));
	}
	else
	{
		error = ilGetError();
		EDITOR_LOG("Error loading image %s. Error %d.", path, error);
	}

	return m;
}

void MaterialManager::SaveAsDDS()
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
			EDITOR_LOG("New material texture saved: %s.", file);
		}
	}

}

void MaterialManager::RemoveMaterial(Material * mat)
{
	for (std::list<Material*>::iterator m = materials.begin(); m != materials.end(); ++m)
	{
		if (*m == mat)
		{
			RELEASE(*m);

			materials.erase(m);
			break;
		}
	}
}
