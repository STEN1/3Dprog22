#pragma once

#include "Scene.h"

class EksamenScene : public Scene
{
public:
	EksamenScene();
private:
	uint32_t m_TrophyCount{ 10 };
};
