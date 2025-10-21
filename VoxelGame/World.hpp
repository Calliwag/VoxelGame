#pragma once
#include "ChunkManager.hpp"
#include "Renderer.hpp"

class World;

class World
{
public:
	Window* window;
	ChunkManager cm;
	Renderer r;
	PlayerCreature* player;
	list<Creature*> creatures;
	std::unordered_map<ivec3, list<Creature*>> chunkCreatures;

	float sunHAngle = 3.1416 / 6.f;
	float sunVAngle = 3.1416 / 4.f;
	
	World();
	~World();
	void AddCreature(Creature* creature);
	void SetPlayer(PlayerCreature* player);

	bool ShouldQuit();
	void Step(float dt);
	void Render();
	void GenChunks(int count);
	void UpdateToGenerateList();
	void SetBlock(ivec3 coord, blockType value);
};