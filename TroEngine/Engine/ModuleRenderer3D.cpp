#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleWindow.h"
#include "SceneManager.h"
#include "ModuleGUI.h"
#include "JSONManager.h"
#include "GLInclude.h"
#include "imgui.h"

#include "MeshImporter.h"

#pragma comment (lib, "engine/glew-2.1.0/libx86/glew32.lib")    /* link Glew lib     */
#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

#define CHECKERS_HEIGHT 256
#define CHECKERS_WIDTH 256

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

		//Check for error
		GLenum error = glGetError();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			EDITOR_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_FRAGMENT_SHADER_DERIVATIVE_HINT, GL_NICEST);
		glHint(GL_TEXTURE_COMPRESSION_HINT, GL_NICEST);
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

		glEnable(GL_DEPTH_TEST);
		depth_test = true;
		glEnable(GL_CULL_FACE);
		cull_face = true;
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			EDITOR_LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
	}
	

	// Projection matrix for
	OnResize(App->window->GetWidth(), App->window->GetHeight());

	return ret;
}

bool ModuleRenderer3D::Start()
{
	//create checker texture
	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}
	checker_id = LoadTextureToVRAM(CHECKERS_WIDTH, CHECKERS_HEIGHT, &checkImage[0][0][0], GL_RGBA);

	///TESTESTESTESTESTSETESTEST
	uint test = GenVertexArrayBuffer();
	BindVertexArrayBuffer(test);

	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// light 0 on cam pos
	lights[0].SetPos(App->camera->GetPos().x, App->camera->GetPos().y, App->camera->GetPos().z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	//Rendering pipeline
	if (!App->debug_mode) // for debug proposes avoid drawing geometry, just the debug boxes
	{
		App->camera->SetDrawOnFrustumElements();
		App->scene_manager->DrawScenes();
	}

	//Draw debug
	bool cull_face_state = cull_face; //disable cull face when in debug
	if (cull_face == true)
	{
		cull_face = false;
		ToggleCullFaceState();
	}
	if(App->debug_mode)
		App->scene_manager->DebugDrawScenes();
	cull_face = cull_face_state;
	ToggleCullFaceState();

	//Draw GUI
	App->gui->RenderGUI();
	

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
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error glviewport %s\n", gluErrorString(error));
	}

	App->camera->Resize((float)width / (float)height);
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

		//ImGui::SameLine();
		//if (ImGui::Checkbox("Texture 2D", &texture_2d))
		//	ToggleTexture2DState();

		if (ImGui::Checkbox("Color Material", &color_material))
			ToggleColorMaterialState();



	}
}

uint ModuleRenderer3D::GenBuffer() const
{
	uint ret = 0;
	glGenBuffers(1, (GLuint*)&ret);
	return ret;
}

