#include "Globals.h"
#include "Application.h"
#include "TestScene.h"
#include "ModuleCamera3D.h"
#include "Primitive.h"
#include "imgui.h"
#include "TestScene.h"
#include "GLInclude.h"

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

	glGenBuffers(1, (GLuint*) &test_id);
	glBindBuffer(GL_ARRAY_BUFFER, test_id);

	float ver[36 * 3] = { 10.f, 0.f, 0.f,
						15.f, 0.f, 5.f,
						10.f, 0.f, 5.f,
						10.f, 0.f, 0.f,
						15.f, 0.f, 0.f,
						15.f, 0.f, 5.f,
						10.f, 5.f, 5.f,
						10.f, 0.f, 5.f,
						15.f, 0.f, 5.f,
						10.f, 5.f, 5.f,
						15.f, 0.f, 5.f,
						15.f, 5.f, 5.f,
						15.f, 5.f, 5.f,
						15.f, 0.f, 5.f,
						15.f, 0.f, 0.f,
						15.f, 5.f, 5.f,
						15.f, 0.f, 0.f,
						15.f, 5.f, 0.f,
						15.f, 5.f, 0.f,
						15.f, 0.f, 0.f,
						10.f, 0.f, 0.f,
						15.f, 5.f, 0.f,
						10.f, 0.f, 0.f,
						10.f, 5.f, 0.f,
						10.f, 5.f, 0.f,
						10.f, 0.f, 0.f,
						10.f, 0.f, 5.f,
						10.f, 5.f, 5.f,
						10.f, 5.f, 0.f,
						10.f, 0.f, 5.f,
						10.f, 5.f, 0.f,
						10.f, 5.f, 5.f,
						15.f, 5.f, 5.f,
						10.f, 5.f, 0.f,
						15.f, 5.f, 5.f,
						15.f, 5.f, 0.f };

	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*36 * 3, ver, GL_STATIC_DRAW);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);


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

	//PSphere s(3.0f);
	//s.Render();

	glBegin(GL_TRIANGLES);

	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(5.f, 0.f, 5.f);
	glVertex3f(0.f, 0.f, 5.f);

	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(5.f, 0.f, 0.f);
	glVertex3f(5.f, 0.f, 5.f);

	glVertex3f(0.f, 5.f, 5.f);
	glVertex3f(0.f, 0.f, 5.f);
	glVertex3f(5.f, 0.f, 5.f);

	glVertex3f(0.f, 5.f, 5.f);
	glVertex3f(5.f, 0.f, 5.f);
	glVertex3f(5.f, 5.f, 5.f);

	glVertex3f(5.f, 5.f, 5.f);
	glVertex3f(5.f, 0.f, 5.f);
	glVertex3f(5.f, 0.f, 0.f);

	glVertex3f(5.f, 5.f, 5.f);
	glVertex3f(5.f, 0.f, 0.f);
	glVertex3f(5.f, 5.f, 0.f);

	glVertex3f(5.f, 5.f, 0.f);
	glVertex3f(5.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);

	glVertex3f(5.f, 5.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 5.f, 0.f);

	glVertex3f(0.f, 5.f, 0.f);
	glVertex3f(0.f, 0.f, 0.f);
	glVertex3f(0.f, 0.f, 5.f);

	glVertex3f(0.f, 5.f, 5.f);
	glVertex3f(0.f, 5.f, 0.f);
	glVertex3f(0.f, 0.f, 5.f);

	glVertex3f(0.f, 5.f, 0.f);
	glVertex3f(0.f, 5.f, 5.f);
	glVertex3f(5.f, 5.f, 5.f);

	glVertex3f(0.f, 5.f, 0.f);
	glVertex3f(5.f, 5.f, 5.f);
	glVertex3f(5.f, 5.f, 0.f);

	glEnd();

	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, test_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	glDrawArrays(GL_TRIANGLES, 0, 36 * 3);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}
