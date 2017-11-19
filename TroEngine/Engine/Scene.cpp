#include "Scene.h"
#include "GameObject.h"
#include "ComponentMaterial.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleGUI.h"
#include "Inspector.h"
#include "KDTree.h"
#include "SceneImporter.h"
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
		if((*go)->IsActive() && (*go)->draw)
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

bool Scene::PreUpdate()
{
	bool ret = true;

	for (std::vector<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end();)
	{
		if ((*go)->kill_me == true)
		{
			(*go)->CleanUp();
			RELEASE(*go);

			go = game_objects.erase(go);
		}
		else
			++go;
	}

	for (std::vector<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		(*go)->PreUpdate();
		if ((*go)->HasComponent(Component::C_MeshRenderer))
			(*go)->draw = false;
	}

	return ret;
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

	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
		App->scene_importer->SaveScene(this, "test.json");

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

	kd_tree->EraseTree();
	RELEASE(kd_tree);

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

GameObject * Scene::GetGameObjectByUID(uint uid) const
{
	GameObject* ret = nullptr;

	for (int i = 0; i < game_objects.size(); ++i)
	{
		if (game_objects[i]->GetUID() == uid)
		{
			ret = game_objects[i];
			break;
		}
		else
		{
			game_objects[i]->GetChildByUID(uid, ret);
			if (ret != nullptr)
				break;
		}
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

void Scene::TestToKDTree(const Ray & ray, float p1_distance, float p2_distance, std::vector<GameObject*>& vec_to_fill) const
{
	kd_tree->GetElementsToTest(ray, p1_distance, p2_distance, vec_to_fill);
}

void Scene::TestToKDTree(const AABB & box, std::vector<GameObject*>& vec_to_fill) const
{
	kd_tree->GetElementsToTest(box, vec_to_fill);
}

void Scene::TestToKDTree(const LineSegment & segment, std::vector<GameObject*>& vec_to_fill) const
{
	kd_tree->GetElementsToTest(segment, vec_to_fill);
}

void Scene::TestToKDTree(const Frustum & frustum, std::vector<GameObject*>& vec_to_fill) const
{
	kd_tree->GetElementsToTest(frustum, vec_to_fill);
}

void Scene::DrawHierarchy() const
{
	for (int i = 0; i < game_objects.size(); ++i)
	{
		bool node_open = ImGui::TreeNodeEx(game_objects[i]->name.c_str(), (App->gui->inspector->selected == game_objects[i] ? ImGuiTreeNodeFlags_Selected : 0));

		if(ImGui::IsItemClicked())
			App->gui->inspector->selected = game_objects[i];

		char name[30];
		sprintf_s(name, "GameObject options##scene%d", i);

		if (ImGui::IsItemClicked(1))
		{
			ImGui::OpenPopup(name);
		}

		if (ImGui::BeginPopup(name))
		{
			if (ImGui::MenuItem("Add Child"))
			{
				game_objects[i]->CreateChild();
			}

			if (ImGui::MenuItem("Save as prefab"))
			{
				char name[65];
				sprintf_s(name, "%s_prefab.json",game_objects[i]->name.c_str());
				App->scene_importer->SaveScene(game_objects[i], name);
			}

			if (ImGui::MenuItem("Delete"))
			{
				game_objects[i]->Delete();
				if (App->gui->inspector->selected = game_objects[i])
					App->gui->inspector->selected = nullptr;
			}
			ImGui::EndPopup();
		}

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

void Scene::SerializeScene(JSONDoc * doc)
{
	for (std::vector<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		(*go)->Serialize(doc);
	}
}
