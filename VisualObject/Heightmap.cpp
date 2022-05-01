#include "Heightmap.h"
#include "Core/ShaderManager.h"
#include "stb_image/stb_image.h"
#include "Core/Logger.h"
#include "renderwindow.h"
#include "Core/TextureManager.h"
#include "Core/Utils.h"
#include <future>
#include <chrono>

Heightmap::Heightmap(GameObject& parent, const std::string& heightmap, const std::string& texture, float heightScale)
	:VisualObject(ShaderManager::GetShaderRef("landscape"), parent)
    ,m_texture(TextureManager::GetTexture(texture))
{
    auto totalStart = std::chrono::high_resolution_clock::now();
    auto map = "../3Dprog22/assets/" + heightmap;
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    uint8_t* data = stbi_load(map.c_str(), &width, &height, &nrChannels, 1);
    m_width = width;
    m_height = height;
    m_vertexData.resize(m_width * m_height);
    m_indexData.resize((m_width - 1) * (m_height - 1) * 6);
    if (data)
    {
        size_t x{};
        auto threads = std::thread::hardware_concurrency();
        if (threads < 4)
            threads = 4;
        std::vector<std::future<void>> futures(threads);
        while (x < height)
        {
            for (size_t i{}; i < futures.size(); i++)
            {
                futures[i] = std::async(
                    std::launch::async,
                    [this, height, width, data, heightScale](int x)
                    {
                        if (x < height)
                        {
                            for (size_t z = 0; z < width; z++)
                            {
                                float y = data[(x * width) + z];
                                m_vertexData[(x * width) + z] =
                                    Vertex{ glm::vec3((float)x, y * heightScale, (float)z), glm::vec3(0.f, 1.f, 0.f) };
                            }
                        }
                    },
                    x
                        );
                x++;
            }
            for (size_t i{}; i < futures.size(); i++)
            {
                futures[i].get();
            }
        }
        x = 0;
        while (x < height)
        {
            for (size_t i{}; i < futures.size(); i++)
            {
                futures[i] = std::async(
                    std::launch::async,
                        [this, height, width, data, heightScale](int x)
                        {
                            if (x < height - 1)
                            {
                                for (size_t z = 0; z < width - 1; z++)
                                {
                                    auto aIndex = (x * width) + z;
                                    auto bIndex = (x * width) + z + 1;
                                    auto cIndex = ((x + 1) * width) + z + 1;
                                    auto dIndex = ((x + 1) * width) + z;

                                    auto index = ((x * (width - 1)) + z) * 6;
                                    // first triangle
                                    m_indexData[index] = aIndex;
                                    m_indexData[index + 1] = bIndex;
                                    m_indexData[index + 2] = cIndex;

                                    // second triangle
                                    m_indexData[index + 3] = aIndex;
                                    m_indexData[index + 4] = cIndex;
                                    m_indexData[index + 5] = dIndex;
                                }
                            }
                            if (x > 0 && x < height - 1)
                            {
                                for (size_t z = 1; z < width - 1; z++)
                                {
                                    auto mIndex = (x * width) + z;
                                    auto aIndex = (x * width) + z + 1;
                                    auto bIndex = ((x + 1) * width) + z + 1;
                                    auto cIndex = ((x + 1) * width) + z;
                                    auto dIndex = (x * width) + z - 1;
                                    auto eIndex = ((x - 1) * width) + z - 1;
                                    auto fIndex = ((x - 1) * width) + z;

                                    auto a = m_vertexData[aIndex].pos - m_vertexData[mIndex].pos;
                                    auto b = m_vertexData[bIndex].pos - m_vertexData[mIndex].pos;
                                    auto c = m_vertexData[cIndex].pos - m_vertexData[mIndex].pos;
                                    auto d = m_vertexData[dIndex].pos - m_vertexData[mIndex].pos;
                                    auto e = m_vertexData[eIndex].pos - m_vertexData[mIndex].pos;
                                    auto f = m_vertexData[fIndex].pos - m_vertexData[mIndex].pos;

                                    auto axb = glm::cross(a, b);
                                    auto bxc = glm::cross(b, c);
                                    auto cxd = glm::cross(c, d);
                                    auto dxe = glm::cross(d, e);
                                    auto exf = glm::cross(e, f);
                                    auto fxa = glm::cross(f, a);

                                    auto normal = glm::normalize(axb + bxc + cxd + dxe + exf + fxa);

                                    m_vertexData[mIndex].normal = normal;
                                }
                            }
                        },
                        x
                        );
                x++;
            }
            for (size_t i{}; i < futures.size(); i++)
            {
                futures[i].get();
            }
        }
#if 0
        for (size_t x = 0; x < height; x++)
        {
            for (size_t z = 0; z < width; z++)
            {
                float y = data[(x * width) + z];
                m_vertexData.emplace_back(Vertex{ glm::vec3((float)x, y * heightScale, (float)z), glm::vec3(0.f, 1.f, 0.f) });
            }
        }
        for (size_t x = 0; x < height - 1; x++)
        {
            for (size_t z = 0; z < width - 1; z++)
            {
                auto aIndex = (x * width) + z;
                auto bIndex = (x * width) + z + 1;
                auto cIndex = ((x + 1) * width) + z + 1;
                auto dIndex = ((x + 1) * width) + z;

                // first triangle
                m_indexData.push_back(aIndex);
                m_indexData.push_back(bIndex);
                m_indexData.push_back(cIndex);

                // second triangle
                m_indexData.push_back(aIndex);
                m_indexData.push_back(cIndex);
                m_indexData.push_back(dIndex);
            }
        }
        LOG("indexData.size(): " + std::to_string(m_indexData.size()) + " Triangles: " + std::to_string(m_indexData.size() / 3));
        auto normalStart = std::chrono::high_resolution_clock::now();
        for (size_t x = 1; x < height - 1; x++)
        {

            for (size_t z = 1; z < width - 1; z++)
            {
                auto mIndex = (x * width) + z;
                auto aIndex = (x * width) + z + 1;
                auto bIndex = ((x + 1) * width) + z + 1;
                auto cIndex = ((x + 1) * width) + z;
                auto dIndex = (x * width) + z - 1;
                auto eIndex = ((x - 1) * width) + z - 1;
                auto fIndex = ((x - 1) * width) + z;

                auto a = m_vertexData[aIndex].pos - m_vertexData[mIndex].pos;
                auto b = m_vertexData[bIndex].pos - m_vertexData[mIndex].pos;
                auto c = m_vertexData[cIndex].pos - m_vertexData[mIndex].pos;
                auto d = m_vertexData[dIndex].pos - m_vertexData[mIndex].pos;
                auto e = m_vertexData[eIndex].pos - m_vertexData[mIndex].pos;
                auto f = m_vertexData[fIndex].pos - m_vertexData[mIndex].pos;

                auto axb = glm::cross(a, b);
                auto bxc = glm::cross(b, c);
                auto cxd = glm::cross(c, d);
                auto dxe = glm::cross(d, e);
                auto exf = glm::cross(e, f);
                auto fxa = glm::cross(f, a);

                auto normal = glm::normalize(axb + bxc + cxd + dxe + exf + fxa);

                m_vertexData[mIndex].normal = normal;
            }
        }
        auto normalEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> normalCalcTime = normalEnd - normalStart;
        LOG("HeightmapNormalCalcTime: " + std::to_string(normalCalcTime.count()));
#endif
    }
    else
    {
        LOG_WARNING("Heightmap failed to load texture");
    }
    stbi_image_free(data);
    m_indices = m_indexData.size();
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertexData.size() * sizeof(Heightmap::Vertex), m_vertexData.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexData.size() * sizeof(uint32_t), m_indexData.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    auto totalEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> calcTime = totalEnd - totalStart;
    LOG("TotalHeightmapLoadtime: " + std::to_string(calcTime.count()));
}

