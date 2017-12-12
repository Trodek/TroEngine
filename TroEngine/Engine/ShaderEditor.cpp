#include "ShaderEditor.h"
#include "Shader.h"
#include "imgui.h"
#include "Application.h"
#include "Scene.h"
#include "SceneManager.h"

ShaderEditor::ShaderEditor()
{
}

ShaderEditor::~ShaderEditor()
{
}

update_status ShaderEditor::UpdateGUI(float dt)
{
	if (active)
		CreateShaderEditor();

	return UPDATE_CONTINUE;
}

void ShaderEditor::CreateShaderEditor()
{
	ImGui::Begin("Shader Editor", &active);

	if (curr_shader != nullptr)
	{
		curr_shader->OnEdit();

		if (ImGui::Button("Save##shader"))
		{
			curr_shader->UpdateShader();
			App->scene_manager->GetCurrentScene()->OnShaderEdit(curr_shader);
		}
	}

	ImGui::End();

}
