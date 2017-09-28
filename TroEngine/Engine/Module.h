#ifndef __MODULE__
#define __MODULE__

#include "Globals.h"
#include <string>

class Application;
class JSONDoc;

class Module
{
private:
	bool enabled;
	std::string name;

public:
	Module(bool start_enabled = true)
	{}

	virtual ~Module()
	{}

	virtual bool Awake(JSONDoc* config)
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

	bool IsEnabled() const
	{
		return enabled;
	}

	void SetName(const char* name)
	{
		this->name = name;
	}
	
	std::string GetName() const
	{
		return name;
	}
};

#endif // !__MODULE__

