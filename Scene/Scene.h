#pragma once
#include <unordered_map>
#include <vector>
#include "glm/vec3.hpp"
#include <memory>
#include "Core/PointLight.h"
#include "Core/SoundSource.h"
#include "Core/Threadpool/thread_pool.hpp"
#include "Core/Octree.h"
#include "GameObject/GameObject.h"

class Octree;
class Camera;
class Scene
{
	friend class CameraMesh;
public:
	Scene();
	virtual ~Scene();
	void Simulate(float deltaTime);
	virtual void Update(float deltaTime);
	virtual void Render();
	static float CurrentTime();
	static float TerrainFunc(float x, float z);
	static float TerrainFuncX(float x);
	static float TerrainFuncZ(float z);
	virtual void MouseClicked(int mouseButton);
	virtual void MouseReleased(int mouseButton);
	virtual void KeyPressed(int keyCode);
	void Move(GameObject* gameObject, const glm::vec3& newPos);
	void Destroy(GameObject* gameObject);
	glm::vec3 GetSunDirection();
	glm::vec3 GetSunColor();
	void SetSunDirection(const glm::vec3& dir);
	void SetSunColor(const glm::vec3& color);
	void AddLight(GameObject* go, const glm::vec3& color);
	void ToggleDebugLines();
	bool IsDrawingDebugLines() const { return m_DrawDebugLines; }
	void Init();
	float GetHeightFromHeightmap(const glm::vec3& pos);
	template<class T, class... Args>
	GameObject* SpawnGameObject(Args&&... args);
	template<class T>
	std::vector<T*> GetGameObjectsOfClass();
protected:
	inline static float s_currentTime{};
	class RenderWindow* m_renderWindow;
	std::vector<GameObject*> m_gameObjects;
	std::vector<GameObject*> m_UI;
	std::vector<GameObject*> m_staticGameObjects;
	std::vector<GameObject*> m_dynamicGameObjects;
	std::vector<GameObject*> m_simulatedGameObjects;
	std::vector<GameObject*> m_transparentGameObjects;
	std::vector<GameObject*> m_opaqueGameObjects;
	std::unique_ptr<Octree> m_octree;
	std::shared_ptr<Camera> m_camera;
	std::unordered_map<GameObject*, PointLight> m_pointLights;
	std::vector<GameObject*> m_toBeDestroyed;
	std::unique_ptr<class Terrain> m_heightmap;
	std::shared_ptr<SoundSource> m_startSound;
	std::unique_ptr<class Sky> m_skybox;
	std::unique_ptr<class Sun> m_Sun;
	std::unique_ptr<class TextBillboard> m_testBillboard;
	std::unique_ptr<class CameraMesh> m_CameraMesh;
	class Player* m_Player{ nullptr };
	glm::vec3 m_PlayerStartPos{0.f};
private:
	glm::vec3 m_SunDirection;
	glm::vec3 m_SunColor;
	std::vector<std::pair<GameObject*, GameObject*>> FindCollisions();
	std::vector<std::pair<GameObject*, GameObject*>> FindCollisions2();
	//std::vector<std::pair<GameObject*, GameObject*>> FindCollisions3();
	void ResolveOverlapp(const std::vector<std::pair<GameObject*, GameObject*>>& goVec);
	void RemoveDestroyedGameObjects();
	void RenderQuads();
	bool m_DrawDebugLines = false;
	float m_maxLightDistance = 1000.f;
	//thread_pool m_ThreadPool;
	std::unique_ptr<class Frustum> m_ViewFrustum;
};

template<class T, class ...Args>
inline GameObject* Scene::SpawnGameObject(Args && ...args)
{
	GameObject* go = m_gameObjects.emplace_back(new T(std::forward<Args>(args)...));
	if (go->UI)
		m_UI.push_back(go);
	if (go->objectType == GameObject::ObjectType::Static)
	{
		m_staticGameObjects.push_back(go);
	}
	else if (go->objectType == GameObject::ObjectType::Dynamic)
	{
		m_dynamicGameObjects.push_back(go);
		m_octree->Insert(go);
	}
	else if (go->objectType == GameObject::ObjectType::Simulated)
	{
		m_dynamicGameObjects.push_back(go);
		m_simulatedGameObjects.push_back(go);
		m_octree->Insert(go);
	}
	if (!go->m_vo)
		return go;
	if (go->m_vo->IsTransparent() && go->objectType != GameObject::ObjectType::Static)
	{
		m_transparentGameObjects.push_back(go);
	}
	else if (!go->m_vo->IsTransparent() && go->objectType != GameObject::ObjectType::Static)
	{
		m_opaqueGameObjects.push_back(go);
	}
	return go;
}

template<class T>
inline std::vector<T*> Scene::GetGameObjectsOfClass()
{
	std::vector<T*> out;
	for (auto go : m_gameObjects)
	{
		if (T* goOfClass = dynamic_cast<T*>(go))
		{
			out.push_back(goOfClass);
		}
	}
	return out;
}