void Heightmap::Draw()
{
    m_shader.Use();
    m_shader.SetMat4f(m_globalTransform, "model");
    m_shader.SetInt(m_seaTranparency, "seaTranparency");
    m_shader.SetFloat(m_width * TextureScale, "Uscale");
    m_shader.SetFloat(m_height * TextureScale, "Vscale");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, m_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

bool Heightmap::IsInsideMap(const glm::vec3& pos)
{
    auto inverseTransform = glm::inverse(m_globalTransform);
    glm::vec3 correctedPos = inverseTransform * glm::vec4(pos, 1.f);
    if (correctedPos.x > 0.f && correctedPos.z > 0.f && correctedPos.x < m_height - 1 && correctedPos.z < m_width - 1)
        return true;
    return false;
}

float Heightmap::GetHeightFromPos(const glm::vec3& pos)
{
    if (m_vertexData.empty())
        return pos.y;

    auto inverseTransform = glm::inverse(m_globalTransform);
    glm::vec3 correctedPos = inverseTransform * glm::vec4(pos, 1.f);

    int x = correctedPos.x;
    int z = correctedPos.z;
    auto aIndex = (x * m_width) + z;
    auto bIndex = (x * m_width) + z + 1;
    auto cIndex = ((x + 1) * m_width) + z + 1;
    auto dIndex = ((x + 1) * m_width) + z;

    glm::vec3 transformedA = m_globalTransform * glm::vec4(m_vertexData[aIndex].pos, 1.f);
    glm::vec3 transformedB = m_globalTransform * glm::vec4(m_vertexData[bIndex].pos, 1.f);
    glm::vec3 transformedC = m_globalTransform * glm::vec4(m_vertexData[cIndex].pos, 1.f);
    glm::vec3 transformedD = m_globalTransform * glm::vec4(m_vertexData[dIndex].pos, 1.f);

    glm::vec2 pos2D = { pos.z, pos.x };
    glm::vec2 a = { transformedA.z, transformedA.x };
    glm::vec2 b = { transformedB.z, transformedB.x };
    glm::vec2 c = { transformedC.z, transformedC.x };
    glm::vec2 d = { transformedD.z, transformedD.x };
    auto barycentricCoords = Utils::BarycentricCoord(pos2D, a, b, c);
    if (Utils::BarycentricIsInside(barycentricCoords))
    {
        auto ah = transformedA.y * barycentricCoords.x;
        auto bh = transformedB.y * barycentricCoords.y;
        auto ch = transformedC.y * barycentricCoords.z;
        auto h = ah + bh + ch;
        return h;
    }
    else
    {
        barycentricCoords = Utils::BarycentricCoord(pos2D, a, c, d);
        auto ah = transformedA.y * barycentricCoords.x;
        auto ch = transformedC.y * barycentricCoords.y;
        auto dh = transformedD.y * barycentricCoords.z;
        auto h = ah + ch + dh;
        return h;
    }
}
