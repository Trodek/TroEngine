#include "PlayPauseUI.h"
#include "imgui.h"
#include "Application.h"
#include "SceneManager.h"

PlayPauseUI::PlayPauseUI()
{
}

PlayPauseUI::~PlayPauseUI()
{
}

update_status PlayPauseUI::UpdateGUI(float dt)
{
	CreatePlayPauseUI();

	return UPDATE_CONTINUE;
}

void PlayPauseUI::CreatePlayPauseUI()
{
	if(ImGui::Begin("PlayPause", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar))
	{
		if (ImGui::Button("Play"))
		{
			App->scene_manager->Play();
		}
		ImGui::SameLine();

		if (ImGui::Button("Pause"))
		{
			App->scene_manager->Pause();
		}
		ImGui::SameLine();

		if (ImGui::Button("Tick"))
		{
			App->scene_manager->Tick();
		}
		ImGui::SameLine();
		
		ImGui::Text("%.2f", App->scene_manager->ReadGameTimer());

		ImGui::End();
	}
}
