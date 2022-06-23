#pragma once
#include "VisualObject/VisualObject.h"
#include "Core/PhysicsShapes.h"
#include "renderwindow.h"
#include "Core/Logger.h"
#include "Core/SoundSource.h"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/quaternion.hpp"

class Scene;
class GameObject
{
	friend class Octree;
	friend class Scene;
	friend class HouseScene;
	friend class WoodsScene;
	friend class HeightmapScene;
public:
	enum class ObjectType
	{
		Static = 0,
		Dynamic,
		Simulated
	};
	GameObject() = delete;
	GameObject(Scene& scene, glm::mat4 transform);
	GameObject(Scene& scene);
	virtual ~GameObject();
	virtual void Draw();
	virtual void Update(float deltaTime);
	virtual void BeginOverlap(GameObject* other);
	virtual void EndOverlap(GameObject* other);
	void AddPositionOffset(const glm::vec3& offset);
	void SetPosition(const glm::vec3& pos);
	void SetVisualObject(std::unique_ptr<VisualObject> visualObject) { m_vo = std::move(visualObject); }
	float GetLightRange() { return m_lightRange; }
	glm::mat4 GetTransform() { return m_transform; }
	glm::vec3 GetPosition() const;
	glm::vec3 GetVelocity() const { return m_Velocity; }
	void SetVelocity(const glm::vec3& newVel) { m_Velocity = newVel; }
	void AddVelocity(const glm::vec3& vel) { m_Velocity += vel; }
	std::string GetName() { return m_name; }
	void Destroy();
	ObjectType objectType{ ObjectType::Static };
	void SetLightRange(float range) { m_lightRange = range; }
	bool Solid{ false };
	bool UI{ false };
	PhysicsShape* GetPhysicsShape() { return m_physicsShape.get(); }
protected:
	std::string m_name;
	std::unique_ptr<VisualObject> m_vo;
	glm::mat4 m_transform;
	glm::vec3 m_Velocity{ 0.f };
	std::unique_ptr<PhysicsShape> m_physicsShape{};
	std::vector<GameObject*> m_overlappingGameObjects;
	Scene& m_scene;
	float m_lightRange{ 50.f };
	bool m_Grounded{ false };
};


