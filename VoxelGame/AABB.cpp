#include "AABB.hpp"
#include <limits>

float AABB::SweepCollision(vec3 thisDelta, AABB other, vec3& normal)
{
    vec3 distEntry = { 0,0,0 };
    vec3 distExit = { 0,0,0 };
    if (thisDelta.x > 0.0f)
    {
        distEntry.x = other.pos.x - (pos.x + size.x);
        distExit.x = (other.pos.x + other.size.x) - pos.x;
    }
    else
    {
        distEntry.x = (other.pos.x + other.size.x) - pos.x;
        distExit.x = other.pos.x - (pos.x + size.x);
    }
    if (thisDelta.y > 0.0f)
    {
        distEntry.y = other.pos.y - (pos.y + size.y);
        distExit.y = (other.pos.y + other.size.y) - pos.y;
    }
    else
    {
        distEntry.y = (other.pos.y + other.size.y) - pos.y;
        distExit.y = other.pos.y - (pos.y + size.y);
    }
    if (thisDelta.z > 0.0f)
    {
        distEntry.z = other.pos.z - (pos.z + size.z);
        distExit.z = (other.pos.z + other.size.z) - pos.z;
    }
    else
    {
        distEntry.z = (other.pos.z + other.size.z) - pos.z;
        distExit.z = other.pos.z - (pos.z + size.z);
    }

    vec3 timeEntry = { -std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity() };
    vec3 timeExit = { std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity(),std::numeric_limits<float>::infinity() };
    if (thisDelta.x != 0.0f)
    {
        timeEntry.x = distEntry.x / thisDelta.x;
        timeExit.x = distExit.x / thisDelta.x;
    }
    if (thisDelta.y != 0.0f)
    {
        timeEntry.y = distEntry.y / thisDelta.y;
        timeExit.y = distExit.y / thisDelta.y;
    }
    if (thisDelta.z != 0.0f)
    {
        timeEntry.z = distEntry.z / thisDelta.z;
        timeExit.z = distExit.z / thisDelta.z;
    }

    float entryTime = max(timeEntry.x, max(timeEntry.y, timeEntry.z));
    float exitTime = min(timeExit.x, min(timeExit.y, timeExit.z));

    if (entryTime > exitTime || timeEntry.x < 0.0f && timeEntry.y < 0.0f && timeEntry.z < 0.0f || timeEntry.x > 1.0f || timeEntry.y > 1.0f || timeEntry.z > 1.0f)
    {
        normal = { 0,0,0 };
        return 1.0f;
    }

    if (timeEntry.x > timeEntry.y && timeEntry.x > timeEntry.z)
    {
        if (thisDelta.x < 0.0f)
        {
            normal = { 1,0,0 };
        }
        else
        {
            normal = { -1,0,0 };
        }
    }
    else if (timeEntry.y > timeEntry.z)
    {
        if (thisDelta.y < 0.0f)
        {
            normal = { 0,1,0 };
        }
        else
        {
            normal = { 0,-1,0 };
        }
    }
    else
    {
        if (thisDelta.z < 0.0f)
        {
            normal = { 0,0,1 };
        }
        else
        {
            normal = { 0,0,-1 };
        }
    }
    return entryTime;
}

AABB AABB::BroadExpand(vec3 delta)
{
    vec3 nPos;
    vec3 nSize;

    nPos.x = delta.x > 0 ? pos.x : pos.x + delta.x;
    nPos.y = delta.y > 0 ? pos.y : pos.y + delta.y;
    nPos.z = delta.z > 0 ? pos.z : pos.z + delta.z;

    nSize.x = delta.x > 0 ? size.x + delta.x : size.x - delta.x;
    nSize.y = delta.y > 0 ? size.y + delta.y : size.y - delta.y;
    nSize.z = delta.z > 0 ? size.z + delta.z : size.z - delta.z;

    return AABB(nPos, nSize);
}
