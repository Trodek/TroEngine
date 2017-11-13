#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "MathGeoLib.h"

class GameObject;
class Camera;

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void ConfigGUI();

	float* GetProjectionMatrix() const;
	float* GetViewMatrix() const;
	float3 GetPos()const;

	void Resize(float new_aspect);
	
private:
	GameObject* cam_go = nullptr;
	Camera* cam = nullptr;

	//Movement variables
	float speed = 10.f;
	float mouse_wheel_speed = 5.f;
	float mouse_sensivility = 0.25f;
};