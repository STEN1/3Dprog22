#pragma once
#include <string>

class TextureManager
{
public:
	static uint32_t GetTexture(const std::string& texture, bool flip = true);
};
