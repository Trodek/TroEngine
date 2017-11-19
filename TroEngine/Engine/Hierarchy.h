#ifndef __HIERARCHY__
#define __HIERARCHY__

#include "Globals.h"
#include "GUIElement.h"

class GameObject;

class Hierarchy : public GUIElement
{
public:
	Hierarchy();
	~Hierarchy();

	update_status UpdateGUI(float dt);
	bool CleanUp();

public:
	bool active = true;

private:
	void CreateHierarchy();
};
	

#endif // !__HIERARCHY__

