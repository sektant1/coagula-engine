# Operator Training {#tutorial}

A guided tour from "open a window" to "spawn a physics-enabled actor with
a 3D-positioned audio loop". Every snippet is copy-pasteable; later
lessons assume the previous ones were followed.

## Suggested order

1. @ref build — get a binary running.
2. @ref overview — understand the subsystems.
3. Walk the four lessons below.

## Prerequisites

- A working build (`./compile.sh`) — see @ref build.
- An `assets/` folder under the repo root (the engine resolves every
  relative path against this location at runtime).

---

## Lesson 1 — window, main loop, log

The smallest possible game using the engine. Subclass mnd::Application,
hand it to the singleton, and call mnd::Engine::Run().

```cpp
#include <Monad.h>

class HelloGame : public mnd::Application
{
public:
    bool Init() override
    {
        LOG_INFO("HelloGame booting");
        return true;
    }

    void RegisterTypes() override {}

    void Update(float dt) override
    {
        if (mnd::Engine::GetInstance().GetInput().IsKeyDown(mnd::Key::Escape))
            RequestClose();
    }

    void Destroy() override { LOG_INFO("HelloGame shutting down"); }
};

int main()
{
    auto &engine = mnd::Engine::GetInstance();
    engine.SetApplication(new HelloGame());
    engine.Init(1280, 720);
    engine.Run();
    engine.Destroy();
    return 0;
}
```

What you get for free:
- A 1280x720 GLFW window with an OpenGL 3.3 core context.
- The Editor overlay (toggle with `F1`).
- The mnd::Log macros writing colour-coded output to the terminal.

---

## Lesson 2 — a textured cube on screen

Move past the cleared screen by submitting a draw command per frame.

```cpp
bool HelloGame::Init() override
{
    auto &gfx     = mnd::Engine::GetInstance().GetGraphics();
    auto &textures = mnd::Engine::GetInstance().GetTextures();

    // 1. Build a cube on the GPU.
    m_mesh = std::make_unique<mnd::Mesh>();
    m_mesh->Upload(mnd::Builder::Cube(1.0F));   // 1m cube

    // 2. Compile a shader and wrap it in a material.
    m_shader = std::make_shared<mnd::ShaderProgram>();
    m_shader->LoadFromFiles("shaders/lit.vert", "shaders/lit.frag");
    m_material = std::make_shared<mnd::Material>(m_shader);
    m_material->SetTexture("uAlbedo", textures.Load("textures/crate.png"));

    return true;
}

void HelloGame::Update(float dt) override
{
    auto &queue = mnd::Engine::GetInstance().GetRenderQueue();

    // Push one command per visible mesh; the queue drains in Engine::Run.
    queue.Submit({
        .mesh      = m_mesh.get(),
        .material  = m_material.get(),
        .transform = glm::translate(glm::mat4(1.0F), {0.0F, 0.0F, -3.0F}),
    });
}
```

Asset paths are interpreted by mnd::FileSystem against the `ASSETS_DIR`
that CMake bakes in at configure time, so `"textures/crate.png"` resolves
to `assets/textures/crate.png` inside the repo.

---

## Lesson 3 — Scene, GameObjects, Components

Doing everything from `Update()` works for one mesh; for a real world
you want a tree of objects with attached behaviours. That's
mnd::Scene + mnd::GameObject + mnd::Component.

```cpp
bool HelloGame::Init() override
{
    auto &scene = mnd::Engine::GetInstance().GetScene();

    // Camera ----------------------------------------------------------------
    auto *cam = scene.CreateGameObject("MainCamera");
    cam->SetPosition({0.0F, 1.6F, 4.0F});
    cam->AddComponent<mnd::CameraComponent>();
    cam->AddComponent<mnd::AudioListenerComponent>();

    // One point light -------------------------------------------------------
    auto *light = scene.CreateGameObject("Sun");
    light->SetPosition({2.0F, 4.0F, 2.0F});
    auto *lightCmp = light->AddComponent<mnd::LightComponent>();
    lightCmp->SetColor({1.0F, 0.9F, 0.7F});

    // The cube as a GameObject ---------------------------------------------
    auto *cube = scene.CreateGameObject("Crate");
    cube->SetPosition({0.0F, 0.5F, 0.0F});
    auto *mc = cube->AddComponent<mnd::MeshComponent>();
    mc->SetMesh(mnd::Builder::Cube(1.0F));
    mc->SetMaterial(m_material);

    return true;
}
```

`Update()` no longer touches the render queue — mnd::MeshComponent does it
for you. To make the cube spin, write your own component:

```cpp
class SpinComponent : public mnd::Component
{
    COMPONENT(SpinComponent)
public:
    void Update(float dt) override
    {
        auto t = GetOwner()->GetRotation();
        t = glm::rotate(t, dt, glm::vec3(0.0F, 1.0F, 0.0F));
        GetOwner()->SetRotation(t);
    }
};

// In Application::RegisterTypes:
void HelloGame::RegisterTypes() override
{
    mnd::Component::Register<SpinComponent>();
}

// In Init, after the cube is built:
cube->AddComponent<SpinComponent>();
```

The `COMPONENT(...)` macro generates the type-id glue mnd::Scene needs
to deserialize and inspect components in the Editor.

---

## Lesson 4 — physics, audio, input

Round things off with rigid bodies, sound, and a player controller.

```cpp
// One falling crate -----------------------------------------------------
auto *crate = scene.CreateGameObject("Crate");
crate->SetPosition({0.0F, 5.0F, 0.0F});
crate->AddComponent<mnd::MeshComponent>()->SetMesh(mnd::Builder::Cube(1.0F));

auto *body = crate->AddComponent<mnd::PhysicsComponent>();
body->SetCollider(mnd::Collider::Box({0.5F, 0.5F, 0.5F}));
body->SetMass(2.0F);

// A 3D-positioned looping ambience ------------------------------------
auto hum = mnd::Audio::Load("sfx/server-room.ogg");
auto *audio = crate->AddComponent<mnd::AudioComponent>();
audio->RegisterAudio("hum", hum);
audio->Play("hum", true);

// Replace the camera with a first-person walker -----------------------
cam->AddComponent<mnd::PlayerControllerComponent>();
```

The mnd::PhysicsComponent attaches a Bullet rigid body to the GameObject
and keeps the transform synchronised every frame. mnd::AudioComponent
forwards the GameObject's world position to every clip, so the hum pans
correctly as you walk around. mnd::PlayerControllerComponent reads
mnd::InputManager and produces walk + look behaviour with mouse-look
clamped to ±89°.

---

## Where to next

- Skim the topic tree (the **Topics** menu in the navbar) to see every
  group at a glance.
- Open `engine/source/Monad.h` — the umbrella header lists every public
  type with a one-line description.
- The `assets/` directory in the repo has reference shaders, textures,
  and a few `.scene.json` files you can load via mnd::Scene::LoadFromFile().
