#include "Camera.h"
#include "Globals.h"
#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "imgui.h"

Camera::Camera(GameObject * owner) : Component(Component::Type::Camera, owner)
{
	frustum.SetPos(float3(0, 0, 0));
	frustum.SetFront(float3::unitZ);
	frustum.SetUp(float3::unitY);
	frustum.SetVerticalFovAndAspectRatio(60.f*DEGTORAD, 1.3f);
	frustum.SetViewPlaneDistances(0.1f, 500.0f);
	frustum.SetKind(math::FrustumProjectiveSpace::FrustumSpaceGL, math::FrustumLeftHanded);
}

Camera::~Camera()
{
}

bool Camera::Start()
{
	SetCamera();
	return true;
}

void Camera::OnUpdateTransform()
{
	frustum.SetPos(GetOwner()->GetTransform().TranslatePart());
	frustum.SetFront(GetOwner()->GetTransform().WorldZ());
	frustum.SetUp(GetOwner()->GetTransform().WorldY());
}

void Camera::SetAspectRatio(float aspect)
{
	float r = DEGTORAD * aspect;

	if (r > 0.0f)
	{
		frustum.SetVerticalFovAndAspectRatio(fov*DEGTORAD, aspect);
		aspect_ratio = aspect;
	}
}

void Camera::SetFOV(float angle)
{
	frustum.SetVerticalFovAndAspectRatio(angle*DEGTORAD, aspect_ratio);
	fov = angle;
}

void Camera::SetCamera(float fov_angle, float aspect_ratio, float near_dist, float far_dist)
{
	float r = DEGTORAD * aspect_ratio;

}

float * Camera::GetViewMatrix() const
{
	static float4x4 m;

	m = frustum.ViewMatrix();
	m.Transpose();
	return (float*)m.v;
}

float * Camera::GetProjectionMatrix() const
{
	static float4x4 m;

	m = frustum.ProjectionMatrix();
	m.Transpose();
	return (float*)m.v;
}

void Camera::DrawConfig()
{	
	if (ImGui::CollapsingHeader("Camera"))
	{
		
	}
}

void Camera::DebugDraw()
{
	GLenum mode = App->renderer3D->GetPolyMode();
	App->renderer3D->PolygonModeWireframe();

	vec* points = nullptr;
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
