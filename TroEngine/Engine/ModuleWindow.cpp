#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "imgui.h"
#include "JSONManager.h"

ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	window = NULL;
	screen_surface = NULL;

	SetName("window");
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Awake(JSONDoc* config)
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
		title = config->GetString("app.title");

		size = config->GetNumber("window.screen_size");
		width = config->GetNumber("window.width") * size;
		height = config->GetNumber("window.height") * size;

		//Set SDL flag for OpenGL
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Set OpenGL Attributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		mode = config->GetString("window.window_mode");

		if (mode == "windowed")
			windowed = true;
		else if (mode == "fullscreen")
			fullscreen = true;
		else if (mode == "fullscreen_desktop")
			fullscreen_desktop = true;
		
		resizable = config->GetBool("window.resizable");
		borderless = config->GetBool("window.borderless");

		if(fullscreen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
			fullscreen = true;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(fullscreen_desktop == true)
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

			brightness = config->GetNumber("window.brightness");
			ChangeBrightness(brightness);
		}

	}

	return ret;
}

void ModuleWindow::SaveConfig(JSONDoc * config)
{
	config->SetNumber("window.width", width);
	config->SetNumber("window.height", height);
	config->SetNumber("window.screen_size", size);
	config->SetNumber("window.brightness", brightness);
	config->SetString("window.window_mode", mode.c_str());
	config->SetBool("window.resizable", resizable);
	config->SetBool("window.borderless", borderless);

	config->SetString("app.title", title.c_str());
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
		{	}
		if (ImGui::IsItemHovered())
			ImGui::SetTooltip("Requires restart");
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

int ModuleWindow::GetWidth() const
{
	return width;
}

int ModuleWindow::GetHeight() const
{
	return height;
}

int ModuleWindow::GetSize() const
{
	return size;
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
	mode = "fullscreen";
}

void ModuleWindow::SetFullScreenDesktop()
{
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	windowed = false;
	fullscreen_desktop = true;
	fullscreen = false;
	mode = "fullscreen_desktop";
}

void ModuleWindow::SetWindowed()
{
	SDL_SetWindowFullscreen(window, 0);
	windowed = true;
	fullscreen_desktop = false;
	fullscreen = false;
	mode = "windowed";
}

void ModuleWindow::SetWindowSize(int w, int h)
{
	SDL_SetWindowSize(window, w, h);
	App->renderer3D->OnResize(w, h);
}

void ModuleWindow::SetBorderless()
{
	if (borderless)
	{
		SDL_SetWindowBordered(window, SDL_TRUE);
	}
	else
		SDL_SetWindowBordered(window, SDL_FALSE);

}
