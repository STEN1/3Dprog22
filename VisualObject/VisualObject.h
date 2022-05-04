#pragma once
#include <QOpenGLFunctions_4_5_Core>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "shader.h"

class GameObject;
class VisualObject : protected QOpenGLFunctions_4_5_Core
{
public:
	VisualObject() = delete;
	VisualObject(Shader& shader, GameObject& parent, glm::mat4 transform = glm::mat4(1.f));
	virtual ~VisualObject();
	virtual void Draw();
	std::string GetName() { return m_name; }
	bool IsTransparent() { return m_isTransparent; }
	void SetTransparent(bool t) { m_isTransparent = t; }
	glm::vec3 GetLocalPosition() const;
	glm::vec3 GetGlobalPosition() const;
	glm::mat4 GetLocalTransform() const;
	glm::mat4 GetGlobalTransform() const;
	void SetLocalPosition(glm::vec3 pos);
	void SetLocalTransform(glm::mat4 transform);
	void AddPositionOffset(glm::vec3 offset);
protected:
	bool m_isTransparent = false;
	std::string m_name;
	glm::mat4 m_globalTransform{};
	glm::mat4 m_localTransform{};
	GameObject& m_parent;
	Shader& m_shader;
	uint32_t m_VAO{};
	uint32_t m_VBO{};
	uint32_t m_EBO{};
	uint32_t m_indices{};
	uint32_t m_vertexCount{};
};