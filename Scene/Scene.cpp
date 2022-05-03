#include "Scene.h"

#include "renderwindow.h"
#include "Core/Logger.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "Core/ObjLoader.h"
#include "GameObject/GameObject.h"
#include "Core/Octree.h"
#include "Core/PointLight.h"
#include "VisualObject/Quad.h"
#include "Core/Utils.h"

#include <future>
#include "GameObject/Terrain.h"
#include "VisualObject/Heightmap.h"
#include "GameObject/Player.h"
#include "VisualObject/StaticMesh.h"
#include "Core/SoundManager.h"
#include "GameObject/Sky.h"
#include "GameObject/Sun.h"
#include "GameObject/TextBillboard.h"
#include "GameObject/CameraMesh.h"


Scene::Scene()
{
    for (auto& [path, instanceData] : StaticMesh::s_InstanceDataMap)
    {
        auto rw = RenderWindow::Get();
        rw->glDeleteVertexArrays(1, &instanceData.VAO);
    }
    StaticMesh::s_InstanceDataMap.clear();
    AABB worldExtent{};
    worldExtent.extent = glm::vec3{ 5000.f, 5000.f, 5000.f };
    m_octree = std::make_unique<Octree>(worldExtent);
    m_skybox = std::make_unique<Sky>(*this);
    m_Sun = std::make_unique<Sun>(*this);
	m_renderWindow = RenderWindow::Get();
    m_camera = m_renderWindow->GetPlayCamera();
    m_ViewFrustum = std::make_unique<Frustum>(m_camera);
    m_CameraMesh = std::make_unique<CameraMesh>(*this);
    

    m_Player = static_cast<Player*>(m_gameObjects.emplace_back(new Player(*this, m_renderWindow->GetPlayCamera())));
    m_startSound = std::make_shared<SoundSource>(*m_Player, "BabyElephantWalk60.wav");
    m_startSound->Play();
    static_cast<AABB*>(m_Player->m_physicsShape.get())->extent *= 2.f;

    SetSunDirection({ -1.f, -1.f, -1.f });
    SetSunColor({ 0.9f, 0.9f, 0.9f });

    m_testBillboard = std::make_unique<TextBillboard>(*this, "TESTING!", glm::translate(glm::vec3(-6.f, 3.5f, 0.f)));
}

Scene::~Scene()
{
    for (auto go : m_gameObjects)
        delete go;
}

void Scene::Simulate(float deltaTime)
{
    static constexpr float gravity = 9.81f;
    for (auto go : m_simulatedGameObjects)
    {
        // handle heightmap
        auto pos = go->GetPosition();
        auto minHeight = m_heightmap->GetHeightFromPos(pos);
        auto vel = go->GetVelocity();
        minHeight += 1.f;
        vel.y -= gravity * deltaTime;
        pos += vel * deltaTime;
        if (pos.y < minHeight)
        {
            vel.y = -20.f;
            pos.y = minHeight;
            go->m_Grounded = true;
        }
        else
        {
            go->m_Grounded = false;
        }
        go->SetPosition(pos);
        go->SetVelocity(vel);
        // handle walls
        for (auto overlappingGo : go->m_overlappingGameObjects)
        {
            if (!overlappingGo->Solid)
                continue;
            // only support aabb aabb interactions for now.
            AABB* overlappingAABB = dynamic_cast<AABB*>(overlappingGo->m_physicsShape.get());
            AABB* goAABB = dynamic_cast<AABB*>(go->m_physicsShape.get());
            if (overlappingAABB == nullptr || goAABB == nullptr)
                continue;
            auto toOverlapping = overlappingAABB->pos - goAABB->pos;
            float xOverlap = overlappingAABB->extent.x + goAABB->extent.x - abs(toOverlapping.x);
            float zOverlap = overlappingAABB->extent.z + goAABB->extent.z - abs(toOverlapping.z);
            if (xOverlap < zOverlap)
            {
                if (goAABB->pos.x > overlappingAABB->pos.x)
                {
                    pos.x += xOverlap;
                }
                else // goAABB->pos.x <= overlappingAABB->pos.x
                {
                    pos.x -= xOverlap;
                }
            }
            else // xOverlap >= zOverlap
            {
                if (goAABB->pos.z > overlappingAABB->pos.z)
                {
                    pos.z += zOverlap;
                }
                else // goAABB->pos.z <= overlappingAABB->pos.z
                {
                    pos.z -= zOverlap;
                }
            }
        }
        go->SetPosition(pos);
    }
}

