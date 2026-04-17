#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 vColor;

uniform mat4 uModel;

void main()
{
    vColor = color;
    gl_Position = uModel * vec4(position, 1.0);
}
