#pragma once
#include "Scene.h"

class HouseScene : public Scene
{
public:
	HouseScene();
	virtual void KeyPressed(int keyCode) override;
	virtual void Update(float deltaTime) override;
};