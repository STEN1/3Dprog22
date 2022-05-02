#include "Mesh.h"
#include "Core/ObjLoader.h"
#include "Core/ShaderManager.h"
#include "renderwindow.h"

Mesh::Mesh(GameObject& parent, const std::string& mesh, uint32_t texture, const glm::mat4& transform)
    : VisualObject(ShaderManager::GetShaderRef("light"), parent, transform)
    , m_texture{ texture }
    , m_vertexLayout{ VertexLayout::Indexed }
    , m_drawMode{ GL_TRIANGLES }
{
    auto meshKey = mesh + std::to_string(texture);
    auto it = s_meshCache.find(meshKey);
    if (it != s_meshCache.end())
    {
        auto& cachedMesh = it->second;

        m_VAO = cachedMesh.VAO;
        m_vertexLayout = cachedMesh.Layout;
        m_vertexCount = cachedMesh.VertexCount;
        m_texture = cachedMesh.Texture;
        m_indices = cachedMesh.Indices;
        m_IBO = cachedMesh.IBO;
        s_meshCacheCounter[m_VAO] += 1;
    }
    else
    {
        ObjLoader meshObj(mesh);
        auto [vData, iData] = meshObj.GetIndexedData();
        m_name = "IndexedMesh";
        m_indices = iData.size();
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, vData.size() * sizeof(float), vData.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, iData.size() * sizeof(uint32_t), iData.data(), GL_STATIC_DRAW);
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        // UV
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        // Normal
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
        glEnableVertexAttribArray(2);

        // instanced transforms
        glGenBuffers(1, &m_IBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_IBO);
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)0);

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(1 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(2 * sizeof(glm::vec4)));

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(glm::vec4), (void*)(3 * sizeof(glm::vec4)));

        glVertexAttribDivisor(3, 1);
        glVertexAttribDivisor(4, 1);
        glVertexAttribDivisor(5, 1);
        glVertexAttribDivisor(6, 1);

        CachedMesh cachedMesh;
        cachedMesh.VAO = m_VAO;
        cachedMesh.IBO = m_IBO;
        cachedMesh.Layout = VertexLayout::Indexed;
        cachedMesh.Texture = texture;
        cachedMesh.VertexCount = 0;
        cachedMesh.Indices = m_indices;

        s_meshCache.insert(std::pair<std::string, Mesh::CachedMesh>(meshKey, cachedMesh));
        s_meshCacheCounter[m_VAO] += 1;
    }
}

Mesh::~Mesh()
{
    s_meshCacheCounter[m_VAO] -= 1;
    if (s_meshCacheCounter[m_VAO] == 0)
    {
        for (auto& [key, val] : s_meshCache)
        {
            if (val.VAO == m_VAO)
            {
                s_meshCache.erase(key);
                glDeleteVertexArrays(1, &m_VAO);
                break;
            }
        }
    }
}

void Mesh::Draw()
{
    switch (m_vertexLayout)
    {
    case VertexLayout::Indexed:
        DrawIndexed();
        break;
    case VertexLayout::Arrays:
        DrawArrays();
        break;
    default:
        break;
    }
}

void Mesh::SetTexture(uint32_t texture)
{
    m_texture = texture;
}

void Mesh::DrawIndexed()
{
    /*m_shader.Use();
    m_shader.SetMat4f(m_globalTransform, "model");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray(m_VAO);
    glDrawElements(m_drawMode, m_indices, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);*/
    RenderWindow::InstancedSubmitData submitData;
    submitData.elementCount = m_indices;
    submitData.IBO = m_IBO;
    RenderWindow::Get()->InstancedSubmit(m_VAO, m_texture, submitData, m_globalTransform);
}

void Mesh::DrawArrays()
{
    m_shader.Use();
    m_shader.SetMat4f(m_globalTransform, "model");
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray(m_VAO);
    glDrawArrays(m_drawMode, 0, m_vertexCount);
    glBindVertexArray(0);
}