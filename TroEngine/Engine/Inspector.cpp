#include "Inspector.h"
#include "imgui.h"
#include "Application.h"
#include "Scene.h"
#include "SceneManager.h"
#include "GameObject.h"

Inspector::Inspector()
{
}

Inspector::~Inspector()
{
}

update_status Inspector::UpdateGUI(float dt)
{
	CreateInspector();

	return UPDATE_CONTINUE;
}

void Inspector::CreateInspector()
{
	ImGui::Begin("Inspector");

	App->scene_manager->GetCurrentScene()->GetGameObject(0)->DrawConfig();

	ImGui::End();
}
