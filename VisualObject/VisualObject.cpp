#include "VisualObject.h"

#include "Core/Camera.h"
#include "Core/Logger.h"
#include "renderwindow.h"
#include "Core/ShaderManager.h"
#include "Core/ObjLoader.h"
#include "GameObject/GameObject.h"

#include <fstream>

VisualObject::VisualObject(Shader& shader, GameObject& parent, glm::mat4 transform)
	: m_localTransform{ transform }
    , m_shader{ shader }
    , m_name{ "VisualObject" }
    , m_parent{ parent }
{
	initializeOpenGLFunctions();
    m_globalTransform = m_parent.GetTransform() * m_localTransform;
}

VisualObject::~VisualObject()
{
}

void VisualObject::Draw()
{
}

void VisualObject::AddPositionOffset(glm::vec3 offset)
{
    m_localTransform = glm::translate(m_localTransform, offset);
    m_globalTransform = m_parent.GetTransform() * m_localTransform;
}

glm::vec3 VisualObject::GetLocalPosition() const
{
    return { m_localTransform[3].x, m_localTransform[3].y, m_localTransform[3].z };
}

glm::vec3 VisualObject::GetGlobalPosition() const
{
    return { m_globalTransform[3].x, m_globalTransform[3].y, m_globalTransform[3].z };
}

void VisualObject::SetLocalPosition(glm::vec3 pos)
{
    m_localTransform = glm::translate(glm::mat4(1.f), pos);
    m_globalTransform = m_parent.GetTransform() * m_localTransform;
}

glm::mat4 VisualObject::GetLocalTransform() const
{
    return m_localTransform;
}

glm::mat4 VisualObject::GetGlobalTransform() const
{
    return m_globalTransform;
}

void VisualObject::SetLocalTransform(glm::mat4 transform)
{
    m_localTransform = transform;
    m_globalTransform = m_parent.GetTransform() * m_localTransform;
}