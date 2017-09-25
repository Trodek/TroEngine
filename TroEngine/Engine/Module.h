#ifndef __MODULE__
#define __MODULE__

#include "Globals.h"

class Application;
struct PhysBody3D;

class Module
{
private:
	bool enabled;

public:
	Module(bool start_enabled = true)
	{}

	virtual ~Module()
	{}

	virtual bool Awake()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual void ConfigGUI()
	{

	}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2)
	{}

	bool IsEnabled() const
	{
		return enabled;
	}
};

#endif // !__MODULE__

