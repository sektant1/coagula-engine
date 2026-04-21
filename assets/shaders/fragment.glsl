#version 330 core

struct Light {
    vec3 color;
    vec3 position;
};

uniform Light uLight;

out vec4 FragColor;

in vec2 vUV;
in vec3 vNormal;
in vec3 vFragPos;

uniform sampler2D baseColorTexture;

void main()
{
    vec3 normal = normalize(vNormal);

    vec3 lightDir = normalize(uLight.position - vFragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 ambient = 0.1 * uLight.color;
    vec3 diffuse = diff * uLight.color;

    vec4 texColor = texture(baseColorTexture, vUV);

    FragColor = texColor * vec4(ambient + diffuse, 1.0);
}
