#include "EksamenScene.h"

#include "GameObject/Terrain.h"
#include "Core/Globals.h"
#include "GameObject/Player.h"

EksamenScene::EksamenScene()
{
	m_heightmap = std::make_unique<Terrain>(*this,
		"EksamenHeightmap.png",
		"Landscape.png");
	Player* player = static_cast<Player*>(m_gameObjects[0]);

	Init();
	player->SetPosition({ 256.f, 0.f, 256.f });
}
