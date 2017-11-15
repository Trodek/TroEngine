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

GameObject * SceneImporter::LoadScene(const char * path)
{
	return nullptr;
}

void SceneImporter::LoadScene(const char * path, Scene * scene)
{
}
