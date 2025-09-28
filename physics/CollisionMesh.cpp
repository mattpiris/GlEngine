#include "CollisionMesh.h"
#include <algorithm>
#include "../utils/stuff/Log.h"
std::optional<Physics::RayCastHit> Physics::ray_intersects_triangle(RayCast& ray, Triangle triangle) {
	constexpr float epsilon = 1e-5f;
	glm::vec3 edge1 = triangle.v1 - triangle.v0;
	glm::vec3 edge2 = triangle.v2 - triangle.v0;

	glm::vec3 cross2 = glm::cross(ray.direction, edge2);

	float det = glm::dot(edge1, cross2);
	// ray parallel to the plane the triangle lies in
	if (abs(det) < epsilon) { return {}; }

	float inv_det = 1.0f / det;
	glm::vec3 space = ray.origin - triangle.v0;
	float u = glm::dot(space, cross2) * inv_det;

	if ((u < 0 && abs(u) > epsilon) || (u > 1 && abs(u - 1) > epsilon)) { return {}; }

	glm::vec3 s_cross1 = glm::cross(space, edge1);
	float v = glm::dot(ray.direction, s_cross1) * inv_det;
	if ((v < 0 && abs(v) > epsilon) || (u + v > 1 && abs(u + v - 1) > epsilon)) { return {}; }

	float t = inv_det * glm::dot(edge2, s_cross1);
	
	
	if (t > epsilon && t <= ray.maxDist) { 
		glm::vec3 target = ray.origin + glm::normalize(ray.direction) * t; 
		return RayCastHit(target, t);
	}
	else return {};

	return {};
}

std::optional<Physics::RayCastHit> Physics::ray_intersects_mesh(RayCast& ray, CollisionMesh& mesh) {
	std::optional<RayCastHit> bestTarget;
	float t_min = ray.maxDist;
	int intersections = 0; 

	for (auto& triangle : mesh.getCollisionMesh()) {
		auto cast = ray_intersects_triangle(ray, triangle);
		if (cast && cast->t < t_min) {
				bestTarget = cast;
				t_min = cast->t;
		}
	}
	return bestTarget;
}
