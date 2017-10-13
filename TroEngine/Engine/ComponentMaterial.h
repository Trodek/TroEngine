#ifndef __COMPONENTMATERIAL__
#define __COMPONENTMATERIAL__

#include "Globals.h"
#include "Component.h"

class Material;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(GameObject* owner);
	ComponentMaterial(GameObject* owner, Material* mat);
	~ComponentMaterial();

	void CleanUp();

	void SetMaterial(Material* new_mat);
	void SetMaterial(uint mat_id);
	void ApplyMaterial() const;

	void DrawConfig();

	void UseChecker();

private:
	Material* material = nullptr;
	bool use_checker = false;

};

#endif // !__COMPONENTMATERIAL__

