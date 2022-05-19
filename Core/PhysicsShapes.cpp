#include "PhysicsShapes.h"
#include "logger.h"
#include <algorithm>
#include "Camera.h"

PhysicsShape::PhysicsShape(Shape shape)
	: shape(shape)
{
}

float PhysicsShape::DistanceFromPointToPlane(const glm::vec3& point, const glm::vec3& planePos, const glm::vec3& planeNormal)
{
	return glm::dot(point - planePos, planeNormal);
}

bool PhysicsShape::Intersect(PhysicsShape* shape)
{
	LOG_HIGHLIGHT("COLLOSION WITH NONE?!?!?!");
	return false;
}

bool PhysicsShape::Intersect(AABB* a, AABB* b)
{
	auto& aExtent = a->extent;
	auto& aPos = a->pos;

	auto& bExtent = b->extent;
	auto& bPos = b->pos;
	
	float xDist = abs(aPos.x - bPos.x);
	float yDist = abs(aPos.y - bPos.y);
	float zDist = abs(aPos.z - bPos.z);
	float xMinDist = aExtent.x + bExtent.x;
	float yMinDist = aExtent.y + bExtent.y;
	float zMinDist = aExtent.z + bExtent.z;

	return (xDist < xMinDist&& yDist < yMinDist&& zDist < zMinDist);
}

bool PhysicsShape::Intersect(AABB* box, Sphere* sphere)
{
	glm::vec3 boxPoint;
	boxPoint.x = std::clamp<float>(sphere->pos.x, box->pos.x - box->extent.x, box->pos.x + box->extent.x);
	boxPoint.y = std::clamp<float>(sphere->pos.y, box->pos.y - box->extent.y, box->pos.y + box->extent.y);
	boxPoint.z = std::clamp<float>(sphere->pos.z, box->pos.z - box->extent.z, box->pos.z + box->extent.z);
	float dist = glm::distance(boxPoint, sphere->pos);
	return (dist < sphere->radius);
}

bool PhysicsShape::Intersect(Sphere* a, Sphere* b)
{
	float minDist = a->radius + b->radius;
	float dist = glm::distance(a->pos, b->pos);
	return (dist < minDist);
}

bool PhysicsShape::Intersect(AABB* aabb, Plane* plane)
{
	float r = 
		aabb->extent.x * glm::abs(plane->normal.x) + 
		aabb->extent.y * glm::abs(plane->normal.y) + 
		aabb->extent.z * glm::abs(plane->normal.z);
	return DistanceFromPointToPlane(aabb->pos, plane->pos, plane->normal) < -r;
}

bool PhysicsShape::Intersect(Sphere* sphere, Plane* plane)
{
	auto distance = DistanceFromPointToPlane(sphere->pos, plane->pos, plane->normal);
	return distance < -sphere->radius; // Ericson p.161
}

bool PhysicsShape::Intersect(Plane* a, Plane* b)
{
	return glm::abs(a->normal) != glm::abs(b->normal);
}

bool PhysicsShape::Intersect(AABB* aabb, OBB* obb)
{
	// TODO:
	return false;
}

bool PhysicsShape::Intersect(Sphere* sphere, OBB* obb)
{
	// TODO:
	return false;
}

bool PhysicsShape::Intersect(Plane* plane, OBB* obb)
{
	float r = 
		obb->extent.x * glm::abs(glm::dot(plane->normal, obb->normals[0])) +
		obb->extent.y * glm::abs(glm::dot(plane->normal, obb->normals[1])) +
		obb->extent.z * glm::abs(glm::dot(plane->normal, obb->normals[2]));
	float dist = DistanceFromPointToPlane(obb->pos, plane->pos, plane->normal);
	return dist < -r;
}

bool PhysicsShape::Intersect(OBB* a, OBB* b)
{
	// TODO:
	const auto ab = b->pos - a->pos;
	const auto lengthAB = glm::length(ab);

	for (const auto& aNormal : a->normals)
	{
		
	}
	for (const auto& bNormal : b->normals)
	{

	}
	return true;
}

AABB::AABB()
	: PhysicsShape(Shape::AABB)
{
}