void Scene::Update(float deltaTime)
{
    RemoveDestroyedGameObjects();

    s_currentTime += deltaTime;

    //m_octree->ResolveOverlappingGameObjects();

    ResolveOverlapp(FindCollisions2());

    Simulate(deltaTime);

    for (uint32_t i = 0; i < m_gameObjects.size(); i++)
        m_gameObjects[i]->Update(deltaTime);
}

void Scene::Render()
{
    auto camera = RenderWindow::Get()->GetCamera();
    m_ViewFrustum->SetCamera(RenderWindow::Get()->GetCamera());
    m_ViewFrustum->UpdateFrustum();
    std::vector<PointLight> lights;
    static const float maxLightDistance2 = m_maxLightDistance * m_maxLightDistance;
    for (auto [go, light] : m_pointLights)
    {
        auto distanceToCamera2 = glm::length2(go->GetPosition() - camera->GetPosition());
        if (distanceToCamera2 > maxLightDistance2)
            continue;
        Sphere lightSphere;
        light.position = glm::vec4(go->GetPosition(), 0.f);
        lightSphere.pos = light.position;
        float lightRange = go->GetLightRange();
        light.lightRange = lightRange;
        lightSphere.radius = light.lightRange;
        if (!m_ViewFrustum->Intersect(&lightSphere))
            continue;
        light.constant = 1.f;
        light.linear = 4.5f / lightRange;
        light.quadratic = 75.f / (lightRange * lightRange);
        lights.push_back(light);
    }
    //LOG("Lightcount: " + std::to_string(lights.size()));
    m_renderWindow->UpdatePointLights(lights);

    RenderWindow::Get()->glDepthMask(GL_FALSE);
    if (m_skybox)
        m_skybox->Draw();

    if (m_Sun)
        m_Sun->Draw();
    RenderWindow::Get()->glDepthMask(GL_TRUE);

    for (auto go : m_opaqueGameObjects)
    {
        if (m_ViewFrustum->Intersect(go->m_physicsShape.get()))
            go->Draw();
    }

    if (m_CameraMesh)
        if (RenderWindow::Get()->GetEditorMode() == RenderWindow::EditorMode::Debug)
            m_CameraMesh->Draw();

    ShaderManager::GetShaderRef("instancedlight").Use();
    for (auto& [path, instanceData] : StaticMesh::s_InstanceDataMap)
    {
        auto rw = RenderWindow::Get();
        rw->glBindVertexArray(instanceData.VAO);
        rw->glBindBuffer(GL_ARRAY_BUFFER, instanceData.IBO);
        rw->glActiveTexture(GL_TEXTURE1);
        rw->glBindTexture(GL_TEXTURE_2D, instanceData.specularTexture);
        rw->glActiveTexture(GL_TEXTURE0);
        rw->glBindTexture(GL_TEXTURE_2D, instanceData.texture);
        rw->glDrawElementsInstanced(GL_TRIANGLES, instanceData.indexCount, GL_UNSIGNED_INT, 0, instanceData.transforms.size());
    }

    RenderWindow::Get()->InstancedPresent();

    if (m_heightmap)
        m_heightmap->Draw();


    RenderWindow::Get()->glDepthMask(GL_FALSE);

    Quad::s_quadInstanceTransforms.clear();
    Quad::s_quadTextures.clear();
    for (auto go : m_transparentGameObjects)
    {
        if (m_ViewFrustum->Intersect(go->m_physicsShape.get()))
            go->Draw();
    }
    RenderQuads();
    
    RenderWindow::Get()->glDisable(GL_DEPTH_TEST);

    if (m_testBillboard && m_Player)
    {
        auto score = m_Player->GetScore();
        m_testBillboard->UpdateText("SCORE: " + std::to_string(score));
        auto pos = m_Player->GetPosition();
        float distFromCamera = 10.f;
        auto scorePos = m_camera->GetCameraForward();
        scorePos *= distFromCamera;
        scorePos = scorePos + (m_camera->GetCameraRight() * 3.f) + (m_camera->GetCameraUp() * 3.f);
        m_testBillboard->m_transform = glm::translate(m_camera->GetPosition() + scorePos);
        auto rotation = Utils::Vec3ToRotationMatrix(-m_camera->GetCameraForward());
        m_testBillboard->m_transform = m_testBillboard->m_transform * rotation;
        m_testBillboard->Draw();
    }

    for (auto ui : m_UI)
        ui->Draw();

    RenderWindow::Get()->glEnable(GL_DEPTH_TEST);


    RenderWindow::Get()->glDepthMask(GL_TRUE);


    if (m_DrawDebugLines)
    {
        m_octree->DrawExtents();
        std::vector<GameObject*> objects_in_frustum;
        m_octree->Find(m_ViewFrustum.get(), objects_in_frustum);
        glm::vec4 debugColor = { 1.f, 0.f, 0.f, 1.f };
        for (auto go : objects_in_frustum)
        {
            switch (go->m_physicsShape->shape)
            {
            case PhysicsShape::Shape::AABB:
                RenderWindow::Get()->DrawAABB(static_cast<AABB*>(go->m_physicsShape.get())->pos, static_cast<AABB*>(go->m_physicsShape.get())->extent, debugColor);
                break;
            case PhysicsShape::Shape::Sphere:
                RenderWindow::Get()->DrawSphere(static_cast<Sphere*>(go->m_physicsShape.get())->pos, static_cast<Sphere*>(go->m_physicsShape.get())->radius, debugColor);
                break;
            default:
                break;
            }
        }

        glm::vec4 lightDebugColor = { 1.f, 1.f, 0.f, 1.f };
        for (auto& light : lights)
        {
            RenderWindow::Get()->DrawSphere(light.position, light.lightRange, lightDebugColor);
        }
    }
}

