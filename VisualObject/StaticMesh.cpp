#include "StaticMesh.h"
#include "Core/Logger.h"
#include "Core/ShaderManager.h"
#include "Core/ObjLoader.h"

StaticMesh::StaticMesh(GameObject& parent, const std::string& mesh, uint32_t texture, const glm::mat4& transform)
	:VisualObject(ShaderManager::GetShaderRef("instancedlight"), parent, transform)
{
	m_name = "StaticMesh";
	auto it = s_InstanceDataMap.find(mesh);
	if (it == s_InstanceDataMap.end())
	{
		ObjLoader meshObj(mesh);
		auto [vData, iData] = meshObj.GetIndexedData();
		m_indices = iData.size();

		glGenVertexArrays(1, &m_VAO);
		glBindVertexArray(m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

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

		glBindVertexArray(0);
		s_InstanceDataMap[mesh].indexCount = m_indices;
		s_InstanceDataMap[mesh].texture = texture;
        s_InstanceDataMap[mesh].specularTexture = texture;
		s_InstanceDataMap[mesh].transforms.push_back(m_globalTransform);
		s_InstanceDataMap[mesh].IBO = m_IBO;
		s_InstanceDataMap[mesh].VAO = m_VAO;
	}
	else
	{
		s_InstanceDataMap[mesh].transforms.push_back(m_globalTransform);
	}
}

void StaticMesh::Draw()
{
	LOG_ERROR("Should not call draw on StaticMesh.");
}
