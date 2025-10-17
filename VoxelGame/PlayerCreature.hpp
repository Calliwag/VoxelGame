#pragma once
#include "World.hpp"
#include "Inventory.hpp"

class PlayerCreature : public Creature
{
public:
	// Inventory

	Inventory inv;

	// Controls

	bool mouseLock;

	PlayerCreature();
	void Act(World& world) override;

private:
	void PlaceBlock(World& world, u8 type);
	void BreakBlock(World& world);
};

