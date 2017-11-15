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

		if (selected != nullptr)
		{
			selected->DrawConfig();

			if (ImGui::Button("Add"))
			{
				ImGui::OpenPopup("inspector options");
			}

			if (ImGui::BeginPopup("inspector options"))
			{
				Component::Type comp_type = Component::Type::Null;

				if (ImGui::MenuItem("Camera"))
				{
					comp_type = Component::Type::Camera;
				}

				if (ImGui::MenuItem("Mesh Renderer"))
				{
					comp_type = Component::Type::MeshRenderer;
				}

				if (ImGui::MenuItem("Material"))
				{
					comp_type = Component::Type::C_Material;
				}

				if (comp_type != Component::Type::Null)
					selected->AddComponent(comp_type);

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}
