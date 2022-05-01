#include "TriangleSurface.h"
#include "Core/Logger.h"
#include <fstream>
#include <iomanip>
#include "Scene/Scene.h"

#define BINARY_READWRITE 1

TriangleSurface::TriangleSurface(GameObject& parent, Shader& shader, uint32_t texture, const std::string& surfacePath, glm::mat4 transform)
    : VisualObject(shader, parent, transform)
    , m_texture{ texture }
{
    m_name = "TriangleSurface";
#if BINARY_READWRITE
    std::ifstream inFile(surfacePath, std::ios::binary | std::ios::ate);
    if (!inFile.is_open())
    {
        LOG_ERROR("Cant open surface: " + surfacePath);
        return;
}
    auto fileSize = inFile.tellg();
    std::vector<float> vertexData(fileSize / sizeof(float));
    inFile.seekg(0);
    inFile.read(reinterpret_cast<char*>(vertexData.data()), fileSize);
    m_vertexCount = vertexData.size() / (sizeof(Vertex) / sizeof(float));
    // get vertex data from file
#else
    std::ifstream inFile(surfacePath);
    if (!inFile.is_open())
    {
        LOG_ERROR("Cant open surface: " + surfacePath);
        return;
    }
    std::vector<float> vertexData;
    std::string s{};
    inFile >> s;
    m_vertexCount = std::stoi(s);
    while (inFile >> s)
    {
        vertexData.push_back(std::stof(s));
    }
#endif

    inFile.close();

    // layout: x y z - r g b - u v - nX nY nZ
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

TriangleSurface::~TriangleSurface()
{
    glDeleteVertexArrays(1, &m_VAO);
}

void TriangleSurface::Draw()
{
    m_shader.Use();
    m_shader.SetMat4f(m_globalTransform, "model");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glBindVertexArray(0);
}

void TriangleSurface::MakeSurface(const std::string& destinationPath)
{
    float size = 5000.f;
    float step = 8.f;
    std::vector<Vertex> positions;
    positions.reserve((size * size) / step);
    for (float x = -(size / 2.f); x < (size / 2.f); x += step)
    {
        for (float z = -(size / 2.f); z < (size / 2.f); z += step)
        {
            float y_xz = Scene::TerrainFunc(x, z);
            float y_xzStep = Scene::TerrainFunc(x, z + step);
            float y_xStepz = Scene::TerrainFunc(x + step, z);
            float y_xStepzStep = Scene::TerrainFunc(x + step, z + step);

            glm::vec3 a = { x, y_xz, z };
            glm::vec3 b = { x, y_xzStep, z + step };
            glm::vec3 c = { x + step, y_xStepzStep, z + step };
            glm::vec3 dir = glm::cross(b - a, c - a);
            glm::vec3 normal = glm::normalize(dir);

            positions.emplace_back(TriangleSurface::Vertex{ x, y_xz, z, normal.x, normal.y, normal.z });
            positions.emplace_back(TriangleSurface::Vertex{ x, y_xzStep, z + step, normal.x, normal.y, normal.z });
            positions.emplace_back(TriangleSurface::Vertex{ x + step, y_xStepzStep, z + step, normal.x, normal.y, normal.z });

            a = { x + step, y_xStepzStep, z + step };
            b = { x + step, y_xStepz, z };
            c = { x, y_xz, z };
            dir = glm::cross(b - a, c - a);
            normal = glm::normalize(dir);

            positions.emplace_back(TriangleSurface::Vertex{ x + step, y_xStepzStep, z + step, normal.x, normal.y, normal.z });
            positions.emplace_back(TriangleSurface::Vertex{ x + step, y_xStepz, z, normal.x, normal.y, normal.z });
            positions.emplace_back(TriangleSurface::Vertex{ x, y_xz, z, normal.x, normal.y, normal.z });
        }
    }

    std::ofstream of(destinationPath, std::ios::binary);
    if (!of.is_open())
    {
        LOG_ERROR("Cant open file: " + destinationPath);
        return;
    }
#if BINARY_READWRITE
    of.write(reinterpret_cast<char*>(positions.data()), sizeof(Vertex) * positions.size());
#else
    of << positions.size() << '\n';
    for (auto& v : positions)
    {
        of << v.x << ' ' << v.y << ' ' << v.z  << ' ' << v.nX << ' ' << v.nY << ' ' << v.nZ << '\n';
    }
#endif
    of.close();
}