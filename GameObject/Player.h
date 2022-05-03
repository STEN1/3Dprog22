#pragma once
#include "GameObject.h"

class Player : public GameObject
{
public:
	Player(Scene& scene, std::shared_ptr<class Camera> camera);
	virtual void Update(float deltaTime) override;
	virtual void EndOverlap(GameObject* other) override;
	virtual void BeginOverlap(GameObject* other) override;
	void Win();
	void Lose();
	int GetScore() { return m_score; }
	float MoveSpeed{ 40.f };
	float TurnSpeed{ 3.f };
	bool GameOver{ false };
private:
	void PlaceTextInfrontOfPlayer(const std::string& UPPERCASETEXT);
	std::shared_ptr<class Camera> m_camera;
	int m_score{};
	float m_currentRotation{};
	float m_cameraRadius{ 20.f };
	float m_JumpForce{ 40.f };
	bool m_Stunned{ false };
	float m_Stuntimer{ 0.f };
};