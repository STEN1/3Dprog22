#include "SolidWall.h"

#include "VisualObject/Mesh.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Core/Globals.h"


SolidWall::SolidWall(Scene& scene, const glm::mat4 transform)
	: GameObject(scene, transform)
{
	objectType = ObjectType::Dynamic;
	m_vo = std::make_unique<Mesh>(*this, 
		Globals::AssetPath + std::string("halfextentcube.obj"),
		TextureManager::GetTexture("Rect.png"));
	m_name = "SolidWall";
	aabb = static_cast<AABB*>(m_physicsShape.get());
	aabb->extent.x = transform[0].x;
	aabb->extent.y = transform[1].y;
	aabb->extent.z = transform[2].z;
	aabb->extent *= 0.5f;
	aabb->pos = GetPosition();
	Solid = true;
}
