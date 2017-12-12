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

ComponentMaterial::ComponentMaterial(GameObject* owner) : Component(C_Material, owner)
{
	//set default shaders
	fragment_shader = App->shader_manager->GetFragmentDefaultShader();
	vertex_shader = App->shader_manager->GetVertexDefaultShader();

	UpdateShaderProgram();
}

ComponentMaterial::ComponentMaterial(GameObject * owner, Material * mat) : Component(C_Material, owner), material(mat)
{
	//set default shaders
	fragment_shader = App->shader_manager->GetFragmentDefaultShader();
	vertex_shader = App->shader_manager->GetVertexDefaultShader();

	UpdateShaderProgram();
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

void ComponentMaterial::SetVertexShader(Shader * vertex_shader)
{
	if (vertex_shader != nullptr)
		this->vertex_shader = vertex_shader;
}

void ComponentMaterial::SetVertexShader(uint shader_uid)
{
	if (shader_uid != 0)
	{
		Resource* res = App->resources->GetResource(shader_uid);
		if (res != nullptr)
		{
			Shader* shader = App->shader_manager->GetShader(res->manager_id);
			if (shader->GetType() == ST_VERTEX)
				vertex_shader = shader;
		}
	}
}

void ComponentMaterial::SetFragmentShader(Shader * fragment_shader)
{
	if (fragment_shader != nullptr)
		this->fragment_shader = fragment_shader;
}

void ComponentMaterial::SetFragmentShader(uint shader_uid)
{
	if (shader_uid != 0)
	{
		Resource* res = App->resources->GetResource(shader_uid);
		if (res != nullptr)
		{
			Shader* shader = App->shader_manager->GetShader(res->manager_id);
			if (shader->GetType() == ST_FRAGMENT)
				fragment_shader = shader;
		}
	}
}

void ComponentMaterial::OnShaderEdit(Shader * shader)
{
	if (shader == vertex_shader || shader == fragment_shader) //the modified shader affects our program, update it
	{
		UpdateShaderProgram();
	}
}

void ComponentMaterial::UpdateShaderProgram()
{
	if (shader_program != 0)
	{
		App->renderer3D->DeleteProgram(shader_program);
		shader_program = 0;
	}

	shader_program = App->renderer3D->CreateShaderProgram();
	App->renderer3D->AttachShaderToProgram(shader_program, vertex_shader->GetShaderID());
	App->renderer3D->AttachShaderToProgram(shader_program, fragment_shader->GetShaderID());
	if (App->renderer3D->LinkProgram(shader_program) == false)
	{
		EDITOR_LOG("Error while updating shader program, check errors above.");
		App->renderer3D->DeleteProgram(shader_program);
		shader_program = 0;
	}
	else EDITOR_LOG("Shader Program %d created :)", shader_program);
}

uint ComponentMaterial::GetProgram() const
{
	return shader_program;
}

void ComponentMaterial::UseShader() const
{
	bool a = glIsProgram(shader_program);

	bool b = App->renderer3D->LinkProgram(shader_program);

	glValidateProgram(shader_program);

	GLenum error = glGetError();

	//Check for error
	if (error != GL_NO_ERROR)
	{
		EDITOR_LOG("Error at validate shader program: %s\n", gluErrorString(error));
	}

	App->renderer3D->UseShaderProgram(shader_program);
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
			ImGui::Text("No material assigned");
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

	if (vertex_shader != nullptr)
		doc->SetNumber("vertex_shader", vertex_shader->GetUID());
	else
		doc->SetNumber("vertex_shader", 0);

	if (fragment_shader != nullptr)
		doc->SetNumber("fragment_shader", fragment_shader->GetUID());
	else
		doc->SetNumber("fragment_shader", 0);
}
