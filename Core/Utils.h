#pragma once
#include "glm/vec3.hpp"
#include "glm/vec2.hpp"
#include "glm/mat4x4.hpp"

class Utils
{
public:
	Utils() = delete;
	static glm::vec3 BarycentricCoord(const glm::vec2& point, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);
	static bool BarycentricIsInside(const glm::vec3& baryc);
	static glm::mat4 Vec3ToRotationMatrix(glm::vec3 dir);
	static glm::vec3 ColorFromScalar(float t);
};