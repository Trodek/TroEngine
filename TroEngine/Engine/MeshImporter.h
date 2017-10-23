#ifndef __MESHIMPORTER__
#define __MESHIMPORTER__

#include "Module.h"
#include <list>

class Mesh;
class GameObject;
class aiNode;

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
	std::list<Mesh*> meshes;
	int id = 0;
	uint save_id = 0;
};

#endif // !__MESHIMPORTER__


