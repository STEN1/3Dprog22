#pragma once
#include "VisualObject.h"

class Kube : public VisualObject
{
public:
	Kube() = delete;
	Kube(GameObject& parent, Shader& shader, uint32_t texture, glm::mat4 transform = glm::mat4(1.f));
	~Kube();
	void Draw() override;
private:
	uint32_t m_texture;
};