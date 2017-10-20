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
	if (active)
	{
		ImGui::Begin("Inspector", &active);

		if(selected != nullptr)
			selected->DrawConfig();

		ImGui::End();
	}
}
