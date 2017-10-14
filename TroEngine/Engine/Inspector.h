#ifndef __INSPECTOR_GUI__
#define __INSPECTOR_GUI__

#include "GUIElement.h"

class Inspector : public GUIElement
{
public:
	Inspector();
	~Inspector();

	update_status UpdateGUI(float dt);

	void CreateInspector();

private:
};

#endif // !__INSPECTOR_GUI__

