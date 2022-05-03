#include "PathfindingNPC.h"

#include "VisualObject/Mesh.h"
#include "Core/Globals.h"
#include "Core/TextureManager.h"
#include "BlueTrophy.h"
#include "GameObject/Player.h"

PathfindingNPC::PathfindingNPC(Scene& scene, const glm::mat4& transform, const glm::vec3& pathfindingCenter, const glm::vec3& pathfindingExtents)
	: GameObject(scene, transform)
{
	objectType = ObjectType::Simulated;
	m_vo = std::make_unique<Mesh>(*this,
		Globals::AssetPath + std::string("SecretObject.obj"),
		TextureManager::GetTexture("treehouse_door_color.png"));
	m_name = "PathfindingNPC";
	UpdateTarget();
	glm::vec3 color{ 0.8f, 0.8f, 0.8f };
	m_scene.AddLight(this, color);
}

void PathfindingNPC::Update(float deltaTime)
{
	if (GameOver)
		return;
	if (m_Stunned)
	{
		m_StunTimer += deltaTime;
		if (m_StunTimer > 2.f)
			m_Stunned = false;
		return;
	}
	MoveToTarget(deltaTime);
}

void PathfindingNPC::BeginOverlap(GameObject* other)
{
	if (other->GetName() == "Explosion" && m_Stunned == false)
	{
		m_Stunned = true;
		m_StunTimer = 0.f;
	}
	if (other->GetName() == "BlueTrophy")
	{
		LOG_HIGHLIGHT("ENEMY SCORE: " + std::to_string(++m_Score));
		if (m_Score == 10)
		{
			auto player = m_scene.GetGameObjectsOfClass<Player>();
			player[0]->Lose();
		}
	}
}

void PathfindingNPC::MoveToTarget(float deltaTime)
{
	glm::vec3 toTarget = m_Target - GetPosition();
	float distanceToTarget = glm::length(toTarget);
	if (distanceToTarget < 0.5f)
	{
		UpdateTarget();
	}
	else
	{
		AddPositionOffset(glm::normalize(toTarget) * deltaTime * m_Speed);
	}
}

void PathfindingNPC::UpdateTarget()
{
	auto targets = m_scene.GetGameObjectsOfClass<BlueTrophy>();
	if (targets.empty())
	{
		m_Target = GetPosition();
		LOG("No more targets. End game!");
		return;
	}
	BlueTrophy* closest{};
	float dist2{ powf(5000.f, 2.f) };
	for (auto target : targets)
	{
		auto toTrophy = target->GetPosition() - GetPosition();
		float distToTrophy2 = glm::length2(toTrophy);
		if (distToTrophy2 < dist2)
		{
			dist2 = distToTrophy2;
			closest = target;
		}
	}
	if (closest == nullptr)
	{
		m_Target = GetPosition();
		return;
	}
	m_Target = closest->GetPosition();
}
