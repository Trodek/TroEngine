#ifndef __KDTree__
#define __KDTree__

#include "Globals.h"
#include <vector>
#include "MathGeoLib.h"

class GameObject;

class KDTree
{
public:
	class Node
	{
	public:
		enum Axis 
		{
			A_NULL,
			A_X,
			A_Y,
			A_Z
		};

	public:
		Node(uint partition_num);
		~Node();

		//Add an element to this node.
		void AddElement(GameObject* element);

		void CheckPartition();

		//fills elements vector with all elements of this node and child nodes
		void GetElements(std::vector<GameObject*>& elements) const;

		Node* GetLeft()const;
		Node* GetRight()const;

		void DrawPlane(float size_x, float size_z, float3 prev_translation);

		void GetElementsToTest(const Ray& ray, float p1_distance, float p2_distance, std::vector<GameObject*> vec_to_fill)const;
		void GetElementsToTest(const AABB& box, std::vector<GameObject*> vec_to_fill) const;
		void GetElementsToTest(const Frustum& frustum, std::vector<GameObject*> vec_to_fill) const;

	private:
		//Divides this node into 2 nodes that contais the elements (half on each node)
		void CreatePartition();
		void SetParent(Node* parent);

	public:
		Plane cut_plane;
		Axis axis = A_NULL;

	private:
		Node* right = nullptr;
		Node* left = nullptr;
		Node* parent = nullptr;

		std::vector<GameObject*> elements;

		uint partition_num = 1;
	};

public:
	KDTree();
	~KDTree();

	void CreateTree(std::vector<GameObject*>& elements, uint ele_on_partition = 5);

	void EraseTree();

	void GetElementsToTest(const Frustum& frustum, std::vector<GameObject*> vec_to_fill) const;
	void GetElementsToTest(const Ray& ray, float p1_distance, float p2_distance, std::vector<GameObject*> vec_to_fill) const;
	void GetElementsToTest(const AABB& box, std::vector<GameObject*> vec_to_fill) const;

	bool HasTree()const;
	void DebugDraw()const;

private:
	Node* root_node = nullptr;

};

#endif
