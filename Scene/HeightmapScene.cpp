#include "HeightmapScene.h"
#include "GameObject/Player.h"
#include "GameObject/Terrain.h"
#include "Core/ShaderManager.h"
#include "VisualObject/StaticMesh.h"
#include "Core/TextureManager.h"
#include "GameObject/ItemPickup.h"
#include "Core/PhysicsShapes.h"
#include "Core/Utils.h"
#include "VisualObject/Heightmap.h"

HeightmapScene::HeightmapScene()
{
	m_heightmap = std::make_unique<Terrain>(*this, "loraheightmap.png", "lorasatelitt.png");
    //m_heightmap = std::make_unique<Terrain>(*this, "brsundlargeheight.png", "brsundlargetexture.png", 0.3f);
    //m_heightmap = std::make_unique<Terrain>(*this, "fasterthanlight.png", "fasterthanlight.png", 0.3f);
    float HeightmapScale = 2.f;
    auto transform = glm::scale(glm::vec3(HeightmapScale));
    m_heightmap->m_vo->SetLocalTransform(transform);
    static_cast<Heightmap*>(m_heightmap->m_vo.get())->TextureScale = HeightmapScale;
	auto player = m_gameObjects[0];

    int mapSize = 1080 * (int)HeightmapScale;

    for (uint32_t i = 0; i < 5000 * HeightmapScale;)
    {
        float x = rand() % mapSize;
        float z = rand() % mapSize;
        float y = m_heightmap->GetHeightFromPos({ x, 0.f, z });
        if (y < 16.f * HeightmapScale || y > 60.f * HeightmapScale)
            continue;
        i++;
        glm::mat4 transform = glm::translate(glm::mat4(1.f), { x, y, z });
        transform = glm::rotate(transform, glm::radians<float>((float)(rand() % 180)), { 0.f, 1.f, 0.f });
        auto go = m_gameObjects.emplace_back(new GameObject(*this, transform));
        if (go)
            go->SetVisualObject(std::make_unique<StaticMesh>(*go, "../3Dprog22/assets/HappyTree.obj", TextureManager::GetTexture("HappyTree.png")));
    }

    PointLight wispLight;
    wispLight.position = { 0.f, 1.f, 0.f, 0.f };
    wispLight.diffuse = { 0.4f,0.4f,0.4f, 0.f };
    wispLight.ambient = wispLight.diffuse * 0.1f;
    wispLight.constant = 1.f;
    wispLight.linear = 0.022f;
    wispLight.quadratic = 0.0019f;
    wispLight.specular = glm::vec4(1.f);

    auto extraItems = 50;

    for (auto i = 0; i < extraItems; i++)
    {
        float z = rand() % mapSize;
        float x = rand() % mapSize;
        float y;
        if (m_heightmap)
            y = m_heightmap->GetHeightFromPos({ x, 0.f, z });
        else
            y = TerrainFunc(x, z);
        glm::mat4 transform = glm::translate(glm::mat4(1.f), { x, y + 2.f, z });
        auto item = m_gameObjects.emplace_back(new ItemPickup(*this, transform));
        static_cast<Sphere*>(item->m_physicsShape.get())->radius *= 4.f;
        //item->SetLightRange(500.f);
        //reinterpret_cast<ItemPickup*>(item)->m_bob = false;
        static_cast<ItemPickup*>(item)->LightScaleRange = 50.f;

        wispLight.diffuse = glm::vec4(Utils::ColorFromScalar(rand() / 1000.f), 0.f);
        wispLight.ambient = wispLight.diffuse * 0.1f;
        wispLight.specular = wispLight.diffuse;
        m_pointLights[item] = wispLight;
    }

	Init();
	player->SetPosition({ 500.f, 0.f, 500.f });

    auto& lightShader = ShaderManager::GetShaderRef("light");
    auto& landscapeShader = ShaderManager::GetShaderRef("landscape");
    auto& instancedlightShader = ShaderManager::GetShaderRef("instancedlight");
    glm::vec3 skyColor(0.3f);
    glm::vec3 skyDirection(1.f, -1.f, 0.f);
    lightShader.Use();
    lightShader.SetVec3f(skyColor, "skyColor");
    lightShader.SetVec3f(skyDirection, "sunDirection");
    landscapeShader.Use();
    landscapeShader.SetVec3f(skyColor, "skyColor");
    landscapeShader.SetVec3f(skyDirection, "sunDirection");
    instancedlightShader.Use();
    instancedlightShader.SetVec3f(skyColor, "skyColor");
    instancedlightShader.SetVec3f(skyDirection, "sunDirection");


}
