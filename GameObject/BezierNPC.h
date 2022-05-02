#pragma once
#include "GameObject.h"

class BezierNPC : public GameObject
{
public:
	BezierNPC(Scene& scene, const glm::mat4 transform);
	virtual void Update(float deltaTime) override;
	virtual void Draw() override;
private:
	std::unique_ptr<class Mesh> m_NPCMesh;
	float m_DropTimer{};
	float m_DropInterval{ 2.f };
};
