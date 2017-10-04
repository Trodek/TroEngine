#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include <string>
#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Awake(JSONDoc* config);
	void SaveConfig(JSONDoc* config);
	bool CleanUp();

	void SetTitle(const char* title);

	void ConfigGUI();
	int GetSize() const;
	int GetWidth() const;
	int GetHeight() const;

public:
	//The window we'll be rendering to
	SDL_Window* window;

	//The surface contained by the window
	SDL_Surface* screen_surface;

	std::string title;

private:

	void ChangeBrightness(float brightness);
	void SetFullScreen();
	void SetFullScreenDesktop();
	void SetWindowed();
	void SetWindowSize(int w, int h);
	void SetBorderless();

private:
	bool fullscreen = false;
	bool fullscreen_desktop = false;
	bool windowed = false;
	bool resizable = false;
	bool borderless = false;

	int width, height, size;
	float brightness = 1.0;
	std::string mode;
};

#endif // __ModuleWindow_H__