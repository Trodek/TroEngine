#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "SceneManager.h"
#include "ModuleGUI.h"
#include "JSONManager.h"
#include "glew-2.1.0\include\GL\glew.h"
#include "SDL\include\SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Brofiler\Brofiler.h"
#include "imgui.h"

#pragma comment (lib, "engine/glew-2.1.0/libx86/glew32.lib")    /* link Glew lib     */
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(bool start_enabled) : Module(start_enabled)
{
	SetName("renderer");
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Awake(JSONDoc* config)
{
	EDITOR_LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		EDITOR_LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Init Glew lib
		GLenum err = glewInit();
		if (err != GL_NO_ERROR)
		{
			EDITOR_LOG("Error initializing Glew! %s\n", glewGetErrorString(err));
			ret = false;
		}
		else
			EDITOR_LOG("Using Glew %s.", glewGetString(GLEW_VERSION));

		//Use Vsync
		vsync = config->GetBool("renderer.vsync");
		if (vsync && SDL_GL_SetSwapInterval(1) < 0)
			EDITOR_LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			EDITOR_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			EDITOR_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			EDITOR_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		depth_test = true;
		glEnable(GL_CULL_FACE);
		cull_face = true;
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		lighting = true;
		glEnable(GL_COLOR_MATERIAL);
		color_material = true;
	}
	

	// Projection matrix for
	OnResize(App->window->GetWidth(), App->window->GetHeight());

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	BROFILER_CATEGORY("Render PostUpdate", Profiler::Color::Green);

	//Rendering pipeline
	App->scene_manager->DrawScenes();
	//Draw debug

	//Draw GUI
	if(lighting == true)		//Disable Lighting befor drawing gui
		ToggleLightingState(); 
	App->gui->RenderGUI();
	if (lighting == true)		//Enable Lighting after drawing gui if needed
		ToggleLightingState();

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

void ModuleRenderer3D::SaveConfig(JSONDoc * config)
{
	config->SetBool("renderer.vsync", vsync);
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	EDITOR_LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::ConfigGUI()
{
	if (ImGui::CollapsingHeader("Renderer"))
	{
		//Get hardware capavilities
		ImGui::Text("Vendor: %s", glGetString(GL_VENDOR));
		ImGui::Text("Renderer: %s", glGetString(GL_RENDERER));
		ImGui::Text("OpenGL version supported %s", glGetString(GL_VERSION));
		ImGui::Text("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

		ImGui::Separator();

		if (ImGui::Checkbox("VSYNC", &vsync))
			ToggleVSYNC();

		ImGui::Separator();
		
		ImGui::Text("Draw Modes:");

		if (ImGui::Checkbox("Wireframe Mode", &wireframe))
			PolygonModeWireframe();	

		ImGui::SameLine();
		if (ImGui::Checkbox("Points Mode", &points))
			PolygonModePoints();

		if (ImGui::Checkbox("Fill Mode", &fill))
			PolygonModeFill();

		ImGui::Separator();

		ImGui::Text("OpenGL Capabilities:");

		if (ImGui::Checkbox("Depth Test", &depth_test))
			ToggleDepthTestState();

		ImGui::SameLine();
		if (ImGui::Checkbox("Cull Face", &cull_face))
			ToggleCullFaceState();

		if (ImGui::Checkbox("Lighting", &lighting))
			ToggleLightingState();

		ImGui::SameLine();
		if (ImGui::Checkbox("Texture 2D", &texture_2d))
			ToggleTexture2DState();

		if (ImGui::Checkbox("Color Material", &color_material))
			ToggleColorMaterialState();



	}
}

void ModuleRenderer3D::PolygonModePoints()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	wireframe = false;
	points = true;
	fill = false;
}

void ModuleRenderer3D::PolygonModeWireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	wireframe = true;
	points = false;
	fill = false;
}

void ModuleRenderer3D::PolygonModeFill()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	wireframe = false;
	points = false;
	fill = true;
}

void ModuleRenderer3D::ToggleDepthTestState()
{
	if (depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void ModuleRenderer3D::ToggleCullFaceState()
{
	if (cull_face)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
}

void ModuleRenderer3D::ToggleLightingState()
{
	if (lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
}

void ModuleRenderer3D::ToggleTexture2DState()
{
	if (texture_2d)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

void ModuleRenderer3D::ToggleColorMaterialState()
{
	if (color_material)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);
}

void ModuleRenderer3D::ToggleVSYNC()
{
	if (vsync)
	{
		if (SDL_GL_SetSwapInterval(1) == 0)
			EDITOR_LOG("VSYNC active");
	}
	else
	{
		if (SDL_GL_SetSwapInterval(0) == 0)
			EDITOR_LOG("VSYNC disabled");
	}
}

void ModuleRenderer3D::SetCurrentPolygonMode()
{
	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	if (points)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	}
	if (fill)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
