#include "EksamenScene.h"

#include "GameObject/Terrain.h"
#include "Core/Globals.h"
#include "GameObject/Player.h"

EksamenScene::EksamenScene()
{
	m_heightmap = std::make_unique<Terrain>(*this,
		"EksamenHeightmap.png",
		"Landscape.png");
	m_PlayerStartPos = { 256.f, 0.f, 256.f };
}
