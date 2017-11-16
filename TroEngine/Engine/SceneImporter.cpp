#include "Application.h"
#include "SceneImporter.h"
#include "JSONManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"

SceneImporter::SceneImporter(bool start_enabled) : Module(start_enabled)
{
}

SceneImporter::~SceneImporter()
{
}

void SceneImporter::SaveScene(Scene * scene, const char * path)
{
	JSONDoc* doc = App->json->LoadJSONFile(path);

	if (doc == nullptr)
		doc = App->json->CreateJSONFile(path);
	else
		doc->EraseContent();

	doc->AddArray("GameObjects");
	doc->AddArray("Components");

	scene->SerializeScene(doc);

	doc->SaveFile();
}

void SceneImporter::SaveScene(Scene * scene, JSONDoc * doc)
{
	doc->AddArray("GameObjects");
	doc->AddArray("Components");

	scene->SerializeScene(doc);
}

void SceneImporter::SaveScene(GameObject * go, const char * path)
{
	JSONDoc* doc = App->json->LoadJSONFile(path);

	if (doc == nullptr)
		doc = App->json->CreateJSONFile(path);
	else
		doc->EraseContent();

	doc->AddArray("GameObjects");
	doc->AddArray("Components");

	go->Serialize(doc);

	doc->SaveFile();
}

void SceneImporter::SaveScene(GameObject * go, JSONDoc * doc)
{

}

void SceneImporter::SaveSceneToBuffer(Scene * scene)
{
	if (buffer == nullptr)
		buffer = App->json->CreateJSONFile("tmp.json");
	else
		buffer->EraseContent();
	
	SaveScene(scene, buffer);
}


GameObject * SceneImporter::LoadScene(const char * path)
{
	return nullptr;
}

void SceneImporter::LoadScene(const char * path, Scene * scene)
{
}

void SceneImporter::LoadSceneFromBuffer(Scene * scene)
{
	if (buffer != nullptr)
	{
		//reset scene
		scene->CleanUp();
		scene->Start();

		for (int i = 0; i < buffer->GetArraySize("GameObjects"); ++i)
		{
			buffer->MoveToSectionFromArray("GameObjects", i);

			uint parent = buffer->GetNumber("parent");
			std::string name = buffer->GetString("name");
			uint uid = buffer->GetNumber("UID");
			bool active = buffer->GetBool("active");
			bool is_static = buffer->GetBool("is_static");
			uint new_child_id = buffer->GetNumber("new_child_id");
			if (parent == 0)
			{
				GameObject* go = new GameObject(name.c_str(), uid, active);
				scene->AddGameObject(go);
				go->SetStatic(is_static);
				go->SetNewChildId(new_child_id);
			}
			else
			{
				GameObject* p = App->scene_manager->GetCurrentScene()->GetGameObjectByUID(parent);
				GameObject* go = new GameObject(name.c_str(), uid, active, p);
				p->AddChild(go);
				go->SetStatic(is_static);
				go->SetNewChildId(new_child_id);
			}

			buffer->MoveToRoot();
		}

		//LOAD COMPONENTS TODO: needs resource manager
	}
}
