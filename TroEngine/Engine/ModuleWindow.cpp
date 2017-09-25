#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "imgui.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	window = NULL;
	screen_surface = NULL;

	title = "TroEngine";
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Awake()
{
	EDITOR_LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		EDITOR_LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		width = SCREEN_WIDTH * SCREEN_SIZE;
		height = SCREEN_HEIGHT * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
			fullscreen = true;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
			fullscreen_desktop = true;
		}

		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);

		if(window == NULL)
		{
			EDITOR_LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);
		}

		if (!fullscreen && !fullscreen_desktop)
			windowed = true;
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	EDITOR_LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
	this->title = title;
}

void ModuleWindow::ConfigGUI()
{
	if (ImGui::CollapsingHeader("Window"))
	{
		//Sliders
		if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f, "%.2f"))
		{
			ChangeBrightness(brightness);
		}
		if (ImGui::SliderInt("Width", &width, 720, 4096))
		{
			SetWindowSize(width, height);
		}
		if (ImGui::SliderInt("Height", &height, 480, 2304))
		{
			SetWindowSize(width, height);
		}
		//Checkboxes
		if (ImGui::Checkbox("Fullscreen", &fullscreen))
			SetFullScreen();
		ImGui::SameLine();
		if (ImGui::Checkbox("Resizable", &resizable))
		{}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Not Working Yet.");
		if (ImGui::Checkbox("Borderless", &borderless))
			SetBorderless();
		ImGui::SameLine();
		if (ImGui::Checkbox("FullScreen Desktop", &fullscreen_desktop))
		{
			SetFullScreenDesktop();
		}
		if (ImGui::Checkbox("Windowed", &windowed))
			SetWindowed();
	}
}

void ModuleWindow::ChangeBrightness(float brightness)
{
	SDL_SetWindowBrightness(window, brightness);
}

void ModuleWindow::SetFullScreen()
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	windowed = false;
	fullscreen_desktop = false;
	fullscreen = true;
}

void ModuleWindow::SetFullScreenDesktop()
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	windowed = false;
	fullscreen_desktop = true;
	fullscreen = false;
}

void ModuleWindow::SetWindowed()
{
	SDL_SetWindowFullscreen(window, 0);
	windowed = true;
	fullscreen_desktop = false;
	fullscreen = false;
}

void ModuleWindow::SetWindowSize(int w, int h)
{
	SDL_SetWindowSize(window, w, h);
	App->renderer3D->OnResize(w, h);
}

void ModuleWindow::SetBorderless()
{
	if(borderless)
		SDL_SetWindowBordered(window, SDL_TRUE);
	else
		SDL_SetWindowBordered(window, SDL_FALSE);
}
