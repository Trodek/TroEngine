#include "Transform.h"
#include "imgui.h"
#include "GameObject.h"

Transform::Transform(GameObject * owner) :Component(Component::Type::C_Transform, owner)
{
	rotation.Set(0, 0, 0, 1);
	position.Set(0, 0, 0);
	scale.Set(1, 1, 1);

	global_trans = float4x4::FromTRS(position, rotation, scale);
}

Transform::~Transform()
{
}

void Transform::OnUpdateTransform()
{
	if (GetOwner()->GetParent() == nullptr)
		global_trans = float4x4::FromTRS(position, rotation, scale);
	else
	{
		Transform* parent_trans = (Transform*)GetOwner()->GetParent()->GetComponent(Component::Type::C_Transform);
		global_trans = parent_trans->GetTransform()*float4x4::FromTRS(position, rotation, scale);
	}
}

void Transform::SetTransform(float3 pos, float3 scale, Quat rot)
{
	rotation = rot;
	this->scale = scale;
	position = pos;

	UpdateTransform();
}

void Transform::SetTranslate(float3 new_translate)
{
	position = new_translate;
	UpdateTransform();
}

void Transform::SetRotation(Quat rot)
{
	rotation = rot;
	UpdateTransform();
}

void Transform::Translate(float3 movement)
{
	position += movement;
	UpdateTransform();
}

void Transform::Rotate(Quat _rotation)
{
	rotation = rotation*_rotation;
	UpdateTransform();
}

void Transform::Scale(float3 scale)
{
	this->scale = scale;
}

float4x4 Transform::GetTransform() const
{
	return global_trans;
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

	if (!GetOwner()->IsStatic())
	{
		Quat new_rot = Quat::FromEulerXYZ(rot[0] * DEGTORAD, rot[1] * DEGTORAD, rot[2] * DEGTORAD);

		bool update_trans = false;

		if (pos[0] != position.x || pos[1] != position.y || pos[2] != position.z)
		{
			position = float3(pos[0], pos[1], pos[2]);
			update_trans = true;
		}

		if (s[0] != scale.x || s[1] != scale.y || s[2] != scale.z)
		{
			scale = float3(s[0], s[1], s[2]);
			update_trans = true;
		}

		if (rotation.Equals(new_rot) == false)
		{
			rotation = new_rot;
			update_trans = true;
		}

		if (update_trans)
			UpdateTransform();
	}
}

void Transform::UpdateTransform()
{
	if (GetOwner()->GetParent() == nullptr)
		global_trans = float4x4::FromTRS(position, rotation, scale);
	else
	{
		Transform* parent_trans = (Transform*)GetOwner()->GetParent()->GetComponent(Component::Type::C_Transform);
		global_trans = parent_trans->GetTransform()*float4x4::FromTRS(position, rotation, scale);
	}

	GetOwner()->TransformUpdate();
}
