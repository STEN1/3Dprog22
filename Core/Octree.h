#pragma once
#include "PhysicsShapes.h"
#include <vector>

class GameObject;
class Octree
{
	friend class Scene;
public:
	Octree(const AABB& physicsBox);
	~Octree();
	void DrawExtents();
	void Insert(GameObject* gameObject);
	void Move(GameObject* gameObject, const glm::vec3& pos);
	void Remove(GameObject* gameObject);
	// Has some problems. see cpp comment.
	void RemoveAndEndOverlap(GameObject* gameObject);
	void ResolveOverlappingGameObjects();
	void Find(PhysicsShape* shape, std::vector<GameObject*>& outVector);
	void FindCollisions(std::vector<std::pair<GameObject*, GameObject*>>& outVector);
	uint32_t NumGameObjects();
	bool IsLeaf();
	void FindActiveLeafs(std::vector<Octree*>& outVector);
private:
	bool IsActiveLeaf();
	void TryCollapse();
	void Divide();
	void Collapse();
	bool HasOnlyLeafs();
	std::vector<GameObject*> m_gameObjects;
	std::vector<Octree*> m_childNodes;
	AABB m_physicsBox{};
	uint32_t m_maxObjects{ 20 };
	float m_minExtent{ 5.f };
};