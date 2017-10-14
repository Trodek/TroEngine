#include "Scene.h"
#include "GameObject.h"
#include "ComponentMaterial.h"

bool Scene::Start()
{
	bool ret = true;

	//For now create a game object with mesh renderer, material and transform
	GameObject* go = CreateGameObject();
	go->AddComponent(Component::Type::MeshRenderer);
	go->AddComponent(Component::Type::C_Material);

	return ret;
}

void Scene::Draw()
{
	for (std::list<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		(*go)->Draw();
	}
}

update_status Scene::Update(float dt)
{
	update_status ret = UPDATE_CONTINUE;

	bool go_ret = true;
	for (std::list<GameObject*>::iterator go = game_objects.begin(); go != game_objects.end(); ++go)
	{
		go_ret = (*go)->Update(dt);

		if (go_ret == false)
		{
			EDITOR_LOG("Error Updating GameObject: %s", (*go)->name.c_str());
		}
	}

	if (!go_ret)
		ret = UPDATE_STOP;

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

GameObject * Scene::GetGameObject(uint id) const
{
	GameObject* ret = nullptr;

	uint i = 0;
	for (std::list<GameObject*>::const_iterator go = game_objects.begin(); go != game_objects.end(); ++go)
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