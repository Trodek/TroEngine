#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "ComponentMaterial.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "Transform.h"
#include "imgui.h"
#include "ModuleGUI.h"
#include "Inspector.h"
#include "Camera.h"
#include "SceneManager.h"
#include "Scene.h"
#include "JSONManager.h"
#include "UID.h"
#include "SceneImporter.h"

GameObject::GameObject(const char * name, bool active, GameObject * parent) : name(name), active(active), parent(parent)
{
	AddComponent(Component::Type::C_Transform);

	void* a = this;
	void** a_ptr = &a;
	uint size = sizeof(this);
	char* data = new char[size];
	memcpy(data, a_ptr, size);

	uint* uid = md5(data, size);
	UID = *uid;
	RELEASE_ARRAY(data);
}

GameObject::~GameObject()
{
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}

bool GameObject::IsActive() const
{
	return active;
}

GameObject * GameObject::GetParent() const
{
	return parent;
}

uint GameObject::GetUID() const
{
	return UID;
}

void GameObject::SetNewParent(GameObject * new_parent)
{
	this->parent = parent;
}

bool GameObject::Start()
{
	bool ret = true;

	//Start components
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		ret = (*c)->Start();
	}

	//Start childs
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		ret = (*go)->Start();
	}

	return ret;
}

bool GameObject::PreUpdate()
{
	bool ret = true;
	//Remove marked components
	RemoveComponents();

	//Delete marked children
	RemoveChilds();

	//Preupdate children
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		ret = (*go)->PreUpdate();
	}

	return ret;
}

bool GameObject::Update(float dt)
{
	bool ret = true;

	//change static state
	if (change_static)
	{
		change_static = false;
		SetChildrenStatic(is_static);
	}

	//Update components
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		ret = (*c)->Update(dt);
	}

	//Update childs
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end();++go)
	{
		ret = (*go)->Update(dt);
	}

	return ret;
}

void GameObject::CleanUp()
{
	//Clean all components
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end();)
	{
		(*c)->CleanUp();
		RELEASE(*c);

		c = components.erase(c);
	}

	//Clean all childs
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end();)
	{
		(*go)->CleanUp();
		RELEASE(*go);

		go = childs.erase(go);
	}
}

void GameObject::Draw()
{
	//Just Components Transform, Material and MeshRenderer are needed for drawing

	//Find if the GameObject has a Material
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c)->GetType() == Component::Type::C_Material)
		{
			ComponentMaterial* m = (ComponentMaterial*)(*c);
			m->ApplyMaterial();
		}
	}

	//Draw MeshRenderer
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c)->GetType() == Component::Type::MeshRenderer)
		{
			App->renderer3D->PushMatrix();
			Transform* trans = (Transform*)GetComponent(Component::Type::C_Transform);
			App->renderer3D->MultMatrix(trans->GetTransform().Transposed().ptr());

			MeshRenderer* mr = (MeshRenderer*)(*c);
			mr->Draw();

			App->renderer3D->PopMatrix();
			break;
		}
	}

	//Unbind textures after rendering
	App->renderer3D->UnbindTexture();

	//Draw Childs
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		if ((*go)->active)
			(*go)->Draw();
	}

}

void GameObject::DebugDraw()
{
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		(*c)->DebugDraw();
	}

	//Draw Childs
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		(*go)->DebugDraw();
	}
}

void GameObject::DrawConfig()
{
	char new_name[50];
	strcpy_s(new_name, name.c_str());

	if (ImGui::InputText(new_name, new_name, 50))
	{
		name = new_name;
	}

	ImGui::Checkbox("Active##go", &active);
	ImGui::SameLine();
	if (ImGui::Checkbox("Static##go", &is_static))
	{
		change_static = true;
	}
	if (ImGui::IsItemHovered())
		ImGui::SetTooltip("This will affect all childs");

	ImGui::Text("UID: %u", UID);

	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		(*c)->DrawConfig();
	}
}

