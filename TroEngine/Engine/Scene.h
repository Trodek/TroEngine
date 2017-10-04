#ifndef __SCENE__
#define __SCENE__

#include "Globals.h"

class Scene
{
public:
	Scene(bool active = true) : active(active)
	{	}
	~Scene()
	{	}

	virtual bool Start()
	{
		return true;
	}

	virtual void Draw()
	{	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	bool IsActive()
	{
		return active;
	}

private:
	bool active;
};


#endif // !__SCENE__

