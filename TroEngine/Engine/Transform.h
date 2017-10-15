#ifndef __TRANSFORM__
#define __TRANSFORM__

#include "Component.h"
#include "MathGeoLib.h"

class Transform : public Component
{
public:
	Transform(GameObject* owner);
	~Transform();

	void SetTransform(const float* values);
	void SetTransform(const float4x4& matrix);

	void DrawConfig();

private:
	float4x4 transform;
	float3 position;
	float3 rotation;
	float3 scale;
};

#endif // !__TRANSFORM__

