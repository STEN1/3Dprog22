#include "Bomb.h"
#include "Explosion.h"
#include "VisualObject/Mesh.h"
#include "Core/TextureManager.h"
#include "Core/Globals.h"

Bomb::Bomb(Scene& scene, const glm::mat4& transform)
	: GameObject(scene, transform)
{
	m_vo = std::make_unique<Mesh>(*this,
		Globals::AssetPath + std::string("Ball.obj"),
		TextureManager::GetTexture("white1x1.png"));
	objectType = ObjectType::Simulated;
	m_name = "Bomb";
	m_physicsShape = std::make_unique<Sphere>();
	static_cast<Sphere*>(m_physicsShape.get())->radius = 2.f;
	static_cast<Sphere*>(m_physicsShape.get())->pos = GetPosition();
}

void Bomb::Update(float deltaTime)
{
	if (auto minY = 1.2f + m_scene.GetHeightFromHeightmap(GetPosition()); GetPosition().y < minY)
	{
		m_scene.SpawnGameObject<Explosion>(m_scene, glm::translate(GetPosition()));
		Destroy();
	}
}
