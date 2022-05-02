#pragma once

#include "GameObject.h"

class RedTrophy : public GameObject
{
public:
	RedTrophy(Scene& scene, const glm::mat4& transform);
	virtual void BeginOverlap(GameObject* other) override;
};
