#include "Camera.h"

Camera::Camera(GameObject * owner) : Component(Component::Type::Camera, owner)
{
	//frustum = FrustumType::PerspectiveFrustum;
	frustum.SetPos(float3(0, 0, 0));
	//frustum.Front = float3::unitZ;
	//frustum.HorizontalFov = 0;
	//frustum.VerticalFov = 0;
	//frustum.FarPlaneDistance = 500.0f;
	//frustum.NearPlaneDistance = 0.01f;
}

Camera::~Camera()
{
}

void Camera::SetAspectRatio(float aspect_ratio)
{
}
