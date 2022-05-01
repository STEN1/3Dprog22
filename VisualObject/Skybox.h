#pragma once
#include "VisualObject.h"

class Skybox : public VisualObject
{
public:
	Skybox(GameObject& go, const std::vector<std::string>& cubemapTextures);
	~Skybox();
	virtual void Draw() override;
private:
	uint32_t LoadCubemap(const std::vector<std::string>& cubemapTextures);
	uint32_t m_Texture;
};