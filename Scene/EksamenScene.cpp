#include "EksamenScene.h"

#include "GameObject/Terrain.h"
#include "Core/Globals.h"
#include "GameObject/Player.h"
#include "GameObject/RotatingSun.h"
#include "GameObject/BezierNPC.h"
#include "GameObject/RedTrophy.h"
#include "GameObject/BlueTrophy.h"
#include "GameObject/PathfindingNPC.h"

EksamenScene::EksamenScene()
{
	m_heightmap = std::make_unique<Terrain>(*this,
		"EksamenHeightmap.png",
		"Landscape.png");
	m_PlayerStartPos = { 256.f, 0.f, 256.f };
	auto sunTransform = glm::translate(glm::vec3{ 256.f, 20.f, 256.f });
	auto rotatingSun = m_gameObjects.emplace_back(new RotatingSun(*this, sunTransform));
	PointLight rotatingSunLight;
	glm::vec4 rotatingSunColor{ 0.95f, 0.95f, 0.05f, 1.f };
	rotatingSunLight.diffuse = rotatingSunColor;
	rotatingSunLight.ambient = rotatingSunColor * 0.1f;
	rotatingSunLight.specular = glm::vec4(1.f);
	m_pointLights[rotatingSun] = rotatingSunLight;
	auto bezierNPCTransform = glm::translate(glm::vec3{ 256.f, 20.f, 256.f });
	auto bezierNpc = m_gameObjects.emplace_back(new BezierNPC(*this, bezierNPCTransform));
	for (uint32_t i = 0; i < m_TrophyCount; i++)
	{
		float x = (rand() / (float)RAND_MAX) * 400.f + 50.f;
		float z = (rand() / (float)RAND_MAX) * 400.f + 50.f;
		float y = m_heightmap->GetHeightFromPos(glm::vec3{ x, 0.f, z }) + 1.f;
		m_gameObjects.emplace_back(new RedTrophy(*this, glm::translate(glm::vec3{ x, y, z })));
	}
	for (uint32_t i = 0; i < m_TrophyCount; i++)
	{
		float x = (rand() / (float)RAND_MAX) * 400.f + 50.f;
		float z = (rand() / (float)RAND_MAX) * 400.f + 50.f;
		float y = m_heightmap->GetHeightFromPos(glm::vec3{ x, 0.f, z }) + 1.f;
		m_gameObjects.emplace_back(new BlueTrophy(*this, glm::translate(glm::vec3{ x, y, z })));
	}
	auto pathfindingNPCSTransform = glm::translate(glm::vec3{ 50.f, 0.f, 50.f });
	glm::vec3 pathfindingCenter{ 256.f, 20.f, 256.f };
	glm::vec3 pathfindingExtents{ 512.f };
	m_gameObjects.emplace_back(new PathfindingNPC(*this, pathfindingNPCSTransform,
		pathfindingCenter, pathfindingExtents));
}
