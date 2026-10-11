#version 330 core

uniform sampler2D tex;

in vec2 v_texcoord;
in float v_distance;
in float v_ao;

out vec4 frag_color;

const vec3 fog_color = vec3(0.62, 0.81, 1.0);
const float fog_end = 120;
const float fog_start = fog_end * 0.7;

void main()
{
    vec4 tex_color = texture(tex, v_texcoord);
    if (tex_color.a < 0.1) discard;

    float fog_factor = (v_distance - fog_start) / (fog_end - fog_start);
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    frag_color = vec4(mix(tex_color.rgb * v_ao, fog_color, fog_factor), tex_color.a);
}