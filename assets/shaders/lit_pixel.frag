#version 330 core

// MRT output:
//   location 0 -> lit colour (flat ambient + diffuse, no rim/specular)
//   location 1 -> view-space normal, encoded as N*0.5+0.5 in RGB
//
// The "highlight" in the three.js-style pixel-art look comes from the
// outline post-pass brightening interior crease edges — not from a rim
// term in this shader. Keep the lit pass simple.

in vec3 vWorldPos;
in vec3 vViewNormal;
in vec3 vWorldNormal;
in vec2 vUV;
in vec3 vColor;

layout(location = 0) out vec4 oColor;
layout(location = 1) out vec4 oNormal;

uniform vec3 uCameraPos;

struct Light {
    vec3 color;
    vec3 position;
};
uniform Light uLight;

uniform float uAmbient;
uniform vec3  color;

void main()
{
    vec3 N = normalize(vWorldNormal);
    vec3 L = normalize(uLight.position - vWorldPos);

    float diffuse = max(dot(N, L), 0.0);
    vec3  lit     = color * (uAmbient + diffuse * uLight.color);

    oColor  = vec4(lit, 1.0);
    oNormal = vec4(normalize(vViewNormal) * 0.5 + 0.5, 1.0);
}
