#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in float aShading;

out vec2 v_TexCoord;
out float v_Distance;
out float v_Shading;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

void main()
{
    vec4 viewPos = v * m * vec4(aPos, 1.0);

    v_Shading = aShading;
    v_Distance = length(viewPos.xyz);
    v_TexCoord = aUV;

    gl_Position = p * viewPos;
}