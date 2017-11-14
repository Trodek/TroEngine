#include "Camera.h"
#include "Globals.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "imgui.h"

Camera::Camera(GameObject * owner) : Component(Component::Type::Camera, owner)
{
	frustum.SetKind(math::FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);
	frustum.SetHorizontalFovAndAspectRatio(60.f*DEGTORAD, 1.3f);
	frustum.SetViewPlaneDistances(0.1f, 1000.0f);
}

Camera::~Camera()
{
}

bool Camera::Start()
{
	return true;
}

void Camera::OnUpdateTransform()
{
	frustum.SetPos(GetOwner()->GetTransform().TranslatePart());
}

void Camera::SetAspectRatio(float aspect)
{
	float r = DEGTORAD * aspect;

	if (r > 0.0f)
	{
		frustum.SetHorizontalFovAndAspectRatio(fov*DEGTORAD, aspect);
		aspect_ratio = aspect;
	}
}

void Camera::SetFOV(float angle)
{
	frustum.SetHorizontalFovAndAspectRatio(angle*DEGTORAD, aspect_ratio);
	fov = angle;
	frustum.ComputeProjectionMatrix();
}

void Camera::SetCamera(float fov_angle, float aspect_ratio, float near_dist, float far_dist)
{
	float r = DEGTORAD * aspect_ratio;

	if (r > 0.0f)
	{
		frustum.SetHorizontalFovAndAspectRatio(fov_angle*DEGTORAD, aspect_ratio);
		this->aspect_ratio = aspect_ratio;
		fov = fov_angle;
	}

	frustum.SetViewPlaneDistances(near_dist, far_dist);
}

float * Camera::GetViewMatrix() const
{
	static float4x4 m;

	m = frustum.ViewMatrix();
	m.Transpose();
	return m.ptr();
}

float * Camera::GetProjectionMatrix() const
{
	static float4x4 m;

	m = frustum.ProjectionMatrix();
	m.Transpose();
	return m.ptr();
}

void Camera::DrawConfig()
{	
	if (ImGui::CollapsingHeader("Camera"))
	{
		ImGui::SliderFloat("FOV##cam", &fov, 0, 360, "%.2f");
		SetFOV(fov);
		ImGui::SliderFloat("Aspect Ratio##cam", &aspect_ratio, 1, 2);
		SetAspectRatio(aspect_ratio);

	}
}

void Camera::DebugDraw()
{
	GLenum mode = App->renderer3D->GetPolyMode();
	App->renderer3D->PolygonModeWireframe();

	float3* points = nullptr;
	frustum.GetCornerPoints(points);

	glLineWidth(2.0f);
	App->renderer3D->DisableState(GL_CULL_FACE);

	glBegin(GL_QUADS);

	glVertex3fv((GLfloat*)&points[1]);
	glVertex3fv((GLfloat*)&points[5]);
	glVertex3fv((GLfloat*)&points[7]);
	glVertex3fv((GLfloat*)&points[3]);

	glVertex3fv((GLfloat*)&points[4]);
	glVertex3fv((GLfloat*)&points[0]);
	glVertex3fv((GLfloat*)&points[2]);
	glVertex3fv((GLfloat*)&points[6]);

	glVertex3fv((GLfloat*)&points[5]);
	glVertex3fv((GLfloat*)&points[4]);
	glVertex3fv((GLfloat*)&points[6]);
	glVertex3fv((GLfloat*)&points[7]);

	glVertex3fv((GLfloat*)&points[0]);
	glVertex3fv((GLfloat*)&points[1]);
	glVertex3fv((GLfloat*)&points[3]);
	glVertex3fv((GLfloat*)&points[2]);

	glVertex3fv((GLfloat*)&points[3]);
	glVertex3fv((GLfloat*)&points[7]);
	glVertex3fv((GLfloat*)&points[6]);
	glVertex3fv((GLfloat*)&points[2]);

	glVertex3fv((GLfloat*)&points[0]);
	glVertex3fv((GLfloat*)&points[4]);
	glVertex3fv((GLfloat*)&points[5]);
	glVertex3fv((GLfloat*)&points[1]);

	glEnd();

	glLineWidth(1.0f);

	if(App->renderer3D->GetCullFace())
		App->renderer3D->EnableState(GL_CULL_FACE);

	if (mode == GL_POINT)
	{
		App->renderer3D->PolygonModePoints();
	}
	else if (mode == GL_FILL)
	{
		App->renderer3D->PolygonModeFill();
	}
}

void Camera::MoveFront(const float & speed)
{
	float3 movement = frustum.Front()*speed;

	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);
	comp_transform->Translate(movement);
}

void Camera::MoveBack(const float & speed)
{
	float3 movement = frustum.Front()*speed;

	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);
	comp_transform->Translate(-movement);
}

void Camera::MoveRight(const float & speed)
{
	float3 movement = frustum.WorldRight()*speed;

	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);
	comp_transform->Translate(movement);
}

void Camera::MoveLeft(const float & speed)
{
	float3 movement = frustum.WorldRight()*speed;

	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);
	comp_transform->Translate(-movement);
}

void Camera::MoveUp(const float & speed)
{
	float3 movement = frustum.Up()*speed;

	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);
	comp_transform->Translate(-movement);
}

void Camera::MoveDown(const float & speed)
{
	float3 movement = frustum.Up()*speed;

	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);
	comp_transform->Translate(movement);
}

void Camera::OrbitCamera(const float3 & orbit_center, const float & mouse_dx, const float & mouse_dy)
{
	float3 distance = frustum.Pos() - orbit_center;

	Quat X(frustum.WorldRight(), mouse_dy*DEGTORAD);
	Quat Y(frustum.Up(), mouse_dx*DEGTORAD);

	distance = X.Transform(distance);
	distance = Y.Transform(distance);

	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);
	comp_transform->SetTranslate(distance);
}

void Camera::RotateCamera(const float & mouse_dx, const float & mouse_dy)
{
	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);
	
	// Rotate Up
	if (mouse_dx != 0.f)
	{
		Quat rot = Quat::RotateY(mouse_dx*DEGTORAD);
		frustum.SetFront(rot.Mul(frustum.Front()).Normalized());
		frustum.SetUp(rot.Mul(frustum.Up()).Normalized());
		comp_transform->SetRotation(frustum.WorldMatrix().RotatePart().ToQuat());
	}

	//Rotate Front
	if (mouse_dy != 0.f)
	{
		Quat rot = Quat::RotateAxisAngle(frustum.WorldRight(), mouse_dy*DEGTORAD);
		float3 new_up = rot.Mul(frustum.Up()).Normalized();
		
		frustum.SetUp(new_up);
		frustum.SetFront(rot.Mul(frustum.Front()).Normalized());
	}
}

void Camera::FocusCamera(const float3 & focus_point, float distance)
{
	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);

	float3 new_cam_pos = focus_point + frustum.Front()*distance;

	comp_transform->SetTranslate(new_cam_pos);
}

void Camera::LookAt(const float3 & spot) // TODO: fix
{
	Transform* comp_transform = (Transform*)GetOwner()->GetComponent(Component::Type::C_Transform);

	float3 target_dir = spot - comp_transform->GetTransform().TranslatePart();

	//comp_transform->Rotate(float3x3::LookAt(frustum.Front(), target_dir.Normalized(), frustum.Up(), float3::unitY).ToQuat());
}
