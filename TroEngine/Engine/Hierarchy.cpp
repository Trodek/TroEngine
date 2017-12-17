#include "Hierarchy.h"
#include "imgui.h"
#include "Application.h"
#include "SceneManager.h"
#include "Scene.h"
#include "Inspector.h"
#include "ModuleGUI.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "MeshImporter.h"

Hierarchy::Hierarchy()
{
}

Hierarchy::~Hierarchy()
{
}

update_status Hierarchy::UpdateGUI(float dt)
{
	CreateHierarchy();

	return UPDATE_CONTINUE;
}

bool Hierarchy::CleanUp()
{
	bool ret = true;

	return ret;
}

void Hierarchy::CreateHierarchy()
{
	if (active)
	{
		ImGui::Begin("Hierarchy##window");

		App->scene_manager->GetCurrentScene()->DrawHierarchy();

		if (ImGui::Button("Add"))
		{
			ImGui::OpenPopup("hierarchy options");
		}

		if (ImGui::BeginPopup("hierarchy options"))
		{
			if (ImGui::MenuItem("GameObject"))
			{
				App->scene_manager->GetCurrentScene()->CreateGameObject();
			}
			if (ImGui::MenuItem("Plane##p"))
			{
				GameObject* go = App->scene_manager->GetCurrentScene()->CreateGameObject();
				MeshRenderer* mr = (MeshRenderer*) go->AddComponent(Component::C_MeshRenderer);
				mr->SetMesh(App->mesh->hd_plane);
				go->AddComponent(Component::C_Material);
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}
}
