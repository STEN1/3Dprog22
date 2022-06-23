#include "RotatingSun.h"

#include "VisualObject/Mesh.h"
#include "Core/TextureManager.h"
#include "Core/Globals.h"
#include "Scene/Scene.h"

RotatingSun::RotatingSun(Scene& scene, const glm::mat4& transform)
	: GameObject(scene, transform)
{
	m_vo = std::make_unique<Mesh>(*this,
		Globals::AssetPath + std::string("Ball.obj"),
		TextureManager::GetTexture("Yellow1x1.png"));
	m_StartPos = GetPosition();
	objectType = ObjectType::Dynamic;
	m_physicsShape = std::make_unique<Sphere>();
	static_cast<Sphere*>(m_physicsShape.get())->radius = 1.f;
	m_lightRange = 200.f;
}

void RotatingSun::Update(float deltaTime)
{
	float currentTime = m_scene.CurrentTime();
	float x = sinf(currentTime) * m_Radius;
	float z = cosf(currentTime) * m_Radius;
	auto pos = m_StartPos;
	pos.x += x;
	pos.z += z;
	SetPosition(pos);
}
