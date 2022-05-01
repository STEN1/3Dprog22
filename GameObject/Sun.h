#pragma once

#include "GameObject.h"
#include <QOpenGLFunctions_4_5_Core>


class Sun : public GameObject, protected QOpenGLFunctions_4_5_Core
{
	struct ParticleData
	{
		glm::vec4 worldPos;
		glm::vec4 color;
		glm::vec4 scale;
	};
public:
	Sun(Scene& scene);
	virtual void Draw() override;
private:
	std::shared_ptr<SSBOManager> m_SSBOManager;
	Shader& m_Shader;
	uint32_t m_Texture;
	inline static bool s_Initialized{ false };
	inline static uint32_t s_VAO{};
	std::vector<ParticleData> m_SunParticleData;
	float m_DistanceToSun{ 10.f };
};