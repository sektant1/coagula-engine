#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec2 vUV;

void main()
{
    vUV         = position.xy + vec2(0.5);
    gl_Position = uProjection * uView * uModel * vec4(position, 1.0);
}
