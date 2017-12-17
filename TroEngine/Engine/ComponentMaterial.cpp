#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Material.h"
#include "MaterialManager.h"
#include "GameObject.h"
#include "imgui.h"
#include "JSONManager.h"
#include "ResourceManager.h"
#include "ShaderManager.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "ModuleGUI.h"
#include "ShaderEditor.h"

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(C_Material, owner)
{
	//Set default program
	shaders = App->shader_manager->GetDefaultShaderProgram();
}

ComponentMaterial::ComponentMaterial(GameObject * owner, Material * mat) : Component(C_Material, owner), material(mat)
{
	//Set default program
	shaders = App->shader_manager->GetDefaultShaderProgram();
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::CleanUp()
{
	material = nullptr;
}

void ComponentMaterial::SetMaterial(Material * new_mat)
{
	if (material != nullptr) //just for now
		CleanUp();
	material = new_mat;
}

void ComponentMaterial::SetMaterial(uint mat_id)
{
	if (mat_id != 0) //invalid id
	{
		Resource* res = App->resources->GetResource(mat_id);
		if(res!=nullptr)
			material = App->materials->GetMaterial(res->manager_id);
	}
}

void ComponentMaterial::ApplyMaterial() const
{
	if (use_checker)
		App->renderer3D->SetCheckerTexture();
	else if (material != nullptr)
		App->renderer3D->BindTexure(material->GetMaterialID());
}

void ComponentMaterial::SetVertexShader(Shader * vertex_shader) //FIX
{
	if (vertex_shader != nullptr)
	{
		//look for a program that uses vertex_shader and curr fragment shader
		ShaderProgram* p = App->shader_manager->GetProgram(vertex_shader, shaders->GetFragmentShader());
		
		if (p == nullptr)//program doesn't exist, create it
		{
			p = new ShaderProgram(vertex_shader, shaders->GetFragmentShader());
			App->shader_manager->AddShaderProgram(p);
		}
		shaders = p;
	}
}

void ComponentMaterial::SetVertexShader(uint shader_uid)
{
	if (shader_uid != 0)
	{
		Shader* s = App->shader_manager->GetShaderByUID(shader_uid);
		if (s != nullptr && s->GetType() == ST_VERTEX)
			SetVertexShader(s);
	}
}

void ComponentMaterial::SetFragmentShader(Shader * fragment_shader) //FIX
{
	if (fragment_shader != nullptr)
	{
		//look for a program that uses fragment_shader and curr vertex shader
		ShaderProgram* p = App->shader_manager->GetProgram(shaders->GetVertexShader(), fragment_shader);

		if (p == nullptr)//program doesn't exist, create it
		{
			p = new ShaderProgram(shaders->GetVertexShader(), fragment_shader);
			App->shader_manager->AddShaderProgram(p);
		}
		shaders = p;
	}
}

void ComponentMaterial::SetFragmentShader(uint shader_uid)
{
	if (shader_uid != 0)
	{
		Shader* s = App->shader_manager->GetShaderByUID(shader_uid);
		if (s != nullptr && s->GetType() == ST_FRAGMENT)
			SetFragmentShader(s);
	}
}

void ComponentMaterial::UpdateShaderProgram()
{
	shaders->LinkShaderProgram();
}

uint ComponentMaterial::GetProgram() const
{
	return shaders->GetProgramID();
}

void ComponentMaterial::UseShader() const
{
	shaders->UseProgram();
}

void ComponentMaterial::DrawConfig()
{
	if (ImGui::CollapsingHeader("Material"))
	{
		if (ImGui::Button("Delete"))
			GetOwner()->RemoveComponent(this);
		if (material != nullptr)
		{
			ImGui::Checkbox("Use Checker Material", &use_checker);

			ImGui::LabelText("ID##material", "%d", material->GetMaterialID());
			ImGui::LabelText("Width##material", "%d", material->GetWidth());
			ImGui::LabelText("Height##material", "%d", material->GetHeight());
			ImGui::LabelText("Path##material", "%s", material->GetPath().c_str());
		}
		else
		{
			ImGui::Text("No material assigned");
		}

		ImGui::Separator();
		ImGui::Text("Shaders");

		//vertex shaders
		if (ImGui::Button("Vertex Shader"))
			ImGui::OpenPopup("VertShaders##pop");

		char txt[100];
		sprintf(txt, "shader %d", selected_vert_shader);

		ImGui::SameLine();
		ImGui::Text(selected_vert_shader == -1 ? "<None>" : txt);
		if (ImGui::BeginPopup("VertShaders##pop"))
		{
			ImGui::Text("Available Shaders");
			ImGui::Separator();
			for (int i = 0; i < App->shader_manager->ShadersCount(); ++i)
			{
				char name[50];
				switch (App->shader_manager->GetShader(i)->GetType())
				{
				case ST_VERTEX:
					sprintf(name, "vertex shader %d", i);
					break;
				case ST_FRAGMENT:
					sprintf(name, "fragment shader %d", i);
					break;
				default:
					break;
				}
				if (ImGui::Selectable(name))
				{
					selected_vert_shader = i;
					SetVertexShader(App->shader_manager->GetShader(i));
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("New##vshader"))
		{
			Shader* shader = new Shader(ST_VERTEX);
			App->shader_manager->AddShader(shader);
			SetVertexShader(shader);
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit##vshader"))
		{
			App->gui->shader_editor->SetShader(shaders->GetVertexShader());
			App->gui->shader_editor->active = true;
		}

		//fragment shader
		if (ImGui::Button("Fragment Shader"))
			ImGui::OpenPopup("FragShaders##pop");

		sprintf(txt, "shader %d", selected_frag_shader);

		ImGui::SameLine();
		ImGui::Text(selected_frag_shader == -1 ? "<None>" : txt);
		if (ImGui::BeginPopup("FragShaders##pop"))
		{
			ImGui::Text("Available Shaders");
			ImGui::Separator();
			for (int i = 0; i < App->shader_manager->ShadersCount(); ++i)
			{
				char name[50];
				switch (App->shader_manager->GetShader(i)->GetType())
				{
				case ST_VERTEX:
					sprintf(name, "vertex shader %d", i);
					break;
				case ST_FRAGMENT:
					sprintf(name, "fragment shader %d", i);
					break;
				default:
					break;
				}
				if (ImGui::Selectable(name))
				{
					selected_frag_shader = i;
					SetFragmentShader(App->shader_manager->GetShader(i));
				}
			}
			ImGui::EndPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("New##fshader"))
		{
			Shader* shader = new Shader(ST_FRAGMENT);
			App->shader_manager->AddShader(shader);
			SetFragmentShader(shader);
		}
		ImGui::SameLine();
		if (ImGui::Button("Edit##fshader"))
		{
			App->gui->shader_editor->SetShader(shaders->GetFragmentShader());
			App->gui->shader_editor->active = true;
		}
	}
}

void ComponentMaterial::UseChecker()
{
	use_checker = true;
}

void ComponentMaterial::Serialize(JSONDoc * doc)
{
	doc->AddSectionToArray("Components");
	doc->MoveToSectionFromArray("Components", doc->GetArraySize("Components") - 1);

	doc->SetNumber("type", GetType());
	doc->SetNumber("owner", GetOwner()->GetUID());
	if (material != nullptr)
		doc->SetNumber("material", material->GetUID());
	else
		doc->SetNumber("material", 0);

	if (shaders != nullptr)
		doc->SetNumber("vertex_shader", shaders->GetVertexShader()->GetUID());
	else
		doc->SetNumber("vertex_shader", 0);

	if (shaders != nullptr)
		doc->SetNumber("fragment_shader", shaders->GetFragmentShader()->GetUID());
	else
		doc->SetNumber("fragment_shader", 0);
}
