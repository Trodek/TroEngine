#include "Application.h"
#include "SceneImporter.h"
#include "JSONManager.h"
#include "SceneManager.h"
#include "Scene.h"
#include "GameObject.h"
#include "Transform.h"
#include "ComponentMaterial.h"
#include "MeshRenderer.h"
#include "Camera.h"
#include "ResourceManager.h"

SceneImporter::SceneImporter(bool start_enabled) : Module(start_enabled)
{
}

SceneImporter::~SceneImporter()
{
}

bool SceneImporter::Awake(JSONDoc * doc)
{
	App->CreateFolder("Assets\\Prefabs");
	App->CreateFolder("Library\\Prefabs");

	App->CreateFolder("Assets\\Scenes");
	App->CreateFolder("Library\\Scenes");
	return true;
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
	doc->EraseContent();

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
	doc->EraseContent();

	doc->AddArray("GameObjects");
	doc->AddArray("Components");

	go->Serialize(doc);

	doc->SaveFile();
}

void SceneImporter::SaveSceneToBuffer(Scene * scene)
{
	if (buffer == nullptr)
		buffer = App->json->CreateJSONFile("tmp.json");
	else
		buffer->EraseContent();
	
	SaveScene(scene, buffer);
}


GameObject * SceneImporter::LoadPrefab(const char * path)
{
	JSONDoc* doc = App->json->LoadJSONFile(path);
	GameObject* ret = nullptr;

	Resource* res = new Resource();
	res->extension = "json";
	res->type = ResourceType::R_PREFAB;
	res->name = App->resources->GetNameFromPath(path);
	res->rel_path = App->resources->GetRelativePathFromPath(path);

	if (doc != nullptr)
	{
		for (int i = 0; i < doc->GetArraySize("GameObjects"); ++i)
		{
			doc->MoveToSectionFromArray("GameObjects", i);

			uint parent = doc->GetNumber("parent");
			std::string name = doc->GetString("name");
			uint uid = doc->GetNumber("UID");
			bool active = doc->GetBool("active");
			bool is_static = doc->GetBool("is_static");
			uint new_child_id = doc->GetNumber("new_child_id");
			if (parent == 0)
			{
				ret = new GameObject(name.c_str(), uid, active);
				ret->SetStatic(is_static);
				ret->SetNewChildId(new_child_id);
			}
			else
			{

				GameObject* p = (parent == ret->GetUID()) ? ret : nullptr;
				if (p == nullptr) ret->GetChildByUID(parent, p);
				GameObject* go = new GameObject(name.c_str(), uid, active, p);
				p->AddChild(go);
				go->SetStatic(is_static);
				go->SetNewChildId(new_child_id);
			}

			doc->MoveToRoot();
		}

		//LOAD COMPONENTS
		for (int i = 0; i < doc->GetArraySize("Components"); ++i)
		{
			doc->MoveToSectionFromArray("Components", i);

			Component::Type t = static_cast<Component::Type>((int)doc->GetNumber("type"));
			uint owner = doc->GetNumber("owner");
			GameObject* go = (owner == ret->GetUID()) ? ret : nullptr;
			if (go == nullptr) ret->GetChildByUID(owner, go);

			switch (t)
			{
			case Component::C_Transform:
			{
				float3 pos = doc->GetVector3("position");
				float3 scale = doc->GetVector3("scale");
				float4 rot = doc->GetVector4("rotation");
				Quat rotation(rot.x, rot.y, rot.z, rot.w);

				Transform* t = (Transform*)go->GetComponent(Component::C_Transform);
				t->SetTransform(pos, scale, rotation);
				break; 
			}
			case Component::C_MeshRenderer:
			{
				uint mesh_uid = doc->GetNumber("mesh");

				MeshRenderer* mr = (MeshRenderer*)go->AddComponent(Component::C_MeshRenderer);
				mr->SetMesh(mesh_uid);
				break; 
			}
			case Component::C_Material:
			{
				uint mat_uid = doc->GetNumber("material");
				uint vertex_shader = doc->GetNumber("vertex_shader");
				uint fragment_shader = doc->GetNumber("fragment_shader");

				ComponentMaterial* m = (ComponentMaterial*)go->AddComponent(Component::C_Material);
				m->SetMaterial(mat_uid);
				m->SetVertexShader(vertex_shader);
				m->SetFragmentShader(fragment_shader);

				break;
			}
			case Component::Camera:
			{
				float aspect = doc->GetNumber("aspect_ratio");
				float fov = doc->GetNumber("fov");
				float near_plane = doc->GetNumber("near");
				float far_plane = doc->GetNumber("far");

				Camera* c = (Camera*)go->AddComponent(Component::Camera);
				c->SetCamera(fov, aspect, near_plane, far_plane);
				break;
			}
			}

			doc->MoveToRoot();
		}
	}

	if (ret != nullptr)
	{
		prefabs.push_back(ret);
		res->manager_id = prefabs.size() - 1;
		App->resources->resources[App->resources->CreateResource(path)].push_back(res);
	}

	return ret;
}

