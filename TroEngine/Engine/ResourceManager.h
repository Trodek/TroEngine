#ifndef __RESOURCEMANAGER__
#define __RESOURCEMANAGER__

#include "Module.h"
#include <map>
#include <vector>

class Mesh;
class GameObject;
class Material;
class JSONDoc;

enum ResourceType
{
	R_NULL,
	R_MESH,
	R_TEXTURE,
	R_PREFAB,
	R_SCENE,
	R_META,
};

class Resource
{
public:
	Resource();
	Resource(JSONDoc* doc, int id);
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
	bool flipped = false;
};

class ResourceManager : public Module
{
public:
	ResourceManager(bool start_enabled = true);
	~ResourceManager();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	Resource* GetResource(uint uid)const;

	void Load(const char* path);
	Resource* LoadMeta(const char* path);

	//Check if the resouce exist. Returns a pointer to the resource or nullptr
	Resource* ExistResource(std::string& file, int file_id = -1);

	//Create an entry on the resources map and returns a reference to its Resource vector
	uint CreateResource(const char* path);
	void CreateMeta(const char* path);

	std::string BuildAssetPath(uint key);
	std::string BuildAssetPath(Resource* resource);
	std::string BuildAssetPath(JSONDoc* doc);
	std::string BuildLibraryPath(uint key, int id = -1);
	std::string BuildLibraryPath(Resource* resource);
	std::string BuildLibraryPath(JSONDoc* doc, int id);
	std::string BuildMetaPath(uint key);
	std::string BuildMetaPath(Resource* resource);

	bool GenerateFileUID(const char* path, uint& uid);

	std::string GetNameFromPath(const char* path);
	std::string GetExtensionFromPath(const char* path);
	std::string GetRelativePathFromPath(const char* path);

private:
	void LoadFolder(const char* path);
	ResourceType GetTypeFromPath(const char* path) const;
	ResourceType GetTypeFromMeta(JSONDoc* doc, int id) const;

public:
	std::map<uint, std::vector<Resource*>> resources;

};

#endif // !__RESOURCEMANAGER__

