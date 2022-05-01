#pragma once
#include "GameObject.h"

class AnimatedBezier : public GameObject
{
public:
	AnimatedBezier(Scene& scene);
	virtual void Update(float deltaTime) override;
};
