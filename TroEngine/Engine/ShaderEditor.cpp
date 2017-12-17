#include "ShaderEditor.h"
#include "Shader.h"
#include "imgui.h"
#include "Application.h"
#include "Scene.h"
#include "SceneManager.h"

ShaderEditor::ShaderEditor()
{
	editor.SetLanguageDefinition(TextEditor::LanguageDefinition::GLSL());
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
	if (ImGui::Begin("Shader Editor", &active,ImGuiWindowFlags_MenuBar))
	{

		ImGui::BeginMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				if (curr_shader != nullptr)
				{
					curr_shader->SetShaderCode(editor.GetText().c_str());
					curr_shader->SaveToAssets();
				}
			}
			if (ImGui::MenuItem("Quit"))
				this->active = false;
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = editor.IsReadOnly();

			if (ImGui::MenuItem("Undo", nullptr, !ro && editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", nullptr, !ro && editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", nullptr, !ro && editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", nullptr, !ro && editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();

		if (curr_shader != nullptr)
		{
			if (shader_changed)
			{
				shader_changed = false;
				editor.SetText(std::string(curr_shader->GetCodeText()));
			}

			ImGuiIO& io = ImGui::GetIO();
			ImGui::PushFont(io.Fonts->Fonts[0]);
			editor.Render("Render...");
			ImGui::PopFont();
		}

		ImGui::End();
	}

}

void ShaderEditor::SetShader(Shader * shader)
{
	curr_shader = shader;
	shader_changed = true;
}