void GameObject::AddComponent(Component * component)
{
	components.push_back(component);
}

//Create a new component of given type. Returns nullptr if component can't be created.
Component* GameObject::AddComponent(Component::Type type)
{
	Component* new_comp = nullptr;

	switch (type)
	{
	case Component::Null:
		break;
	case Component::C_Transform:
	{
		if (!HasComponent(Component::C_Transform))
		{
			Transform* t = new Transform(this);
			components.push_back(t);
			new_comp = t;
		}
		break;
	}
	case Component::MeshRenderer:
	{
		if (!HasComponent(Component::MeshRenderer))
		{
			MeshRenderer* mr = new MeshRenderer(this);
			components.push_back(mr);
			new_comp = mr;
		}
		break; 
	}
	case Component::C_Material:
	{
		if (!HasComponent(Component::C_Material))
		{
			ComponentMaterial* cm = new ComponentMaterial(this);
			components.push_back(cm);
			new_comp = cm;
		}
		break;
	}
	case Component::Camera:
	{
		if (!HasComponent(Component::Camera))
		{
			Camera* cam = new Camera(this);
			components.push_back(cam);
			new_comp = cam;
		}
	}
		break;
	default:
		break;
	}

	return new_comp;
}

//Returns first component of specified type. nullptr if not found
Component * GameObject::GetComponent(Component::Type type) const
{
	Component* ret = nullptr;

	for (std::vector<Component*>::const_iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c)->GetType() == type)
		{
			ret = (*c);
			break;
		}
	}

	return ret;
}

bool GameObject::HasComponent(Component::Type type) const
{
	bool ret = false;

	for (std::vector<Component*>::const_iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c)->GetType() == type)
		{
			ret = true;
			break;
		}
	}

	return ret;
}

void GameObject::RemoveComponent(Component * comp)
{
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c) == comp)
		{
			(*c)->kill_me = true;
			break;
		}
	}
}

void GameObject::RemoveComponentsByType(Component::Type type)
{
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end();)
	{
		if ((*c)->GetType() == type)
		{
			(*c)->kill_me = true;
		}
		else
			++c;
	}
}

void GameObject::RemoveAllComponents()
{
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end();)
	{
		(*c)->kill_me = true;
	}
}

GameObject * GameObject::CreateChild(const char* name)
{
	GameObject* go = new GameObject(name,true,this);
	childs.push_back(go);
	return go;
}

GameObject * GameObject::CreateChild()
{
	char name[20];
	sprintf_s(name, "GameObject %d", new_child_id++);

	GameObject* go = new GameObject(name, true, this);
	childs.push_back(go);
	return go;
}

GameObject * GameObject::GetChild(uint id) const
{
	GameObject* ret = nullptr;
	
	for (int i = 0; i < childs.size(); ++i)
	{
		if (i == id)
		{
			ret = childs[i];
			break;
		}
	}

	return ret;
}

void GameObject::GetAllChilds(std::vector<GameObject*>& go) const
{
	for (int i = 0; i < childs.size(); ++i)
	{
		go.push_back(childs[i]);
	}

	//get children childrens
	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i]->GetAllChilds(go);
	}
}

void GameObject::GetAllStaticChilds(std::vector<GameObject*>& go) const
{
	for (int i = 0; i < childs.size(); ++i)
	{
		if (childs[i]->IsStatic())
			go.push_back(childs[i]);
	}

	//get children childrens
	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i]->GetAllStaticChilds(go);
	}
}

void GameObject::GetAllDynamicChilds(std::vector<GameObject*>& go) const
{
	for (int i = 0; i < childs.size(); ++i)
	{
		if (!childs[i]->IsStatic())
			go.push_back(childs[i]);
	}

	//get children childrens
	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i]->GetAllDynamicChilds(go);
	}
}

uint GameObject::GetNumChilds() const
{
	return childs.size();
}

