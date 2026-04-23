@page recipes Recipes
@tableofcontents

# Recipes

Short, copy-paste answers to "how do I do X". Assumes the engine is already booted.

---

## Quit the application

```cpp
SetNeedsToBeClosed(true); // inside Application::Update
```

## Poll a key

```cpp
auto &in = COA::Engine::GetInstance().GetInputManager();
if (in.IsKeyPressed(GLFW_KEY_SPACE)) { /* ... */ }
```

## Read mouse position

```cpp
double x, y;
glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
```

## Build a procedural mesh

```cpp
auto data = COA::Builder::CreateRectangle(2.0f, 1.0f);
auto mesh = data.buildMesh(); // std::unique_ptr<Mesh>
```

Available factories: `CreateTriangle`, `CreateRectangle`, `CreateFullscreenQuad`, `CreateBox`.

## Submit a draw call

```cpp
auto &q = COA::Engine::GetInstance().GetRenderQueue();
q.Submit({ mesh.get(), material.get() });
```

## Set a shader uniform

```cpp
material->SetParam("iTime", elapsed);
material->SetParam("tint",  COA::vec3(1.0f, 0.5f, 0.2f));
```

Supported types: `f32`, `vec2`, `vec3`, `vec4`, `mat4`, textures.

## Load a material from JSON

```cpp
auto mat = COA::Material::Load("materials/brick.mat");
```

Schema: `{ shader: { vertex, fragment }, params: { textures: [{name, path}] } }`.

## Spawn a dynamic rigid body

```json
{
  "type": "PhysicsComponent",
  "collider": { "type": "box", "x": 1, "y": 1, "z": 1 },
  "body":     { "type": "dynamic", "mass": 5, "friction": 0.5 }
}
```

Static bodies: `mass = 0`, `type = static`.

## Add a light

```json
{ "type": "LightComponent", "color": { "r": 1, "g": 1, "b": 1 } }
```

Position comes from the owning GameObject.

## Switch the active scene

```cpp
COA::Scene::Load("scenes/level2.json");
```

## Log something

```cpp
LOG_INFO("hp=%d", hp);
LOG_WARN("shader missing uniform: %s", name);
LOG_ERROR("failed to load %s", path.c_str());
LOG_FATAL("unrecoverable"); // aborts
```

## Parent one GameObject under another

Place it in the `children` array of the parent in the scene JSON, or `parent->AddChild(std::move(obj))` in code.
