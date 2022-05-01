#include "Utils.h"

#include "glm/gtx/rotate_vector.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/glm.hpp"

glm::vec3 Utils::BarycentricCoord(const glm::vec2& point, const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
    glm::vec2 ab = b - a;
    glm::vec2 ac = c - a;
    glm::vec3 n = glm::cross(glm::vec3(ab.x, ab.y, 0.f), glm::vec3(ac.x, ac.y, 0.f));
    float area_abc = glm::length(n);
    glm::vec3 baryc;
    // u
    glm::vec2 p = b - point;
    glm::vec2 q = c - point;
    n = glm::cross(glm::vec3(p.x, p.y, 0.f), glm::vec3(q.x, q.y, 0.f));
    baryc.x = n.z / area_abc;
    // v
    p = c - point;
    q = a - point;
    n = glm::cross(glm::vec3(p.x, p.y, 0.f), glm::vec3(q.x, q.y, 0.f));
    baryc.y = n.z / area_abc;
    // w
    p = a - point;
    q = b - point;
    n = glm::cross(glm::vec3(p.x, p.y, 0.f), glm::vec3(q.x, q.y, 0.f));
    baryc.z = n.z / area_abc;
	return baryc;
}

bool Utils::BarycentricIsInside(const glm::vec3& baryc)
{
    for (size_t i = 0; i < 3; i++)
    {
        if (baryc[i] < 0.f || baryc[i] > 1.f)
            return false;
    }
    return true;
}

glm::mat4 Utils::Vec3ToRotationMatrix(glm::vec3 dir)
{
    // Kode rett fra unreal engine...
    // https://github.com/EpicGames/UnrealEngine/blob/c3caf7b6bf12ae4c8e09b606f10a09776b4d1f38/Engine/Source/Runtime/Core/Private/Math/UnrealMath.cpp#L277
    // men med modifikasjon at x = z, y = x, z = y.
    // både i quaternionen og rettning.
    // dette for at unreal har Z som opp.
    // Lager en quaternion fra en rettning.
    // For så å gjøre den til en rotasjonsmatrise.

    float yaw = atan2f(dir.x, dir.z);
    float pitch = atan2f(dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
    float sp = sinf(pitch * 0.5f);
    float sy = sinf(yaw * 0.5f);
    float cp = cosf(pitch * 0.5f);
    float cy = cosf(yaw * 0.5f);
    glm::quat q;
    q.z = sp * sy;
    q.x = -sp * cy;
    q.y = cp * sy;
    q.w = cp * cy;

	return glm::toMat4(q);
}

glm::vec3 Utils::ColorFromScalar(float t)
{
    float colorX = sin(t + glm::quarter_pi<float>()) * 0.5f + 0.5f;
    float colorY = sin(t + glm::half_pi<float>() + glm::quarter_pi<float>()) * 0.5f + 0.5f;
    float colorZ = sin(t) * 0.5f + 0.5f;
    return glm::vec3(colorX, colorY, colorZ);
}
