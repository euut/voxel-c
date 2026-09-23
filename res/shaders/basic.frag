#version 330 core

uniform sampler2D tex;

in vec2 v_TexCoord;
in float v_Distance;
in float v_Shading;

out vec4 FragColor;

const vec3 fogColor = vec3(0.62, 0.81, 1.0);
const float fogEnd = 120;
const float fogStart = fogEnd * 0.7;

void main()
{
    vec4 texColor = texture(tex, v_TexCoord);
    if (texColor.a < 0.1) discard;

    float fogFactor = (v_Distance - fogStart) / (fogEnd - fogStart);
    fogFactor = clamp(fogFactor, 0.0, 1.0);

    vec3 finalColor = mix(texColor.rgb * v_Shading, fogColor, fogFactor);

    FragColor = vec4(finalColor, texColor.a);
}