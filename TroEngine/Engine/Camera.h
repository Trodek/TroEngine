#ifndef __CAMERA__
#define __CAMERA__

#include "Component.h"
#include "MathGeoLib.h"

class GameObject;

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

private:
	Frustum frustum;
	float aspect_ratio = 1.3f;
	float fov = 60.f;

};


#endif // !__CAMERA__

