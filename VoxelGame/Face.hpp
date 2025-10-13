#pragma once

#include <glm/glm.hpp>

using namespace glm;

struct Face
{
    vec3* verts;
    u8 type;

    Face(vec3* verts, u8 type) : verts(verts), type(type)
    {
        vec3 center = { 0,0,0 };
        for (int i = 0; i < 4; i++)
            center += verts[i];
        center /= 4.f;
        for (int i = 0; i < 4; i++)
        {
            vec3 axis = verts[i] - center;
            verts[i].x = center.x + axis.x + 0.00025f * sign(axis.x);
            verts[i].y = center.y + axis.y + 0.00025f * sign(axis.y);
            verts[i].z = center.z + axis.z + 0.00025f * sign(axis.z);
        }
    };
    Face(const Face& other) : type(other.type)
    {
        if (other.verts) {
            verts = new vec3[4];
            for (int i = 0; i < 4; ++i)
                verts[i] = other.verts[i];
        }
        else {
            verts = nullptr;
        }
    }

    Face& operator=(const Face& other)
    {
        if (this == &other)
            return *this;
        type = other.type;
        delete[] verts;
        if (other.verts) {
            verts = new vec3[4];
            for (int i = 0; i < 4; ++i)
                verts[i] = other.verts[i];
        }
        else {
            verts = nullptr;
        }
        return *this;
    }

    ~Face()
    {
        if(verts)
            delete[] verts;
    }
};

