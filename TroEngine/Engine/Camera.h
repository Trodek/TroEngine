#ifndef __CAMERA__
#define __CAMERA__

#include "Component.h"
#include "MathGeoLib.h"

class GameObject;

class Camera : public Component
{
public:
	Camera(GameObject* owner);
	~Camera();

	void SetAspectRatio(float aspect_ratio);

private:
	Frustum frustum;

};


#endif // !__CAMERA__

