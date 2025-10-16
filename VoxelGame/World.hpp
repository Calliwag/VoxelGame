#pragma once
#include "ChunkManager.hpp"
#include "Renderer.hpp"

class World;

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

	// Thinking
	virtual void Act(World& world) = 0;

private:
	void SweepBBox(World& world, vec3 delta, float timeLeft, float dt);
};

class World
{
public:
	Window* window;
	ChunkManager cm;
	Renderer r;
	list<Creature*> creatures;
	Creature* viewCreature;

	float sunHAngle = 3.1416 / 6.f;
	float sunVAngle = 3.1416 / 4.f;
	
	World();
	~World();
	void AddCreature(Creature* creature);
	void SetViewCreature(Creature* creature);

	bool ShouldQuit();
	void Step(float dt);
	void Render();
	void GenChunks(int count);
	void UpdateToGenerateList();
};