#pragma once
#include "VisualObject.h"

class BezierCurve : public VisualObject
{
	friend class AnimatedBezier;
public:
	BezierCurve(GameObject& parent, const glm::vec4& controlColor, const glm::vec4& curveColor, const std::vector<glm::vec3>& controlPoints, const glm::mat4& transform = glm::mat4(1.f));
	void Draw() override;
	void SetT(float t);
	void SetCurveColor(const glm::vec4& color) { m_curveColor = color; }
	glm::vec3 CalcBezier(float t);
private:
	float T;
	glm::vec4 m_controlColor;
	glm::vec4 m_curveColor;
	std::vector<glm::vec3> m_controlPoints;
	glm::vec3 m_curvePoint;
};
