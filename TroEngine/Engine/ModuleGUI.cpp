#include "Module.h"
#include "ModuleGUI.h"
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "MenuGUI.h"
#include "ConsoleGUI.h"
#include "Inspector.h"
#include "Hierarchy.h"
#include "GameObject.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "ModuleRenderer3D.h"
#include "Transform.h"
#include "PlayPauseUI.h"
#include "Explorer.h"
#include "ShaderEditor.h"

ModuleGUI::ModuleGUI(bool start_enabled) : Module(start_enabled) 
{
	SetName("gui");

	console = new ConsoleGUI();
	AddElement(console);
}

ModuleGUI::~ModuleGUI()
{}

bool ModuleGUI::Awake(JSONDoc* config)
{
	bool ret = false;

	if (ImGui_ImplSdlGL3_Init(App->window->window))
		ret = true;
	
	SetGUIColors();

	return ret;
}

bool ModuleGUI::Start()
{
	AddElement(new MenuGUI());
	inspector = new Inspector();
	AddElement(inspector);
	AddElement(new Hierarchy());
	AddElement(new PlayPauseUI());
	explorer = new Explorer();
	AddElement(explorer);
	shader_editor = new ShaderEditor();
	AddElement(shader_editor);
	return true;
}

update_status ModuleGUI::PreUpdate(float dt)
{
	ImGui_ImplSdlGL3_NewFrame(App->window->window);
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);

	return UPDATE_CONTINUE;
}

update_status ModuleGUI::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	float3 snap(2, 2, 2);

	if (inspector->selected != nullptr)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("guizmo", 0, ImVec2(App->window->GetWidth(), App->window->GetHeight()), 0.001f,ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
		ImGuizmo::SetDrawlist();
		float transformation[16];
		float4x4 t = inspector->selected->GetTransform().Transposed();
		ImGuizmo::Manipulate(App->camera->GetViewMatrix(), App->camera->GetProjectionMatrix(), gizmo_op, ImGuizmo::MODE::WORLD, t.ptr(), transformation);
		
		if (ImGuizmo::IsUsing())
		{
			float pos[3];
			float rot[3];
			float scale[3];
			ImGuizmo::DecomposeMatrixToComponents(transformation, pos, rot, scale);
			
			Transform* trans = (Transform*)inspector->selected->GetComponent(Component::Type::C_Transform);

			if (gizmo_op == ImGuizmo::TRANSLATE)
			{
				float3 translate(pos[0], pos[1], pos[2]);
				if(inspector->selected->GetParent() != nullptr)
					translate = inspector->selected->GetParent()->GetTransform().Inverted().TransformPos(translate);
				trans->Translate(translate);
			}
			else if (gizmo_op == ImGuizmo::SCALE)
			{
				float3 curr_scale(inspector->selected->GetTransform().GetScale());
				float3 scale_mod(scale[0], scale[1], scale[2]);
				if (inspector->selected->GetParent() != nullptr)
					scale_mod = inspector->selected->GetParent()->GetTransform().Inverted().TransformPos(scale_mod);
				curr_scale.x *= scale_mod.x;
				curr_scale.y *= scale_mod.y;
				curr_scale.z *= scale_mod.z;

				trans->SetScale(curr_scale);
			}
			else if (gizmo_op == ImGuizmo::ROTATE) //TODO: fix rotate with parent transform
			{
				float3 rotation(rot[0] * DEGTORAD, rot[1] * DEGTORAD, rot[2] * DEGTORAD);
				if (inspector->selected->GetParent() != nullptr)
					rotation = inspector->selected->GetParent()->GetTransform().Inverted().TransformPos(rotation);
				trans->Rotate(rotation);
			}
		}
		ImGui::End();
	}

	for (std::list<GUIElement*>::iterator ele = gui_elements.begin(); ele != gui_elements.end(); ++ele)
	{
		ret = (*ele)->UpdateGUI(dt);
	}

	if (!ImGuizmo::IsUsing())
	{
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
			gizmo_op = ImGuizmo::OPERATION::TRANSLATE;
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
			gizmo_op = ImGuizmo::OPERATION::ROTATE;
		if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
			gizmo_op = ImGuizmo::OPERATION::SCALE;
	}
	

	return ret;
}

void ModuleGUI::RenderGUI()
{
	if (inspector->selected != nullptr)
	{
		inspector->selected->DebugDraw();
	}
	ImGui::Render();
}

bool ModuleGUI::CleanUp()
{
	//clear all elements
	for (std::list<GUIElement*>::iterator ele = gui_elements.begin(); ele != gui_elements.end(); )
	{
		(*ele)->CleanUp();
		RELEASE(*ele);

		ele = gui_elements.erase(ele);
	}

	//close ImGui
	ImGui_ImplSdlGL3_Shutdown();

	return true;
}

void ModuleGUI::SetGUIColors(float _alpha)
{
	//Set GUI Style
	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0f;
	style.FrameRounding = 3.0f;
	style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
	style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
	style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
	style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
	style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

	for (int i = 0; i <= ImGuiCol_COUNT; i++)
	{
		ImVec4& col = style.Colors[i];
		float H, S, V;
		ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

		if (S < 0.1f)
		{
			V = 1.0f - V;
		}
		ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
		if (col.w < 1.00f)
		{
			col.w *= _alpha;
		}
	}
}

void ModuleGUI::AddElement(GUIElement * ele)
{
	gui_elements.push_back(ele);
}
