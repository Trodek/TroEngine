#include "KDTree.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include <algorithm>
#include "Color.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "MeshImporter.h"
#include "Mesh.h"

KDTree::Node::Node(uint partition_num) : partition_num(partition_num)
{
}

KDTree::Node::~Node()
{
}

void KDTree::Node::AddElement(GameObject * element)
{
	if(element->HasComponent(Component::C_MeshRenderer))
		elements.push_back(element);
}

void KDTree::Node::CreatePartition()
{
	// Difficult one, and the key of the K-D tree.
	if (left == nullptr && right == nullptr)
	{
		left = new Node(partition_num);
		left->SetParent(this);
		right = new Node(partition_num);
		right->SetParent(this);

		//decide next plane axis
		if (parent == nullptr) //for root node
			axis = A_X;
		else {
			switch (parent->axis)
			{
			case KDTree::Node::A_X:
				axis = A_Z;
				break;
			case KDTree::Node::A_Y:
				axis = A_X;
				break;
			case KDTree::Node::A_Z:
				axis = A_Y;
				break;
			}
		}

		//get max and min points of the AABB depending on axis
		std::vector<float> points;
		for (int i = 0; i < elements.size(); ++i)
		{
			if (!elements[i]->HasComponent(Component::Type::C_MeshRenderer))
				continue;

			MeshRenderer* mr = (MeshRenderer*)elements[i]->GetComponent(Component::Type::C_MeshRenderer);

			switch (axis)
			{
			case KDTree::Node::A_X:
				points.push_back(mr->GetMeshAABB().MaxX());
				points.push_back(mr->GetMeshAABB().MinX());
				break;
			case KDTree::Node::A_Y:
				points.push_back(mr->GetMeshAABB().MaxY());
				points.push_back(mr->GetMeshAABB().MinY());
				break;
			case KDTree::Node::A_Z:
				points.push_back(mr->GetMeshAABB().MaxZ());
				points.push_back(mr->GetMeshAABB().MinZ());
				break;
			}
		}

		//get mid_point
		std::sort(points.begin(),points.end());
		float mid_point = points[points.size()/2];


		//create a 3d point from that mid_point and create the plane from the point
		float3 plane_point = float3::zero;
		switch (axis)
		{
		case KDTree::Node::A_X:
			plane_point.x = mid_point;
			cut_plane.Set(plane_point, float3::unitX);
			break;
		case KDTree::Node::A_Y:
			plane_point.y = mid_point;
			cut_plane.Set(plane_point, float3::unitY);
			break;
		case KDTree::Node::A_Z:
			plane_point.z = mid_point;
			cut_plane.Set(plane_point, float3::unitZ);
			break;
		}

		//set each element of the node to one/both of the created nodes
		for (std::vector<GameObject*>::iterator ele = elements.begin(); ele != elements.end();)
		{
			int node = 0; // new node is decided between positive part or negative part of the plane.

			MeshRenderer* mr = (MeshRenderer*)(*ele)->GetComponent(Component::Type::C_MeshRenderer);

			if (mr == nullptr)
			{
				ele = elements.erase(ele);
				continue;
			}

			switch (axis)
			{
			case KDTree::Node::A_X:
				if (mr->GetMeshAABB().MaxX() >= mid_point)
				{
					if (mr->GetMeshAABB().MinX() >= mid_point)
						node = 1;
					else
						node = 0;
				}
				else
				{
					if (mr->GetMeshAABB().MinX() < mid_point)
						node = -1;
					else
						node = 0;
				}
				break;
			case KDTree::Node::A_Y:
				if (mr->GetMeshAABB().MaxY() >= mid_point)
				{
					if (mr->GetMeshAABB().MinY() >= mid_point)
						node = 1;
					else
						node = 0;
				}
				else
				{
					if (mr->GetMeshAABB().MinY() < mid_point)
						node = -1;
					else
						node = 0;
				}
				break;
			case KDTree::Node::A_Z:
				if (mr->GetMeshAABB().MaxZ() >= mid_point)
				{
					if (mr->GetMeshAABB().MinZ() >= mid_point)
						node = 1;
					else
						node = 0;
				}
				else
				{
					if (mr->GetMeshAABB().MinZ() < mid_point)
						node = -1;
					else
						node = 0;
				}
				break;
			}

			switch (node)
			{
			case 1:
				right->AddElement(*ele);
				break;
			case 0:
				right->AddElement(*ele);
				left->AddElement(*ele);
				break;
			case -1:
				left->AddElement(*ele);
				break;
			}

			ele = elements.erase(ele);
		}

		//Check if nodes need partition
		left->CheckPartition();
		right->CheckPartition();
	}
	else
	{
		EDITOR_LOG("K-D Tree: This node already has partitions")
	}
}

void KDTree::Node::SetParent(Node * parent)
{
	this->parent = parent;
}

