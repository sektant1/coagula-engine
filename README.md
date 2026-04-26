# Monad Engine

Small C++17 game engine for learning. OpenGL 3.3, GLFW, GLEW, Bullet, glm.

## Build

```sh
./compile.sh            # debug build + run
./compile.sh --release  # release build + run
```

Binary lands in `bin/<type>/monad-engine`. The script `cd`s to the repo root so shaders under `assets/` resolve.

CMake also writes `compile_commands.json` for `clangd`.

## Layout

```
source/            app entrypoint + sandbox (Game, Player, main.cpp)
engine/thirdparty/ vendored GLFW + GLEW
engine/source/     engine library (Engine, Scene, render, physics, input, io)
assets/
    shaders/       GLSL pairs (.vert + .frag)
    scenes/        JSON scene files
    models/        glTF models
    materials/     material JSON
    textures/
```

## Starting point

`source/main.cpp` creates an `ENG::Application` subclass, hands it to the `Engine` singleton, and runs the main loop. Swap the app by editing one line in `main.cpp`.

## Recipes

### New application

Subclass `COA::Application`, then point `source/main.cpp` at it.

```cpp
class MyApp : public COA::Application {
public:
    bool Init() override        { return true; }
    void RegisterTypes() override {}
    void Update(float dt) override {}
    void Destroy() override     {}
};
```

### Load a scene from JSON

Scenes live in `assets/scenes/*.json`. Load once in `Init()`.

```cpp
auto scene = COA::Scene::Load("scenes/scene.json");
COA::Engine::GetInstance().SetScene(scene.get());
m_scene = scene; // keep the shared_ptr alive
```

In `Update(dt)`, call `m_scene->Update(dt)`.

### Spawn an object + components

```cpp
auto *obj = scene->CreateObject("Crate");
obj->SetPosition(vec3(0, 1, -3));
obj->AddComponent(new COA::MeshComponent(material, mesh));
```

Child objects: `scene->CreateObject("Camera", parentObj);`.

### Custom component / object type

Use the `COMPONENT(T)` macro and register the type in `RegisterTypes()`.

```cpp
class HealthComponent : public COA::Component {
    COMPONENT(HealthComponent)
public:
    void Update(float dt) override {}
};

void MyApp::RegisterTypes() {
    HealthComponent::Register();
}
```

Same pattern for game-object subclasses (see `source/Player.h` — `GAMEOBJECT(T)` + `Player::Register()`).

### Read input

```cpp
auto &input = COA::Engine::GetInstance().GetInputManager();
if (input.IsKeyPressed(GLFW_KEY_W))     { /* forward */ }
if (input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT)) { /* fire */ }
```

Mouse position: `glfwGetCursorPos` on the current context.

### Lights

```cpp
auto *l = scene->CreateObject("Sun");
l->SetPosition(vec3(10, 20, 5));
auto *lc = new COA::LightComponent();
lc->SetColor(vec3(1.0f, 0.95f, 0.8f));   // warm white
l->AddComponent(lc);
```

Multiple `LightComponent`s are collected into the per-frame uniform array.

### Physics (rigid body)

Easiest path: declare it in the scene JSON — `PhysicsComponent` reads
`collider` + `body` blocks and registers itself with `PhysicsManager`.

```json
{
  "type": "PhysicsComponent",
  "collider": { "shape": "box", "halfExtents": [0.5, 0.5, 0.5] },
  "body":     { "mass": 1.0 }
}
```

In code:

```cpp
auto body = std::make_shared<COA::RigidBody>(/*...*/);
obj->AddComponent(new COA::PhysicsComponent(body));
```

The component copies the simulated transform back onto its GameObject each frame.

### glTF model

```cpp
auto *gltfRoot = COA::GameObject::LoadGLTF("models/scene.glb", scene);
gltfRoot->SetPosition(vec3(0, 0, -5));
```

Builds a GameObject hierarchy mirroring the file's node tree, with
`MeshComponent` + `AnimationComponent` attached where present.

### Quit cleanly

```cpp
if (input.IsKeyPressed(GLFW_KEY_ESCAPE)) SetNeedsToBeClosed(true);
```

## Docs

```sh
cmake --build bin/debug --target engine-docs
# open bin/debug/docs/html/index.html
```

## License

Educational. No warranty.
