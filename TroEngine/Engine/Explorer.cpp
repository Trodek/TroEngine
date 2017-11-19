#include "Explorer.h"
#include "Application.h"
#include "ResourceManager.h"
#include "imgui.h"

Explorer::Explorer()
{
}

Explorer::~Explorer()
{
}

update_status Explorer::UpdateGUI(float dt)
{
	CreateExplorer();

	return UPDATE_CONTINUE;
}

void Explorer::CreateExplorer()
{
	if (active)
	{
		if (ImGui::Begin("Explorer##menu", &active))
		{
			static float w = 200.0f;
			static float h = 300.f;
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
			if (ImGui::BeginChild("Folders##child", ImVec2(w, h), true))
			{
				DrawFolderTree("Assets\\");
				ImGui::EndChild();
			}
			ImGui::SameLine();
			ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, h));
			if (ImGui::IsItemActive())
				w += ImGui::GetIO().MouseDelta.x;
			ImGui::SameLine();
			if (ImGui::BeginChild("Files##child", ImVec2(0, h), true))
			{
				if (selected != "")
				{
					ListFiles();
				}
				ImGui::EndChild();
			}
			ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
			if (ImGui::IsItemActive())
				h += ImGui::GetIO().MouseDelta.y;
			ImGui::BeginChild("child3", ImVec2(0, 0), true);
			ImGui::EndChild();
			ImGui::PopStyleVar();

		}
		ImGui::End();
	}
}

void Explorer::DrawFolderTree(const char* path)
{
	std::vector<std::string> paths;
	App->GetFilesInPath(paths, path);
	for (int i = 0; i < paths.size(); ++i)
	{
		if (paths[i].find(".") == -1)
		{
			bool node_open = ImGui::TreeNodeEx(App->resources->GetNameFromPath(paths[i].c_str()).c_str(), (selected ==paths[i] ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
				selected = paths[i] + "\\";

			if (node_open)
			{
				std::string folder_path;
				folder_path = paths[i];
				folder_path += "\\";
				DrawFolderTree(folder_path.c_str());
				ImGui::TreePop();
			}
		}
	}
}

void Explorer::ListFiles()
{
	std::vector<std::string> paths;
	App->GetFilesInPath(paths, selected.c_str());
	for (int i = 2; i < paths.size(); ++i)
	{
		if (paths[i].find(".") != -1)
		{
			bool node_open = ImGui::TreeNodeEx(App->resources->GetNameFromPath(paths[i].c_str()).c_str(), (selected_file == paths[i] ? ImGuiTreeNodeFlags_Selected : 0));

			if (ImGui::IsItemClicked())
				selected_file = paths[i];
			if (node_open)
			{
				ImGui::TreePop();
			}

			char name[30];
			sprintf_s(name, "file options##f%d", i);

			if (ImGui::IsItemClicked(1))
			{
				ImGui::OpenPopup(name);
			}

			if (ImGui::BeginPopup(name))
			{
				if (ImGui::MenuItem("Load"))
				{
					
				}
				ImGui::EndPopup();
			}
		}
	}
}
