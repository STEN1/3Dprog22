#include "Door.h"
#include "VisualObject/Mesh.h"
#include "Core/TextureManager.h"

Door::Door(Scene& scene, const glm::mat4 transform)
    : GameObject(scene, transform)
{
    m_vo = std::make_unique<Mesh>(
            *this,
            "../3Dprog22/assets/treehouse_door.obj",
            TextureManager::GetTexture("treehouse_door_color.png"));
    objectType = ObjectType::Dynamic;
    m_closedTransform = m_vo->GetLocalTransform();
    auto physicsBox = static_cast<AABB*>(m_physicsShape.get());
    physicsBox->extent *= 15.f;
    physicsBox->pos.x += 3.f;
    physicsBox->pos.y += 4.f;
    physicsBox->pos.z += 4.f;
    m_name = "Door";
}

void Door::BeginOverlap(GameObject* other)
{
    GameObject::BeginOverlap(other);
    if (other->GetName() == "Player")
        m_shouldOpen = true;
}

void Door::EndOverlap(GameObject* other)
{
    GameObject::EndOverlap(other);
    if (other->GetName() == "Player")
        m_shouldOpen = false;
}

void Door::Update(float deltaTime)
{
    if (m_shouldOpen && m_doorAngle < m_doorMaxAngle)
    {
        m_doorAngle += deltaTime * m_doorSpeed;
        auto transform = glm::rotate(glm::mat4(1.f), -m_doorAngle, { 0.f, 1.f, 0.f });
        m_vo->SetLocalTransform(transform);
    }
    else if (!m_shouldOpen && m_doorAngle > 0.f)
    {
        m_doorAngle -= deltaTime * m_doorSpeed;
        auto transform = glm::rotate(glm::mat4(1.f), -m_doorAngle, { 0.f, 1.f, 0.f });
        m_vo->SetLocalTransform(transform);
    }
    //RenderWindow::Get()->DrawAABB(m_physicsBox.pos, m_physicsBox.extent);
}