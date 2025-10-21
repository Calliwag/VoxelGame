#pragma once

#include <glm/glm.hpp>

using namespace glm;

struct Face
{
    vec3* verts;
    vec2* uvs;
    vec3 norm;
    int texIdx;

    Face(vec3* verts, vec2* uvs, vec3 norm, int texIdx) : verts(verts), uvs(uvs), norm(norm), texIdx(texIdx)
    {
    };
    Face(const Face& other) : texIdx(other.texIdx)
    {
        if (other.verts) {
            verts = new vec3[4];
            uvs = new vec2[4];
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = other.verts[i];
                uvs[i] = other.uvs[i];
            }
            norm = other.norm;
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
            uvs = new vec2[4];
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = other.verts[i];
                uvs[i] = other.uvs[i];
            }
            norm = other.norm;
        }
        else {
            verts = nullptr;
            uvs = nullptr;
        }
        return *this;
    }

    ~Face()
    {
        if(verts)
            delete[] verts;
    }
};

