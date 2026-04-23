@page layout Repository Layout
@tableofcontents

# Repository Layout

```
source/            executable entrypoint + sandbox apps
engine/source/     engine static library
engine/thirdparty/ vendored GLFW, GLEW, Bullet, glm
assets/
    shaders/       GLSL pairs (.vert + .frag)
    scenes/        JSON scene files
    models/        glTF models and their textures
    materials/     .mat JSON — shader + uniform bindings
    textures/      loose image files
docs/              handwritten .md pages + Doxygen output (generated/)
scripts/           build helpers sourced by compile.sh
lessons/           archived lesson snapshots — not built
```

## `source/` — the executable

- `main.cpp` — constructs an `Application`, hands it to the `Engine` singleton, runs the main loop.
- `Game.h` / `Game.cpp` — the current sandbox app. Swap which app runs by changing one `new Xxx()` line in `main.cpp`.
- `Player.h` / `Player.cpp` — example custom `GameObject` subclass using `GAMEOBJECT(T)`.

## `engine/source/` — the library

Organized by subsystem, one folder each:

| Folder        | What's inside                                               |
|---------------|-------------------------------------------------------------|
| `graphics/`   | GL wrappers — `ShaderProgram`, `Texture`, `VertexLayout`.   |
| `render/`     | `Mesh`, `Material`, `Builder`, `RenderQueue`.               |
| `scene/`      | `Scene`, `GameObject`, `Component`, built-in components.    |
| `physics/`    | Bullet world, colliders, rigid bodies, character controller.|
| `input/`      | `InputManager` — keyboard + mouse state.                    |
| `io/`         | Asset-path resolution, file readers.                        |
| *root*        | `Engine`, `Application`, `Log`, `Types`, `Common`, `COA.h`. |

Include `COA.h` from app code — it pulls every public engine header.

## `assets/` — runtime data

Resolved relative to the binary's cwd (the repo root; see @ref build).

- Shaders: pair a `.vert` and `.frag` under `assets/shaders/`.
- Materials: `.mat` JSON names the shader pair and any texture uniforms.
- Scenes: `.json` declares a tree of `GameObject`s with components.
- Models: glTF / GLB plus sibling textures.

## `engine/thirdparty/` — vendored libs

Built from source via `add_subdirectory`. Do not edit — patches belong in the
engine library, not upstream code.

## `build/` — CMake output

`compile.sh` writes here. Safe to delete — the script reconfigures.
