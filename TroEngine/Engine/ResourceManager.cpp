#include "ResourceManager.h"
#include "Application.h"
#include "JSONManager.h"
#include <algorithm>
#include "MeshImporter.h"
#include "MaterialManager.h"
#include "SceneImporter.h"
#include "SceneManager.h"
#include "Scene.h"
#include "UID.h"

Resource::Resource()
{
}

Resource::Resource(JSONDoc * doc, int id)
{
	doc->MoveToSectionFromArray("resources", id);
	name = doc->GetString("name");
	extension = doc->GetString("extension");
	rel_path = doc->GetString("rel_path");
	lib_name = doc->GetString("lib_name");
	lib_path = doc->GetString("lib_path");
	file_id = doc->GetNumber("file_id");
	type = static_cast<ResourceType>((int)doc->GetNumber("type"));
	UID = doc->GetNumber("UID");
	doc->MoveToRoot();
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

	for (std::map<uint, std::vector<Resource*>>::iterator it = resources.begin(); it != resources.end();)
	{
		for (std::vector<Resource*>::iterator r = it->second.begin(); r != it->second.end();)
		{
			RELEASE(*r);

			r = it->second.erase(r);
		}
		it = resources.erase(it);
	}

	return ret;
}

Resource * ResourceManager::GetResource(uint uid_key) const
{
	Resource* ret = nullptr;

	for (std::vector<Resource*>::const_iterator r = resources.find(uid_key)->second.begin(); r != resources.find(uid_key)->second.end(); ++r)
	{
		ret = (*r);
		break;
		//TODO: think abiut something to solve that
	}

	return ret;
}

void ResourceManager::Load(const char * path)
{
	//check if the resource already exist
	if (resources.find(CreateResource(path))!=resources.end()) {
		EDITOR_LOG("Resource %s already exist", GetNameFromPath(path).c_str());
	}
	else
	{
		//identidy type of the file
		ResourceType file_type = GetTypeFromPath(path);

		bool create_meta = true;
		std::string copy_path;
		switch (file_type)
		{
		case R_NULL:
			break;
		case R_MESH:
		{
			App->CopyFileTo(path, "Assets\\Meshes", &copy_path);
			App->mesh->ImportFile(copy_path.c_str());
			break;
		}
		case R_TEXTURE:
			App->CopyFileTo(path, "Assets\\Materials", &copy_path);
			App->materials->ImportImage(copy_path.c_str());
			create_meta = false; //since other importes can request texture loading, the material importer will call createmeta autonomously
			break;
		case R_PREFAB:
			App->scene_manager->GetCurrentScene()->AddGameObject(App->scene_importer->LoadPrefab(path));
			break;
		case R_SCENE:
			App->scene_importer->LoadScene(path, App->scene_manager->GetCurrentScene());
			break;
		case R_META: //if its a meta file, create the resource that it descrives.
			LoadMeta(path);
			create_meta = false;
			break;
		default:
			break;
		}

		if (create_meta)
			CreateMeta(copy_path.c_str());
	}
}

Resource * ResourceManager::LoadMeta(const char * path)
{
	Resource* ret = nullptr;

	JSONDoc* meta_doc = App->json->LoadJSONFile(path);

	if (meta_doc != nullptr)
	{
		//create the entrance on the map for this file
		std::string assets_path = BuildAssetPath(meta_doc);
		uint key = CreateResource(assets_path.c_str());

		for (int i = 0; i < meta_doc->GetArraySize("resources"); ++i)
		{
			Resource* res = new Resource(meta_doc, i);
			std::string lib_path = BuildLibraryPath(meta_doc, i);
			switch (GetTypeFromMeta(meta_doc, i))
			{
			case R_NULL:
				break;
			case R_MESH:
				if (App->mesh->LoadFile(lib_path.c_str(), res) == false)
				{
					//the file doesnt exist on the library, import it and update resource info
					App->mesh->ImportMesh(assets_path.c_str(), res->file_id, res);
				}
				break;
			case R_TEXTURE:
				break;
			case R_PREFAB:
				break;
			case R_SCENE:
				break;
			case R_META:
				break;
			default:
				break;
			}
			resources[key].push_back(res);
		}
		
	}

	return ret;
}

Resource * ResourceManager::ExistResource(std::string & file, int file_id)
{
	Resource* ret = nullptr;

	uint cut = file.find_last_of(".");
	std::string file_name = file.substr(0, cut);

	for (std::map<uint, std::vector<Resource*>>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		for (std::vector<Resource*>::iterator r = it->second.begin(); r != it->second.end(); ++r)
		{
			if ((*r)->name == file_name)
			{
				if (file_id != -1)
				{
					if ((*r)->file_id == file_id)
					{
						ret = (*r);
						break;
					}
				}
				else
				{
					ret = (*r);
					break;
				}
			}
		}
	}

	return ret;
}

uint ResourceManager::CreateResource(const char * path)
{
	uint uid_key = 0;
	GenerateFileUID(path, uid_key);
	return uid_key;
}