void SceneImporter::LoadScene(const char * path, Scene * scene)
{
	JSONDoc* doc = App->json->LoadJSONFile(path);

	if (doc != nullptr)
	{
		//reset scene
		scene->CleanUp();
		scene->Start();

		for (int i = 0; i < doc->GetArraySize("GameObjects"); ++i)
		{
			doc->MoveToSectionFromArray("GameObjects", i);

			uint parent = doc->GetNumber("parent");
			std::string name = doc->GetString("name");
			uint uid = doc->GetNumber("UID");
			bool active = doc->GetBool("active");
			bool is_static = doc->GetBool("is_static");
			uint new_child_id = doc->GetNumber("new_child_id");
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

			doc->MoveToRoot();
		}

		//LOAD COMPONENTS
		for (int i = 0; i < doc->GetArraySize("Components"); ++i)
		{
			doc->MoveToSectionFromArray("Components", i);

			Component::Type t = static_cast<Component::Type>((int)doc->GetNumber("type"));
			uint owner = doc->GetNumber("owner");
			GameObject* go = App->scene_manager->GetCurrentScene()->GetGameObjectByUID(owner);
			switch (t)
			{
			case Component::C_Transform:
			{
				float3 pos = doc->GetVector3("position");
				float3 scale = doc->GetVector3("scale");
				float4 rot = doc->GetVector4("rotation");
				Quat rotation(rot.x, rot.y, rot.z, rot.w);

				Transform* t = (Transform*)go->GetComponent(Component::C_Transform);
				t->SetTransform(pos, scale, rotation);
				break;
			}
			case Component::C_MeshRenderer:
			{
				uint mesh_uid = doc->GetNumber("mesh");

				MeshRenderer* mr = (MeshRenderer*)go->AddComponent(Component::C_MeshRenderer);
				mr->SetMesh(mesh_uid);
				break;
			}
			case Component::C_Material:
			{
				uint mat_uid = doc->GetNumber("material");

				ComponentMaterial* m = (ComponentMaterial*)go->AddComponent(Component::C_Material);
				m->SetMaterial(mat_uid);
				break;
			}
			case Component::Camera:
			{
				float aspect = doc->GetNumber("aspect_ratio");
				float fov = doc->GetNumber("fov");
				float near_plane = doc->GetNumber("near");
				float far_plane = doc->GetNumber("far");

				Camera* c = (Camera*)go->AddComponent(Component::Camera);
				c->SetCamera(fov, aspect, near_plane, far_plane);
				break;
			}
			}

			doc->MoveToRoot();
		}
	}
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

		//LOAD COMPONENTS
		for (int i = 0; i < buffer->GetArraySize("Components"); ++i)
		{
			buffer->MoveToSectionFromArray("Components", i);

			Component::Type t = static_cast<Component::Type>((int)buffer->GetNumber("type"));
			uint owner = buffer->GetNumber("owner");
			GameObject* go = App->scene_manager->GetCurrentScene()->GetGameObjectByUID(owner);
			switch (t)
			{
			case Component::C_Transform:
			{
				float3 pos = buffer->GetVector3("position");
				float3 scale = buffer->GetVector3("scale");
				float4 rot = buffer->GetVector4("rotation");
				Quat rotation(rot.x, rot.y, rot.z, rot.w);

				Transform* t = (Transform*)go->GetComponent(Component::C_Transform);
				t->SetTransform(pos, scale, rotation);
				break;
			}
			case Component::C_MeshRenderer:
			{
				uint mesh_uid = buffer->GetNumber("mesh");

				MeshRenderer* mr = (MeshRenderer*)go->AddComponent(Component::C_MeshRenderer);
				mr->SetMesh(mesh_uid);
				break;
			}
			case Component::C_Material:
			{
				uint mat_uid = buffer->GetNumber("material");

				ComponentMaterial* m = (ComponentMaterial*)go->AddComponent(Component::C_Material);
				m->SetMaterial(mat_uid);
				break;
			}
			case Component::Camera:
			{
				float aspect = buffer->GetNumber("aspect_ratio");
				float fov = buffer->GetNumber("fov");
				float near_plane = buffer->GetNumber("near");
				float far_plane = buffer->GetNumber("far");

				Camera* c = (Camera*)go->AddComponent(Component::Camera);
				c->SetCamera(fov, aspect, near_plane, far_plane);
				break;
			}
			}

			buffer->MoveToRoot();
		}
	}
}
