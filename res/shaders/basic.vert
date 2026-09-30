#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in float shading;

out vec2 v_texcoord;
out float v_distance;
out float v_shading;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

void main()
{
    vec4 view_pos = v * m * vec4(position, 1.0);

    v_shading = shading;
    v_distance = length(view_pos.xyz);
    v_texcoord = uv;

    gl_Position = p * view_pos;
}