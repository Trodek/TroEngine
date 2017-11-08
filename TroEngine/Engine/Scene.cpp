#include "Scene.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "Inspector.h"
#include "KDTree.h"

#include "ModuleInput.h"

bool Scene::Start()
{
	bool ret = true;

	kd_tree = new KDTree();

	return ret;
}

void Scene::Draw()
{
	for (std::vector<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		if((*go)->IsActive())
		(*go)->Draw();
	}
}

void Scene::DebugDraw()
{
	for (std::vector<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		(*go)->DebugDraw();
	}

	kd_tree->DebugDraw();
}

update_status Scene::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;
	
	if (update_kd_tree)
	{
		update_kd_tree = false;
		CreateTree();
	}

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

	for (std::vector<GameObject*>::iterator go = game_objects.begin(); go!=game_objects.end();)
	{
		(*go)->CleanUp();
		RELEASE(*go);

		go = game_objects.erase(go);
	}

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

	char name[30];
	sprintf_s(name, "GameObject %d", ++new_go_id);

	new_go = new GameObject(name);

	AddGameObject(new_go);
	
	return new_go;
}

GameObject * Scene::CreateGameObject(const char * name)
{
	GameObject* new_go = nullptr;

	new_go = new GameObject(name);

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

void Scene::GetAllStaticGameObjects(std::vector<GameObject*>& vector_to_fill) const
{
	for (int i = 0; i < game_objects.size(); ++i)
	{
		if (game_objects[i]->IsStatic())
			vector_to_fill.push_back(game_objects[i]);
		game_objects[i]->GetAllStaticChilds(vector_to_fill);
	}
}

void Scene::GetAllDynamicGameObjects(std::vector<GameObject*>& vector_to_fill) const
{
	for (int i = 0; i < game_objects.size(); ++i)
	{
		if(!game_objects[i]->IsStatic())
			vector_to_fill.push_back(game_objects[i]);
		game_objects[i]->GetAllDynamicChilds(vector_to_fill);
	}
}

void Scene::GetAllGameObjects(std::vector<GameObject*>& vector_to_fill) const
{
	for (int i = 0; i < game_objects.size(); ++i)
	{
		vector_to_fill.push_back(game_objects[i]);
		game_objects[i]->GetAllChilds(vector_to_fill);
	}
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

void Scene::CreateTree() const
{
	if (kd_tree->HasTree())
	{
		kd_tree->EraseTree();
	}

	std::vector<GameObject*> go;

	GetAllStaticGameObjects(go);

	kd_tree->CreateTree(go);
}
