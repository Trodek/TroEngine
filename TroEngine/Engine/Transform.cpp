#include "Transform.h"
#include "imgui.h"

Transform::Transform(GameObject * owner) :Component(Component::Type::Transform, owner)
{
	transform.SetIdentity();

	float3x3 rot_mat;
	transform.Decompose(position, rot_mat, scale);
	rotation = rot_mat.ToEulerXYZ();
}

Transform::~Transform()
{
}

void Transform::SetTransform(const float * values)
{
	transform.Set(values);

	float3x3 rot_mat;
	transform.Decompose(position, rot_mat, scale);
	rotation = rot_mat.ToEulerXYZ();
}

void Transform::SetTransform(const float4x4 & matrix)
{
	transform.Set(matrix);

	float3x3 rot_mat;
	transform.Decompose(position, rot_mat, scale);
	rotation = rot_mat.ToEulerXYZ();
}

void Transform::DrawConfig()
{
	float pos[3] = { position.x,position.y,position.z };
	float rot[3] = { rotation.x,rotation.y,rotation.z };
	float s[3] = { scale.x,scale.y,scale.z };

	if (ImGui::CollapsingHeader("Transform"))
	{
		ImGui::InputFloat3("Pos##transform", pos, 2);
		ImGui::InputFloat3("Rot##transform", rot, 2);
		ImGui::InputFloat3("Scale##transform", s, 2);
	}
}
