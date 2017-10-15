#include "Component.h"

Component::Component(Type type, GameObject * owner) : type(type), owner(owner)
{
}

Component::~Component()
{
}

GameObject * Component::GetOwner() const
{
	return owner;
}

Component::Type Component::GetType() const
{
	return type;
}

void Component::SetActive(bool active)
{
	this->active = active;

	if (active == true)
	{
		OnActivate();
	}
	else
	{
		OnDeactivate();
	}
}

bool Component::IsActive() const
{
	return active;
}