void ResourceManager::CreateMeta(const char * path)
{
	uint key = CreateResource(path);
	std::string lib_path = GetRelativePathFromPath(path);
	lib_path += GetNameFromPath(path);
	lib_path += "_meta.json";
	JSONDoc* meta = App->json->CreateJSONFile(lib_path.c_str());

	meta->AddArray("resources");
	for (int i = 0; i < resources[key].size(); ++i)
	{
		meta->AddSectionToArray("resources");
		meta->MoveToSectionFromArray("resources", meta->GetArraySize("resources") - 1);

		meta->SetString("name", resources[key][i]->name.c_str());
		meta->SetString("extension", resources[key][i]->extension.c_str());
		meta->SetString("rel_path", resources[key][i]->rel_path.c_str());
		meta->SetString("lib_name", resources[key][i]->lib_name.c_str());
		meta->SetString("lib_path", resources[key][i]->lib_path.c_str());
		meta->SetNumber("file_id", resources[key][i]->file_id);
		meta->SetNumber("type", resources[key][i]->type);
		meta->SetNumber("UID", resources[key][i]->UID);

		meta->MoveToRoot();
	}

	meta->SaveFile();
}

std::string ResourceManager::BuildAssetPath(uint key)
{
	std::string path;

	Resource* resource = resources.find(key)->second.front(); //asset path will be the same for all resources a key has

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

std::string ResourceManager::BuildAssetPath(JSONDoc * doc)
{
	std::string path;

	doc->MoveToSectionFromArray("resources", 0); //assets path is the same for each resource on the file
	path = doc->GetString("rel_path");
	path += doc->GetString("name");
	path += ".";
	path += doc->GetString("extension");
	doc->MoveToRoot();

	return path;
}

std::string ResourceManager::BuildLibraryPath(uint key, int id)
{
	std::string path;
	Resource* resource = nullptr;
	if(id == -1)
		resource = resources.find(key)->second.front();
	else
		resource = resources.find(key)->second[id];
	
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

std::string ResourceManager::BuildLibraryPath(JSONDoc * doc, int id)
{
	std::string path;

	doc->MoveToSectionFromArray("resources", id);
	path = doc->GetString("lib_path");
	path += doc->GetString("lib_name");
	path += ".";
	switch (static_cast<ResourceType>((int)doc->GetNumber("type")))
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
	doc->MoveToRoot();

	return path;
}

std::string ResourceManager::BuildMetaPath(uint key)
{
	std::string path;

	Resource* resource = resources.find(key)->second.front(); //meta path will be the same for all resources a key has
	
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

bool ResourceManager::GenerateFileUID(const char * path, uint & uid)
{
	bool ret = true;
	FILE* f = fopen(path, "rb");
	if (f != nullptr)
	{
		fseek(f, 0L, SEEK_END);
		uint total_size = ftell(f);
		rewind(f); //go back to file begining

		char* data = new char[total_size];
		memccpy(data, f, sizeof(char),total_size);
		fclose(f);

		uid = *(md5(data, total_size));
		RELEASE_ARRAY(data);
	}
	else
		ret = false;

	return ret;
}

std::string ResourceManager::GetNameFromPath(const char * path)
{
	std::string _path = path;
	std::string name;

	uint dot = _path.find_last_of(".");
	uint slash = _path.find_last_of("\\");
	slash += 1;
	name = _path.substr(slash, dot - slash);

	return name;
}

std::string ResourceManager::GetExtensionFromPath(const char * path)
{
	std::string _path = path;
	std::string ext;

	uint dot = _path.find_last_of(".");
	ext = _path.substr(dot+1);

	return ext;
}

std::string ResourceManager::GetRelativePathFromPath(const char * path)
{
	std::string _path = path;
	std::string rel_path;

	uint slash = _path.find_last_of("\\");
	rel_path = _path.substr(0, slash+1);

	return rel_path;
}

void ResourceManager::LoadFolder(const char * path)
{
	std::vector<std::string> files;
	App->GetFilesInPath(files, path);

	for(std::vector<std::string>::iterator f = files.begin(); f!= files.end();++f)
	{
		//TODO
	}
}

ResourceType ResourceManager::GetTypeFromPath(const char * path) const
{
	ResourceType ret = R_NULL;

	std::string p = path;
	uint cut = p.find_last_of(".");
	std::string extension = p.substr(cut + 1);
	std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

	if (extension == "json") // meta, scenes and prefabs.
	{
		int type_cut = p.find_last_of("_");
		if (type_cut != -1)
		{
			std::string type_str = p.substr(type_cut + 1, cut - (type_cut + 1));
			if (type_str == "meta")
				ret = R_META;
			else if (type_str == "prefab")
				ret = R_PREFAB;
			else if (type_str == "scene")
				ret = R_SCENE;
		}
		else
			EDITOR_LOG("The file %s is not supported.", path);
	}
	else if (extension == "fbx") // meshes
	{
		ret = R_MESH;
	}
	else if (extension == "png" || extension == "tga") // textures
	{
		ret = R_TEXTURE;
	}

	return ret;
}

ResourceType ResourceManager::GetTypeFromMeta(JSONDoc * doc, int id) const
{
	doc->MoveToSectionFromArray("resources", id);
	ResourceType ret = static_cast<ResourceType>((int)doc->GetNumber("type"));
	doc->MoveToRoot();
	return ret;
}