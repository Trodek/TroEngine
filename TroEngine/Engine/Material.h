#ifndef __MATERIAL__
#define __MATERIAL__

#include "Globals.h"
#include <string>

class Material
{
public:
	Material();
	~Material();

	uint GetMaterialID()const;
	std::string GetPath() const;

private:
	std::string path;
	uint id_material = 0;
};

#endif // !__MATERIAL__

