# Monad Engine {#mainpage}

```
ROBCO INDUSTRIES (TM) TERMLINK PROTOCOL
MONAD-OS v0.1.4 — UNAUTHORISED ACCESS PROHIBITED
> CONNECTION ESTABLISHED
```

Welcome, operator. The **Monad Engine** is a C++17 game engine assembled
lesson by lesson. This terminal serves as the operator's manual: every
subsystem, every entry point, every override is mapped here.

## Status Report

The engine ships as two linked artifacts:

- **`Engine`** — static library containing every subsystem.
- **`monad-engine`** — host executable that links the library.

Game code subclasses @ref COA::Application and registers with
@ref COA::Engine before entering the main loop.

## Available Modules

- @ref overview — **Subsystem Map** (architecture overview)
- @ref build — **Compile / Link / Run** (build instructions)
- @ref tutorial — **Operator Training** (lessons)

@note Toggle phosphor / amber via the colour switch in the upper-right.
