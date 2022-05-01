#include "TextureManager.h"
#include "renderwindow.h"

uint32_t TextureManager::GetTexture(const std::string& texture, bool flip)
{
	return RenderWindow::Get()->LoadTexture(texture, flip);
}
