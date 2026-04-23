@page recipes Recipes
@tableofcontents

# Recipes

Short, copy-paste answers to "how do I do X". Assumes the engine is booted
and you are inside an `Application` subclass. See @ref starting_point for
boot details.

---

## New application

Subclass `COA::Application`, then point `source/main.cpp` at it.

```cpp
class MyApp : public COA::Application {
public:
    bool Init() override          { return true; }
    void RegisterTypes() override {}
    void Update(float dt) override{}
    void Destroy() override       {}
};
```

## Quit cleanly

```cpp
if (input.IsKeyPressed(GLFW_KEY_ESCAPE)) SetNeedsToBeClosed(true);
```

## Read input

```cpp
auto &input = COA::Engine::GetInstance().GetInputManager();
if (input.IsKeyPressed(GLFW_KEY_W))                       { /* forward */ }
if (input.IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))   { /* fire */   }
```

Mouse position — poll GLFW directly:

```cpp
double x, y;
glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
```

## Load a scene from JSON

Scenes live in `assets/scenes/*.json`. Load once in `Init()`, keep the
`shared_ptr` alive, update each frame.

```cpp
m_scene = COA::Scene::Load("scenes/scene.json");
COA::Engine::GetInstance().SetScene(m_scene.get());
// ...
void Update(float dt) override { m_scene->Update(dt); }
```

## Create an object + attach components

```cpp
auto *obj = scene->CreateObject("Crate");
obj->SetPosition(vec3(0, 1, -3));
obj->AddComponent(new COA::MeshComponent(material, mesh));
```

Child objects pass the parent as second argument:

```cpp
auto *cam = scene->CreateObject("Camera", player);
```

## Custom component

Use the `COMPONENT(T)` macro and register the type in `RegisterTypes()` so
the JSON loader knows the name.

```cpp
class HealthComponent : public COA::Component {
    COMPONENT(HealthComponent)
public:
    int hp = 100;
    void Update(float dt) override {}
};

void MyApp::RegisterTypes() {
    HealthComponent::Register();
}
```

Same pattern for game-object subclasses — see `source/Player.h`
(`GAMEOBJECT(T)` + `Player::Register()`).

## Spawn a light

```cpp
auto *l  = scene->CreateObject("Sun");
l->SetPosition(vec3(10, 20, 5));
auto *lc = new COA::LightComponent();
lc->SetColor(vec3(1.0f, 0.95f, 0.8f));  // warm white
l->AddComponent(lc);
```

Multiple `LightComponent`s are collected into a per-frame uniform array —
the shader just reads `u_lights[i]`.

## Physics (rigid body)

Easiest path: declare it in the scene JSON. `PhysicsComponent` reads the
`collider` + `body` blocks and registers itself with `PhysicsManager`.

```json
{
  "type": "PhysicsComponent",
  "collider": { "shape": "box", "halfExtents": [0.5, 0.5, 0.5] },
  "body":     { "mass": 1.0 }
}
```

Static bodies: `"mass": 0`. The component copies the simulated transform
back onto its GameObject each frame.

In code:

```cpp
auto body = std::make_shared<COA::RigidBody>(/*...*/);
obj->AddComponent(new COA::PhysicsComponent(body));
```

## Load a glTF model

```cpp
auto *root = COA::GameObject::LoadGLTF("models/scene.glb", scene);
root->SetPosition(vec3(0, 0, -5));
```

Builds a GameObject hierarchy mirroring the file's node tree with
`MeshComponent` + `AnimationComponent` attached where present.

## Procedural mesh

```cpp
auto data = COA::Builder::CreateRectangle(2.0f, 1.0f);
auto mesh = data.buildMesh();  // std::unique_ptr<Mesh>
```

Available factories: `CreateTriangle`, `CreateRectangle`, `CreateBox`,
`CreateFullscreenQuad`.

## Set a shader uniform

```cpp
material->SetParam("iTime", elapsed);
material->SetParam("tint",  vec3(1.0f, 0.5f, 0.2f));
```

Supported types: `f32`, `vec2`, `vec3`, `vec4`, `mat4`, textures.

## Log something

```cpp
LOG_INFO ("hp=%d", hp);
LOG_WARN ("shader missing uniform: %s", name);
LOG_ERROR("failed to load %s", path.c_str());
LOG_FATAL("unrecoverable");   // aborts
```
