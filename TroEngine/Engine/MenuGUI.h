#ifndef __EDITORGUI__
#define __EDITORGUI__

#include "GUIElement.h"
#include <list>
#include "MathGeoLib.h"
#include "Primitive.h"

class MenuGUI : public GUIElement
{
public:
	MenuGUI();
	~MenuGUI();

	update_status UpdateGUI(float dt);

private:
	void CreateGUI();
	void AboutPanel();
	void GUIConfig();
	void MathTest();
	void Config();
	void PerformanceMenu();

private:
	bool show_math_test = false;
	bool show_about = false;
	bool show_test_window = false;
	bool show_config = false;
	bool show_performance = false;

	//Math Test variables
	float rand_float = 0.0f;
	int min = 0;
	int max = 0;
	int rand_int = 0;

	int min_x = 0;
	int max_x = 0;
	int min_y = 0;
	int max_y = 0;
	int min_z = 0;
	int max_z = 0;
	int rand_x = 0;
	int rand_y = 0;
	int rand_z = 0;

	int num_obj = 0;
	int contacts = 0;
	std::list<AABB> aabb_objects;

	

};

#endif // !__EDITORCONSOLE__