bool GameObject::IsStatic() const
{
	return is_static;
}

void GameObject::Delete()
{
	if (parent == nullptr)
		kill_me = true;
	else
		parent->RemoveChild(this);
}

void GameObject::Serialize(JSONDoc * doc)
{
	//Save this game object
	doc->AddSectionToArray("GameObjects");
	doc->MoveToSectionFromArray("GameObjects", doc->GetArraySize("GameObjects") - 1);

	doc->SetNumber("UID", UID);
	doc->SetString("name", name.c_str());
	doc->SetBool("active", active);
	doc->SetBool("is_static", is_static);
	if(parent != nullptr)
		doc->SetNumber("parent", parent->GetUID());
	else
		doc->SetNumber("parent", 0);
	doc->SetNumber("new_child_id", new_child_id);

	doc->MoveToRoot();
	//Save components 
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		(*c)->Serialize(doc);
	}
	doc->MoveToRoot();
	//Save childs
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		(*go)->Serialize(doc);
	}
	doc->MoveToRoot();

}

void GameObject::RemoveChild(GameObject * child)
{
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		if ((*go) == child)
		{
			(*go)->kill_me = true;
			break;
		}
	}
}

void GameObject::TransformUpdate()
{
	//Ask all components for updates related to the transform
	for (int i = 0; i < components.size(); ++i)
	{
		components[i]->OnUpdateTransform();
	}

	//Ask all childs to update to this transform change
	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i]->TransformUpdate();
	}
}

float4x4 GameObject::GetTransform() const
{
	for (int i = 0; i < components.size(); ++i)
	{
		if (components[i]->GetType() == Component::C_Transform)
		{
			Transform* t = (Transform*)components[i];
			return t->GetTransform();
		}
	}
}

void GameObject::DrawHierarchy()
{
	for (int i = 0; i < childs.size(); ++i)
	{
		bool node_open = ImGui::TreeNodeEx(childs[i]->name.c_str(), (App->gui->inspector->selected == childs[i] ? ImGuiTreeNodeFlags_Selected : 0));
		bool clicked = false;

		if (ImGui::IsItemClicked())
			App->gui->inspector->selected = childs[i];

		char name[30];
		sprintf_s(name, "GameObject options##go%d", i);

		if (clicked = ImGui::IsItemClicked(1))
		{		
			ImGui::OpenPopup(name);
		}

		if (ImGui::BeginPopup(name))
		{
			if (ImGui::MenuItem("Add Child"))
			{
				childs[i]->CreateChild();
			}

			if (ImGui::MenuItem("Save as prefab"))
			{
				char name[65];
				sprintf_s(name, "%s_prefab.json", childs[i]->name.c_str());
				App->scene_importer->SaveScene(childs[i], name);
			}

			if (ImGui::MenuItem("Delete"))
			{
				childs[i]->Delete();
				if (App->gui->inspector->selected == childs[i])
					App->gui->inspector->selected = nullptr;
			}
			ImGui::EndPopup();
		}
		

		if (node_open)
		{
			childs[i]->DrawHierarchy();
		}
	}
	ImGui::TreePop();
}

void GameObject::SetChildrenStatic(bool _is_static)
{
	is_static = _is_static;

	for (int i = 0; i < childs.size(); ++i)
	{
		childs[i]->SetChildrenStatic(is_static);
	}

	App->scene_manager->GetCurrentScene()->update_kd_tree = true;
}

void GameObject::RemoveComponents()
{
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end();)
	{
		if ((*c)->kill_me == true)
		{
			(*c)->CleanUp();
			RELEASE(*c);

			c = components.erase(c);
		}
		else
			++c;
	}
}

void GameObject::RemoveChilds()
{
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end();)
	{
		if ((*go)->kill_me == true)
		{
			(*go)->CleanUp();
			RELEASE(*go);

			go = childs.erase(go);
		}
		else
			++go;
	}
}