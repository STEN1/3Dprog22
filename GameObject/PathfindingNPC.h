#pragma once

#include "GameObject.h"

class PathfindingNPC : public GameObject
{
public:
	PathfindingNPC(Scene& scene, const glm::mat4& transform, const glm::vec3& pathfindingCenter,
		const glm::vec3& pathfindingExtents);
	virtual void Update(float deltaTime) override;
private:
	void MoveToTarget(float deltaTime);
	void UpdateTarget();
	glm::vec3 m_Target;
	float m_Speed{ 20.f };
};
