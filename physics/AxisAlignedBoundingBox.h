#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/vec3.hpp>
#include <glm/glm.hpp>

#include <optional>
#include "RayCast.h"

namespace Physics
{
	struct AABB
	{
	public:
		const glm::vec3 minDist;
		const glm::vec3 maxDist;

		AABB(glm::vec3 min, glm::vec3 max) : minDist(min), maxDist(max) {}
	};
}