void ModuleRenderer3D::BindArrayBuffer(uint id) const
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error bind array buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::BindElementArrayBuffer(uint id) const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error bind buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::RenderElement(uint num_indices) const
{
	glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, NULL);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error draw elements: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UnbindArraybuffer() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error unbind array buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UnbindElementArrayBuffer() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error unbind buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::EnableState(GLenum type) const
{
	glEnableClientState(type);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error enable state: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::DisableState(GLenum type) const
{
	glDisableClientState(type);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error disable state: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::SetVertexPointer() const
{
	glVertexPointer(3, GL_FLOAT, 0, NULL);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error set vertex pointer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::SetNormalsPointer() const
{
	glNormalPointer(GL_FLOAT, 0, NULL);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error set normals pointer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::SetCheckerTexture() const
{
	glBindTexture(GL_TEXTURE_2D, checker_id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error set checker txture: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::BindTexure(uint id) const
{
	glBindTexture(GL_TEXTURE_2D, id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error bind texture: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UnbindTexture() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error unbind texture: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::SetTexCoordPointer()
{
	glTexCoordPointer(3, GL_FLOAT, 0, NULL);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error set texcoord pointer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::LoadArrayToVRAM(uint size, float * values, GLenum type) const
{
	glBufferData(GL_ARRAY_BUFFER, size, values, type);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error load array to vram: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::LoadArrayToVRAM(uint size, uint * values, GLenum type) const
{
	glBufferData(GL_ARRAY_BUFFER, size, values, type);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error load array to vram: %s\n", gluErrorString(error));
	}
}

uint ModuleRenderer3D::LoadTextureToVRAM(uint w, uint h, GLubyte * tex_data, GLint format) const
{
	uint buff_id = 0;

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error load texture to vram e1: %s\n", gluErrorString(error));
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &buff_id);
	glBindTexture(GL_TEXTURE_2D, buff_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, tex_data);

	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error load texture to vram e2: %s\n", gluErrorString(error));
	}

	return buff_id;
}

GLenum ModuleRenderer3D::GetPolyMode() const
{
	return poly_mode;
}

void ModuleRenderer3D::SetPolyMode(GLenum mode)
{
	if (mode == GL_FILL)
	{
		PolygonModeFill();
	}
	else if (mode == GL_LINE)
	{
		PolygonModeWireframe();
	}
	else if (mode == GL_POINT)
	{
		PolygonModePoints();
	}
}

void ModuleRenderer3D::PolygonModePoints()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error polygon mode points: %s\n", gluErrorString(error));
	}

	wireframe = false;
	points = true;
	fill = false;
	poly_mode = GL_POINT;
}

void ModuleRenderer3D::PolygonModeWireframe()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error polygon mode line: %s\n", gluErrorString(error));
	}

	wireframe = true;
	points = false;
	fill = false;
	poly_mode = GL_LINE;
}

void ModuleRenderer3D::PolygonModeFill()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error polygon mode fill: %s\n", gluErrorString(error));
	}

	wireframe = false;
	points = false;
	fill = true;
	poly_mode = GL_FILL;
}

void ModuleRenderer3D::PushMatrix()
{
	glPushMatrix();
}

void ModuleRenderer3D::PopMatrix()
{
	glPopMatrix();
}

void ModuleRenderer3D::MultMatrix(float * matrix)
{
	glMultMatrixf(matrix);
}

void ModuleRenderer3D::SetLineWidth(float size)
{
	glLineWidth(size);
}

void ModuleRenderer3D::SetColor(Color color)
{
	glColor4f(color.r, color.g, color.b, color.a);
}

void ModuleRenderer3D::ResetColor()
{
	glColor4f(1.f, 1.f, 1.f, 1.f);
}

bool ModuleRenderer3D::GetCullFace() const
{
	return cull_face;
}

//-------- Shaders modification ------------

uint ModuleRenderer3D::GenVertexArrayBuffer() const
{
	uint ret = 0;
	glGenVertexArrays(1, (GLuint*)&ret);
	return ret;
}

void ModuleRenderer3D::BindVertexArrayBuffer(uint id) const
{
	glBindVertexArray(id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error bind vertex array buffer: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UnbindVertexArrayBuffer() const
{
	glBindVertexArray(0);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error unbind array buffer: %s\n", gluErrorString(error));
	}
}

uint ModuleRenderer3D::CreateVertexShader(const char * source)
{
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &source, NULL);
	glCompileShader(vertexShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		EDITOR_LOG("Shader compilation error:\n %s", infoLog);
		glDeleteShader(vertexShader);
		return 0;
	}
	return vertexShader;
}

uint ModuleRenderer3D::CreateFragmentShader(const char * source)
{
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &source, NULL);
	glCompileShader(fragmentShader);
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (success == 0)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		EDITOR_LOG("Shader compilation error:\n %s", infoLog);
		glDeleteShader(fragmentShader);
		return 0;
	}
	return fragmentShader;
}

void ModuleRenderer3D::DeleteShader(uint shader_id)
{
	if (shader_id != 0)
	{
		glDeleteShader(shader_id);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			EDITOR_LOG("Error deleting shader %s\n", gluErrorString(error));
		}
	}
}

uint ModuleRenderer3D::GetProgramBinary(uint program_id, uint buff_size, char * buff) const
{
	uint ret = 0;

	GLint formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	GLint *binaryFormats = new GLint[formats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);

	glGetProgramBinary(program_id, buff_size, (GLsizei*)&ret, (GLenum*)binaryFormats, buff);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error geting shader program binary %s\n", gluErrorString(error));
	}

	RELEASE_ARRAY(binaryFormats);

	return ret;
}

int ModuleRenderer3D::GetProgramSize(uint program_id) const
{
	int ret = 0;

	glGetProgramiv(program_id, GL_PROGRAM_BINARY_LENGTH, &ret);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error geting shader program size %s\n", gluErrorString(error));

	}

	return ret;
}

