#pragma once
#include "Scene.h"

class WoodsScene : public Scene
{
public:
	WoodsScene();
	virtual void KeyPressed(int keyCode) override;
	virtual void Update(float deltaTime) override;
};