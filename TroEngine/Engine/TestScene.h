#pragma once
#include "Scene.h"
#include "Primitive.h"
#include "Gizmo.h"

class TestScene : public Scene
{
public:
	TestScene(bool active = true);
	~TestScene();

	bool Start();
	update_status Update(float dt);
	void Draw();
	bool CleanUp();


private:

private:
	bool show_test_window = true;

	bool show_about = false;

	uint test_id = 0;

	GCubeVA c1;
	GCubeIE c2;
	GSphere s;
	
};
