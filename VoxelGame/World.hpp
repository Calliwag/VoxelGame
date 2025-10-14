#pragma once
#include "ChunkManager.hpp"

class World;

class Creature
{
public:
	// Positional/ physics info
	vec3 position;
	vec3 velocity;
	float hAngle;

	// Bbox info
	float bboxWidth;
	float bboxHeight;

	// Info from physics
	bool onGround;

	// Physics
	void Force(vec3 force);
	void PhysUpdate(World& world, float dt);

	// Thinking
	void Act(World& world);
};

class World
{
public:
	ChunkManager cm;
	list<Creature*> creatures;
	

};