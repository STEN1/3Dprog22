#pragma once
#include "shader.h"

class ShaderManager
{
public:
	static Shader& GetShaderRef(const std::string& shader);
};
