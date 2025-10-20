#pragma once
#include "Definitions.hpp"
#include "AABB.hpp"

class Creature
{
public:
	// Positional/ physics info
	vec3 position;
	vec3 velocity;
	vec2 targetVel;
	vec3 acceleration;
	float hAngle;
	float vAngle;

	// Movement info
	float moveSpeed;
	float angleSpeed;

	// Bbox info
	float bboxWidth;
	float bboxHeight;

	// Info from physics
	bool onGround;

	// Physics
	void Force(vec3 force);
	void Jump();
	void PhysUpdate(World& world, float dt);
	vec2 GetXYDir();
	vec3 GetDir();
	AABB GetAABB();

	// Thinking
	virtual void Act(World& world) = 0;

private:
	void SweepBBox(World& world, vec3 delta, float timeLeft, float dt);
};