float Scene::CurrentTime()
{
    return s_currentTime;
}

float Scene::TerrainFunc(float x, float z)
{
    constexpr float pi = 3.14159265358979323846f;
    // return sin(pi * x) * sin(pi * z);
    return  1.5 * (cos(0.1 * z) + sin(0.1 * x)) - 2.0;
}

float Scene::TerrainFuncX(float x)
{
    return 0.1 * cos(0.1 * x);
}

float Scene::TerrainFuncZ(float z)
{
    return -0.2 * sin(0.2 * z);
}

void Scene::Init()
{
    for (auto& [path, instanceData] : StaticMesh::s_InstanceDataMap)
    {
        auto rw = RenderWindow::Get();
        rw->glBindVertexArray(instanceData.VAO);
        rw->glBindBuffer(GL_ARRAY_BUFFER, instanceData.IBO);
        rw->glBufferData(GL_ARRAY_BUFFER, instanceData.transforms.size() * sizeof(glm::mat4), instanceData.transforms.data(), GL_STATIC_DRAW);

    }
    for (auto go : m_gameObjects)
    {
        if (go->objectType == GameObject::ObjectType::Static)
        {
            m_staticGameObjects.push_back(go);
        }
        else if (go->objectType == GameObject::ObjectType::Dynamic)
        {
            m_dynamicGameObjects.push_back(go);
            m_octree->Insert(go);
        }
        else if (go->objectType == GameObject::ObjectType::Simulated)
        {
            m_dynamicGameObjects.push_back(go);
            m_simulatedGameObjects.push_back(go);
            m_octree->Insert(go);
        }
        if (!go->m_vo)
            continue;
        if (go->m_vo->IsTransparent() && go->objectType != GameObject::ObjectType::Static)
        {
            m_transparentGameObjects.push_back(go);
        }
        else if (!go->m_vo->IsTransparent() && go->objectType != GameObject::ObjectType::Static)
        {
            m_opaqueGameObjects.push_back(go);
        }
    }
    if (m_Player)
        m_Player->SetPosition(m_PlayerStartPos);
    if (m_CameraMesh)
        m_CameraMesh->Draw();
    Update(0.1f);
}

float Scene::GetHeightFromHeightmap(const glm::vec3& pos)
{
    return m_heightmap->GetHeightFromPos(pos);
}


std::vector<std::pair<GameObject*, GameObject*>> Scene::FindCollisions()
{
    if (m_octree->IsLeaf())
    {
        std::vector<std::pair<GameObject*, GameObject*>> outVec;
        m_octree->FindCollisions(outVec);
        return outVec;
    }
    else
    {
        // Matching number of childnodes.
        static constexpr uint32_t THREADS = 8;
        std::vector<std::vector<std::pair<GameObject*, GameObject*>>> outVectors(THREADS);
        std::vector<std::future<void>> futures(THREADS);

        for (uint32_t i = 0; i < THREADS; i++)
            futures[i] = std::async(std::launch::async, &Octree::FindCollisions, m_octree->m_childNodes[i], std::ref(outVectors[i]));

        for (auto& fut : futures)
            fut.get();

        std::vector<std::pair<GameObject*, GameObject*>> outVec;
        for (auto& vec : outVectors)
        {
            outVec.insert(outVec.end(), vec.begin(), vec.end());
        }
        return outVec;
    }
}

