#include "EksamenScene.h"

#include "GameObject/Terrain.h"
#include "Core/Globals.h"
#include "GameObject/Player.h"
#include "GameObject/RotatingSun.h"

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
}
