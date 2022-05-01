#pragma once
#include "VisualObject.h"

class Quad : public VisualObject
{
	friend class Scene;
public:
	Quad() = delete;
	Quad(GameObject& parent, uint32_t texture, glm::mat4 transform = glm::mat4(1.f));
	~Quad();
	void Draw() override;
private:
	uint32_t m_texture;
	static uint32_t s_VAO;
	static uint32_t s_IBO;
	inline static std::vector<glm::mat4> s_quadInstanceTransforms;
	inline static std::vector<uint32_t> s_quadTextures;
	inline static std::vector<Quad*> s_allQuads;
};
