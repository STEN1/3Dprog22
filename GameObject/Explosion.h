#pragma once

#include "GameObject.h"

class Explosion : public GameObject
{
public:
	Explosion(Scene& scene, const glm::mat4& transform);
	virtual void Update(float deltaTime) override;
private:
	float m_Lifetime{};
	float m_MaxLifetime{ 1.f };
	Sphere* m_CollisionSphere;
};
