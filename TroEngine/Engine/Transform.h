#ifndef __TRANSFORM__
#define __TRANSFORM__

#include "Component.h"
#include "MathGeoLib.h"

class Transform : public Component
{
public:
	Transform(GameObject* owner);
	~Transform();

	void OnUpdateTransform();

	void SetTransform(float3 pos, float3 scale, Quat rot);
	void SetTranslate(float3 new_translate);
	void SetRotation(Quat rot);
	void Translate(float3 movement);
	void Rotate(Quat rotation);
	void Scale(float3 scale);

	float4x4 GetTransform() const;

	void DrawConfig();

private:
	void UpdateTransform();

private:
	float3 position;
	Quat rotation;
	float3 scale;

	float4x4 global_trans;
};

#endif // !__TRANSFORM__

