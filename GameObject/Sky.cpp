#include "Sky.h"

#include "Core/Globals.h"
#include "VisualObject/Skybox.h"

Sky::Sky(Scene& scene)
	:GameObject(scene)
{
	std::vector<std::string> skyTextures = {
		Globals::AssetPath + std::string("skybox/right.jpg"),
		Globals::AssetPath + std::string("skybox/left.jpg"),
		Globals::AssetPath + std::string("skybox/top.jpg"),
		Globals::AssetPath + std::string("skybox/bottom.jpg"),
		Globals::AssetPath + std::string("skybox/front.jpg"),
		Globals::AssetPath + std::string("skybox/back.jpg"),
	};
	SetVisualObject(std::make_unique<Skybox>(*this, skyTextures));
}
