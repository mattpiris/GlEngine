#include "AxisAlignedBoundingBox.h"

bool Physics::ray_intersects_AABB(RayCast& ray, AABB& aabb) { 
	float tNear = -FLT_MAX;
	float tFar = FLT_MAX;

	for (int i = 0; i < 3; i++) {
		// min and max distance along all the axis of the aabb
		float t1 = (aabb.minDist[i] - ray.origin[i]) / ray.direction[i];
		float t2 = (aabb.maxDist[i] - ray.origin[i]) / ray.direction[i];
	
		if (t1 > t2) std::swap(t1, t2);

		tNear = std::max(tNear, t1);
		tFar = std::min(tFar, t2);

		if (tNear > tFar) return false;
		if (tFar < 0) return false;
	}

	return true;
}
