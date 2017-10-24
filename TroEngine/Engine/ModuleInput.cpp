#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "MeshImporter.h"
#include "MaterialManager.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Scene.h"
#include "SceneManager.h"
#include "Component.h"
#include "ComponentMaterial.h"

#define MAX_KEYS 300

ModuleInput::ModuleInput(bool start_enabled) : Module(start_enabled)
{
	keyboard = new KEY_STATE[MAX_KEYS];
	memset(keyboard, KEY_IDLE, sizeof(KEY_STATE) * MAX_KEYS);
	memset(mouse_buttons, KEY_IDLE, sizeof(KEY_STATE) * MAX_MOUSE_BUTTONS);

	SetName("input");
}

// Destructor
ModuleInput::~ModuleInput()
{
	delete[] keyboard;
}

// Called before render is available
bool ModuleInput::Awake(JSONDoc* config)
{
	EDITOR_LOG("Init SDL input event system");
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		EDITOR_LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	return ret;
}

// Called every draw update
update_status ModuleInput::PreUpdate(float dt)
{
	SDL_PumpEvents();

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	
	for(int i = 0; i < MAX_KEYS; ++i)
	{
		if(keys[i] == 1)
		{
			if(keyboard[i] == KEY_IDLE)
				keyboard[i] = KEY_DOWN;
			else
				keyboard[i] = KEY_REPEAT;
		}
		else
		{
			if(keyboard[i] == KEY_REPEAT || keyboard[i] == KEY_DOWN)
				keyboard[i] = KEY_UP;
			else
				keyboard[i] = KEY_IDLE;
		}
	}

	Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

	mouse_x /= App->window->GetSize();
	mouse_y /= App->window->GetSize();
	mouse_z = 0;

	for(int i = 0; i < 5; ++i)
	{
		if(buttons & SDL_BUTTON(i))
		{
			if(mouse_buttons[i] == KEY_IDLE)
				mouse_buttons[i] = KEY_DOWN;
			else
				mouse_buttons[i] = KEY_REPEAT;
		}
		else
		{
			if(mouse_buttons[i] == KEY_REPEAT || mouse_buttons[i] == KEY_DOWN)
				mouse_buttons[i] = KEY_UP;
			else
				mouse_buttons[i] = KEY_IDLE;
		}
	}

	mouse_x_motion = mouse_y_motion = 0;

	bool quit = false;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		ImGui_ImplSdlGL2_ProcessEvent(&e);
		switch(e.type)
		{
			case SDL_MOUSEWHEEL:
			mouse_z = e.wheel.y;
			break;

			case SDL_MOUSEMOTION:
			mouse_x = e.motion.x / App->window->GetSize();
			mouse_y = e.motion.y / App->window->GetSize();

			mouse_x_motion = e.motion.xrel / App->window->GetSize();
			mouse_y_motion = e.motion.yrel / App->window->GetSize();
			break;

			case SDL_QUIT:
			quit = true;
			break;

			case SDL_WINDOWEVENT:
			{
				if(e.window.event == SDL_WINDOWEVENT_RESIZED)
					App->renderer3D->OnResize(e.window.data1, e.window.data2);
				break;
			}

			case SDL_DROPFILE:
			{
				OnFileDropped(e.drop.file);
				break;
			}
		}
	}

	if(quit == true || keyboard[SDL_SCANCODE_ESCAPE] == KEY_UP)
		return UPDATE_STOP;

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleInput::CleanUp()
{
	EDITOR_LOG("Quitting SDL input event subsystem");
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	return true;
}

void ModuleInput::OnFileDropped(const char * path)
{
	std::string file = path;
	uint cut = file.find_last_of(".");
	std::string ext = file.substr(cut + 1, file.size() - cut + 1);
	
	//check extension to do proper action
	if(ext == "fbx" ||ext == "FBX")
	{ 
		//Copy the file to Assets\Meshes
		App->CopyFileTo(path, "Assets\\Meshes");

		App->mesh->ImportFile(path);
	}
	else if (ext == "png")
	{
		//Copy the file to Assets\Materials
		App->CopyFileTo(path, "Assets\\Materials");

		App->materials->ImportImage(path);
	}
}
