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

	float4x4 GetTransform();

	void DrawConfig();

private:
	float3 position;
	Quat rotation;
	float3 scale;

	float4x4 global_trans;
	bool update_trans = true;
};

#endif // !__TRANSFORM__

