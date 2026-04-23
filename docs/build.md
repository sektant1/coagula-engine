@page build Build
@tableofcontents

# Build

## Quick start

```sh
./compile.sh            # configure + build + run (debug)
./compile.sh --release  # release variant
./compile.sh -r         # short form
```

Binary lands in `bin/<type>/coagula-engine` where `<type>` is `debug` or `release`.

## Why the script `cd`s to the repo root

Shaders, scenes, models, and textures are loaded with paths like
`assets/shaders/vertex.glsl` — relative to the current working directory.
`compile.sh` changes into the repo root before launching, so running the
binary from anywhere else will fail to find assets.

## Requirements

- C++17 compiler (gcc / clang).
- CMake ≥ 3.16.
- OpenGL 3.3 Core-capable GPU and driver.
- System X11 / Wayland headers (for GLFW).

GLFW, GLEW, Bullet, and glm are **vendored** under `engine/thirdparty/` and
built from source via `add_subdirectory` — no system install needed.

## CMake by hand

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
./bin/debug/coagula-engine
```

Two CMake projects live in the tree:

- `coagula-engine` (executable, `source/`)
- `Engine` (static library, `engine/source/`)

The executable links the library.

## Developer tooling

- `compile_commands.json` is emitted at the repo root and inside `engine/` — point `clangd` at either.
- `.clang-format` and `.clang-tidy` apply to the whole codebase.
- `engine-docs` Doxygen target — see @ref build_docs.

No tests, no linter target, no CI.
