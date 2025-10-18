#pragma once

const char* uiVertexShader = /* vertex shader:*/ R"(
#version 460 core

in vec2 pos;
in vec2 uv;

uniform vec4 color;
uniform mat3 matrix;

out vec2 fUV;
out vec4 fColor;

void main()
{
    gl_Position = vec4((matrix * vec3(pos,1)).xy, 0, 1);
    fColor = color;
    fUV = uv;
};

)";

const char* uiFragmentShader = /* fragment shader:*/ R"(
#version 460 core

in vec2 fUV;
in vec4 fColor;

uniform sampler2D tex;

out vec4 FragColor;

void main()
{
    FragColor = texture(tex, fUV) * fColor;
}

)";