std::vector<std::pair<GameObject*, GameObject*>> Scene::FindCollisions2()
{
    std::vector<Octree*> activeLeafs;
    m_octree->FindActiveLeafs(activeLeafs);

    std::vector<std::vector<std::pair<GameObject*, GameObject*>>> outVectors(activeLeafs.size());
    std::vector<std::future<void>> futures(activeLeafs.size());

    for (uint32_t i = 0; i < activeLeafs.size(); i++)
    {
        futures[i] = std::async(std::launch::async, &Octree::FindCollisions, activeLeafs[i], std::ref(outVectors[i]));
    }

    for (auto& fut : futures)
        fut.get();

    std::vector<std::pair<GameObject*, GameObject*>> outVec;
    for (auto& vec : outVectors)
    {
        outVec.insert(outVec.end(), vec.begin(), vec.end());
    }
    return outVec;
}

std::vector<std::pair<GameObject*, GameObject*>> Scene::FindCollisions3()
{
    std::vector<Octree*> activeLeafs;
    m_octree->FindActiveLeafs(activeLeafs);
    std::vector<std::vector<std::pair<GameObject*, GameObject*>>> outVectors(activeLeafs.size());
    m_ThreadPool.sleep_duration = 0;
    m_ThreadPool.paused = false;
    for (uint32_t i = 0; i < activeLeafs.size(); i++)
    {
        m_ThreadPool.push_task(std::bind(&Octree::FindCollisions, activeLeafs[i], std::ref(outVectors[i])));
    }
    std::vector<std::pair<GameObject*, GameObject*>> outVec;
    m_ThreadPool.wait_for_tasks();
    m_ThreadPool.paused = true;
    m_ThreadPool.sleep_duration = 1000;
    for (auto& vec : outVectors)
    {
        outVec.insert(outVec.end(), vec.begin(), vec.end());
    }
    return outVec;
}

void Scene::ResolveOverlapp(const std::vector<std::pair<GameObject*, GameObject*>>& goVec)
{
    for (auto [go1, go2] : goVec)
    {
        if (go1 == go2)
            continue;

        auto it = std::find(go1->m_overlappingGameObjects.begin(), go1->m_overlappingGameObjects.end(), go2);

        if (it == go1->m_overlappingGameObjects.end())
        {
            go1->m_overlappingGameObjects.push_back(go2);
            go2->m_overlappingGameObjects.push_back(go1);
            go1->BeginOverlap(go2);
            go2->BeginOverlap(go1);
        }
    }
    for (auto go : m_dynamicGameObjects)
    {
        for (auto otherGoIter = go->m_overlappingGameObjects.begin(); otherGoIter != go->m_overlappingGameObjects.end(); otherGoIter++)
        {
            if (!go->m_physicsShape->Intersect((*otherGoIter)->m_physicsShape.get()))
            {
                auto go2 = (*otherGoIter);
                otherGoIter = go->m_overlappingGameObjects.erase(otherGoIter);
                go->EndOverlap(go2);
                break;
            }
        }
    }
}

