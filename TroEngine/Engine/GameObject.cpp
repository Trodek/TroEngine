#include "GameObject.h"
#include "Component.h"
#include "MeshRenderer.h"
#include "ComponentMaterial.h"

GameObject::GameObject(const char * name, bool active, GameObject * parent) : name(name), active(active), parent(parent)
{
	//Create transform Component
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
	for (std::list<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		ret = (*c)->Start();
	}

	//Start childs
	for (std::list<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		ret = (*go)->Start();
	}

	return ret;
}

bool GameObject::Update(float dt)
{
	bool ret = true;

	//Update components
	for (std::list<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		ret = (*c)->Update(dt);
	}

	//Update childs
	for (std::list<GameObject*>::iterator go = childs.begin(); go != childs.end();++go)
	{
		ret = (*go)->Update(dt);
	}

	return ret;
}

void GameObject::CleanUp()
{
	//Clean all components
	for (std::list<Component*>::iterator c = components.begin(); c != components.end();)
	{
		(*c)->CleanUp();
		RELEASE(*c);

		c = components.erase(c);
	}

	//Clean all childs
	for (std::list<GameObject*>::iterator go = childs.begin(); go != childs.end();)
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
	for (std::list<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c)->GetType() == Component::Type::C_Material)
		{
			ComponentMaterial* m = (ComponentMaterial*)(*c);
			m->ApplyMaterial();
		}
	}

	//Draw each MeshRenderer
	for (std::list<Component*>::iterator c = components.begin(); c != components.end(); ++c)
	{
		if ((*c)->GetType() == Component::Type::MeshRenderer)
		{
			MeshRenderer* mr = (MeshRenderer*)(*c);
			mr->Draw();
		}
	}

	//Draw Childs
	for (std::list<GameObject*>::iterator go = childs.begin(); go != childs.end(); ++go)
	{
		(*go)->Draw();
	}

}

void GameObject::DebugDraw()
{
}

void GameObject::DrawConfig()
{
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
		
		break;
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
