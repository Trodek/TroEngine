#include "Transform.h"
#include "imgui.h"
#include "GameObject.h"

Transform::Transform(GameObject * owner) :Component(Component::Type::Transform, owner)
{
	rotation.Set(0, 0, 0, 1);
	position.Set(0, 0, 0);
	scale.Set(1, 1, 1);
}

Transform::~Transform()
{
}

void Transform::SetTransform(float3 pos, float3 scale, Quat rot)
{
	rotation = rot;
	this->scale = scale;
	position = pos;
}

float4x4 Transform::GetTransform()
{
	float4x4 trans = float4x4::FromTRS(position,rotation,scale);

	if (GetOwner()->GetParent() == nullptr)
		return trans;
	else
	{
		Transform* parent_trans = (Transform*)GetOwner()->GetParent()->GetComponent(Component::Type::Transform);
		return parent_trans->GetTransform()*trans;
	}
}

void Transform::DrawConfig()
{
	float pos[3] = { position.x,position.y,position.z };
	float rot[3] = { rotation.ToEulerXYZ().x*RADTODEG,rotation.ToEulerXYZ().y*RADTODEG,rotation.ToEulerXYZ().z*RADTODEG };
	float s[3] = { scale.x,scale.y,scale.z };

	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::DragFloat3("Position##transform", pos, 0.1f, -INFINITY, INFINITY);
		ImGui::DragFloat3("Rotation##transform", rot, 0.1f, -360, 360);
		ImGui::DragFloat3("Scale##transform", s, 0.1f, 0, INFINITY);

	}

	Quat new_rot = Quat::FromEulerXYZ(rot[0] * DEGTORAD, rot[1] * DEGTORAD, rot[2] * DEGTORAD);

	position = float3(pos[0], pos[1], pos[2]);
	scale = float3(s[0], s[1], s[2]);
	rotation = new_rot;
}
