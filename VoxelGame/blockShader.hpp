#pragma once

const char* blockVertexShader = /* vertex shader:*/ R"(
#version 460 core

in vec3 pos;
in vec2 uv;
in int normIdx;
in int texIndex;

uniform mat4 matrix;
uniform vec3 offset;
uniform vec3 lightDir;

flat out int texLayer;
out float lightValue;
out vec2 fUV;

void main()
{
	gl_Position = matrix * vec4(offset + pos,1);
    texLayer = texIndex; // x = block type, y = block face
    vec3 norm = vec3(0,0,0);
    switch (normIdx)
    {
        // XY faces
        case 0:
            norm.z = -1;
            break;
        case 1:
            norm.z = 1;
            break;
        // YZ faces
        case 2:
            norm.x = -1;
            break;
        case 3:
            norm.x = 1;
            break;
        // XZ faces
        case 4:
            norm.y = -1;
            break;
        case 5:
            norm.y = 1;
            break;
    }
    lightValue = 0.5 * max(0.0, dot(-norm,lightDir)) + 0.5;
    fUV = uv;
};

)";

const char* blockFragmentShader = /* fragment shader:*/ R"(
#version 460 core
#extension GL_ARB_texture_query_lod : enable

flat in int texLayer; // x = block type, y = block face
in float lightValue;
in vec2 fUV;

uniform sampler2DArray atlas;

out vec4 FragColor;

void main()
{
    vec3 texCoord = vec3(fUV, texLayer);
    vec4 texColor = texture(atlas, texCoord);
    if(texColor.a <= 0.125) discard;
    FragColor = texColor * vec4(lightValue,lightValue,lightValue,1.0);
}

)";