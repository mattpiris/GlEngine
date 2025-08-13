// get a random point inside the shape
// 
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>

class IEmissionShape
{
public:
	virtual glm::vec3 generatePosition() = 0;
	virtual glm::vec3 generateDirection() { return glm::sphericalRand(1.0f); }
};