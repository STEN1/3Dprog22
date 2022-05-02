#pragma once

#include "GameObject.h"

class RotatingSun : public GameObject
{
public:
	RotatingSun(Scene& scene, const glm::mat4& transform);
	virtual void Update(float deltaTime) override;
private:
	glm::vec3 m_StartPos;
	float m_Radius = 50.f;
};
