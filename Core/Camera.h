#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"


class Camera
{
	friend class PhysicsShape;
	friend class Frustum;
public:
	enum class Projection
	{
		Perspective,
		Orthographic
	};

	Camera() = delete;
	Camera(const Camera&) = delete;
	Camera(Camera&&) = delete;
	auto operator=(const Camera&) = delete;
	auto operator=(Camera&&) = delete;

	Camera(int width, int height, Projection projection = Projection::Perspective, 
		glm::vec3 position = glm::vec3{ 0.f, 0.f, 3.f }, glm::vec3 target = glm::vec3{ 0.f, 0.f, 0.f },
		glm::vec3 cameraUp = glm::vec3{ 0.f, 1.f, 0.f })
		: m_viewMatrix{1.0f}, 
		m_width{width}, 
		m_height{height},
		m_position{position},
		m_target{target},
		m_cameraUp{cameraUp},
		m_Near{0.1f},
		m_Far{10000.f},
		m_Aspect{0.5f},
		m_Fov{45.f}
	{
		SetProjection(projection);
		UpdateViewMatrix();
	}

	glm::mat4 GetViewMatrix() const { return m_viewMatrix; }
	glm::mat4 GetProjectionMatrix() const { return m_projectionMatrix; }
	void SetProjection(Projection projection)
	{
		float zoom = 250.f;
		m_Aspect = m_width / (float)m_height;
		if (projection == Projection::Perspective)
			m_projectionMatrix = glm::perspective(glm::radians(m_Fov), m_Aspect, m_Near, m_Far);
		else if (projection == Projection::Orthographic)
			m_projectionMatrix = glm::ortho(-(float)m_width / zoom, (float)m_width / zoom, -(float)m_height / zoom, (float)m_height / zoom, m_Near, m_Far);
		m_projectionType = projection;
	}
	Projection GetProjection() { return m_projectionType; }
	void SetViewportSize(int width, int hight)
	{
		m_width = width;
		m_height = hight;
		SetProjection(m_projectionType);
	}
	void SetTarget(const glm::vec3& target)
	{
		m_target = target;
		UpdateViewMatrix();
	}
	void SetPosition(const glm::vec3& position)
	{
		m_position = position;
		UpdateViewMatrix();
	}
	glm::vec3 GetPosition() const
	{
		return m_position;
	}
	void AddPositionOffset(const glm::vec3& offset)
	{
		m_position += offset;
		UpdateViewMatrix();
	}
	glm::vec3 GetCameraForward() const
	{
		return m_cameraForward;
	}
	glm::vec3 GetCameraRight() const
	{
		return m_cameraRight;
	}
	glm::vec3 GetCameraUp() const
	{
		return m_cameraUp;
	}
private:
	void UpdateViewMatrix() 
	{ 
		UpdateVectors();
		m_viewMatrix = glm::lookAt(m_position, m_position + m_cameraForward, m_cameraUp);
	}
	void UpdateVectors()
	{
		m_cameraForward = glm::normalize(m_target - m_position);
		glm::vec3 up{ 0.f, 1.f, 0.f };
		m_cameraRight = glm::normalize(glm::cross(m_cameraForward, up));
		m_cameraUp = glm::normalize(glm::cross(m_cameraForward, -m_cameraRight));
	}
	int m_width;
	int m_height;
	float m_Near;
	float m_Far;
	float m_Fov;
	float m_Aspect;
	glm::mat4 m_viewMatrix;
	glm::mat4 m_projectionMatrix;
	Projection m_projectionType;

	glm::vec3 m_position;
	glm::vec3 m_target;
	glm::vec3 m_cameraUp;
	glm::vec3 m_cameraRight;
	glm::vec3 m_cameraForward;
};
