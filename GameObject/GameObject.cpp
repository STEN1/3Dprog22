#include "GameObject.h"
#include "Core/Camera.h"
#include "Core/Input.h"
#include "Core/ShaderManager.h"
#include "Core/TextureManager.h"
#include <QCursor>
#include "Scene/Scene.h"
#include "Core/Logger.h"
#include "renderwindow.h"


GameObject::GameObject(Scene& scene, glm::mat4 transform)
    : m_transform{ transform }
    , m_name{ "GameObject" }
    , m_scene{ scene }
    , m_physicsShape{ std::make_unique<AABB>() }
{
    m_physicsShape->pos = { m_transform[3].x, m_transform[3].y, m_transform[3].z };
}

GameObject::GameObject(Scene& scene)
    : m_vo{ nullptr }
    , m_transform{ glm::mat4(1.f) }
    , m_name{ "GameObject" }
    , m_scene{ scene }
    , m_physicsShape{ std::make_unique<AABB>() }
{
}

GameObject::~GameObject()
{
    //LOG("GO Base destructor: " + m_name + " With VO: " + m_vo->GetName());
}

void GameObject::Draw()
{
	if (m_vo)
		m_vo->Draw();
}

void GameObject::Update(float deltaTime)
{
}

void GameObject::BeginOverlap(GameObject* other)
{
    //LOG("BeginOverlap");
    //LOG(m_name + " overlapping " + other->m_name);
}

void GameObject::EndOverlap(GameObject* other)
{
    //LOG("EndOverlap");
}

void GameObject::AddPositionOffset(const glm::vec3& offset)
{
    glm::vec3 newPos = { m_transform[3].x,m_transform[3].y,m_transform[3].z };
    newPos.x += offset.x;
    newPos.y += offset.y;
    newPos.z += offset.z;
    m_scene.Move(this, newPos);
    if (m_vo)
        m_vo->SetLocalTransform(m_vo->GetLocalTransform());
}

void GameObject::SetPosition(const glm::vec3& pos)
{
    glm::vec3 newPos = { m_transform[3].x,m_transform[3].y,m_transform[3].z };
    newPos.x = pos.x;
    newPos.y = pos.y;
    newPos.z = pos.z;
    m_scene.Move(this, newPos);
    if (m_vo)
        m_vo->SetLocalTransform(m_vo->GetLocalTransform());
}

glm::vec3 GameObject::GetPosition() const
{
    return glm::vec3{ m_transform[3].x, m_transform[3].y, m_transform[3].z };
}

void GameObject::Destroy()
{
    m_scene.Destroy(this);
}