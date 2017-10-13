#include "MaterialManager.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Material.h"
#include "SceneManager.h"
#include "Scene.h"

#pragma comment (lib, "engine/Devil/libx86/DevIL.lib")
#pragma comment (lib, "engine/Devil/libx86/ILU.lib")
#pragma comment (lib, "engine/Devil/libx86/ILUT.lib")

MaterialManager::MaterialManager()
{
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

	return ret;
}

void MaterialManager::ImportImage(const char* path)
{
	uint id = 0;
	//Data will be handled by renderer. Devil is only used to load the image.
	if (ilLoad(IL_TYPE_UNKNOWN, path))
	{
		int w = ilGetInteger(IL_IMAGE_WIDTH);
		int h = ilGetInteger(IL_IMAGE_HEIGHT);
		id = App->renderer3D->LoadTextureToVRAM(w, h, ilGetData());
		Material* m = new Material(id, w, h, path);
		materials.push_back(m);
		//for now, set this as the scene game object material
		App->scene_manager->GetCurrentScene()->GetGameObject(0);

		ilDeleteImage(ilGetInteger(IL_ACTIVE_IMAGE)); 
	}
	else
	{
		ILenum error;
		error = ilGetError();
		EDITOR_LOG("Error loading image %s. Error %d.", path, error);
	}
}
