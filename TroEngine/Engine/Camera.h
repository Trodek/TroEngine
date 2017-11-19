#ifndef __CAMERA__
#define __CAMERA__

#include "Component.h"
#include "MathGeoLib.h"
#include <vector>

class GameObject;
class Transform;

class Camera : public Component
{
public:
	Camera(GameObject* owner);
	~Camera();

	bool Start();

	void OnUpdateTransform();

	void SetAspectRatio(float aspect_ratio);
	void SetFOV(float angle);

	void SetCamera(float fov_angle = 60.f, float aspect_ratio = 16/9, float near_dist=0.1f, float far_dist=500.f);

	float* GetViewMatrix() const;
	float* GetProjectionMatrix() const;
	float GetNearPlaneDistance()const;
	float GetFarPlaneDistance()const;

	void DrawConfig();
	void DebugDraw();

	void MoveFront(const float& speed);
	void MoveBack(const float& speed);
	void MoveRight(const float& speed);
	void MoveLeft(const float& speed);
	void MoveUp(const float& speed);
	void MoveDown(const float& speed);

	void OrbitCamera(const float3& orbit_center, const float& mouse_dx, const float& mouse_dy);
	void RotateCamera(const float& mouse_dx, const float& mouse_dy);
	void FocusCamera(const float3& focus_point, float distance);
	void LookAt(const float3& Spot);

	void Serialize(JSONDoc* doc);
	Ray UnProject(float x, float y);
	LineSegment UnProjectSegment(float x, float y);

	void SetElementsToDraw()const;

private:
	bool FrustumContains(const AABB& box) const;

private:
	Frustum frustum;
	float aspect_ratio = 1.3f;
	float fov = 60.f;

};


#endif // !__CAMERA__

