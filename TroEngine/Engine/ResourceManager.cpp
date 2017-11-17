#include "ResourceManager.h"
#include "Application.h"
#include "JSONManager.h"

Resource::Resource()
{
}

Resource::~Resource()
{
}

ResourceManager::ResourceManager(bool start_enabled) : Module(start_enabled)
{
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Start()
{
	bool ret = true;


	return ret;
}

update_status ResourceManager::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ResourceManager::CleanUp()
{
	bool ret = true;

	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end();)
	{
		RELEASE(it->second);

		it = resources.erase(it);
	}

	return ret;
}

void ResourceManager::AddResource(uint uid_key, Resource * resource)
{
	resources.insert(std::pair<uint, Resource*>(uid_key, resource));
	//check if the meta file exist for that file
	JSONDoc* meta_doc = App->json->LoadJSONFile(BuildMetaPath(resource).c_str());

	//if file doesn't exist, create it
	if (meta_doc == nullptr)
	{
		meta_doc = App->json->CreateJSONFile(BuildMetaPath(resource).c_str());

		meta_doc->SetString("name", resource->name.c_str());
		meta_doc->SetString("extension", resource->extension.c_str());
		meta_doc->SetString("rel_path", resource->rel_path.c_str());
		meta_doc->SetString("lib_name", resource->lib_name.c_str());
		meta_doc->SetString("lib_path", resource->lib_path.c_str());
		meta_doc->SetNumber("type", resource->type);
		meta_doc->SetNumber("UID", resource->UID);

		meta_doc->SaveFile();
	}
}

void ResourceManager::Load(const char * path)
{
}

Resource * ResourceManager::ExistResource(std::string & file, int file_id)
{
	Resource* ret = nullptr;

	uint cut = file.find_last_of(".");
	std::string file_name = file.substr(0, cut);

	for (std::map<uint, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (it->second->name == file_name)
		{
			if (file_id != -1)
			{
				if (it->second->file_id == file_id)
				{
					ret = it->second;
					break;
				}
			}
			else
			{
				ret = it->second;
				break;
			}
		}
	}

	return ret;
}

std::string ResourceManager::BuildAssetPath(uint key)
{
	std::string path;

	Resource* resource = resources.find(key)->second;

	path = resource->rel_path;
	path += resource->name;
	path += ".";
	path += resource->extension;

	return path;
}

std::string ResourceManager::BuildAssetPath(Resource * resource)
{
	std::string path;

	path = resource->rel_path;
	path += resource->name;
	path += ".";
	path += resource->extension;

	return path;
}

std::string ResourceManager::BuildLibraryPath(uint key)
{
	std::string path;

	Resource* resource = resources.find(key)->second;

	path = resource->lib_path;
	path += resource->lib_name;
	path += ".";
	switch (resource->type)
	{
	case R_MESH:
		path += "tromesh";
		break;
	case R_TEXTURE:
		path += "dds";
		break;
	case R_PREFAB:
		path = "";
		break;
	case R_SCENE:
		path = "";
		break;
	default:
		break;
	}

	return path;
}

std::string ResourceManager::BuildLibraryPath(Resource * resource)
{
	std::string path;

	path = resource->lib_path;
	path += resource->lib_name;
	path += ".";
	switch (resource->type)
	{
	case R_MESH:
		path += "tromesh";
		break;
	case R_TEXTURE:
		path += "dds";
		break;
	case R_PREFAB:
		path = "";
		break;
	case R_SCENE:
		path = "";
		break;
	default:
		break;
	}

	return path;
}

std::string ResourceManager::BuildMetaPath(uint key)
{
	std::string path;

	Resource* resource = resources.find(key)->second;

	path = resource->rel_path;
	path += resource->name;
	path += "_meta.json";

	return path;
}

std::string ResourceManager::BuildMetaPath(Resource * resource)
{
	std::string path;

	path = resource->rel_path;
	path += resource->name;
	path += "_meta.json";

	return path;
}

void ResourceManager::LoadFolder(const char * path)
{
	std::vector<std::string> files;
	App->GetFilesInPath(files, path);

	for(std::vector<std::string>::iterator f = files.begin(); f!= files.end();++f)
	{

	}
}
