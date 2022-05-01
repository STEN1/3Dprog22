#include "ShaderManager.h"
#include "renderwindow.h"

Shader& ShaderManager::GetShaderRef(const std::string& shader)
{
	return RenderWindow::Get()->LoadShader(shader);
}
