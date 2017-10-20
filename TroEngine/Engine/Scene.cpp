#include "Scene.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "Inspector.h"

bool Scene::Start()
{
	bool ret = true;

	return ret;
}

void Scene::Draw()
{
	for (std::vector<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		(*go)->Draw();
	}
}

update_status Scene::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	bool go_ret = true;
	for (std::vector<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		go_ret = (*go)->Update(dt);

		if (go_ret == false)
		{
			EDITOR_LOG("Error Updating GameObject: %s", (*go)->name.c_str());
			ret = UPDATE_STOP;
		}
	}		

	return ret;
}

bool Scene::CleanUp()
{
	bool ret = true;

	//TODO

	return ret;
}

bool Scene::IsActive()
{
	return active;
}

void Scene::AddGameObject(GameObject * go)
{
	game_objects.push_back(go);
}

GameObject * Scene::CreateGameObject()
{
	GameObject* new_go = nullptr;

	std::string go_name = "GameObject";

	new_go = new GameObject(go_name.c_str());

	AddGameObject(new_go);
	
	return new_go;
}

GameObject * Scene::GetGameObject(uint id) const //FIX this to work with childs
{
	GameObject* ret = nullptr;

	uint i = 0;
	for (std::vector<GameObject*>::const_iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		if (i == id)
		{
			ret = (*go);
			break;
		}
		++i;
	}

	return ret;
}

void Scene::DrawHierarchy() const
{
	for (int i = 0; i < game_objects.size(); ++i)
	{
		bool node_open = ImGui::TreeNodeEx(game_objects[i]->name.c_str(), (App->gui->inspector->selected == game_objects[i] ? ImGuiTreeNodeFlags_Selected : 0));

		if(ImGui::IsItemClicked())
			App->gui->inspector->selected = game_objects[i];

		if (node_open)
		{
			game_objects[i]->DrawHierarchy();
		}
	}
}
