#pragma once
#include "World.hpp"

class PlayerCreature : public Creature
{
public:
	bool mouseLock;

	PlayerCreature();
	void Act(World& world) override;

private:
	void PlaceBlock(World& world, u8 type);
	void BreakBlock(World& world);
};

