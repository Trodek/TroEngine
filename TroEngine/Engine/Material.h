#ifndef __MATERIAL__
#define __MATERIAL__

#include "Globals.h"
#include <string>

class Material
{
public:
	Material(uint id, uint w, uint h, const char* path);
	~Material();

	uint GetMaterialID()const;
	std::string GetPath() const;
	uint GetWidth() const;
	uint GetHeight() const;

private:
	std::string path;
	uint id_material	= 0;
	uint width			= 0;
	uint height			= 0;
};

#endif // !__MATERIAL__

