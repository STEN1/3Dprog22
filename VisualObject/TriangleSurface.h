#pragma once
#include "VisualObject.h"

class TriangleSurface : public VisualObject
{
public:
	TriangleSurface() = delete;
	TriangleSurface(GameObject& parent, Shader& shader, uint32_t texture, const std::string& surfacePath, glm::mat4 transform = glm::mat4(1.f));
	~TriangleSurface();
	void Draw() override;
	static void MakeSurface(const std::string& destinationPath);
	struct Vertex
	{
		float x;
		float y;
		float z;
		//float r;
		//float g;
		//float b;
		//float u;
		//float v;
		float nX;
		float nY;
		float nZ;
	};
private:
	uint32_t m_texture;
};