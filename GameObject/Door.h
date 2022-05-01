#pragma once
#include "GameObject.h"

class Door : public GameObject
{
public:
	Door(Scene& scene, const glm::mat4 transform);
	virtual void BeginOverlap(GameObject* other) override;
	virtual void EndOverlap(GameObject* other) override;
	virtual void Update(float deltaTime) override;
private:
	bool m_shouldOpen{};
	glm::mat4 m_closedTransform;
	float m_doorAngle{};
	float m_doorMaxAngle{ glm::half_pi<float>() };
	float m_doorSpeed{ 1.f };
};
