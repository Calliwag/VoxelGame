#pragma once

#include <glm/glm.hpp>

using namespace glm;

class Plane
{
public:
	vec3 normal;
	float orgDist;

	Plane()
	{
		normal = { 0,0,0 };
		orgDist = 0;
	}
	Plane(vec3 normal, float orgDist)
	{
		this->normal = normal;
		this->orgDist = orgDist;
	}
	float GetSignedDist(vec3 point)
	{
		point = point - orgDist * normal;
		return dot(normal, point);
	}
};

