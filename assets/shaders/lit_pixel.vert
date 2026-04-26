#version 330 core

// Pixel-art lit shader: writes lit colour to attachment 0 and view-space
// normals to attachment 1 so the outline post-pass can do depth+normal
// edge detection. Vertex layout matches the engine's canonical slots
// (see VertexLayout.h): 0=pos, 1=color, 2=uv, 3=normal.

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec3 aNormal;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 vWorldPos;
out vec3 vViewNormal;
out vec3 vWorldNormal;
out vec2 vUV;
out vec3 vColor;

void main()
{
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    vWorldPos = worldPos.xyz;

    // Cheap normal transform — assumes uniform scale. Swap for an explicit
    // normal matrix uniform if non-uniform scale starts showing artefacts.
    mat3 normalMat = mat3(uModel);
    vec3 worldNormal = normalize(normalMat * aNormal);
    vWorldNormal = worldNormal;
    vViewNormal  = normalize(mat3(uView) * worldNormal);

    vUV    = aTexCoord;
    vColor = aColor;

    gl_Position = uProjection * uView * worldPos;
}
