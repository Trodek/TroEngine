#ifndef __GUIELEMENT__
#define __GUIELEMENT__

#include "Globals.h"

class GUIElement
{
public:
	GUIElement()
	{	}

	~GUIElement()
	{	}

	virtual update_status UpdateGUI(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

};
#endif // !__GUIELEMENT__

