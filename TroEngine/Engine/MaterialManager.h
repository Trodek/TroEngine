#ifndef __MATERIALMANAGER__
#define __MATERIALMANAGER__

#include "Globals.h"
#include "Module.h"
#include "GLInclude.h"
#include <list>

class Material;

class MaterialManager : public Module
{
public:
	MaterialManager();
	~MaterialManager();

	bool Awake(JSONDoc* config);
	bool Start();
	bool CleanUp();

	void ImportImage(const char* path);

	void RemoveMaterial(Material* mat);

private:
	std::list<Material*> materials;
};

#endif // !__MATERIALMANAGER__

