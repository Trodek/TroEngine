#ifndef __MATERIALMANAGER__
#define __MATERIALMANAGER__

#include "Globals.h"
#include "Module.h"
#include "GLInclude.h"
#include <list>
#include <vector>

class Material;
class Resource;

class MaterialManager : public Module
{
public:
	MaterialManager();
	~MaterialManager();

	bool Awake(JSONDoc* config);
	bool Start();
	bool CleanUp();

	Material* ImportImage(const char* path);

	void SaveAsDDS(Resource* res);

	void RemoveMaterial(Material* mat);

private:
	std::vector<Material*> materials;

	uint save_id = 0;
};

#endif // !__MATERIALMANAGER__

