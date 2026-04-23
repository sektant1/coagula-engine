@mainpage Coagula Engine

> Small C++17 game engine for learning.
> **OpenGL 3.3 · GLFW · GLEW · Bullet · glm**

Coagula is a tiny, component-based engine built as a teaching codebase:
small enough to read end-to-end, large enough to ship a playable
first-person sandbox with physics, glTF models, and JSON scenes.

---

## Get going

| If you want to...                    | Go to                        |
|--------------------------------------|------------------------------|
| Compile and run                      | @ref build                   |
| Know what lives where                | @ref layout                  |
| Understand the entrypoint            | @ref starting_point          |
| Follow a step-by-step walk           | @ref tutorials               |
| Copy-paste a quick snippet           | @ref recipes                 |
| Read a longer explanation            | @ref guides                  |
| Regenerate this site                 | @ref build_docs              |
| Check the license                    | @ref license                 |
| Browse the API by subsystem          | @ref overview                |

## One-minute tour

```cpp
#include "COA.h"

class Game : public COA::Application {
public:
    bool Init() override {
        m_scene = COA::Scene::Load("scenes/scene.json");
        COA::Engine::GetInstance().SetScene(m_scene.get());
        return true;
    }
    void RegisterTypes() override {}
    void Update(float dt) override { m_scene->Update(dt); }
    void Destroy() override {}
private:
    std::shared_ptr<COA::Scene> m_scene;
};

int main() {
    auto &e = COA::Engine::GetInstance();
    e.SetApplication(new Game());
    e.Init(1280, 720);
    e.Run();
    e.Destroy();
}
```

Then:

```sh
./compile.sh
```

That is the whole boot sequence. Everything else — rendering, physics,
input, scene graph — is driven by the `Scene` and its `Component`s.

## Features at a glance

- **Singleton engine** with window, GL context, input, render queue, physics.
- **Command-queue renderer** — `Mesh` + `Material` paired at submit time.
- **Component scene graph** — `GameObject` tree, JSON-loadable.
- **Bullet physics** — static / dynamic / kinematic rigid bodies, capsule character controller.
- **glTF import** — mesh + skeletal animation, one call.
- **Shadertoy-style shader sandbox** — `iTime`, `iResolution`, `iMouse` uniforms wired for quick experiments.

## Status

Educational. No warranty. See @ref license.
