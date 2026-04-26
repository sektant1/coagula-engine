# Subsystem Map {#overview}

A whirlwind tour of every module the engine ships with. Each row links to
the canonical entry point — click through for the full API reference.

## Module hierarchy

```
                    +-----------------+
                    |  mnd::Engine    |    singleton, owns everything
                    +--------+--------+
                             |
   +----------+----------+---+----+---------+----------+----------+
   |          |          |        |         |          |          |
GraphicsAPI  Input    Physics    Audio   FileSystem  Scene     Editor
(GL)         (GLFW)   (Bullet)   (mini)  (assets)    (graph)   (ImGui)
                                                        |
                                                  Components
                                                  (Mesh, Camera,
                                                   Light, Player, ...)
```

## Subsystems

The mnd::Engine singleton (see `engine/source/Engine.h`) holds one of each
subsystem. Reach any of them from anywhere in the codebase via
`mnd::Engine::GetInstance().GetX()`.

| Module                | Header                       | Purpose                                                                 |
| --------------------- | ---------------------------- | ----------------------------------------------------------------------- |
| mnd::GraphicsAPI      | `graphics/GraphicsAPI.h`     | Thin OpenGL wrapper — buffers, shaders, draw calls.                     |
| mnd::ShaderProgram    | `graphics/ShaderProgram.h`   | GLSL compile/link + uniform upload.                                     |
| mnd::Texture          | `graphics/Texture.h`         | 2D textures with a TextureManager cache.                                |
| mnd::RenderTarget     | `graphics/RenderTarget.h`    | Offscreen FBO for low-res internal rendering.                           |
| mnd::RenderSettings   | `graphics/RenderSettings.h`  | Renderer tuning (internal-resolution, fog, dithering).                  |
| mnd::Mesh / mnd::Material | `render/Mesh.h`, `render/Material.h` | GPU mesh and shader+uniform bundle.                            |
| mnd::Builder          | `render/Builder.h`           | MeshData factories (cube, plane, sphere ...).                           |
| mnd::RenderQueue      | `render/RenderQueue.h`       | Per-frame draw command queue.                                           |
| mnd::InputManager     | `input/InputManager.h`       | Keyboard / mouse polling.                                               |
| mnd::Key              | `input/Key.h`                | GLFW-compatible key/button enum.                                        |
| mnd::FileSystem       | `io/FileSystem.h`            | Asset path resolution against `ASSETS_DIR`.                             |
| mnd::FileReader       | `io/FileReader.h`            | "Read whole file to string" helper.                                     |
| mnd::Scene            | `scene/Scene.h`              | Owns the GameObject list, camera, lights.                               |
| mnd::GameObject       | `scene/GameObject.h`         | Scene node with transform + components.                                 |
| mnd::Component        | `scene/Component.h`          | Base class + `COMPONENT()` macro for behaviours.                        |
| mnd::PhysicsManager   | `physics/PhysicsManager.h`   | Bullet world singleton + per-frame step.                                |
| mnd::RigidBody        | `physics/RigidBody.h`        | Wrapper around `btRigidBody`.                                           |
| mnd::Collider         | `physics/Collider.h`         | Collision shape factories.                                              |
| mnd::KinematicCharacterController | `physics/KinematicCharacterController.h` | FPS-style walker.                                  |
| mnd::AudioManager     | `audio/AudioManager.h`       | miniaudio engine + 3D listener.                                         |
| mnd::Audio            | `audio/Audio.h`              | Single playable audio clip.                                             |
| mnd::Editor           | `editor/Editor.h`            | ImGui in-game editor overlay.                                           |
| mnd::Log              | `Log.h`                      | `LOG_INFO` / `LOG_WARN` / `LOG_ERROR` / `LOG_FATAL` macros.             |
| Constants             | `Constants.h`                | Engine-wide compile-time tunables.                                      |
| Types                 | `Types.h`                    | `u8` / `f32` / `vec3` / `mat4` aliases.                                 |

@note Every subsystem accessor takes the form `GetX()` — e.g.
`engine.GetInput()`, `engine.GetScene()`, `engine.GetPhysics()`. There is
no service locator beyond the singleton itself.

## Including the engine

Game code includes a single umbrella header:

```cpp
#include <Monad.h>
```

That alone exposes the public API of every subsystem listed above. See
@ref tutorial for a worked example, and @ref build for the build steps.

## Frame lifecycle

`mnd::Engine::Run()` drives the main loop. One iteration looks like:

1. `glfwPollEvents()` — GLFW dispatches input callbacks into mnd::InputManager.
2. Compute `deltaTime` from a `std::chrono::steady_clock` timestamp.
3. `physics.Step(deltaTime)` — Bullet integrates rigid bodies.
4. `app->Update(deltaTime)` — your game logic runs here.
5. `scene.Update(deltaTime)` — calls `Update()` on every `Component`.
6. The Scene supplies a mnd::CameraData and a vector of mnd::LightData to mnd::RenderQueue.
7. `renderQueue.Draw(...)` issues GL calls for every submitted command.
8. `editor.Draw()` overlays the ImGui panels.
9. `glfwSwapBuffers()` presents the frame.
