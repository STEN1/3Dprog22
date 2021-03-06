#include "EksamenScene.h"

#include "GameObject/Terrain.h"
#include "Core/Globals.h"
#include "GameObject/Player.h"
#include "GameObject/RotatingSun.h"
#include "GameObject/BezierNPC.h"
#include "GameObject/RedTrophy.h"
#include "GameObject/BlueTrophy.h"
#include "GameObject/PathfindingNPC.h"
#include "GameObject/SolidWall.h"

EksamenScene::EksamenScene()
{
	m_heightmap = std::make_unique<Terrain>(*this, "loraheightmap.png", "lorasatelitt.png");
	float mapWidth = m_heightmap->GetWidth();
	float mapHeight = m_heightmap->GetHeight();
	srand(time(nullptr));
	m_PlayerStartPos = { mapHeight / 2.f, 0.f, mapWidth / 2.f };
	SetSunColor({ 0.1f, 0.1f, 0.1f });
	auto sunTransform = glm::translate(glm::vec3{ mapHeight / 2.f, 100.f, mapWidth / 2.f });
	auto rotatingSun = m_gameObjects.emplace_back(new RotatingSun(*this, sunTransform));
	PointLight rotatingSunLight;
	glm::vec4 rotatingSunColor{ 0.95f, 0.95f, 0.05f, 1.f };
	rotatingSunLight.diffuse = rotatingSunColor;
	rotatingSunLight.ambient = rotatingSunColor * 0.1f;
	rotatingSunLight.specular = glm::vec4(1.f);
	m_pointLights[rotatingSun] = rotatingSunLight;
	auto bezierNPCTransform = glm::translate(glm::vec3{ mapHeight / 2.f, 100.f, mapWidth / 2.f });
	auto bezierNpc = m_gameObjects.emplace_back(new BezierNPC(*this, bezierNPCTransform));
	for (uint32_t i = 0; i < m_WallCount; i++)
	{
		float x = (rand() / (float)RAND_MAX) * mapHeight;
		float z = (rand() / (float)RAND_MAX) * mapWidth;
		float y = m_heightmap->GetHeightFromPos(glm::vec3{ x, 0.f, z }) + 1.f;
		auto wallTransform = glm::translate(glm::vec3{ x, y, z });
		x = (rand() / (float)RAND_MAX) * 80.f + 1.f;
		z = (rand() / (float)RAND_MAX) * 80.f + 1.f;
		y = (rand() / (float)RAND_MAX) * 200.f + 10.f;
		wallTransform = glm::scale(wallTransform, glm::vec3{ x, y, z });
		m_gameObjects.emplace_back(new SolidWall(*this, wallTransform));
	}
	auto walls = GetGameObjectsOfClass<SolidWall>();
	for (uint32_t i = 0; i < m_TrophyCount;)
	{
		float x = (rand() / (float)RAND_MAX) * mapHeight;
		float z = (rand() / (float)RAND_MAX) * mapWidth;
		float y = m_heightmap->GetHeightFromPos(glm::vec3{ x, 0.f, z }) + 1.f;
		Sphere sphere;
		sphere.pos = { x, y, z };
		sphere.radius = 2.f;
		bool collidedWithWall{ false };
		for (auto wall : walls)
		{
			if (wall->aabb->Intersect(&sphere))
			{
				collidedWithWall = true;
				break;
			}
		}
		if (collidedWithWall == false)
		{
			m_gameObjects.emplace_back(new RedTrophy(*this, glm::translate(glm::vec3{ x, y, z })));
			i++;
		}
	}
	for (uint32_t i = 0; i < m_TrophyCount;)
	{
		float x = (rand() / (float)RAND_MAX) * mapHeight;
		float z = (rand() / (float)RAND_MAX) * mapWidth;
		float y = m_heightmap->GetHeightFromPos(glm::vec3{ x, 0.f, z }) + 1.f;
		Sphere sphere;
		sphere.pos = { x, y, z };
		sphere.radius = 4.f;
		bool collidedWithWall{ false };
		for (auto wall : walls)
		{
			if (wall->aabb->Intersect(&sphere))
			{
				collidedWithWall = true;
				break;
			}
		}
		if (collidedWithWall == false)
		{
			m_gameObjects.emplace_back(new BlueTrophy(*this, glm::translate(glm::vec3{ x, y, z })));
			i++;
		}
	}
	float pathfinderY = GetHeightFromHeightmap(glm::vec3{ 50.f, 0.f, 50.f });
	auto pathfindingNPCSTransform = glm::translate(glm::vec3{ 50.f, pathfinderY, 50.f });
	glm::vec3 pathfindingCenter{ mapHeight / 2.f, 100.f, mapWidth / 2.f };
	glm::vec3 pathfindingExtents{ mapHeight / 2.f, (mapHeight + mapWidth) / 4.f, mapWidth / 2.f };
	m_gameObjects.emplace_back(new PathfindingNPC(*this, pathfindingNPCSTransform,
		pathfindingCenter, pathfindingExtents));
}
