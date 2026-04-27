#pragma once

// Forward declarations for the few GL types that public engine headers expose
// in their member layouts (Texture, Mesh, ShaderProgram, VertexLayout).
//
// Pulling <GL/glew.h> into a header drags ~30k lines into every TU that
// includes it transitively. The OpenGL spec fixes these typedefs, so we can
// safely declare them here and keep the heavy GLEW include confined to .cpp
// files. C/C++ permit identical typedef redeclarations, so TUs that *do*
// include GLEW after this header still compile cleanly.
typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef int          GLint;
typedef int          GLsizei;
