#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "GameObject.h"
#include "Camera.h"
#include "ModuleWindow.h"
#include "Scene.h"
#include "KDTree.h"
#include "SceneManager.h"
#include "MeshRenderer.h"
#include "ModuleGUI.h"
#include "Inspector.h"

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
	return (main_cam_active == true) ? main_cam->GetProjectionMatrix():cam->GetProjectionMatrix();
}

float * ModuleCamera3D::GetViewMatrix() const
{
	return (main_cam_active == true) ? main_cam->GetViewMatrix():cam->GetViewMatrix();
}

float3 ModuleCamera3D::GetPos() const
{
	return cam_go->GetTransform().TranslatePart();
}

void ModuleCamera3D::Resize(float new_aspect)
{
	cam->SetAspectRatio(new_aspect);
}

void ModuleCamera3D::Pick(uint mouse_x, uint mouse_y)
{
	//convert pick to -1, 1 range (linear interpolation info here: https://en.wikipedia.org/wiki/Linear_interpolation)
	float x = -1 + ((float)2 / App->window->GetWidth())*mouse_x;
	float y = 1 + ((float)-2 / App->window->GetHeight())*mouse_y;

	//create a ray to test
	Ray ray = cam->UnProject(x, y);

	//get the list of gameobjects to test
	std::vector<GameObject*> objects_to_test;
	App->scene_manager->GetCurrentScene()->GetAllDynamicGameObjects(objects_to_test);
	///clean all objects that doesn't have aabb
	for (std::vector<GameObject*>::iterator it = objects_to_test.begin(); it != objects_to_test.end();)
	{
		if (!(*it)->HasComponent(Component::Type::C_MeshRenderer))
			it = objects_to_test.erase(it);
		else it++;
	}
	App->scene_manager->GetCurrentScene()->TestToKDTree(ray, cam->GetNearPlaneDistance(), cam->GetFarPlaneDistance(), objects_to_test);

	//test the pick ray vs the objects aabb. if they dont colide, erase from the vector
	for (std::vector<GameObject*>::iterator it = objects_to_test.begin(); it != objects_to_test.end();)
	{
		MeshRenderer* mr = (MeshRenderer*)(*it)->GetComponent(Component::Type::C_MeshRenderer);
		AABB box = mr->GetMeshAABB();
		if (!ray.Intersects(box))
			it = objects_to_test.erase(it);
		else it++;
	}

	//create segment from the ray
	LineSegment segment(ray.GetPoint(cam->GetNearPlaneDistance()), ray.GetPoint(cam->GetFarPlaneDistance()));

	GameObject* picked = nullptr;
	float distance = cam->GetFarPlaneDistance();

	for (std::vector<GameObject*>::iterator it = objects_to_test.begin(); it != objects_to_test.end(); ++it)
	{
		MeshRenderer* mr = (MeshRenderer*)(*it)->GetComponent(Component::Type::C_MeshRenderer);
		float dist;
		float3 hit;
		if (mr->TestSegmentToMesh(segment, dist, hit))
		{
			if (dist < distance)
			{
				distance = dist;
				picked = (*it);
			}
		}
	}

	if (picked != nullptr)
	{
		App->gui->inspector->selected = picked;
	}

}

void ModuleCamera3D::DrawElementsOnFrustum() const
{
	std::vector<GameObject*> elements;
	
	(main_cam_active == true) ? main_cam->GetElementsToDraw(elements) : cam->GetElementsToDraw(elements);

	for (std::vector<GameObject*>::iterator go = elements.begin(); go != elements.end(); ++go)
	{
		if ((*go)->IsActive())
			(*go)->Draw();
	}
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
			cam->OrbitCamera(float3(0,0,0), (-App->input->GetMouseXMotion()*mouse_sensivility), (-App->input->GetMouseYMotion()*mouse_sensivility));
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

	if (App->input->GetMouseButton(1) == KEY_DOWN && !ImGuizmo::IsUsing())
	{
		Pick(App->input->GetMouseX(), App->input->GetMouseY());
	}

	return UPDATE_CONTINUE;
}

