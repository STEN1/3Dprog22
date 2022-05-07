#pragma once
#include "GameObject.h"

class Terrain : public GameObject
{
	friend class Scene;
	friend class HeightmapScene;
public:
	Terrain(Scene& scene);
	Terrain(Scene& scene, const std::string& heightmap, const std::string& texture, float heightScale = 0.5f);
	float GetHeightFromPos(const glm::vec3& pos);
	float GetWidth();
	float GetHeight();
private:
	bool m_isHeightmap;
};