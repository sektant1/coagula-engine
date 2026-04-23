@page overview Engine Overview
@tableofcontents

# Coagula Engine

Small educational C++17 game engine. OpenGL 3.3 Core, GLFW, GLEW, Bullet, glm.

## Where to go

| If you want to...                        | Start here                              |
|------------------------------------------|-----------------------------------------|
| Understand startup / main loop           | @ref coa_core                           |
| Write a new Component                    | @ref coa_components, COA::Component     |
| Load a scene from JSON                   | COA::Scene::Load                        |
| Draw geometry                            | @ref coa_render, COA::RenderQueue       |
| Add a rigid body / collider              | @ref coa_physics                        |
| Read keyboard / mouse                    | @ref coa_input, COA::InputManager       |
| Look up a type alias (`vec3`, `f32`, …)  | @ref coa_types                          |

## Project pages

- @ref build — how to compile and run.
- @ref layout — where each folder lives and what it holds.
- @ref starting_point — the entrypoint and the `Application` hooks.
- @ref build_docs — how this site is generated.
- @ref license — terms for engine code, vendored libs, assets.

## Learn by task

- @ref tutorials — step-by-step walks: hello window, first triangle, scenes, physics, FPS player.
- @ref recipes — short copy-paste snippets for common tasks.
- @ref guides — longer explanations of lifecycle, rendering, scene graph, physics.

## Subsystems

- @ref coa_core — `Engine`, `Application`, main loop.
- @ref coa_render — `Mesh`, `Material`, `Builder`, `RenderQueue`.
- @ref coa_graphics — GL wrappers: shaders, textures, vertex layout.
- @ref coa_scene — `Scene`, `GameObject`, `Component`.
- @ref coa_components — built-in components (camera, light, mesh, physics, …).
- @ref coa_physics — Bullet world, colliders, rigid bodies, character controller.
- @ref coa_input — keyboard + mouse state.
- @ref coa_io — asset paths and file loading.
- @ref coa_types — type aliases, logging macros, shared plain-data structs.

## One-frame data flow

1. `glfwPollEvents` → `InputManager` updated via callbacks.
2. `PhysicsManager::Update(dt)` steps Bullet.
3. `Application::Update(dt)` → `Scene::Update(dt)` → each `GameObject::Update` → each `Component::Update`.
4. `MeshComponent` submits `RenderCommand`s to `RenderQueue`.
5. `Engine` resolves main camera + lights, `RenderQueue::Draw` flushes to `GraphicsAPI`.
6. `glfwSwapBuffers`.
