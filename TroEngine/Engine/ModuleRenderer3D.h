#pragma once
#include "Module.h"
#include "Globals.h"
#include "GLInclude.h"
#include "Light.h"
#include "Color.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(bool start_enabled = true);
	~ModuleRenderer3D();

	bool Awake(JSONDoc* config);
	bool Start();
	update_status PreUpdate(float dt);
	update_status PostUpdate(float dt);
	void SaveConfig(JSONDoc* config);
	bool CleanUp();

	void OnResize(int width, int height);

	void ConfigGUI();

	uint GenBuffer() const;
	void BindArrayBuffer(uint id) const;
	void BindElementArrayBuffer(uint id) const;
	void RenderElement(uint num_indices) const;
	void UnbindArraybuffer() const;
	void UnbindElementArrayBuffer() const;
	void EnableState(GLenum type) const;
	void DisableState(GLenum type) const;
	void SetVertexPointer() const;
	void SetNormalsPointer() const;
	void SetCheckerTexture() const;
	void BindTexure(uint id) const;
	void UnbindTexture() const;

	void SetTexCoordPointer();

	void LoadArrayToVRAM(uint size, float* values, GLenum type = GL_STATIC_DRAW) const;
	void LoadArrayToVRAM(uint size, uint* values, GLenum type = GL_STATIC_DRAW) const;
	uint LoadTextureToVRAM(uint w, uint h, GLubyte* tex_data, GLint format) const;

	GLenum GetPolyMode()const;
	void SetPolyMode(GLenum mode);
	void PolygonModePoints();
	void PolygonModeWireframe();
	void PolygonModeFill();

	void PushMatrix();
	void PopMatrix();
	void MultMatrix(float* matrix);

	void SetLineWidth(float size);

	void SetColor(Color color);
	void ResetColor();

	bool GetCullFace()const;

	uint CreateVertexShader(const char* source);
	uint CreateFragmentShader(const char* source);
	void DeleteShader(uint shader_id);

	void EnableVertexAttributeArray(uint id);
	void DisableVertexAttributeArray(uint id);
	void SetVertexAttributePointer(uint id, uint element_size, uint elements_gap, uint infogap);

	void UseShaderProgram(uint id);
	void SetUniformMatrix(uint program, const char* name, float* data);
	void SetUniformForViewAndProjection(uint program, const char* view_name, const char* proj_name);

	uint CreateShaderProgram();
	void AttachShaderToProgram(uint program_id, uint shader_id);
	bool LinkProgram(uint program_id);
	void DeleteProgram(uint program_id);

private:
	void ToggleDepthTestState();
	void ToggleCullFaceState();
	void ToggleLightingState();
	void ToggleTexture2DState();
	void ToggleColorMaterialState();
	void ToggleVSYNC();

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
	GLenum poly_mode    = GL_FILL;

	//capabilities
	bool depth_test		= false;
	bool cull_face		= false;
	bool lighting		= false;
	bool texture_2d		= false;
	bool color_material = false;

	bool vsync			= false;

	uint checker_id		= 0;

};