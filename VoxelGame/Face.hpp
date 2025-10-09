#pragma once

#include <glm/glm.hpp>

using namespace glm;

struct Face
{
    ivec3* verts;
    u8 type;

    Face(ivec3* verts, u8 type) : verts(verts), type(type) {};
    Face(const Face& other)
        : type(other.type)
    {
        if (other.verts) {
            verts = new ivec3[4];
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
            verts = new ivec3[4];
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
        delete[] verts;
    }
};

