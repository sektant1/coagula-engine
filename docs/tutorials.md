@page tutorials Tutorials
@tableofcontents

# Tutorials

Step-by-step walks through building things end-to-end. Start here if new to the engine.

---

## 1. Hello Window

Goal: boot the engine, show a blank window, quit on `ESC`.

1. Subclass `COA::Application` (see `source/Lab.h` / `source/Game.h`).
2. Implement `Init`, `Update`, `Destroy`.
3. In `source/main.cpp`, swap the `new Lab()` for your class.

```cpp
#include "COA.h"

class Hello : public COA::Application {
public:
    void Init() override {}
    void Update(float dt) override {
        auto &in = COA::Engine::GetInstance().GetInputManager();
        if (in.IsKeyPressed(GLFW_KEY_ESCAPE)) SetNeedsToBeClosed(true);
    }
    void Destroy() override {}
};

int main() {
    auto &e = COA::Engine::GetInstance();
    e.SetApplication(new Hello());
    e.Init(1280, 720);
    e.Run();
    e.Destroy();
}
```

Build and run with `./compile.sh`.

---

## 2. First Triangle

Goal: draw a procedural mesh with a material.

1. `COA::Builder::CreateTriangle()` → `MeshData`.
2. `meshData.buildMesh()` → `std::unique_ptr<Mesh>`.
3. Load a `Material` from `.mat` JSON or build one in-code with a shader pair under `assets/shaders/`.
4. Each frame submit `RenderCommand{ mesh.get(), material.get() }` to `COA::Engine::GetInstance().GetRenderQueue()`.

Shaders are resolved relative to the binary cwd (the repo root — `compile.sh` handles this).

---

## 3. Load a Scene

Goal: replace in-code scene setup with JSON.

1. Place scene file under `assets/scenes/` (see `assets/scenes/scene.json`).
2. `COA::Scene::Load("scenes/myscene.json")` in `Application::Init`.
3. Ensure the scene declares a `camera` field that matches a `GameObject` with a `CameraComponent`.

Supported object types: plain (transform + components), `Player`, `gltf` (set `path` to glTF file). Supported built-in components listed in @ref coa_components.

---

## 4. Add Physics to an Object

Goal: make a box fall onto a static ground.

1. Give the ground a `MeshComponent` + `PhysicsComponent` with `body.type = static` and `body.mass = 0`.
2. Give the falling box the same with `type = dynamic` and non-zero `mass`.
3. `PhysicsManager` steps Bullet before `Scene::Update`, so transforms read back the simulated pose.

See `assets/scenes/scene.json` for a minimal working example.

---

## 5. First-Person Player

Goal: WASD + mouse-look.

1. Create a `GameObject` with `CameraComponent` **and** `PlayerControllerComponent`.
2. Mark it as the active camera by setting the scene's top-level `camera` field to the object's name.
3. Parent the weapon / viewmodel as a child `gltf` object so it inherits the camera transform.
