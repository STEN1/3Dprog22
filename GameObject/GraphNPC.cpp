#include "GraphNPC.h"

#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include "VisualObject/Quad.h"
#include "Core/Utils.h"

GraphNPC::GraphNPC(Scene& scene, float graphFunc(float z), float terrainFunc(float x, float z))
    : GameObject(scene, glm::mat4(1.f))
{
    m_vo = std::make_unique<Quad>(*this, TextureManager::GetTexture("flare.png"));
    objectType = ObjectType::Dynamic;
    m_name = "GraphNPC";
    m_graphFunc = graphFunc;
    m_terrainFunc = terrainFunc;
}

void GraphNPC::Update(float deltaTime)
{
    static constexpr float speed{ 20.f };
    m_z += deltaTime * m_direction * speed * 0.5f;


    float x = m_graphFunc(m_z);
    float y = m_terrainFunc(x, m_z);

    glm::vec3 goal{ x, y + 2.f, m_z };

    glm::vec3 dir = goal - GetPosition();
    if (glm::length(dir) < 1.f)
        m_z += 1.f * m_direction;

    if (m_z > 20.f)
        m_direction = -1.f;
    else if (m_z < -25.f)
        m_direction = 1.f;


    //RenderWindow::Get()->DrawAABB(goal, m_physicsBox.extent / 2.f, glm::vec4(1.f, 0.f, 0.f, 1.f));

    if (glm::length(dir) > 0.001f)
        dir = glm::normalize(dir);

    AddPositionOffset(dir * speed * deltaTime);
    SetPosition({ GetPosition().x, m_terrainFunc(GetPosition().x, GetPosition().z) + 2.f, GetPosition().z });

    auto lookdir = RenderWindow::Get()->GetCamera()->GetPosition() - GetPosition();
    auto rotation = Utils::Vec3ToRotationMatrix(lookdir);
    auto scale = glm::scale(glm::mat4(1.f), glm::vec3(3.f));

    m_vo->SetLocalTransform(rotation * scale);

    //RenderWindow::Get()->DrawAABB(GetPosition(), m_physicsBox.extent);
}