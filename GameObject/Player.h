#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(Scene& scene, std::shared_ptr<class Camera> camera);
	virtual void Update(float deltaTime) override;
	virtual void EndOverlap(GameObject* other) override;
	int GetScore() { return m_score; }
	float MoveSpeed{ 1.f };
private:
	std::shared_ptr<class Camera> m_camera;
	int m_score{};
	float m_currentRotation{};
	float m_cameraRadius{ 10.f };
	float m_JumpForce{ 20.f };
	bool m_Stunned{ false };
	bool m_Stuntimer{ 0.f };
};