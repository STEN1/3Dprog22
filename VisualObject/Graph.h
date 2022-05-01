#pragma once
#include "VisualObject.h"

class Graph : public VisualObject
{
public:
	Graph(GameObject& parent, float func(float z), const glm::vec4& color = glm::vec4(1.f, 1.f, 1.f, 1.f), const glm::mat4& transform = glm::mat4(1.f));
	~Graph();
	void Draw() override;
	void SetColor(const glm::vec4& color);
private:
	glm::vec4 m_color;

	struct Vertex
	{
		float x;
		float y;
		float z;
	};
};
