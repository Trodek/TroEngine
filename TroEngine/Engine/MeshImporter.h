#ifndef __MESHIMPORTER__
#define __MESHIMPORTER__

#include "Module.h"
#include <list>
#include <vector>

class Mesh;
class Material;
class GameObject;
class aiNode;
class aiScene;
class aiMesh;
class Resource;

class MeshImporter : public Module
{
public:
	MeshImporter();
	~MeshImporter();

	bool Awake(JSONDoc* config);
	bool Start();
	bool CleanUp();

	Mesh* GetMesh(uint id) const;

	bool ImportFile(const char* path);
	bool ImportMesh(const char* path, uint mesh_id, Resource* res);

	void SaveToLibrary(Mesh* mesh, Resource* res);
	void SaveToLibrary(Mesh* mesh, const char* path);
	bool LoadFile(const char* path, Resource* res = nullptr);

	void RemoveMesh(Mesh* m);

private:
	bool ImportNode(const aiScene* scene, aiNode* node, GameObject* parent,const std::vector<Material*>& mats, uint res_key, const char* path);
	bool ImportMesh(aiMesh* mesh, GameObject* owner, const std::vector<Material*>& mats,Resource* res);

	void CubeMesh();
	void PlaneMesh();
	void HDPlaneMesh(); //plane with 2500 vertex for testing

	void CheckSaveID(const char* file);

private:
	std::vector<Mesh*> meshes;
	int id = 0;
	uint save_id = 0;

public:
	Mesh* cube = nullptr;
	Mesh* plane = nullptr;
	Mesh* hd_plane = nullptr;
};

#endif // !__MESHIMPORTER__


