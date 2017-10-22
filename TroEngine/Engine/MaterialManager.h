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

	Material* ImportImage(const char* path);

	void SaveAsDDS();

	void RemoveMaterial(Material* mat);

private:
	std::list<Material*> materials;

	uint save_id = 0;
};

#endif // !__MATERIALMANAGER__

