#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "MathGeoLib.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();

	void AdjustCameraToAABB(AABB& bb);

private:
	void CalculateViewMatrix();

	void MoveFront(const float& speed);
	void MoveBack(const float& speed);
	void MoveRight(const float& speed);
	void MoveLeft(const float& speed);
	void MoveUp(const float& speed);
	void MoveDown(const float& speed);

	void OrbitCamera(const vec3 &orbit_center, const float& mouse_dx, const float& mouse_dy);
	void RotateCamera(const float& mouse_dx, const float& mouse_dy);
	void FocusCamera(const vec3& focus_point, float distance);

public:
	vec3 X, Y, Z, Position, Reference;
	
private:
	bool attach = false;
	mat4x4 ViewMatrix, ViewMatrixInverse;

	//Movement variables
	float speed = 10.f;
	float mouse_wheel_speed = 5.f;
	float mouse_sensivility = 0.25f;
};