#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <memory>



class AABB;
class Sphere;
class Plane;
class OBB;
class Frustum;

class Camera;

class PhysicsShape
{
public:
	enum class Shape : uint8_t
	{
		None = 0,
		AABB,
		Sphere,
		Plane,
		OBB,
		Frustum
	};
	virtual bool Intersect(PhysicsShape* shape);

	static bool Intersect(AABB* a, AABB* b);

	static bool Intersect(AABB* box, Sphere* sphere);
	static bool Intersect(Sphere* a, Sphere* b);

	static bool Intersect(AABB* aabb, Plane* plane);
	static bool Intersect(Sphere* sphere, Plane* plane);
	static bool Intersect(Plane* a, Plane* b);

	static bool Intersect(AABB* aabb, OBB* obb);
	static bool Intersect(Sphere* sphere, OBB* obb);
	static bool Intersect(Plane* plane, OBB* obb);
	static bool Intersect(OBB* a, OBB* b);

	const Shape shape;
	glm::vec3 pos{};
protected:
	PhysicsShape(Shape shape);
	static float DistanceFromPointToPlane(const glm::vec3& point, const glm::vec3& planePos, const glm::vec3& planeNormal);
};

class AABB : public PhysicsShape
{
public:
	AABB();
	virtual bool Intersect(PhysicsShape* shape) override;
	glm::vec3 extent{ 0.5f }; // half extent.
};

class Sphere : public PhysicsShape
{
public:
	Sphere();
	virtual bool Intersect(PhysicsShape* shape) override;
	float radius{ 0.5f };
};

class Plane : public PhysicsShape
{
public:
	Plane();
	virtual bool Intersect(PhysicsShape* shape) override;
	glm::vec3 normal{0.f, 1.f, 0.f};
};

class OBB : public PhysicsShape
{
public:
	OBB();
	virtual bool Intersect(PhysicsShape* shape) override;
	glm::vec3 extent{ 0.5f }; // half extent.
	glm::vec3 normals[3];
};

class Frustum : public PhysicsShape
{
public:
	Frustum(std::weak_ptr<Camera> camera);
	virtual bool Intersect(PhysicsShape* shape) override;
	void UpdateFrustum();
private:
	std::weak_ptr<Camera> m_Camera;
	Plane Faces[6]; // near left right bottom top far.
};