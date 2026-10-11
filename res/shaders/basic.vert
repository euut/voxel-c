#version 330 core

layout (location = 0) in vec3 a_position;
layout (location = 1) in vec2 a_uv;
layout (location = 2) in float a_ao;

out vec2 v_texcoord;
out float v_distance;
out float v_ao;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;

void main()
{
    vec4 view_pos = v * m * vec4(a_position, 1.0);

    v_texcoord = a_uv;
    v_distance = length(view_pos.xyz);
    v_ao = a_ao;

    gl_Position = p * view_pos;
}