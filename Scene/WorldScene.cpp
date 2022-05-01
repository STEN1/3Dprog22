#include "WorldScene.h"
#include "GameObject/Terrain.h"
#include "GameObject/Player.h"

WorldScene::WorldScene()
{
	m_heightmap = std::make_unique<Terrain>(*this, "Worldmap-export.png", "earth.png", 0.2f);
	Init();
	Player* player = static_cast<Player*>(m_gameObjects[0]);
	player->SetPosition({ 2000.f, 0.f, 5000.f });
}