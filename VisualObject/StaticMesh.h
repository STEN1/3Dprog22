#pragma once
#include "VisualObject.h"
#include <string>

class StaticMesh : public VisualObject
{
public:
	StaticMesh(GameObject& parent, const std::string& mesh, uint32_t texture, const glm::mat4& transform = glm::mat4(1.f));
	virtual void Draw() override;
	struct InstanceData
	{
		uint32_t VAO;
		uint32_t IBO;
		uint32_t indexCount;
		uint32_t texture;
		uint32_t specularTexture;
		std::vector<glm::mat4> transforms;
	};
	inline static std::unordered_map<std::string, InstanceData> s_InstanceDataMap;
private:
	uint32_t m_IBO;
};