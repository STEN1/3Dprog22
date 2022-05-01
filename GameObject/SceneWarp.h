#pragma once
#include "GameObject.h"

template<class T>
class SceneWarp : public GameObject
{
public:
	SceneWarp(Scene& scene);
	virtual void BeginOverlap(GameObject* other) override;
	virtual void Update(float deltaTime) override;
};

template<class T>
inline SceneWarp<T>::SceneWarp(Scene& scene)
	: GameObject(scene)
{
	objectType = ObjectType::Dynamic;
	m_name = "SceneWarp";
}

template<class T>
inline void SceneWarp<T>::BeginOverlap(GameObject* other)
{
	if (other->GetName() == "Player")
	{
		LOG("Loading scene...");
		RenderWindow::Get()->LoadScene<T>();
	}
}

template<class T>
inline void SceneWarp<T>::Update(float deltaTime)
{
	//RenderWindow::Get()->DrawAABB(GetPosition(), m_physicsBox.extent);
}
