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

GameObject::GameObject(const char * name, bool active, GameObject * parent) : name(name), active(active), parent(parent)
{
	AddComponent(Component::Type::Transform);
}

GameObject::~GameObject()
{
}

void GameObject::SetActive(bool active)
{
	this->active = active;
}

GameObject * GameObject::GetParent() const
{
	return parent;
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

bool GameObject::Update(float dt)
{
	bool ret = true;

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
	/// Currently, transform is not implemented

	//Find if the GameObject has a Material
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c)->GetType() == Component::Type::C_Material)
		{
			ComponentMaterial* m = (ComponentMaterial*)(*c);
			m->ApplyMaterial();
		}
	}

	//Draw each MeshRenderer
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c)->GetType() == Component::Type::MeshRenderer)
		{
			MeshRenderer* mr = (MeshRenderer*)(*c);
			mr->Draw();
		}
	}

	//Unbind textures after rendering
	App->renderer3D->UnbindTexture();

	//Draw Childs
	for (std::vector<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		(*go)->Draw();
	}

}

void GameObject::DebugDraw()
{
}

void GameObject::DrawConfig()
{
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
	case Component::Transform:
	{
		Transform* t = new Transform(this);
		components.push_back(t);
		new_comp = t;
		break;
	}
	case Component::MeshRenderer:
	{
		MeshRenderer* mr = new MeshRenderer(this);
		components.push_back(mr);
		new_comp = mr;
		break; 
	}
	case Component::C_Material:
	{
		ComponentMaterial* cm = new ComponentMaterial(this);
		components.push_back(cm);
		new_comp = cm;
		break;
	}
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
			(*c)->CleanUp();
			RELEASE(*c);

			components.erase(c);
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
			(*c)->CleanUp();
			RELEASE(*c);

			c = components.erase(c);
		}
		else
			++c;
	}
}

void GameObject::RemoveAllComponents()
{
	for (std::vector<Component*>::iterator c = components.begin(); c != components.end();)
	{
		(*c)->CleanUp();
		RELEASE(*c);

		c = components.erase(c);
	}
}

GameObject * GameObject::CreateChild(const char* name)
{
	GameObject* go = new GameObject(name,true,this);
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

uint GameObject::GetNumChilds() const
{
	return childs.size();
}

void GameObject::DrawHierarchy()
{
	for (int i = 0; i < childs.size(); ++i)
	{
		bool node_open = ImGui::TreeNodeEx(childs[i]->name.c_str(), (App->gui->inspector->selected == childs[i] ? ImGuiTreeNodeFlags_Selected : 0));

		if (ImGui::IsItemClicked())
			App->gui->inspector->selected = childs[i];

		if (node_open)
		{	
			childs[i]->DrawHierarchy();
		}
	}
	ImGui::TreePop();
}
