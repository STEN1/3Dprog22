#pragma once

#include "Scene.h"

class EksamenScene : public Scene
{
public:
	EksamenScene();
private:
	uint32_t m_TrophyCount{ 20 };
	uint32_t m_WallCount{ 200 };
};
