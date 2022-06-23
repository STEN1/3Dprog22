#include "Sun.h"

#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Core/Utils.h"
#include "Scene/Scene.h"

static constexpr float vData[] = {
		-0.5f, -0.5f, 0.f,  0.f, 1.f,
		0.5f, -0.5f, 0.f,   1.f, 1.f,
		0.5f, 0.5f, 0.f,    1.f, 0.f,
		-0.5f, 0.5f, 0.f,   0.f, 0.f
};
static constexpr uint32_t iData[] = {
	0, 2, 3,
	1, 2, 0
};

Sun::Sun(Scene& scene)
	: GameObject(scene, glm::mat4(1.f))
	, m_Shader{ ShaderManager::GetShaderRef("particle") }
	, m_Texture{ TextureManager::GetTexture("Sun.png", false) }
{
	m_vo = std::make_unique<VisualObject>(m_Shader, *this);
	m_vo->SetTransparent(true);
	initializeOpenGLFunctions();
	m_SSBOManager = RenderWindow::Get()->m_ssboManager;

	if (!s_Initialized)
	{
		m_SSBOManager->CreateSSBO(
			"Particles",
			sizeof(glm::vec4) * 2 + sizeof(ParticleData) * 2,
			3);

		glGenVertexArrays(1, &s_VAO);
		glBindVertexArray(s_VAO);
		uint32_t vbo;
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vData), vData, GL_STATIC_DRAW);
		uint32_t ebo;
		glGenBuffers(1, &ebo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iData), iData, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindVertexArray(0);

		s_Initialized = true;
	}
	ParticleData sunParticle;
	sunParticle.scale = glm::vec4(5.f);
	m_SunParticleData.push_back(sunParticle);
}

void Sun::Draw()
{
	auto dir = m_scene.GetSunDirection();
	auto pos = RenderWindow::Get()->GetCamera()->GetPosition() + (-dir) * m_DistanceToSun;
	//pos = glm::vec4(0.f, 2.f, 0.f, 0.f);
	auto color = m_scene.GetSunColor();
	//static float t{};
	//t += 0.01f;
	//auto color = Utils::ColorFromScalar(t);
	//color = glm::normalize(color);
	//m_scene.SetSunColor(color);
	for (auto& particle : m_SunParticleData)
	{
		particle.color = glm::vec4(color, 1.f);
		particle.worldPos = glm::vec4(pos, 1.f);
		//particle.scale = glm::vec4(5.f + sinf(t));
	}
	std::vector<glm::vec4> cameraData = {
		glm::vec4(RenderWindow::Get()->GetCamera()->GetCameraRight(), 1.f),
		glm::vec4(RenderWindow::Get()->GetCamera()->GetCameraUp(), 1.f)
	};
	m_SSBOManager->UpdateData("Particles", cameraData, 0);
	m_SSBOManager->UpdateData("Particles", m_SunParticleData, cameraData.size() * sizeof(glm::vec4));
	m_Shader.Use();
	glBindVertexArray(s_VAO);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_SunParticleData.size());
}
