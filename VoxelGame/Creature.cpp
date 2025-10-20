#include "Creature.hpp"
#include "World.hpp"

void Creature::Force(vec3 force)
{
    acceleration += force;
}

void Creature::Jump()
{
    velocity.z += 5;
}

void Creature::PhysUpdate(World& world, float dt)
{
    // Reset flags
    onGround = false;

    velocity += acceleration * dt;

    if (length(targetVel) > 1.0)
        targetVel = moveSpeed * normalize(targetVel);
    else
        targetVel *= moveSpeed;
    constexpr float fValue = 10;
    velocity.x = velocity.x * (1 - fValue * dt) + targetVel.x * fValue * dt;
    velocity.y = velocity.y * (1 - fValue * dt) + targetVel.y * fValue * dt;
    targetVel = { 0,0 };

    acceleration = { 0,0,0 };
    vec3 delta = velocity * dt;

    SweepBBox(world, delta, 1.0, dt);

    if (onGround)
    {
        velocity.x *= 1 - 0.95 * dt;
        velocity.y *= 1 - 0.95 * dt;
    }

    vAngle = clamp(vAngle, -3.1415f / 2, 3.1415f / 2);
}

vec2 Creature::GetXYDir()
{
    vec2 hDir = { cos(hAngle),sin(hAngle) };
    return hDir;
}

vec3 Creature::GetDir()
{
    vec2 hDir = { cos(hAngle),sin(hAngle) };
    vec3 dir = { hDir.x * cos(vAngle),hDir.y * cos(vAngle),sin(vAngle) };
    return dir;
}

AABB Creature::GetAABB()
{
    return AABB::BottomCenterAligned(position, { bboxWidth,bboxWidth,bboxHeight });
}

void Creature::SweepBBox(World& world, vec3 delta, float timeLeft, float dt)
{
    auto cBox = GetAABB();
    auto broadBox = cBox.BroadExpand(delta);
    ivec3 min = floor(broadBox.pos);
    ivec3 max = ceil(broadBox.pos + broadBox.size);

    bool foundIntersect = false;
    float closestIntersect = timeLeft;
    vec3 closestNorm = { 0,0,0 };
    for (int x = min.x; x < max.x; x++)
    {
        for (int y = min.y; y < max.y; y++)
        {
            for (int z = min.z; z < max.z; z++)
            {
                if (!world.cm.IsBlockSolid({ x,y,z }))
                    continue;
                auto bBox = world.cm.GetBlockAABB({ x,y,z });
                vec3 norm;
                float intersect = cBox.SweepCollision(delta, bBox, norm);
                if (intersect < closestIntersect)
                {
                    foundIntersect = true;
                    closestIntersect = intersect;
                    closestNorm = norm;
                }
            }
        }
    }

    if (!foundIntersect)
    {
        position += delta;
    }
    else
    {
        if (closestNorm.z >= 0)
            onGround = true;
        position += delta * closestIntersect;
        timeLeft = timeLeft - closestIntersect;
        velocity -= dot(velocity, closestNorm) * closestNorm;
        delta = velocity * dt * timeLeft;
        if (timeLeft > 0.0)
        {
            SweepBBox(world, delta, timeLeft, dt);
        }
    }
}