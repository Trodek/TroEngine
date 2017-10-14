#ifndef __ASSIMPLOGGER__
#define __ASSIMPLOGGER__

#include "Globals.h"
#include "Assimp\include\DefaultLogger.hpp"

class AssimpLogger : public Assimp::LogStream
{
public:
	AssimpLogger()
	{

	}
	~AssimpLogger()
	{}
	void write(const char* message) 
	{
		log(__FILE__, __LINE__, "%s\n", message);
	}
	
};

#endif // !__ASSIMPLOGGER__
