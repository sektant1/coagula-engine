@page guides Guides
@tableofcontents

# Guides

Longer-form explanations of how subsystems fit together. Read after the @ref tutorials.

---

## Engine lifecycle

`COA::Engine` is a singleton. It owns `InputManager`, `GraphicsAPI`, `RenderQueue`, `PhysicsManager`, and the active `Scene`. All subsystems are reached via `Engine::GetInstance().GetXxx()`.

Boot order:

1. `SetApplication(app)` — singleton takes ownership.
2. `Init(w, h)` — GLFW window, GLEW, then `app->Init()`.
3. `Run()` — main loop until `glfwWindowShouldClose` or `app->NeedsToBeClosed()`.
4. `Destroy()` — tears down subsystems in reverse order.

One frame:

```
poll events
  → InputManager updated (GLFW callbacks)
PhysicsManager::Update(dt)            // Bullet step
Application::Update(dt)
  → Scene::Update(dt)
     → GameObject::Update → Component::Update
     → MeshComponent submits RenderCommand
Engine resolves main camera + lights
RenderQueue::Draw                     // flush to GraphicsAPI
glfwSwapBuffers
```

---

## Scene graph

`Scene` owns a tree of `GameObject`s. Each `GameObject` has a transform and a list of `Component`s. Components are the unit of behaviour; nothing runs unless a component does it.

- Load from JSON: `Scene::Load(path)`.
- Build in code: `scene.AddObject(std::make_unique<GameObject>(...))`.
- Attach behaviour: `obj->AddComponent<MyComponent>(args...)`.

The active scene's `camera` field names the `GameObject` used for view/projection each frame. Exactly one must carry a `CameraComponent`.

---

## Rendering model

Command-queue, not retained-mode.

- `Mesh` owns GPU buffers (VBO/IBO) + a `VertexLayout`.
- `Material` owns a `ShaderProgram` and a map of uniform parameters.
- `RenderCommand { Mesh*, Material* }` pairs them only at submit time — one mesh can be drawn with many materials and vice versa.

`RenderQueue::Draw`:

1. Sort / iterate commands.
2. Bind material → upload uniforms (including engine-provided camera + lights).
3. Bind mesh → issue draw call via `GraphicsAPI`.

Write a new shader pair under `assets/shaders/` and point a `.mat` file at it.

---

## Writing a component

```cpp
class Spinner : public COA::Component {
public:
    float speed = 90.0f; // deg / sec
    void Update(float dt) override {
        auto &t = GetOwner()->GetTransform();
        t.rotation *= COA::angleAxis(COA::radians(speed * dt),
                                     COA::vec3(0, 1, 0));
    }
};
```

Attach: `obj->AddComponent<Spinner>();`. If you want it loadable from JSON, register it with the scene's component factory (see existing components in `engine/source/scene/components/`).

---

## Physics

`PhysicsManager` wraps Bullet's `btDiscreteDynamicsWorld`. Steps once per frame with the engine's `dt`.

- `RigidBody` types: `static` (mass 0, immovable), `dynamic` (mass > 0, simulated), `kinematic` (moved by game code, pushes dynamics).
- `Collider` shapes: `box`, `sphere`, `capsule`. Dimensions live in the same JSON node.
- `KinematicCharacterController` drives a capsule with `walkDirection` + `jump`; use it through `PlayerControllerComponent`.

`PhysicsComponent` is the glue: it reads the GameObject's transform on spawn, then writes it back each frame from the simulated pose.

---

## Assets and paths

Paths are relative to the repo root because the binary is launched from there by `compile.sh`. Running the binary from another cwd will fail to find shaders / models / textures.

Asset layout:

- `assets/shaders/*.{vert,frag,glsl}` — GLSL source.
- `assets/materials/*.mat` — JSON material.
- `assets/models/...` — glTF + textures.
- `assets/textures/...` — loose textures.
- `assets/scenes/*.json` — scene definitions.

---

## Debugging tips

- `compile_commands.json` is emitted at the root and in `engine/` — point `clangd` at either.
- Toggle `--release` on `compile.sh` to rule out debug-only asserts.
- Validation of GL state lives in `GraphicsAPI`; enable it when chasing a black screen.
- `LOG_FATAL` aborts — use `LOG_ERROR` for recoverable errors.
