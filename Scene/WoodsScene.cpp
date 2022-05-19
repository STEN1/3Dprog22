#include "WoodsScene.h"
#include "renderwindow.h"
#include "Core/Logger.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Core/ObjLoader.h"
#include "GameObject/GameObject.h"
#include "Core/Octree.h"
#include "Core/PointLight.h"
#include "HouseScene.h"
#include "GameObject/Player.h"
#include "GameObject/Terrain.h"
#include "GameObject/GraphNPC.h"
#include "GameObject/ItemPickup.h"
#include "GameObject/AnimatedBezier.h"
#include "GameObject/SceneWarp.h"
#include "GameObject/Door.h"
#include "VisualObject/Mesh.h"
#include "VisualObject/Quad.h"
#include "VisualObject/Graph.h"
#include "VisualObject/TriangleSurface.h"
#include "Core/Utils.h"
#include "VisualObject/StaticMesh.h"

static float points1(float z)
{
    return 0.028f * powf(z, 2.f) + 0.525f * z - 8.577f;
}

static float points2(float z)
{
    return -0.00532f * z * z * z - 0.03333f * z * z + 1.7581 * z + 10.90278f;
}

WoodsScene::WoodsScene()
{
    auto& lightShader = ShaderManager::GetShaderRef("light");
    auto& landscapeShader = ShaderManager::GetShaderRef("landscape");
    auto& instancedlightShader = ShaderManager::GetShaderRef("instancedlight");
    lightShader.Use();
    lightShader.SetVec3f({ 0.2f, 0.2f, 0.2f }, "skyColor");
    lightShader.SetVec3f({ -1.f, -1.f, -1.f }, "sunDirection");
    landscapeShader.Use();
    landscapeShader.SetVec3f({ 0.2f, 0.2f, 0.2f }, "skyColor");
    landscapeShader.SetVec3f({ -1.f, -1.f, -1.f }, "sunDirection");
    instancedlightShader.Use();
    instancedlightShader.SetVec3f({ 0.2f, 0.2f, 0.2f }, "skyColor");
    instancedlightShader.SetVec3f({ -1.f, -1.f, -1.f }, "sunDirection");

    TriangleSurface::MakeSurface("../3Dprog22/assets/triangle_surface.txt");
    m_heightmap = std::make_unique<Terrain>(*this);

    {
        glm::vec3 housePos{ -23.f, TerrainFunc(-23.f, -33.f) -4.f, -33.f};
        glm::mat4 transform = glm::translate(glm::mat4(1.f), housePos);
        transform = glm::rotate(transform, glm::radians(-80.f), { 0.f, 1.f, 0.f });
        auto go = m_gameObjects.emplace_back(new GameObject(*this, transform));
        go->SetVisualObject(std::make_unique<StaticMesh>(*go, "../3Dprog22/assets/treehouse.obj", TextureManager::GetTexture("treehouse_color.png")));

        auto doorObject = m_gameObjects.emplace_back(new Door(*this, transform));

        auto sceneWarp = m_gameObjects.emplace_back(new SceneWarp<HouseScene>(*this));
        static_cast<AABB*>(sceneWarp->m_physicsShape.get())->extent *= 10.f;
        sceneWarp->SetPosition({ housePos.x + 3.f, housePos.y, housePos.z - 7.f });
    }

    auto graph = m_gameObjects.emplace_back(new GameObject(
        *this,
        glm::mat4(1.f)));
    graph->m_vo = std::make_unique<Graph>(*graph, points1);

    graph = m_gameObjects.emplace_back(new GameObject(
        *this,
        glm::mat4(1.f)));
    graph->m_vo = std::make_unique<Graph>(*graph, points2);

    PointLight wispLight;
    wispLight.position = { 0.f, 1.f, 0.f , 0.f};
    wispLight.diffuse = { 0.4f,0.4f,0.4f, 0.f};
    wispLight.ambient = wispLight.diffuse * 0.1f;
    wispLight.constant = 1.f;
    wispLight.linear = 0.022f;
    wispLight.quadratic = 0.0019f;
    wispLight.specular = glm::vec4(1.f);

    auto graphNpc = m_gameObjects.emplace_back(new GraphNPC(*this, points1, TerrainFunc));
    m_pointLights[graphNpc] = wispLight;
    graphNpc = m_gameObjects.emplace_back(new GraphNPC(*this, points2, TerrainFunc));
    m_pointLights[graphNpc] = wispLight;

    m_gameObjects.emplace_back(new AnimatedBezier(*this))->SetPosition({ 20.f, 0.f, -20.f });

    std::vector<std::pair<float, float>> pickupPositions = {
        { -35.f, 15.f },
        { -25.f, 5.f },
        { -30.f, -15.f },
        { -5.f, -15.f },
        { 10.f, -10.f },
        { 15.f, 5.f },
        { 10.f, 15.f },
        { 25.f, 20.f }
    };

    std::vector<std::pair<float, float>> pointsGraph2 = {
        { -20.f, 5.f },
        { -15.f, -5.f },
        { 17.f, 5.f },
        { 12.f, 18.f }
    };




    pointsGraph2.clear();
    pickupPositions.clear();
    auto extraItems = 4096 - pickupPositions.size() - pointsGraph2.size() - 2;
    int itemSpread = 10000;
    for (auto i = 0; i < extraItems; i++)
    {
        float z = (float)(rand() % (itemSpread / 2)) - (float)(itemSpread / 4);
        float x = (float)(rand() % (itemSpread / 2)) - (float)(itemSpread / 4);
        float y;
        if (m_heightmap)
            y = m_heightmap->GetHeightFromPos({ x, 0.f, z });
        else
            y = TerrainFunc(x, z);
        glm::mat4 transform = glm::translate(glm::mat4(1.f), { x, y + 2.f, z });
        auto item = m_gameObjects.emplace_back(new ItemPickup(*this, transform));
        static_cast<Sphere*>(item->m_physicsShape.get())->radius *= 4.f;
        //reinterpret_cast<ItemPickup*>(item)->m_bob = false;

        wispLight.diffuse = glm::vec4(Utils::ColorFromScalar(rand() / 1000.f), 0.f);
        wispLight.ambient = wispLight.diffuse * 0.1f;
        wispLight.specular = wispLight.diffuse;
        m_pointLights[item] = wispLight;
    }

    for (auto& [z, x] : pickupPositions)
    {
        auto y = TerrainFunc(x, z);
        glm::mat4 transform = glm::translate(glm::mat4(1.f), { x, y + 2.f, z });
        auto item = m_gameObjects.emplace_back(new ItemPickup(*this, transform));
        static_cast<Sphere*>(item->m_physicsShape.get())->radius *= 4.f;
        wispLight.diffuse = glm::vec4(Utils::ColorFromScalar(rand() / 1000.f), 0.f);
        wispLight.ambient = wispLight.diffuse * 0.1f;
        wispLight.specular = wispLight.diffuse;
        m_pointLights[item] = wispLight;
    }

    for (auto& [z, x] : pointsGraph2)
    {
        auto y = TerrainFunc(x, z);
        glm::mat4 transform = glm::translate(glm::mat4(1.f), { x, y + 2.f, z });
        auto item = m_gameObjects.emplace_back(new ItemPickup(*this, transform));
        static_cast<Sphere*>(item->m_physicsShape.get())->radius *= 4.f;
        wispLight.diffuse = glm::vec4(Utils::ColorFromScalar(rand() / 1000.f), 0.f);
        wispLight.ambient = wispLight.diffuse * 0.1f;
        wispLight.specular = wispLight.diffuse;
        m_pointLights[item] = wispLight;
    }


    std::vector<std::pair<float, float>> treePositions = { {0.f, 5.f} };
    auto numberOfRandomTrees = 5000;
    auto forestTreeline = 2500.f;
    auto treeLineThickness = 2460;
    srand(time(nullptr));
    for (auto i = 0; i < numberOfRandomTrees / 4; i++)
    {
        float zPos = ((rand() % treeLineThickness) - forestTreeline);
        float xPos = ((rand() % (int)forestTreeline * 2) - forestTreeline);
        treePositions.emplace_back(zPos, xPos);

        zPos = ((rand() % (int)forestTreeline * 2) - forestTreeline);
        xPos = ((rand() % treeLineThickness) - forestTreeline);
        treePositions.emplace_back(zPos, xPos);

        zPos = ((rand() % treeLineThickness) + forestTreeline - treeLineThickness);
        xPos = ((rand() % (int)forestTreeline * 2) - forestTreeline);
        treePositions.emplace_back(zPos, xPos);

        zPos = ((rand() % (int)forestTreeline * 2) - forestTreeline);
        xPos = ((rand() % treeLineThickness) + forestTreeline - treeLineThickness);
        treePositions.emplace_back(zPos, xPos);
    }

    for (auto& [z, x] : treePositions)
    {
        float y;
        if (m_heightmap)
            y = m_heightmap->GetHeightFromPos({ x, 0.f, z });
        else
            y = TerrainFunc(x, z);
        float scale = ((rand() / (float)RAND_MAX) * 2.f) + 2.f;
        auto transform = glm::translate(glm::mat4(1.f), {x, y, z});
        transform = glm::scale(transform, glm::vec3(scale));
        transform = glm::rotate(transform, glm::radians<float>((float)(rand() % 180)), { 0.f, 1.f, 0.f });
        auto go = m_gameObjects.emplace_back(new GameObject(*this, transform));
        go->SetVisualObject(std::make_unique<StaticMesh>(*go, "../3Dprog22/assets/HappyTree.obj", TextureManager::GetTexture("HappyTree.png")));
    }
    
    LOG_WARNING("Woods loaded...");
    LOG_WARNING("Press 1 or 2 to make the NPC follow different graphs.");
    LOG_WARNING("Press Q to draw debug lines.");
    LOG_WARNING("Scrollwheel controlls zoom and rightclick hold to rotate camera.");
}

