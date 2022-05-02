#pragma once

#include "GameObject.h"

class PathfindingNPC : public GameObject
{
public:
	PathfindingNPC(Scene& scene, const glm::mat4& transform, const glm::vec3& pathfindingCenter,
		const glm::vec3& pathfindingExtents);
	virtual void Update(float deltaTime) override;
	virtual void BeginOverlap(GameObject* other) override;
private:
	void MoveToTarget(float deltaTime);
	void UpdateTarget();
	glm::vec3 m_Target;
	float m_Speed{ 50.f };
	bool m_Stunned{ false };
	float m_StunTimer{};
};
