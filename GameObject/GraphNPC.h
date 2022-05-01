#pragma once
#include "GameObject.h"

class GraphNPC : public GameObject
{
	friend class Scene;
	friend class WoodsScene;
public:
	GraphNPC(Scene& scene, float graphFunc(float z), float terrainFunc(float x, float z));
	virtual void Update(float deltaTime) override;
private:
	float (*m_graphFunc)(float z);
	float (*m_terrainFunc)(float x, float z);
	float m_z{};
	float m_direction{ 1.f };
};
