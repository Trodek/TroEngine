#ifndef __EDITORGUI__
#define __EDITORGUI__

#include "Module.h"
#include <list>
#include "MathGeoLib.h"
#include "Primitive.h"

class EditorGUI : public Module
{
public:
	EditorGUI();
	~EditorGUI();

	bool Awake();
	update_status Update(float dt);

private:
	void CreateGUI();
	void AboutPanel();
	void GUIConfig();
	void MathTest();
	void Config();

private:
	bool show_math_test = false;
	bool show_about = false;
	bool show_test_window = false;
	bool show_config = false;

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

	//Congiguration
	char* new_title;
	char* new_org;
	int new_fps;

};

#endif // !__EDITORCONSOLE__

