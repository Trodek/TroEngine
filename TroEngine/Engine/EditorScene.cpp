#include "Globals.h"
#include "Application.h"
#include "EditorScene.h"
#include "ModuleCamera3D.h"
#include "Primitive.h"
#include "imgui.h"

EditorScene::EditorScene(bool start_enabled) : Module(start_enabled)
{
}

EditorScene::~EditorScene()
{}

// Load assets
bool EditorScene::Start()
{
	EDITOR_LOG("Loading Intro assets");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));

	return ret;
}

// Load assets
bool EditorScene::CleanUp()
{
	EDITOR_LOG("Unloading Intro scene");

	return true;
}

// Update
update_status EditorScene::Update(float dt)
{
	PPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	return UPDATE_CONTINUE;
}

void EditorScene::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	
}