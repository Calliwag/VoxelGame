#include "World.hpp"
#include <limits>

void Creature::Force(vec3 force)
{
	velocity += force;
}

void Creature::PhysUpdate(World& world, float dt)
{
    // Reset flags
    onGround = false;

    vec3 delta = velocity * dt;
	position += delta;
    if (onGround)
    {
        velocity.x *= 0.99 * dt;
        velocity.y *= 0.99 * dt;
    }
}