#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include <QOpenGLFunctions_4_5_Core>

#include "Logger.h"

struct SSBO
{
	uint32_t ssbo;
	uint32_t size;
};

class SSBOManager : protected QOpenGLFunctions_4_5_Core
{
public:
	SSBOManager();
	void CreateSSBO(const std::string& ssboName, uint32_t size, uint32_t binding);
	template<typename T>
	void UpdateData(const std::string& ssboName, const std::vector<T>& data, uint32_t offset);
	void Resize(SSBO& ssbo, uint32_t newSize);
	void DeleteSSBO(const std::string ssboName);
private:
	std::unordered_map<std::string, SSBO> m_SSBOmap;
};

template<typename T>
inline void SSBOManager::UpdateData(const std::string& ssboName, const std::vector<T>& data, uint32_t offset)
{
	auto it = m_SSBOmap.find(ssboName);
	if (it == m_SSBOmap.end())
	{
		LOG_WARNING("SSBO " + ssboName + " not found.");
		return;
	}
	else
	{
		auto& [name, ssbo] = *it;
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo.ssbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, data.size() * sizeof(T), data.data());
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	}
}
