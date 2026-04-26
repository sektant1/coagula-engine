# Monad Engine {#mainpage}

```
MONAD SYSTEMS — TERMINAL LINK PROTOCOL
MONAD-OS v0.1.4 — UNAUTHORISED ACCESS PROHIBITED
> CONNECTION ESTABLISHED
```

The **Monad Engine** is a C++17 game engine assembled
lesson by lesson. 

## Status Report

The engine ships as two linked artifacts:

- **`Engine`** — static library containing every subsystem.
- **`monad-engine`** — host executable that links the library.

Game code subclasses @ref mnd::Application and registers with
@ref mnd::Engine before entering the main loop.

## Available Modules

- @ref overview — **Subsystem Map** (architecture overview)
- @ref build — **Compile / Link / Run** (build instructions)
- @ref tutorial — **Operator Training** (lessons)

@note Toggle phosphor / amber via the colour switch in the upper-right.
