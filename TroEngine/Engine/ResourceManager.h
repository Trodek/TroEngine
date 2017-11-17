#ifndef __RESOURCEMANAGER__
#define __RESOURCEMANAGER__

#include "Module.h"
#include <map>

class Mesh;
class GameObject;
class Material;

enum ResourceType
{
	R_NULL,
	R_MESH,
	R_TEXTURE,
	R_PREFAB,
	R_SCENE,
};

class Resource
{
public:
	Resource();
	~Resource();

public:
	std::string name;
	std::string extension;
	std::string rel_path;
	std::string lib_name;
	std::string lib_path;
	ResourceType type = R_NULL;
	uint instances = 0;
	uint file_id = 0; // if the file contain more than 1 resource (ex: fbx) the index on file
	uint manager_id; // index on the respective manager vector
	uint UID = 0;
	union ptr
	{
		Mesh* mesh_ptr;
		GameObject* go_ptr;
		Material* mat_ptr;
	};
};

class ResourceManager : public Module
{
public:
	ResourceManager(bool start_enabled = true);
	~ResourceManager();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void AddResource(uint uid_key, Resource* resource);

	void Load(const char* path);

	//Check if the resouce exist. Returns a pointer to the resource or nullptr
	Resource* ExistResource(std::string& file, int file_id = -1);

	std::string BuildAssetPath(uint key);
	std::string BuildAssetPath(Resource* resource);
	std::string BuildLibraryPath(uint key);
	std::string BuildLibraryPath(Resource* resource);
	std::string BuildMetaPath(uint key);
	std::string BuildMetaPath(Resource* resource);

private:
	void LoadFolder(const char* path);

private:
	std::map<uint, Resource*> resources;

};

#endif // !__RESOURCEMANAGER__