void KDTree::Node::GetElements(std::vector<GameObject*>& elements) const
{
	std::vector<const Node*> nodes_to_visit;
	nodes_to_visit.push_back(this);
	
	while (!nodes_to_visit.empty())
	{
		std::vector<const Node*>::iterator node = nodes_to_visit.begin();

		//add childs to visit them later
		if ((*node)->left != nullptr && (*node)->right != nullptr)
		{
			nodes_to_visit.push_back((*node)->left);
			nodes_to_visit.push_back((*node)->right);
		}

		//add curr node elements to elements list
		for (int i = 0; i < (*node)->elements.size(); ++i)
		{
			elements.push_back((*node)->elements[i]);
		}

		//remove curr node from the nodes to visit
		nodes_to_visit.erase(node);
	}
}

KDTree::Node * KDTree::Node::GetLeft() const
{
	return left;
}

KDTree::Node * KDTree::Node::GetRight() const
{
	return right;
}

void KDTree::Node::DrawPlane(float size_x, float size_z, float3 prev_translation)
{
	if (right != nullptr && left != nullptr)
	{
		//calculate the transform to apply to a unit plane defined on axis x,z
		//Scale
		float3 size(size_x, 1.f, size_z);

		//Rotation and translation and set color 
		Quat rotation;
		Color c;
		float3 translation = prev_translation;

		float next_x = size_x;
		float next_z = size_z;
		float center_distance = (size_x > size_z) ? size_z : size_x;

		switch (axis)
		{
		case KDTree::Node::A_X:
			//rotation
			rotation = Quat::FromEulerXYZ(0 * DEGTORAD, 0 * DEGTORAD, 90 * DEGTORAD);

			//translation
			translation.x = cut_plane.d;
			if (parent != nullptr)
			{
				float diff = (center_distance / 2)*((parent->right == this) ? 1 : -1);
				translation.y += diff;
			}
			next_x /= 2;

			//set color green
			c.Set(0, 1.f, 0, .5f);
			break;
		case KDTree::Node::A_Y:
			//rotation
			rotation = Quat::identity;

			//translation
			translation.y = cut_plane.d;
			if (parent != nullptr)
			{
				float diff = (center_distance / 2)*((parent->right == this) ? 1 : -1);
				translation.z += diff;
			}
			//next_x /= 2;

			//set color red
			c.Set(1.f, 0, 0, .5f);
			break;
		case KDTree::Node::A_Z:
			//rotation
			rotation = Quat::FromEulerXYZ(90 * DEGTORAD, 0 * DEGTORAD, 0 * DEGTORAD);

			//translation
			translation.z = cut_plane.d;
			if (parent != nullptr)
			{
				float diff = (center_distance / 2)*((parent->right == this) ? 1 : -1);
				translation.x += diff;
			}
			next_z /= 2;

			//set color red
			c.Set(0, 0, 1.f, .5f);
			break;
		}
		//calculate transform from data below
		float4x4 transfrom = float4x4::FromTRS(translation, rotation, size);

		//Draw the plane
		App->renderer3D->PushMatrix();
		App->renderer3D->MultMatrix(transfrom.Transposed().ptr());

		App->renderer3D->SetColor(c);

		App->mesh->plane->Render();

		App->renderer3D->ResetColor();

		App->renderer3D->PopMatrix();

		//Draw children if needed
		left->DrawPlane(next_x, next_z, translation);
		right->DrawPlane(next_x, next_z, translation);
	}
}

void KDTree::Node::GetElementsToTest(const Ray & ray, float p1_distance, float p2_distance, std::vector<GameObject*>& vec_to_fill) const
{
	if (right != nullptr && left != nullptr)
	{
		//check on which side of the plane the ray is defined
		//get a segment of the ray
		float3 p1 = ray.GetPoint(p1_distance);
		float3 p2 = ray.GetPoint(p2_distance);
		
		//check if this segment is on one of the plane sides or it crosses the plane
		if (cut_plane.AreOnSameSide(p1, p2))
		{
			//check on which side the segment is
			if (cut_plane.IsOnPositiveSide(p1))
			{
				right->GetElementsToTest(ray, p1_distance, p2_distance, vec_to_fill);
			}
			else
			{
				left->GetElementsToTest(ray, p1_distance, p2_distance, vec_to_fill);
			}
		}
		else //the segment 
		{
			right->GetElementsToTest(ray, p1_distance, p2_distance, vec_to_fill);
			left->GetElementsToTest(ray, p1_distance, p2_distance, vec_to_fill);
		}
	}
	else
	{
		GetElements(vec_to_fill);
	}
}

void KDTree::Node::GetElementsToTest(const AABB & box, std::vector<GameObject*>& vec_to_fill) const
{
	if (right != nullptr && left != nullptr)
	{
		//check on which side of the plane the AABB is defined
		//first, check if the box is on one side or the plane crosses the box
		if (cut_plane.AreOnSameSide(box.maxPoint, box.minPoint))
		{
			//check on which side the box is
			if (cut_plane.IsOnPositiveSide(box.maxPoint))
			{
				right->GetElementsToTest(box, vec_to_fill);
			}
			else
			{
				left->GetElementsToTest(box, vec_to_fill);
			}
		}
		else //the segment 
		{
			right->GetElementsToTest(box, vec_to_fill);
			left->GetElementsToTest(box, vec_to_fill);
		}
	}
	else
	{
		GetElements(vec_to_fill);
	}
}

