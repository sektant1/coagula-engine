# Subsystem Map {#overview}

A whirlwind tour of the engine's modules.

## Subsystems

The `COA::Engine` singleton (see `engine/source/Engine.h`) holds one of each:

| Module          | Purpose                                            |
| --------------- | -------------------------------------------------- |
| `GraphicsAPI`   | OpenGL state, shaders, textures, vertex layouts.   |
| `RenderQueue`   | Higher-level draw abstraction over `graphics/`.    |
| `InputManager`  | GLFW keyboard & mouse.                             |
| `FileSystem`    | Asset path resolution against `ASSETS_DIR`.        |
| `Scene`         | Component-based scene graph.                       |

@note Every subsystem is reachable via `Engine::GetInstance().GetX()`.

## Including the engine

Game code includes a single umbrella header:

```cpp
#include <ENG.h>
```

That alone exposes the public API.
