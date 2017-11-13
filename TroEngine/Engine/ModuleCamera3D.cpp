#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "Camera.h"

#define MAX_ADJUST_DISTANCE 300.f

ModuleCamera3D::ModuleCamera3D(bool start_enabled) : Module(start_enabled)
{
	cam_go = new GameObject("cam_go");
	cam = (Camera*)cam_go->AddComponent(Component::Type::Camera);

	SetName("camera");
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	EDITOR_LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	EDITOR_LOG("Cleaning camera");

	return true;
}

void ModuleCamera3D::ConfigGUI()
{
	cam->DrawConfig();
}

float * ModuleCamera3D::GetProjectionMatrix() const
{
	return cam->GetProjectionMatrix();
}

float * ModuleCamera3D::GetViewMatrix() const
{
	return cam->GetViewMatrix();
}

float3 ModuleCamera3D::GetPos() const
{
	return cam_go->GetTransform().TranslatePart();
}

void ModuleCamera3D::Resize(float new_aspect)
{
	cam->SetAspectRatio(new_aspect);
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{
	
	float cam_speed = speed*dt;

	if (App->input->GetMouseWheel() == 1)
	{
		cam->MoveFront(mouse_wheel_speed);
	}
	if (App->input->GetMouseWheel() == -1)
	{
		cam->MoveBack(mouse_wheel_speed);
	}
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		cam->RotateCamera(App->input->GetMouseXMotion()*mouse_sensivility, App->input->GetMouseYMotion()*mouse_sensivility);
	}

	if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		cam->MoveUp(cam_speed);

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		cam->MoveDown(cam_speed);

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		cam->MoveFront(cam_speed);

	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		cam->MoveBack(cam_speed);

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		cam->MoveLeft(cam_speed);

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		cam->MoveRight(cam_speed);

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT)
		{		
			cam->OrbitCamera(float3(0,0,0), (App->input->GetMouseXMotion()*mouse_sensivility), (App->input->GetMouseYMotion()*mouse_sensivility));
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		cam->FocusCamera(float3(0,0,0), 10);
	}
	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		cam->LookAt(float3(1,0,0));
	}

	return UPDATE_CONTINUE;
}

