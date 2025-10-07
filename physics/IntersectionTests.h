#pragma once
#include "AxisAlignedBoundingBox.h"
#include "SphereCollider.h"
#include "CollisionMesh.h"
#include "RayCast.h"

#include <optional>

namespace Physics {
	// möller trumbore intersection algorithm from wikipedia
	std::optional<RayCastHit> ray_intersects_triangle(RayCast& ray, Triangle triangle);
	// returns the nearest intersection point from the raycast
	std::optional<RayCastHit> ray_intersects_mesh(RayCast& ray, CollisionMesh& mesh);

	bool ray_intersects_AABB(RayCast& ray, AABB& aabb);
	bool sphere_intersects_AABB(SphereCollider& sphere, AABB& aabb);
}