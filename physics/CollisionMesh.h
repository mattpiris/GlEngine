#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <vector>
#include "RayCast.h"

namespace Physics
{
	struct Triangle
	{
		glm::vec3 v0;
		glm::vec3 v1;
		glm::vec3 v2;
	};

	class CollisionMesh
	{
	public:
		CollisionMesh() = default;
		CollisionMesh(std::vector<Triangle> mesh) : m_collisionMesh(mesh) {}
		std::vector<Triangle> getCollisionMesh() { return m_collisionMesh; }
	private:
		std::vector<Triangle> m_collisionMesh;
	};

	// möller trumbore intersection algorithm from wikipedia
	std::optional<RayCastHit> ray_intersects_triangle(RayCast& ray, Triangle triangle);
	// returns the nearest intersection point from the raycast
	std::optional<RayCastHit> ray_intersects_mesh(RayCast& ray, CollisionMesh& mesh);
}