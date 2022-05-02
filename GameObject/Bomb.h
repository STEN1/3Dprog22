#pragma once

#include "GameObject.h"

class Bomb : public GameObject
{
public:
	Bomb(Scene& scene, const glm::mat4& transform);
	virtual void Update(float deltaTime) override;
private:
};
