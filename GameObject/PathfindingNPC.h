#pragma once

#include "GameObject.h"

struct Edge;
struct Node
{
	std::list<Edge*> Edges;
	AABB aabb;
	bool Visited{ false };
};

struct Edge
{
	Node* To{};
	Node* From{};
};

struct Path
{
	std::vector<Edge> Edges;
	float Cost{};
	float DistToEnd{};
	Node* GetLastNode() const
	{
		return Edges.back().To;
	}
	operator float() const { return Cost + DistToEnd; }
};

class PathfindingNPC : public GameObject
{
public:
	PathfindingNPC(Scene& scene, const glm::mat4& transform, const glm::vec3& pathfindingCenter,
		const glm::vec3& pathfindingExtents);
	virtual void Update(float deltaTime) override;
	void DebugDraw();
	virtual void BeginOverlap(GameObject* other) override;
	bool GameOver{ false };
private:
	class Path m_Path;
	std::unique_ptr<class Graph> m_Graph;
	void MoveToTarget(float deltaTime);
	void UpdateTarget();
	void UpdatePath();
	glm::vec3 GetClosestTrophyPos();
	glm::vec3 m_Target;
	glm::vec3 m_PathTargetTrophyPos;
	float m_Speed{ 25.f };
	bool m_Stunned{ false };
	float m_StunTimer{};
	uint32_t m_Score{};
};
