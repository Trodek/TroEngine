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

	uint ImportImage(const char* path);

private:
	std::list<Material*> materials;
};

#endif // !__MATERIALMANAGER__

