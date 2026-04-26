#version 330 core

uniform vec3 uEmissive;

out vec4 FragColor;

void main()
{
    FragColor = vec4(uEmissive, 1.0);
}
