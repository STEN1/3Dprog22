#pragma once

#include "GameObject.h"

class SolidWall : public GameObject
{
public:
	SolidWall(Scene& scene, const glm::mat4 transform);
	AABB* aabb{};
};