void Scene::RemoveDestroyedGameObjects()
{
    for (auto go : m_toBeDestroyed)
    {
        m_octree->Remove(go);
        //m_octree->RemoveAndEndOverlap(go);

        // fix for shitty function above.
        for (auto overlappingGO : go->m_overlappingGameObjects)
        {
            auto it = find(overlappingGO->m_overlappingGameObjects.begin(), overlappingGO->m_overlappingGameObjects.end(), go);
            if (it != overlappingGO->m_overlappingGameObjects.end())
            {
                overlappingGO->m_overlappingGameObjects.erase(it);
                overlappingGO->EndOverlap(go);
            }
        }

        auto it = std::find(m_gameObjects.begin(), m_gameObjects.end(), go);
        if (it != m_gameObjects.end())
            m_gameObjects.erase(it);

        it = std::find(m_staticGameObjects.begin(), m_staticGameObjects.end(), go);
        if (it != m_staticGameObjects.end())
            m_staticGameObjects.erase(it);

        it = std::find(m_dynamicGameObjects.begin(), m_dynamicGameObjects.end(), go);
        if (it != m_dynamicGameObjects.end())
            m_dynamicGameObjects.erase(it);

        it = std::find(m_opaqueGameObjects.begin(), m_opaqueGameObjects.end(), go);
        if (it != m_opaqueGameObjects.end())
            m_opaqueGameObjects.erase(it);

        it = std::find(m_transparentGameObjects.begin(), m_transparentGameObjects.end(), go);
        if (it != m_transparentGameObjects.end())
            m_transparentGameObjects.erase(it);

        it = std::find(m_simulatedGameObjects.begin(), m_simulatedGameObjects.end(), go);
        if (it != m_simulatedGameObjects.end())
            m_simulatedGameObjects.erase(it);

        it = std::find(m_UI.begin(), m_UI.end(), go);
        if (it != m_UI.end())
            m_UI.erase(it);

        auto lightIt = m_pointLights.find(go);
        if (lightIt != m_pointLights.end())
            m_pointLights.erase(lightIt);

        delete go;
    }
    m_toBeDestroyed.clear();
}

void Scene::RenderQuads()
{
    if (Quad::s_VAO == 0)
        return;
    auto r = RenderWindow::Get();
    r->glBindBuffer(GL_ARRAY_BUFFER, Quad::s_IBO);
    r->glBufferData(GL_ARRAY_BUFFER, Quad::s_quadInstanceTransforms.size() * sizeof(glm::mat4), 
        Quad::s_quadInstanceTransforms.data(), GL_STATIC_DRAW);
    ShaderManager::GetShaderRef("instanced").Use();
    r->glBindVertexArray(Quad::s_VAO);
    r->glBindTexture(GL_TEXTURE_2D, TextureManager::GetTexture("flare.png"));
    r->glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, Quad::s_quadInstanceTransforms.size());
}

void Scene::MouseClicked(int mouseButton)
{
}

void Scene::MouseReleased(int mouseButton)
{
}

void Scene::KeyPressed(int keyCode)
{
    //if (keyCode == Qt::Key_Q)
        //m_DrawDebugLines = !m_DrawDebugLines;
}

void Scene::Move(GameObject* gameObject, const glm::vec3& newPos)
{
    m_octree->Move(gameObject, newPos);
}

void Scene::Destroy(GameObject* gameObject)
{
    m_toBeDestroyed.push_back(gameObject);
}

glm::vec3 Scene::GetSunDirection()
{
    return m_SunDirection;
}

glm::vec3 Scene::GetSunColor()
{
    return m_SunColor;
}

void Scene::SetSunDirection(const glm::vec3& dir)
{
    m_SunDirection = dir;
    auto& lightShader = ShaderManager::GetShaderRef("light");
    auto& landscapeShader = ShaderManager::GetShaderRef("landscape");
    auto& instancedlightShader = ShaderManager::GetShaderRef("instancedlight");
    lightShader.Use();
    lightShader.SetVec3f(dir, "sunDirection");
    landscapeShader.Use();
    landscapeShader.SetVec3f(dir, "sunDirection");
    instancedlightShader.Use();
    instancedlightShader.SetVec3f(dir, "sunDirection");
}

void Scene::SetSunColor(const glm::vec3& color)
{
    m_SunColor = color;
    auto& lightShader = ShaderManager::GetShaderRef("light");
    auto& landscapeShader = ShaderManager::GetShaderRef("landscape");
    auto& instancedlightShader = ShaderManager::GetShaderRef("instancedlight");
    lightShader.Use();
    lightShader.SetVec3f(color, "skyColor");
    landscapeShader.Use();
    landscapeShader.SetVec3f(color, "skyColor");
    instancedlightShader.Use();
    instancedlightShader.SetVec3f(color, "skyColor");
}

void Scene::AddLight(GameObject* go, const glm::vec3& color)
{
    PointLight light;
    light.diffuse = glm::vec4(color, 1.f);
    light.ambient = glm::vec4(color * 0.1f, 1.f);
    light.specular = glm::vec4(1.f);
    m_pointLights[go] = light;
}

void Scene::ToggleDebugLines()
{
    m_DrawDebugLines = !m_DrawDebugLines;
}