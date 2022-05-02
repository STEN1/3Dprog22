#pragma once

#include "GameObject.h"

class CameraMesh : public GameObject
{
public:
	CameraMesh(Scene& scene);
	virtual void Draw() override;
};
