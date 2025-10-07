#pragma once
#include <glm/glm.hpp>

namespace Physics
{
	class SphereCollider
	{
	public:
		const glm::vec3 center;
		const float radius;

		SphereCollider(glm::vec3 center, float radius) : center(center), radius(radius) {}
		// checks if the point is inside the sphere collider
		bool isInside(glm::vec3 point) {
			glm::vec3 diff = point - center;
			float r = glm::length(diff);
			return r < radius;
		}
	};
}