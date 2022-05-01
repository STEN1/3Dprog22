#pragma once
#include "GameObject.h"

class ItemPickup : public GameObject
{
	friend class WoodsScene;
	friend class HeightmapScene;
public:
	ItemPickup(Scene& scene, glm::mat4 transform);
	virtual void BeginOverlap(GameObject* other) override;
	virtual void Update(float deltaTime) override;
	float LightScaleRange{ 10.f };
private:
	bool m_bob{ true };
	glm::vec3 m_initialPosition;
	float m_hightOffset{};
	float m_bobDir{ 1.f };
	float m_bobHight{ 2.f };
	float m_scaleOffset;
};
