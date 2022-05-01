#pragma once
#include "VisualObject.h"

class Heightmap : public VisualObject
{
	friend class Terrain;
	friend class Scene;
public:
	Heightmap(GameObject& parent, const std::string& heightmap, const std::string& texture, float heightScale);
	virtual void Draw() override;
	bool IsInsideMap(const glm::vec3& pos);
	float GetHeightFromPos(const glm::vec3& pos);
	float TextureScale{ 1.f };
private:
	int m_height;
	int m_width;

	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
	};
	std::vector<Heightmap::Vertex> m_vertexData;
	std::vector<uint32_t> m_indexData;
	uint32_t m_texture;
	int m_seaTranparency = 0;
};
