#version 330 core

in vec2 vUV;

uniform vec4 uColor;

out vec4 FragColor;

void main()
{
    vec2  c = vUV - vec2(0.5);
    float d = length(c) * 2.0;
    float a = clamp(1.0 - d * d, 0.0, 1.0);
    FragColor = vec4(uColor.rgb * uColor.a * a, uColor.a * a);
}
