#include "Graph.h"
#include "Core/ShaderManager.h"

Graph::Graph(GameObject& parent, float func(float z), const glm::vec4& color, const glm::mat4& transform)
    : VisualObject(ShaderManager::GetShaderRef("plainshader"), parent, transform)
    , m_color{ color }
{
    m_name = "Graph";
    std::vector<Graph::Vertex> verts;
    float zmin = -40;
    float zmax = 40;
    float stepsize = 0.5f;
    for (float z = zmin; z < zmax; z += stepsize)
    {
        float x = func(z);
        float y = 0.f;
        verts.emplace_back(Graph::Vertex{ x, y, z });
    }
    m_vertexCount = verts.size();
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Graph::Vertex)* verts.size(), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Graph::Vertex), (void*)0);
    glEnableVertexAttribArray(0);
}

Graph::~Graph()
{
    glDeleteVertexArrays(1, &m_VAO);
}


void Graph::Draw()
{
    m_shader.Use();
    m_shader.SetVec4f(m_color, "uColor");
    m_shader.SetMat4f(m_globalTransform, "model");
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINE_STRIP, 0, m_vertexCount);
    glBindVertexArray(0);
}

void Graph::SetColor(const glm::vec4& color)
{
    m_color = color;
}