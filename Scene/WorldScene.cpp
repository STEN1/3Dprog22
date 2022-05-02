#include "WorldScene.h"
#include "GameObject/Terrain.h"
#include "GameObject/Player.h"

WorldScene::WorldScene()
{
	m_heightmap = std::make_unique<Terrain>(*this, "Worldmap-export.png", "earth.png", 0.2f);
	m_PlayerStartPos = { 2000.f, 0.f, 5000.f };
}