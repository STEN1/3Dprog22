#include "TextBillboard.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include <map>
#include "Core/Utils.h"

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

TextBillboard::TextBillboard(Scene& scene, const std::string& text, const glm::mat4& transform)
	: GameObject(scene, transform)
	, m_Shader{ ShaderManager::GetShaderRef("billboard") }
	, m_Texture{ TextureManager::GetTexture("FontAtlas.png") }
	, m_AtlasSize{ 256 }
	, m_TileSize{ 32 }
	, m_Text{ text }
{
	UI = true;
	m_vo = std::make_unique<VisualObject>(m_Shader, *this);
	m_vo->SetTransparent(true);
	initializeOpenGLFunctions();
	m_SSBOManager = RenderWindow::Get()->m_ssboManager;
	if (!s_Initialized)
	{
		m_SSBOManager->CreateSSBO(
			"TextBillboard",
			sizeof(BillboardData) * 512,
			2);

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
	UpdateText(m_Text);
}

void TextBillboard::Draw()
{
	m_Shader.Use();
	m_Shader.SetMat4f(m_transform, "model");
	UpdateText(m_Text);
	glBindVertexArray(s_VAO);
	glBindTexture(GL_TEXTURE_2D, m_Texture);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_BillboardData.size());
}

void TextBillboard::UpdateText(const std::string& text)
{
	m_Text = text;
	m_BillboardData.clear();
	for (uint32_t i = 0; i < text.size(); i++)
	{
		float kerning = 0.5f;
		//glm::vec4 pos = m_transform[3];
		glm::vec4 pos(0.f);
		pos.x += (float)i * kerning;
		glm::vec2 uvPos = IndexToUV(text[i] - ' ');
		glm::vec2 letterSize(m_TileSize / (float)m_AtlasSize);
		m_BillboardData.emplace_back(BillboardData{ glm::vec4(uvPos, letterSize), pos });
	}
	m_SSBOManager->UpdateData("TextBillboard", m_BillboardData, 0);
}

glm::vec2 TextBillboard::IndexToUV(uint32_t index)
{
	glm::vec2 uv;
	auto columns = m_AtlasSize / m_TileSize;
	auto col = index % columns;
	auto row = index / columns;
	uv.x = (col * m_TileSize) / (float)m_AtlasSize;
	uv.y = (row * m_TileSize) / (float)m_AtlasSize;
	return uv;
}
