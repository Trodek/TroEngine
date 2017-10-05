#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Awake(JSONDoc* config);
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	void SaveConfig(JSONDoc* config);
	bool CleanUp();

	void OnResize(int width, int height);

	void ConfigGUI();

public:
	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;

private:
	//polygon mode
	bool wireframe		= false;
	bool points			= false;
	bool fill			= true;

	//capabilities
	bool depth_test		= false;
	bool cull_face		= false;
	bool lighting		= false;
	bool texture_2d		= false;
	bool color_material = false;

	bool vsync			= false;

private:
	void PolygonModePoints();
	void PolygonModeWireframe();
	void PolygonModeFill();

	void ToggleDepthTestState();
	void ToggleCullFaceState();
	void ToggleLightingState();
	void ToggleTexture2DState();
	void ToggleColorMaterialState();
	void ToggleVSYNC();

};