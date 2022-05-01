#include "SSBO.h"

#include "Logger.h"

SSBOManager::SSBOManager()
{
	initializeOpenGLFunctions();
}

void SSBOManager::CreateSSBO(const std::string& ssboName, uint32_t size, uint32_t binding)
{
	if (auto it = m_SSBOmap.find(ssboName); it != m_SSBOmap.end())
	{
		LOG_WARNING("SSBO " + ssboName + " already created.");
		return;
	}
	else
	{
		auto& ssbo = m_SSBOmap[ssboName];
		ssbo.size = size;
		glGenBuffers(1, &ssbo.ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo.ssbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, binding, ssbo.ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}

void SSBOManager::Resize(SSBO& ssbo, uint32_t newSize)
{
}

void SSBOManager::DeleteSSBO(const std::string ssboName)
{
	if (auto it = m_SSBOmap.find(ssboName); it != m_SSBOmap.end())
	{
		glDeleteBuffers(1, &it->second.ssbo);
		m_SSBOmap.erase(it);
	}
	else
	{
		LOG_WARNING("Tried to delete none existing ssbo: " + ssboName);
	}
}
