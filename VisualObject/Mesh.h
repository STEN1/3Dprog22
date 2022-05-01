#pragma once
#include "VisualObject.h"
#include <unordered_map>

class Mesh : public VisualObject
{
	friend class Scene;
public:
	enum class VertexLayout
	{
		Indexed,
		Arrays,
	};
	Mesh(GameObject& parent, const std::string& mesh, uint32_t texture, const glm::mat4& transform = glm::mat4(1.f));
	~Mesh();

	void Draw();
	void SetTexture(uint32_t texture);
private:
	void DrawIndexed();
	void DrawArrays();

	uint32_t m_texture;
	VertexLayout m_vertexLayout;
	GLenum m_drawMode;
	uint32_t m_IBO;

	struct CachedMesh
	{
		uint32_t VAO{};
		uint32_t IBO{};
		VertexLayout Layout{};
		uint32_t Indices{};
		uint32_t VertexCount{};
		uint32_t Texture{};
	};

	inline static std::unordered_map<uint32_t, uint32_t> s_meshCacheCounter;
	inline static std::unordered_map<std::string, CachedMesh> s_meshCache;
};