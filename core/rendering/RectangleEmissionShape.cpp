#include "RectangleEmissionShape.h"

RectangleEmissionShape::RectangleEmissionShape(float width, float height, float depth, glm::vec3 position)
	:m_width(width), m_height(height), m_depth(depth), m_position(position)
{}

glm::vec3 RectangleEmissionShape::generatePosition() {
	float x = glm::linearRand(0.0f, m_width);
	float y = glm::linearRand(0.0f, m_height);
	float z = glm::linearRand(0.0f, m_depth);

	glm::vec3 random = glm::vec3(x, y, z);
	return random + m_position;
}

glm::vec3 RectangleEmissionShape::generateDirection() {
	return glm::normalize(glm::sphericalRand(1.0f));
}
