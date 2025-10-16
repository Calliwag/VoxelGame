#pragma once

#include "glm/glm.hpp"

using namespace glm;

class AABB
{
private:
	AABB(vec3 pos, vec3 size) : pos(pos), size(size) {}

public:
	vec3 pos;
	vec3 size;

	static AABB BottomCenterAligned(vec3 pos, vec3 size)
	{
		pos.x -= size.x / 2;
		pos.y -= size.y / 2;
		return AABB(pos, size);
	}
	static AABB NegativeAligned(vec3 pos, vec3 size)
	{
		return AABB(pos, size);
	}
	
	float SweepCollision(vec3 thisDelta, AABB other, vec3& normal);
	AABB BroadExpand(vec3 delta);
};

