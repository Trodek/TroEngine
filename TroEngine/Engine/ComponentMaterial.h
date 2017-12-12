#ifndef __COMPONENTMATERIAL__
#define __COMPONENTMATERIAL__

#include "Globals.h"
#include "Component.h"

class Material;
class Shader;

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

	void SetVertexShader(Shader* vertex_shader);
	void SetVertexShader(uint shader_uid);
	void SetFragmentShader(Shader* fragment_shader);
	void SetFragmentShader(uint shader_uid);

	void OnShaderEdit(Shader* shader);
	void UpdateShaderProgram();

	uint GetProgram()const;

	void UseShader()const;

	void DrawConfig();

	void UseChecker();

	void Serialize(JSONDoc* doc);

private:
	Material* material = nullptr;
	bool use_checker = false;

	Shader* vertex_shader = nullptr;
	Shader* fragment_shader = nullptr;

	uint shader_program = 0;

};

#endif // !__COMPONENTMATERIAL__

