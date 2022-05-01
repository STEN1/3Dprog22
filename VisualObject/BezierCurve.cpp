#include "BezierCurve.h"
#include "Core/ShaderManager.h"
#include "renderwindow.h"

BezierCurve::BezierCurve(GameObject& parent, const glm::vec4& controlColor, const glm::vec4& curveColor, const glm::mat4& transform)
    : VisualObject(ShaderManager::GetShaderRef("plainshader"), parent, transform)
    , m_curveColor{ curveColor }
    , m_controlColor{ controlColor }
    , m_controlPoints{
        {0.f, 0.f, 5.f},
        {2.5f, 5.f, 0.f},
        {9.5f, 5.f, 0.f},
        {16.f, 0.f, -5.f}
}
{
    m_name = "BezierCurve";
    SetT(0.5f);

    std::vector<glm::vec3> vData;
    for (float t = 0.f; t < 1.025f; t += 0.05f)
    {
        vData.push_back(CalcBezier(t));
    }
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vData.size(), vData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    m_vertexCount = vData.size();
}

void BezierCurve::Draw()
{
    m_shader.Use();
    m_shader.SetVec4f(m_curveColor, "uColor");
    m_shader.SetMat4f(m_globalTransform, "model");
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_LINE_STRIP, 0, m_vertexCount);
    glBindVertexArray(0);
    RenderWindow::Get()->DrawAABB(
        glm::vec3{ m_globalTransform[3].x, m_globalTransform[3].y, m_globalTransform[3].z } + m_curvePoint,
        glm::vec3(0.2f));
    for (auto& controlPoint : m_controlPoints)
    {
        RenderWindow::Get()->DrawAABB(
            glm::vec3{ m_globalTransform[3].x, m_globalTransform[3].y, m_globalTransform[3].z } + controlPoint,
            glm::vec3(0.1),
            m_controlColor
        );
    }
    for (auto i = 0; i < m_controlPoints.size() - 1; i++)
    {
        auto start = glm::vec3{ m_globalTransform[3].x, m_globalTransform[3].y, m_globalTransform[3].z } + m_controlPoints[i];
        auto end = glm::vec3{ m_globalTransform[3].x, m_globalTransform[3].y, m_globalTransform[3].z } + m_controlPoints[i + 1];
        RenderWindow::Get()->DrawLine(start, end, m_controlColor);
    }
}

void BezierCurve::SetT(float t)
{
    T = std::clamp(t, 0.f, 1.f);
    m_curvePoint = CalcBezier(T);
}

glm::vec3 BezierCurve::CalcBezier(float t)
{
    glm::vec3 a[4];
    for (size_t i = 0; i < 4; i++)
        a[i] = m_controlPoints[i];
    for (int k = 3; k > 0; k--)
        for (int i = 0; i < k; i++)
            a[i] = a[i] * (1.f - t) + a[i + 1] * t;
    return a[0];
}