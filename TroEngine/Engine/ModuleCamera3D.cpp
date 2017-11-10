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
		cam->RotateCamera(-App->input->GetMouseXMotion()*mouse_sensivility, -App->input->GetMouseYMotion()*mouse_sensivility);
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
			cam->OrbitCamera(float3(0,0,0), -App->input->GetMouseXMotion()*mouse_sensivility, -App->input->GetMouseYMotion()*mouse_sensivility);
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		cam->FocusCamera(float3(0,0,0), 10);
	}
	if (App->input->GetKey(SDL_SCANCODE_V) == KEY_DOWN)
	{
		cam->LookAt(float3(0,0,0));
	}

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return cam->GetViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::AdjustCameraToAABB(AABB & bb)
{
	float distance = (bb.Size().Length() *1.5f < MAX_ADJUST_DISTANCE) ? bb.Size().Length() *1.5f : MAX_ADJUST_DISTANCE;
	FocusCamera(vec3(0, 0, 0), distance);
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

void ModuleCamera3D::MoveFront(const float & speed)
{
	vec3 move(0, 0, 0);

	move -= Z*speed;
	Position += move;
	Reference += move;

}

void ModuleCamera3D::MoveBack(const float & speed)
{
	vec3 move(0, 0, 0);

	move += Z*speed;
	Position += move;
	Reference += move;
}

void ModuleCamera3D::MoveRight(const float & speed)
{
	vec3 move(0, 0, 0);

	move += X*speed;
	Position += move;
	Reference += move;
}

void ModuleCamera3D::MoveLeft(const float & speed)
{
	vec3 move(0, 0, 0);

	move -= X*speed;
	Position += move;
	Reference += move;
}

void ModuleCamera3D::MoveUp(const float & speed)
{
	vec3 move(0, 0, 0);

	move += Y*speed;
	Position += move;
	Reference += move;
}

void ModuleCamera3D::MoveDown(const float & speed)
{
	vec3 move(0, 0, 0);

	move -= Y*speed;
	Position += move;
	Reference += move;
}

void ModuleCamera3D::OrbitCamera(const vec3 & orbit_center, const float & mouse_dx, const float & mouse_dy)
{
	Reference = orbit_center;
	Position -= Reference;

	//Rotate arround y axis
	if (mouse_dx != 0.f)
	{
		float delta_x = -mouse_dx*mouse_sensivility;

		X = rotate(X, delta_x, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, delta_x, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, delta_x, vec3(0.0f, 1.0f, 0.0f));
	}

	if (mouse_dy != 0)// Rotate arround the X direction
	{
		float delta_y = -mouse_dy * mouse_sensivility;
		
		Y = rotate(Y, delta_y, X);
		Z = rotate(Z, delta_y, X);
	}

	Position = Reference + Z * length(Position);
}

void ModuleCamera3D::RotateCamera(const float & mouse_dx, const float & mouse_dy)
{
	Position -= Reference;

	if (mouse_dx != 0)
	{
		float delta_x = -mouse_dx * mouse_sensivility;

		X = rotate(X, delta_x, vec3(0.0f, 1.0f, 0.0f));
		Y = rotate(Y, delta_x, vec3(0.0f, 1.0f, 0.0f));
		Z = rotate(Z, delta_x, vec3(0.0f, 1.0f, 0.0f));
	}

	if (mouse_dy != 0)
	{
		float delta_y = -mouse_dy * mouse_sensivility;

		Y = rotate(Y, delta_y, X);
		Z = rotate(Z, delta_y, X);

	}

	Position = Reference + Z * length(Position);
}

void ModuleCamera3D::FocusCamera(const vec3 & focus_point, float distance)
{
	Position = focus_point + Z*distance;

	LookAt(focus_point);
}
