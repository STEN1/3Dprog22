#include "Quad.h"
#include "Core/ShaderManager.h"

uint32_t Quad::s_VAO = 0;
uint32_t Quad::s_IBO = 0;


Quad::Quad(GameObject& parent, uint32_t texture, glm::mat4 transform)
    : VisualObject(ShaderManager::GetShaderRef("instanced"), parent, transform)
    , m_texture{ texture }
{
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
    m_isTransparent = true;
    m_indices = 6;

    s_quadInstanceTransforms.push_back(transform);
    s_quadTextures.push_back(m_texture);
    if (s_VAO == 0 && s_IBO == 0)
    {
        glGenVertexArrays(1, &s_VAO);
        glBindVertexArray(s_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vData), vData, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(iData), iData, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glGenBuffers(1, &s_IBO);
        glBindBuffer(GL_ARRAY_BUFFER, s_IBO);
        glBufferData(GL_ARRAY_BUFFER, s_quadInstanceTransforms.size() * sizeof(glm::mat4), s_quadInstanceTransforms.data(), GL_STATIC_DRAW);

        uint32_t vec4Size = sizeof(glm::vec4);
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

        glVertexAttribDivisor(2, 1);
        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);

        glBindVertexArray(0);
    }
    s_allQuads.push_back(this);
}

Quad::~Quad()
{
    auto it = std::find(s_allQuads.begin(), s_allQuads.end(), this);
    if (it != s_allQuads.end())
        s_allQuads.erase(it);
    if (s_allQuads.empty())
    {
        glDeleteBuffers(1, &s_IBO);
        glDeleteVertexArrays(1, &s_VAO);
        s_VAO = 0;
        s_IBO = 0;
    }
}

void Quad::Draw()
{
    s_quadInstanceTransforms.emplace_back(m_globalTransform);
    s_quadTextures.push_back(m_texture);
}
