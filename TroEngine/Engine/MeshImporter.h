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

class MeshImporter : public Module
{
public:
	MeshImporter();
	~MeshImporter();

	bool Awake(JSONDoc* config);
	bool Start();
	bool CleanUp();

	bool ImportFile(const char* path);

	void SaveToLibrary(Mesh* mesh);
	void LoadFile(const char* path);

	void RemoveMesh(Mesh* m);

private:
	bool ImportNode(const aiScene* scene, aiNode* node, GameObject* parent, const std::vector<Material*>& mats);
	bool ImportMesh(aiMesh* mesh, GameObject* owner, const std::vector<Material*>& mats);

	void CubeMesh();

private:
	std::list<Mesh*> meshes;
	int id = 0;
	uint save_id = 0;

	Mesh* cube = nullptr;

};

#endif // !__MESHIMPORTER__