void ModuleRenderer3D::LoadProgramFromBinary(uint program_id, uint buff_size, const char * buff)
{
	GLint formats = 0;
	glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &formats);
	GLint *binaryFormats = new GLint[formats];
	glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, binaryFormats);

	glProgramBinary(program_id, (GLenum)binaryFormats, buff, buff_size);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error loading shader program binary %s\n", gluErrorString(error));

	}
}

void ModuleRenderer3D::EnableVertexAttributeArray(uint id)
{
	glEnableVertexAttribArray(id);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error enabling vertex attribute Pointer %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::DisableVertexAttributeArray(uint id)
{
	glDisableVertexAttribArray(id);
	
	//Check for error
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error disabling vertex attributePointer %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::SetVertexAttributePointer(uint id, uint element_size, uint elements_gap, uint infogap)
{
	glVertexAttribPointer(id, element_size, GL_FLOAT, GL_FALSE, elements_gap * sizeof(GLfloat), (void*)(infogap * sizeof(GLfloat)));
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error Setting vertex attributePointer %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::UseShaderProgram(uint id)
{
	glUseProgram(id);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error at use shader program: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::SetUniformMatrix(uint program, const char * name, float * data)
{
	GLint modelLoc = glGetUniformLocation(program, name);
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, data);
	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error Seting uniform matrix %s: %s\n",name, gluErrorString(error));
	}
}

void ModuleRenderer3D::SetUniformForViewAndProjection(uint program, const char * view_name, const char * proj_name)
{
	GLint modelLoc_view = glGetUniformLocation(program, view_name);
	glUniformMatrix4fv(modelLoc_view, 1, GL_FALSE, App->camera->GetViewMatrix());

	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error Seting uniform matrix %s %s\n", view_name, gluErrorString(error));
	}

	GLint modelLoc_proj = glGetUniformLocation(program, proj_name);
	glUniformMatrix4fv(modelLoc_proj, 1, GL_FALSE, App->camera->GetProjectionMatrix());

	error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error Seting uniform matrix %s %s\n", proj_name, gluErrorString(error));
	}
}

uint ModuleRenderer3D::CreateShaderProgram()
{
	uint ret = glCreateProgram();
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error creating shader program %s\n", gluErrorString(error));
	}
	return ret;
}

void ModuleRenderer3D::AttachShaderToProgram(uint program_id, uint shader_id)
{
	if (program_id != 0 && shader_id != 0)
	{
		glAttachShader(program_id, shader_id);
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			EDITOR_LOG("Error attaching shader %s\n", gluErrorString(error));
		}
	}
}

bool ModuleRenderer3D::LinkProgram(uint program_id)
{
	bool ret = true;

	if (program_id != 0)
	{
		glLinkProgram(program_id);

		GLint success;
		GLint valid;
		glGetProgramiv(program_id, GL_LINK_STATUS, &success);
		glGetProgramiv(program_id, GL_VALIDATE_STATUS, &valid);
		if (!success || !valid) {
			GLchar infoLog[512];
			glGetProgramInfoLog(program_id, 512, NULL, infoLog);
			EDITOR_LOG("Shader link error: %s", infoLog);
			ret = false;
		}
	}
	else ret = false;

	return ret;
}

void ModuleRenderer3D::DeleteProgram(uint program_id)
{
	if (program_id != 0)
	{
		glDeleteProgram(program_id);

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			EDITOR_LOG("Error deleting shader program %s\n", gluErrorString(error));
		}
	}
}

// ------------------------------------------

void ModuleRenderer3D::ToggleDepthTestState()
{
	if (depth_test)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("ToggleDepthTestState error: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::ToggleCullFaceState()
{
	if (cull_face)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("ToggleCullFaceState error: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::ToggleLightingState()
{
	if (lighting)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("ToggleLightingState error: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::ToggleTexture2DState()
{
	if (texture_2d)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("ToggleTexture2DState error: %s\n", gluErrorString(error));
	}
}

void ModuleRenderer3D::ToggleColorMaterialState()
{
	if (color_material)
		glEnable(GL_COLOR_MATERIAL);
	else
		glDisable(GL_COLOR_MATERIAL);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("ToggleColorMaterialState error: %s\n", gluErrorString(error));
	}
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
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("ToggleVsyncState error: %s\n", gluErrorString(error));
	}
}