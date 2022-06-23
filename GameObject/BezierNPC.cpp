#include "BezierNPC.h"
#include "VisualObject/BezierCurve.h"
#include "VisualObject/Mesh.h"
#include "Core/TextureManager.h"
#include "Core/Globals.h"
#include "Bomb.h"
#include "Scene/Scene.h"

BezierNPC::BezierNPC(Scene& scene, const glm::mat4 transform)
    : GameObject(scene, transform)
{
    m_vo = std::make_unique<BezierCurve>(
        *this,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec4(1.f, 0.5f, 1.f, 1.f),
        std::vector<glm::vec3>{
            { -120.f, 0.f, -100.f},
            { 30.f, 0.f, -100.f },
            { -30.f, 0.f, 100.f },
            { 120.f, 0.f, 100.f }
    });
    m_NPCMesh = std::make_unique<Mesh>(*this,
        Globals::AssetPath + std::string("Ball.obj"),
        TextureManager::GetTexture("testtexture2.png"));
    static_cast<AABB*>(m_physicsShape.get())->extent = glm::vec3(120.f);
    static_cast<AABB*>(m_physicsShape.get())->extent.y = 2.f;
    objectType = GameObject::ObjectType::Dynamic;
    m_name = "BezierNPC";
}

void BezierNPC::Update(float deltaTime)
{
    auto currentTime = Scene::CurrentTime();
    float graphColorX = sin(currentTime + glm::quarter_pi<float>()) * 0.5f + 0.5f;
    float graphColorY = sin(currentTime + glm::half_pi<float>() + glm::quarter_pi<float>()) * 0.5f + 0.5f;
    float graphColorZ = sin(currentTime) * 0.5f + 0.5f;
    glm::vec4 graphColor = { graphColorX, graphColorY, graphColorZ, 1.f };
    if (auto vo = m_vo.get())
    {
        if (auto bezier = dynamic_cast<BezierCurve*>(vo))
        {
            float t = (sin(currentTime * 0.2f) + 1.f) * 0.5f;
            bezier->SetT(t);
            bezier->SetCurveColor(graphColor);
            auto meshTransform = glm::translate(bezier->CalcBezier(t));
            m_NPCMesh->SetLocalTransform(meshTransform);
        }
    }
    m_DropTimer += deltaTime;
    if (m_DropTimer > m_DropInterval)
    {
        m_scene.SpawnGameObject<Bomb>(m_scene, m_NPCMesh->GetGlobalTransform());
        m_DropTimer = 0.f;
    }
}

void BezierNPC::Draw()
{
    GameObject::Draw();
    if (m_NPCMesh)
    {
        m_NPCMesh->Draw();
    }
}
