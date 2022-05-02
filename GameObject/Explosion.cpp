#include "Explosion.h"
#include "VisualObject/Mesh.h"
#include "Core/TextureManager.h"
#include "Core/Globals.h"

Explosion::Explosion(Scene& scene, const glm::mat4& transform)
	: GameObject(scene, transform)
{
	m_vo = std::make_unique<Mesh>(*this,
		Globals::AssetPath + std::string("Ball.obj"),
		TextureManager::GetTexture("Yellow1x1.png"));
	m_name = "Explosion";
	objectType = ObjectType::Dynamic;
	m_physicsShape = std::make_unique<Sphere>();
	m_CollisionSphere = static_cast<Sphere*>(m_physicsShape.get());
	m_CollisionSphere->radius = 5.f;
	m_CollisionSphere->pos = GetPosition();
	glm::vec3 color{ 0.9f, 0.1f, 0.1f };
	m_scene.AddLight(this, color);
}

void Explosion::Update(float deltaTime)
{
	m_CollisionSphere->radius += deltaTime * 2.f;
	m_lightRange = 100.f * m_CollisionSphere->radius;
	auto scaleMat = glm::scale(glm::vec3(m_CollisionSphere->radius));
	m_vo->SetLocalTransform(scaleMat);
	m_Lifetime += deltaTime;
	if (m_Lifetime > m_MaxLifetime)
		Destroy();
}