bool AABB::Intersect(PhysicsShape* shape)
{
	switch (shape->shape)
	{
	case Shape::AABB:
		return PhysicsShape::Intersect(this, reinterpret_cast<AABB*>(shape));
	case Shape::Sphere:
		return PhysicsShape::Intersect(this, reinterpret_cast<Sphere*>(shape));
	case Shape::Plane:
		return PhysicsShape::Intersect(this, reinterpret_cast<Plane*>(shape));
	case Shape::Frustum:
		return shape->Intersect(this);
	case Shape::None:
		LOG_HIGHLIGHT("COLLOSION WITH NONE?!?!?!");
		break;
	}

	return false;
}

Sphere::Sphere()
	: PhysicsShape(Shape::Sphere)
{
}

bool Sphere::Intersect(PhysicsShape* shape)
{
	switch (shape->shape)
	{
	case Shape::AABB:
		return PhysicsShape::Intersect(reinterpret_cast<AABB*>(shape), this);
	case Shape::Sphere:
		return PhysicsShape::Intersect(this, reinterpret_cast<Sphere*>(shape));
	case Shape::Plane:
		return PhysicsShape::Intersect(this, reinterpret_cast<Plane*>(shape));
	case Shape::Frustum:
		return shape->Intersect(this);
	case Shape::None:
		LOG_HIGHLIGHT("COLLOSION WITH NONE?!?!?!");
		break;
	}

	return false;
}

Plane::Plane()
	: PhysicsShape(Shape::Plane)
{
}

bool Plane::Intersect(PhysicsShape* shape)
{
	switch (shape->shape)
	{
	case Shape::AABB:
		return PhysicsShape::Intersect(reinterpret_cast<AABB*>(shape), this);
	case Shape::Sphere:
		return PhysicsShape::Intersect(reinterpret_cast<Sphere*>(shape), this);
	case Shape::Plane:
		return PhysicsShape::Intersect(this, reinterpret_cast<Plane*>(shape));
	case Shape::Frustum:
		return shape->Intersect(this);
	case Shape::None:
		LOG_HIGHLIGHT("COLLOSION WITH NONE?!?!?!");
		break;
	}

	return false;
}

OBB::OBB()
	: PhysicsShape(Shape::OBB)
{
}

bool OBB::Intersect(PhysicsShape* shape)
{
	// TODO:
	return false;
}

Frustum::Frustum(std::weak_ptr<Camera> camera)
	:PhysicsShape(Shape::Frustum)
	,m_Camera{ camera }
{
	UpdateFrustum();
}

bool Frustum::Intersect(PhysicsShape* shape)
{
	for (uint32_t i = 0; i < 6; i++)
	{
		if (Faces[i].Intersect(shape))
			return false;
	}
	return true;
}

void Frustum::UpdateFrustum()
{
	auto camera = m_Camera.lock();
	if (!camera)
	{
		LOG_ERROR("NO CAMERA IN FRUSTUM!");
		return;
	}
	const float halfVSide = camera->m_Far * tanf(camera->m_Fov * .5f);
	const float halfHSide = halfVSide * camera->m_Aspect;
	const glm::vec3 frontMultFar = camera->m_Far * camera->m_cameraForward;

	Faces[0].pos = camera->m_position + camera->m_Near * camera->m_cameraForward; // near
	Faces[0].normal = camera->m_cameraForward;

	Faces[1].pos = camera->m_position;  // left
	Faces[1].normal = glm::normalize(glm::cross(frontMultFar - camera->m_cameraRight * halfHSide, camera->m_cameraUp));

	Faces[2].pos = camera->m_position; // right
	Faces[2].normal = glm::normalize(glm::cross(camera->m_cameraUp, frontMultFar + camera->m_cameraRight * halfHSide));

	Faces[3].pos = camera->m_position; // bottom
	Faces[3].normal = glm::normalize(glm::cross(frontMultFar + camera->m_cameraUp * halfVSide, camera->m_cameraRight));

	Faces[4].pos = camera->m_position; // top
	Faces[4].normal = glm::normalize(glm::cross(camera->m_cameraRight, frontMultFar - camera->m_cameraUp * halfVSide));

	Faces[5].pos = camera->m_position + frontMultFar; // far
	Faces[5].normal = -camera->m_cameraForward;
}

void Frustum::SetCamera(std::weak_ptr<Camera> camera)
{
	m_Camera = camera;
}
