#include "Terrain.h"

#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "VisualObject/TriangleSurface.h"
#include "VisualObject/Heightmap.h"

Terrain::Terrain(Scene& scene)
	: GameObject(scene, glm::mat4(1.f))
	, m_isHeightmap(false)
{
	m_vo = std::make_unique<TriangleSurface>(
		*this,
		ShaderManager::GetShaderRef("landscape"),
		TextureManager::GetTexture("Landscape.png"),
		"../3Dprog22/assets/triangle_surface.txt"
		);
}

Terrain::Terrain(Scene& scene, const std::string& heightmap, const std::string& texture, float heightScale)
	: GameObject(scene, glm::mat4(1.f))
	, m_isHeightmap(true)
{
	m_vo = std::make_unique<Heightmap>(*this, heightmap, texture, heightScale);
}

float Terrain::GetHeightFromPos(const glm::vec3& pos)
{
	if (m_isHeightmap)
	{
		auto heightmap = static_cast<Heightmap*>(m_vo.get());
		if (heightmap->IsInsideMap(pos))
		{
			return heightmap->GetHeightFromPos(pos);
		}
	}
	return Scene::TerrainFunc(pos.x, pos.z);
}

float Terrain::GetWidth()
{
	return static_cast<Heightmap*>(m_vo.get())->m_width;
}

float Terrain::GetHeight()
{
	return static_cast<Heightmap*>(m_vo.get())->m_height;
}
