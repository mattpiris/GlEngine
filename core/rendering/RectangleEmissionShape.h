#pragma once
#include "IEmissionShape.h"

class RectangleEmissionShape : public IEmissionShape
{
public:
	RectangleEmissionShape(float width, float height, float depth, glm::vec3 position);
	glm::vec3 generatePosition() override;
	glm::vec3 generateDirection() override;
private:
	float m_width;
	float m_height;
	float m_depth;
	glm::vec3 m_position;
};