#include "KDTree.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include <algorithm>

KDTree::Node::Node(uint partition_num) : partition_num(partition_num)
{
}

KDTree::Node::~Node()
{
}

void KDTree::Node::AddElement(GameObject * element)
{
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
			if (!elements[i]->HasComponent(Component::Type::MeshRenderer))
				continue;

			MeshRenderer* mr = (MeshRenderer*)elements[i]->GetComponent(Component::Type::MeshRenderer);

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

			MeshRenderer* mr = (MeshRenderer*)(*ele)->GetComponent(Component::Type::MeshRenderer);

			if (mr == nullptr)
			{
				++ele;
				continue;
			}

			switch (axis)
			{
			case KDTree::Node::A_X:
				if (mr->GetMeshAABB().MaxX() > mid_point)
				{
					if (mr->GetMeshAABB().MinX() > mid_point)
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
				if (mr->GetMeshAABB().MaxY() > mid_point)
				{
					if (mr->GetMeshAABB().MinY() > mid_point)
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
				if (mr->GetMeshAABB().MaxZ() > mid_point)
				{
					if (mr->GetMeshAABB().MinZ() > mid_point)
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

void KDTree::GetElementsToTest(const Frustum & frustum) const
{
}

void KDTree::GetElementsToTest(const Ray & ray) const
{
}

void KDTree::GetElementsToTest(const AABB & box) const
{
}

bool KDTree::HasTree() const
{
	return root_node != nullptr;
}
