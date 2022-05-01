#include "ItemPickup.h"
#include "Core/TextureManager.h"
#include "VisualObject/Quad.h"
#include "Core/Utils.h"

ItemPickup::ItemPickup(Scene& scene, glm::mat4 transform)
    : GameObject(scene, transform)
{
    m_physicsShape = std::make_unique<Sphere>();
    m_physicsShape->pos = { m_transform[3].x, m_transform[3].y, m_transform[3].z };
    m_vo = std::make_unique<Quad>(
        *this,
        TextureManager::GetTexture("wisp.png"));
    objectType = ObjectType::Dynamic;
    m_name = "ItemPickup";
    m_initialPosition = GetPosition();
    m_hightOffset = (rand() / (float)RAND_MAX) * m_bobHight;
    m_scaleOffset = (rand() / (float)RAND_MAX) * glm::pi<float>();
}

void ItemPickup::BeginOverlap(GameObject* other)
{
    if (other->GetName() == "Player")
    {
        Destroy();
    }
}

void ItemPickup::Update(float deltaTime)
{
    if (!m_bob)
        return;

    m_hightOffset += m_bobDir * deltaTime;

    if (m_hightOffset > m_bobHight)
        m_bobDir = -1.f;
    else if (m_hightOffset < 0.f)
        m_bobDir = 1.f;

    auto pos = GetPosition();
    pos.y = m_initialPosition.y + m_hightOffset;
    
    SetPosition(pos);

    auto dir = RenderWindow::Get()->GetCamera()->GetPosition() - GetPosition();
    auto rotation = Utils::Vec3ToRotationMatrix(dir);
    float scaleF = 4.f + ((sinf(m_scene.CurrentTime() + m_scaleOffset) + 1.f) * 4.f);
    scaleF *= 0.5f;
    m_lightRange = scaleF * LightScaleRange;
    auto scale = glm::scale(glm::mat4(1.f), glm::vec3(scaleF));

    m_vo->SetLocalTransform(rotation * scale);
    //RenderWindow::Get()->DrawBox(GetPosition(), m_physicsBox.extent);
}