#pragma once

#include "GameObject.h"
#include <QOpenGLFunctions_4_5_Core>
class TextBillboard : public GameObject, protected QOpenGLFunctions_4_5_Core
{
	struct BillboardData
	{
		glm::vec4 uvPosAndScale;
		glm::vec4 worldPos;
	};
public:
	TextBillboard(Scene& scene, const std::string& text, const glm::mat4& transform);
	virtual void Draw() override;
	void UpdateText(const std::string& text);
private:
	glm::vec2 IndexToUV(uint32_t index);
	std::shared_ptr<SSBOManager> m_SSBOManager;
	inline static bool s_Initialized{ false };
	inline static uint32_t s_VAO;
	std::vector<BillboardData> m_BillboardData;
	Shader& m_Shader;
	uint32_t m_Texture;
	uint32_t m_AtlasSize;
	uint32_t m_TileSize;
	std::string m_Text;
};