#pragma once
#include "Module.h"
#include "Globals.h"
#include "Primitive.h"

struct PhysBody3D;
struct PhysMotor3D;

class EditorScene : public Module
{
public:
	EditorScene(bool start_enabled = true);
	~EditorScene();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void OnCollision(PhysBody3D* body1, PhysBody3D* body2);
private:
	bool show_test_window = true;

	bool show_about = false;
	
};
