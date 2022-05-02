#include "CameraMesh.h"

#include "Core/Utils.h"
#include "VisualObject/Mesh.h"
#include "Core/Globals.h"
#include "Core/TextureManager.h"

CameraMesh::CameraMesh(Scene& scene)
	: GameObject(scene)
{
	m_vo = std::make_unique<Mesh>(*this,
		Globals::AssetPath + std::string("SecretObject.obj"),
		TextureManager::GetTexture("testtexture2-big.png"));
}

void CameraMesh::Draw()
{
	auto transform = glm::translate(m_scene.m_camera->GetPosition());
	transform *= Utils::Vec3ToRotationMatrix(m_scene.m_camera->GetCameraForward());
	m_vo->SetLocalTransform(transform);
	GameObject::Draw();
}
