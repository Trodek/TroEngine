#include "Camera.h"
#include "Globals.h"
#include "GameObject.h"
#include "Transform.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "imgui.h"
#include "JSONManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "MeshRenderer.h"

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

float Camera::GetNearPlaneDistance() const
{
	return frustum.NearPlaneDistance();
}

float Camera::GetFarPlaneDistance() const
{
	return frustum.FarPlaneDistance();
}

void Camera::DrawConfig()
{	
	if (ImGui::CollapsingHeader("Camera"))
	{
		if (ImGui::Button("Delete"))
			GetOwner()->RemoveComponent(this);

		ImGui::SliderFloat("FOV##cam", &fov, 0, 360, "%.2f");
		SetFOV(fov);
		ImGui::SliderFloat("Aspect Ratio##cam", &aspect_ratio, 1, 2);
		SetAspectRatio(aspect_ratio);

	}
}

void Camera::DebugDraw()
{
	
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

void Camera::Serialize(JSONDoc * doc)
{
	doc->AddSectionToArray("Components");
	doc->MoveToSectionFromArray("Components", doc->GetArraySize("Components") - 1);

	doc->SetNumber("type", GetType());
	doc->SetNumber("owner", GetOwner()->GetUID());
	doc->SetNumber("aspect_ratio", aspect_ratio);
	doc->SetNumber("fov", fov);
	doc->SetNumber("near", frustum.NearPlaneDistance());
	doc->SetNumber("far", frustum.FarPlaneDistance());
}

LineSegment Camera::UnProjectSegment(float x, float y)
{
	return frustum.UnProjectLineSegment(x, y);
}

void Camera::GetElementsToDraw(std::vector<GameObject*>& vec_to_fill) const
{
	std::vector<GameObject*> objects_to_test;
	App->scene_manager->GetCurrentScene()->GetAllDynamicGameObjects(objects_to_test);
	///clean all objects that doesn't have aabb
	for (std::vector<GameObject*>::iterator it = objects_to_test.begin(); it != objects_to_test.end();)
	{
		if (!(*it)->HasComponent(Component::Type::C_MeshRenderer))
			it = objects_to_test.erase(it);
		else it++;
	}
	App->scene_manager->GetCurrentScene()->TestToKDTree(frustum, objects_to_test);

	//test elements with frustum
	for (std::vector<GameObject*>::iterator it = objects_to_test.begin(); it != objects_to_test.end();)
	{
		MeshRenderer* mr = (MeshRenderer*)(*it)->GetComponent(Component::Type::C_MeshRenderer);

		if (!FrustumContains(mr->GetMeshAABB()))
			it = objects_to_test.erase(it);
		else it++;
	}

	//copy elements to vector
	for (std::vector<GameObject*>::iterator it = objects_to_test.begin(); it != objects_to_test.end(); ++it)
	{
		vec_to_fill.push_back((*it));
	}
}

bool Camera::FrustumContains(const AABB & box) const
{
	bool ret = true;

	float3 corners[8];
	box.GetCornerPoints(corners);

	//test all corners for each plane
	for (int p = 0; p < 6; ++p)
	{
		uint corners_in = 8;
		
		for (int c = 0; c < 8; ++c)
		{
			if (frustum.GetPlane(p).IsOnPositiveSide(corners[c]))
			{
				corners_in--;
			}
		}

		if (corners_in == 0)
		{
			ret = false;
			break;
		}

	}

	return ret;
}

Ray Camera::UnProject(float x, float y)
{
	return frustum.UnProject(x, y);
}
