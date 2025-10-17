#pragma once

#include <glm/glm.hpp>

using namespace glm;

struct Face
{
    vec3* verts;
    int texIdx;

    Face(vec3* verts, int texIdx) : verts(verts), texIdx(texIdx)
    {
    };
    Face(const Face& other) : texIdx(other.texIdx)
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
        texIdx = other.texIdx;
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

