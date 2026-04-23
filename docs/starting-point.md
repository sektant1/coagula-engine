@page starting_point Starting Point
@tableofcontents

# Starting Point

Every Coagula program boots the same way. Read this before the @ref tutorials.

## The entrypoint

`source/main.cpp` is tiny on purpose:

```cpp
int main() {
    auto &engine = COA::Engine::GetInstance();
    engine.SetApplication(new Game());   // <-- swap this line
    engine.Init(1280, 720);
    engine.Run();
    engine.Destroy();
    return 0;
}
```

To run a different app, change the `new Game()` line. That is the only
place the concrete application type appears.

## `Application` — the four hooks

Every app subclasses `COA::Application` and fills in four methods:

```cpp
class Game : public COA::Application {
public:
    bool Init() override;          // load assets, build scene
    void RegisterTypes() override; // declare custom components / objects
    void Update(float dt) override;// per-frame logic
    void Destroy() override;       // release resources
};
```

- `Init` returns `false` to abort startup.
- `RegisterTypes` runs before scene JSON is parsed — this is where you call `MyComponent::Register()` and `MyObject::Register()` so the loader knows the names.
- `Update` is called every frame with the delta in seconds.
- `Destroy` runs on shutdown, before GLFW is torn down.

To request shutdown from anywhere inside the app:

```cpp
SetNeedsToBeClosed(true);
```

## What `Engine::Init` does

1. Initialize GLFW, create a window.
2. Initialize GLEW and query the OpenGL context.
3. Wire keyboard / mouse callbacks into the singleton `InputManager`.
4. Construct `GraphicsAPI`, `RenderQueue`, `PhysicsManager`.
5. Call `app->RegisterTypes()` then `app->Init()`.

## What `Engine::Run` does each frame

```cpp
// poll events              InputManager updated

// compute dt              (clamped)

PhysicsManager::Update(dt)
app->Update(dt)        //  usually Scene::Update(dt)

//clear color + depth

RenderQueue::Draw      //  flush commands to GraphicsAPI

//swap buffers
```

The loop exits when GLFW asks to close **or** `app->NeedsToBeClosed()`
returns true.

## Where the singleton is

```cpp
COA::Engine &e = COA::Engine::GetInstance();
e.GetInputManager();
e.GetRenderQueue();
e.GetGraphicsAPI();
e.GetPhysicsManager();
e.GetScene();           // currently-active scene, may be null
```

One engine, one window, one GL context. If you need more than that, this
is not the engine you are looking for.

## Next

- @ref build — how to compile and run.
- @ref tutorials — write your first app.
- @ref recipes — copy-paste snippets for common tasks.
