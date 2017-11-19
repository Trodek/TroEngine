#include "PlayPauseUI.h"
#include "imgui.h"
#include "Application.h"
#include "SceneManager.h"
#include "ModuleGUI.h"
#include "Inspector.h"

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
			App->gui->inspector->selected = nullptr;
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

		ImGui::SameLine();
		float speed = App->scene_manager->GetGameTimerSpeed();
		if (ImGui::DragFloat("Speed##time", &speed, 0.1f, 0.5f, 3.f, "%.1f"))
		{
			App->scene_manager->SetGameTimerSpeed(speed);
		}

		ImGui::End();
	}
}
