#include "RedTrophy.h"

#include "VisualObject/Mesh.h"
#include "Core/TextureManager.h"
#include "Core/Globals.h"

RedTrophy::RedTrophy(Scene& scene, const glm::mat4& transform)
	: GameObject(scene, transform)
{
	m_vo = std::make_unique<Mesh>(*this,
		Globals::AssetPath + std::string("Ball.obj"),
		TextureManager::GetTexture("Rect.png"));
	m_name = "RedTrophy";
	objectType = ObjectType::Dynamic;
	m_physicsShape = std::make_unique<Sphere>();
	auto sphere = static_cast<Sphere*>(m_physicsShape.get());
	sphere->radius = 5.f;
	sphere->pos = GetPosition();
	glm::vec3 color{ 0.9f, 0.1f, 0.1f };
	m_scene.AddLight(this, color);
}

void RedTrophy::BeginOverlap(GameObject* other)
{
	if (other->GetName() == "Player")
	{
		Destroy();
	}
}
