@mainpage Coagula Engine

@htmlonly
<pre class="coa-ascii" aria-hidden="true">
   ______   ______    ___     ______   __  __   __       ___
  / ____/  / __  /   /   |   / ____/  / / / /  / /      /   |
 / /      / / / /   / /| |  / / __   / / / /  / /      / /| |
/ /___   / /_/ /   / ___ | / /_/ /  / /_/ /  / /___   / ___ |
\____/   \____/   /_/  |_| \____/   \____/  /_____/  /_/  |_|
</pre>
@endhtmlonly

> _Stoke the forge. Strike the iron. Ship the engine._
> Small C++17 game engine for learning.
> **OpenGL 3.3 · GLFW · GLEW · Bullet · glm**

Coagula is a tiny, component-based engine built as a teaching codebase:
small enough to read end-to-end, large enough to ship a playable
first-person sandbox with physics, glTF models, and JSON scenes.

@htmlonly<div class="coa-divider" aria-hidden="true"></div>@endhtmlonly

## Get going

@htmlonly
<pre class="coa-ascii" aria-hidden="true">
+--[ THE WORKSHOP ]-----------------------------------+
|  pick a bench, grab a tool, start hammering.        |
+-----------------------------------------------------+
</pre>
@endhtmlonly

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

@htmlonly<div class="coa-divider" aria-hidden="true"></div>@endhtmlonly

## One-minute tour

@htmlonly
<pre class="coa-ascii" aria-hidden="true">
+--[ ONE-MINUTE SETUP ]-------------------------------+
|  boot the engine in under twenty lines of C++.      |
+-----------------------------------------------------+
</pre>
@endhtmlonly

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

@htmlonly<div class="coa-divider" aria-hidden="true"></div>@endhtmlonly

## Features at a glance

@htmlonly
<pre class="coa-ascii" aria-hidden="true">
+--[ FEATURES ]---------------------------------------+
|  what the engine forges.                            |
+-----------------------------------------------------+
</pre>
@endhtmlonly

- **Singleton engine** with window, GL context, input, render queue, physics.
- **Command-queue renderer** — `Mesh` + `Material` paired at submit time.
- **Component scene graph** — `GameObject` tree, JSON-loadable.
- **Bullet physics** — static / dynamic / kinematic rigid bodies, capsule character controller.
- **glTF import** — mesh + skeletal animation, one call.
- **Shadertoy-style shader sandbox** — `iTime`, `iResolution`, `iMouse` uniforms wired for quick experiments.

@htmlonly<div class="coa-divider" aria-hidden="true"></div>@endhtmlonly

## Status

Educational. No warranty. See @ref license.