void WoodsScene::KeyPressed(int keyCode)
{
    Scene::KeyPressed(keyCode);
    if (keyCode == Qt::Key_1)
    {
        for (auto gameObject : m_gameObjects)
        {
            if (auto graphNPC = dynamic_cast<GraphNPC*>(gameObject))
            {
                graphNPC->m_graphFunc = points1;
            }
        }
    }
    if (keyCode == Qt::Key_2)
    {
        for (auto gameObject : m_gameObjects)
        {
            if (auto graphNPC = dynamic_cast<GraphNPC*>(gameObject))
            {
                graphNPC->m_graphFunc = points2;
            }
        }
    }
    if (keyCode == Qt::Key_L)
    {
        if (m_renderWindow)
        {
            m_renderWindow->LoadScene<HouseScene>();
            return;
        }
    }
}

void WoodsScene::Update(float deltaTime)
{
    Scene::Update(deltaTime);

    /*auto& lightShader = ShaderManager::GetShaderRef("light");
    auto& landscapeShader = ShaderManager::GetShaderRef("landscape");
    auto& instancedlightShader = ShaderManager::GetShaderRef("instancedlight");

    auto sunSpeed = CurrentTime();

    auto x = cos(sunSpeed);
    auto z = sin(sunSpeed);

    lightShader.Use();
    lightShader.SetVec3f({ x, -1.f, z }, "sunDirection");
    landscapeShader.Use();
    landscapeShader.SetVec3f({ x, -1.f, z }, "sunDirection");
    instancedlightShader.Use();
    instancedlightShader.SetVec3f({ x, -1.f, z }, "sunDirection");*/
}
