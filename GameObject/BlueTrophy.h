#pragma once

#include "GameObject.h"

class BlueTrophy : public GameObject
{
public:
	BlueTrophy(Scene& scene, const glm::mat4& transform);
	virtual void BeginOverlap(GameObject* other) override;
};