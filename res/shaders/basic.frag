#version 330 core

uniform sampler2D tex;

in vec3 v_normal;
in vec2 v_texcoord;
in float v_distance;
in float v_ao;

out vec4 frag_color;

const vec3 light_dir = vec3(1.0, 0.4, 0.2);
const vec3 light_color = vec3(0.8, 0.8, 0.75);

const vec3 fog_color = vec3(0.62, 0.81, 1.0);
const float fog_end = 120;
const float fog_start = fog_end * 0.7;

void main()
{
    vec4 tex_color = texture(tex, v_texcoord);
    if (tex_color.a < 0.1) discard;

    vec3 normal = normalize(v_normal);
    vec3 light = normalize(light_dir);

    float diffuse = max(dot(normal, light), 0.0);
    float lighting = v_ao * 0.6 + diffuse * 0.4;

    float fog_factor = (v_distance - fog_start) / (fog_end - fog_start);
    fog_factor = clamp(fog_factor, 0.0, 1.0);

    vec3 final_color = tex_color.rgb * lighting;

    frag_color = vec4(mix(final_color, fog_color, fog_factor), tex_color.a);
}