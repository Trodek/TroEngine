#ifndef __MESHIMPORTER__
#define __MESHIMPORTER__

#include "Module.h"
#include "Geometry.h"
#include <list>


class MeshImporter : public Module
{
public:
	MeshImporter();
	~MeshImporter();

	bool Awake(JSONDoc* config);
	bool Start();
	bool CleanUp();

	bool LoadFile(const char* path);
	void RenderMeshes() const;

private:
	std::list<Geometry*> meshes;

};

#endif // !__MESHIMPORTER__


