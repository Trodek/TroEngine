#include "Globals.h"
#include "Application.h"
#include "TestScene.h"
#include "ModuleCamera3D.h"
#include "Primitive.h"
#include "imgui.h"
#include "TestScene.h"
#include "MeshImporter.h"

TestScene::TestScene(bool active) : Scene(active)
{
}

TestScene::~TestScene()
{}

// Load assets
bool TestScene::Start()
{
	EDITOR_LOG("Test Scene Start");
	bool ret = true;

	App->camera->Move(vec3(1.0f, 1.0f, 0.0f));
	App->camera->LookAt(vec3(0, 0, 0));
	
	c1.Start();
	c2.Start();

	s.Start(5, 9, 9);

	App->mesh->LoadFile("BakerHouse.fbx");

	return ret;
}

// Load assets
bool TestScene::CleanUp()
{
	EDITOR_LOG("Unloading Test scene");

	return true;
}

// Update
update_status TestScene::Update(float dt)
{
	return UPDATE_CONTINUE;
}

void TestScene::Draw()
{
	PPlane p(0, 1, 0, 0);
	p.axis = true;
	p.Render();

	//c1.Render();
	//c2.Render();

	//s.Render();
}
