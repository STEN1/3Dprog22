#include "HouseScene.h"
#include "renderwindow.h"
#include "Core/Logger.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Core/ObjLoader.h"
#include "GameObject/GameObject.h"
#include "Core/Octree.h"
#include "Core/PointLight.h"
#include "WoodsScene.h"
#include "GameObject/Player.h"
#include "GameObject/Terrain.h"
#include "GameObject/SceneWarp.h"
#include "VisualObject/Mesh.h"
#include "VisualObject/TriangleSurface.h"
#include "VisualObject/StaticMesh.h"

HouseScene::HouseScene()
{
    TriangleSurface::MakeSurface("../3Dprog22/assets/triangle_surface.txt");
    m_heightmap = std::make_unique<Terrain>(*this);

    glm::mat4 transform = glm::translate(glm::mat4(1.f), { 0.f, -4.f, 0.f });
    transform = glm::rotate(transform, -90.f, { 0.f, 1.f, 0.f });
    transform = glm::scale(transform, glm::vec3(2.f));
    PointLight wispLight;
    wispLight.position = { 0.f, 1.f, 0.f , 0.f };
    wispLight.diffuse = { 0.4f,0.4f,0.4f, 0.f };
    wispLight.ambient = wispLight.diffuse * 0.1f;
    wispLight.constant = 1.f;
    wispLight.linear = 0.022f;
    wispLight.quadratic = 0.0019f;
    wispLight.specular = glm::vec4(1.f);
    // load house
    {
        auto go = m_gameObjects.emplace_back(new GameObject(
            *this,
            transform));
        go->SetVisualObject(std::make_unique<StaticMesh>(*go, "../3Dprog22/assets/treehouse.obj",
            TextureManager::GetTexture("treehouse_color.png")));
    }
    // load house interior
    {
        auto go = m_gameObjects.emplace_back(new GameObject(
            *this,
            transform));
        go->SetVisualObject(std::make_unique<StaticMesh>(*go, "../3Dprog22/assets/table.obj",
            TextureManager::GetTexture("tabletexture.png")));
    }
    {
        auto go = m_gameObjects.emplace_back(new GameObject(
            *this,
            transform));
        go->SetVisualObject(std::make_unique<StaticMesh>(*go, "../3Dprog22/assets/PictureDag.obj",
            TextureManager::GetTexture("DagPicture.png")));
        auto light = m_gameObjects.emplace_back(new GameObject(*this));
        light->SetPosition({ 17.f, 5.f, -26.f });
        m_pointLights[light] = wispLight;
    }

    auto sceneWarp = m_gameObjects.emplace_back(new SceneWarp<WoodsScene>(*this));
    static_cast<AABB*>(sceneWarp->m_physicsShape.get())->extent *= 15.f;
    sceneWarp->SetPosition({ 17.f, 0.f, 10.f });

    LOG("House loaded");
}

void HouseScene::KeyPressed(int keyCode)
{
    Scene::KeyPressed(keyCode);
    if (keyCode == Qt::Key_L)
    {
        if (m_renderWindow)
        {
            m_renderWindow->LoadScene<WoodsScene>();
            return;
        }
    }
}

void HouseScene::Update(float deltaTime)
{
    Scene::Update(deltaTime);

    m_camera->SetPosition({ -15.f, 20.f, -25.f });
}