void KDTree::Node::GetElementsToTest(const Frustum & frustum, std::vector<GameObject*>& vec_to_fill) const
{
	if (right != nullptr && left != nullptr)
	{
		//check on which side of the plane the frustum is defined
		//first, check if the frustum is on one side or the plane crosses the frustum
		uint on_pos_side = 0;
		uint on_neg_side = 0;

		for (int corner = 0; corner < 8; ++corner)
		{
			if (cut_plane.IsOnPositiveSide(frustum.CornerPoint(corner)))
			{
				on_pos_side++;
			}
			else
				on_neg_side++;
		}

		if (on_pos_side == 8 || on_neg_side == 8)
		{
			//check on which side the frustum is
			if (on_pos_side > 0)
			{
				right->GetElementsToTest(frustum, vec_to_fill);
			}
			else
			{
				left->GetElementsToTest(frustum, vec_to_fill);
			}
		}
		else //the segment 
		{
			right->GetElementsToTest(frustum, vec_to_fill);
			left->GetElementsToTest(frustum, vec_to_fill);
		}
	}
	else
	{
		GetElements(vec_to_fill);
	}
}

void KDTree::Node::GetElementsToTest(const LineSegment & segment, std::vector<GameObject*>& vec_to_fill) const
{
	if (right != nullptr && left != nullptr)
	{
		//check if this segment is on one of the plane sides or it crosses the plane
		if (cut_plane.AreOnSameSide(segment.a, segment.b))
		{
			//check on which side the segment is
			if (cut_plane.IsOnPositiveSide(segment.a))
			{
				right->GetElementsToTest(segment, vec_to_fill);
			}
			else
			{
				left->GetElementsToTest(segment, vec_to_fill);
			}
		}
		else //the segment 
		{
			right->GetElementsToTest(segment, vec_to_fill);
			left->GetElementsToTest(segment, vec_to_fill);
		}
	}
	else
	{
		GetElements(vec_to_fill);
	}
}

void KDTree::Node::CheckPartition()
{
	if (elements.size() > partition_num)
		CreatePartition();
}

KDTree::KDTree()
{
}

KDTree::~KDTree()
{
}

void KDTree::CreateTree(std::vector<GameObject*>& elements, uint ele_on_partition)
{
	if (root_node == nullptr)
	{
		root_node = new Node(ele_on_partition);
		for (int i = 0; i < elements.size(); ++i)
		{
			root_node->AddElement(elements[i]);
		}
		root_node->CheckPartition();
	}
}

void KDTree::EraseTree()
{
	if (root_node != nullptr) 
	{
		std::vector<Node*> nodes_to_visit;
		nodes_to_visit.push_back(root_node);
		root_node = nullptr;

		while (!nodes_to_visit.empty())
		{
			//add childs to visit them later
			if ((*nodes_to_visit.begin())->GetLeft() != nullptr && (*nodes_to_visit.begin())->GetRight() != nullptr)
			{
				nodes_to_visit.push_back((*nodes_to_visit.begin())->GetLeft());
				nodes_to_visit.push_back((*nodes_to_visit.begin())->GetRight());
			}

			//Remove node data
			RELEASE(*nodes_to_visit.begin());

			//remove curr node from the nodes to visit
			nodes_to_visit.erase(nodes_to_visit.begin());
		}
	}
}

void KDTree::GetElementsToTest(const Frustum & frustum, std::vector<GameObject*>& vec_to_fill) const
{
	if (root_node != nullptr)
	{
		root_node->GetElementsToTest(frustum, vec_to_fill);
	}
}

void KDTree::GetElementsToTest(const Ray & ray, float p1_distance, float p2_distance, std::vector<GameObject*>& vec_to_fill) const
{
	if (root_node != nullptr)
	{
		root_node->GetElementsToTest(ray, p1_distance, p2_distance, vec_to_fill);
	}
}

void KDTree::GetElementsToTest(const AABB & box, std::vector<GameObject*>& vec_to_fill) const
{
	if (root_node != nullptr)
	{
		root_node->GetElementsToTest(box, vec_to_fill);
	}
}

void KDTree::GetElementsToTest(const LineSegment & segment, std::vector<GameObject*>& vec_to_fill) const
{
	if (root_node != nullptr)
	{
		root_node->GetElementsToTest(segment, vec_to_fill);
	}
}

bool KDTree::HasTree() const
{
	return root_node != nullptr;
}

void KDTree::DebugDraw() const
{
	if (root_node != nullptr)
	{
		float3 initial_translation = float3::zero;
		//root_node->DrawPlane(100, 100, initial_translation);
	}
}
