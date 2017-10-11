#include "MaterialManager.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"
#include "Application.h"
#include "ModuleRenderer3D.h"

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

uint MaterialManager::ImportImage(const char* path)
{
	uint id = 0;
	//Data will be handled by renderer. Devil is only used to load the image.
	if (ilLoad(IL_TYPE_UNKNOWN, path))
	{
		id = App->renderer3D->LoadTextureToVRAM(ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), ilGetData());
		ilDeleteImage(ilGetInteger(IL_ACTIVE_IMAGE)); 
	}
	else
	{
		ILenum error;
		error = ilGetError();
		EDITOR_LOG("Error loading image %s. Error %d.", path, error);
	}

	return id;
}
