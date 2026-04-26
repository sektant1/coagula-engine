#version 330 core

// Vertex-less fullscreen triangle. Drive with glDrawArrays(GL_TRIANGLES, 0, 3)
// and an empty VAO bound — same pattern as the BlitNearest helper.

out vec2 vUV;

void main()
{
    vec2 p = vec2((gl_VertexID == 1) ? 3.0 : -1.0,
                  (gl_VertexID == 2) ? 3.0 : -1.0);
    vUV = (p + 1.0) * 0.5;
    gl_Position = vec4(p, 0.0, 1.0);
}
