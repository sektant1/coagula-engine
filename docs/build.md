# Compile / Link / Run {#build}

Procedure for building the engine and its example executable.

## The build script

```sh
./compile.sh           # debug build into bin/debug/, then runs it
./compile.sh --release # release build into bin/release/
```

Extra arguments after the flag are forwarded to the produced binary.

## Direct CMake

For IDE integration or single-target builds:

```sh
cmake -B bin/debug -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build bin/debug
cmake --build bin/debug --target Engine        # engine library only
cmake --build bin/debug --target engine-docs   # this terminal (needs doxygen)
```

@attention `compile_commands.json` is exported at the repo root for
clangd / LSP integration. Re-run CMake configure if you add new sources.

## Building the docs terminal

```sh
cmake --build bin/debug --target engine-docs
```

The HTML output appears at `bin/debug/docs/html/index.html`.
