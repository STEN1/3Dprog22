#include "AnimatedBezier.h"
#include "VisualObject/BezierCurve.h"

AnimatedBezier::AnimatedBezier(Scene& scene)
    : GameObject(scene, glm::mat4(1.f))
{
    m_vo = std::make_unique<BezierCurve>(
        *this,
        glm::vec4(1.f, 1.f, 1.f, 1.f),
        glm::vec4(1.f, 0.5f, 1.f, 1.f));
    objectType = GameObject::ObjectType::Dynamic;
    m_name = "AnimatedBezier";
}

void AnimatedBezier::Update(float deltaTime)
{
    auto currentTime = Scene::CurrentTime();
    float graphColorX = sin(currentTime + glm::quarter_pi<float>()) * 0.5f + 0.5f;
    float graphColorY = sin(currentTime + glm::half_pi<float>() + glm::quarter_pi<float>()) * 0.5f + 0.5f;
    float graphColorZ = sin(currentTime) * 0.5f + 0.5f;
    glm::vec4 graphColor = { graphColorX, graphColorY, graphColorZ, 1.f };
    if (auto vo = m_vo.get())
    {
        if (auto bezier = dynamic_cast<BezierCurve*>(vo))
        {
            float t = (sin(currentTime) + 1.f) * 0.5f;
            bezier->SetT(t);
            bezier->m_curveColor = graphColor;
        }
    }
}