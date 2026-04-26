# Compile / Link / Run {#build}

Procedure for building the engine, the sample game, and the docs terminal.

## Toolchain expected

| Tool        | Minimum     | Notes                                                         |
| ----------- | ----------- | ------------------------------------------------------------- |
| C++ compiler | C++17 capable | `gcc >= 9` / `clang >= 11` / MSVC 2019+. ccache is auto-detected. |
| CMake       | 3.10        | Anything in your distro is fine.                              |
| OpenGL      | 3.3 core    | GLFW + GLEW + GLM ship vendored under `engine/thirdparty/`.   |
| Doxygen     | 1.16        | Only needed for the `engine-docs` target.                     |
| Graphviz    | any         | Generates the call/include graphs Doxygen embeds.             |

@note On Linux you'll also need the GLFW system dependencies
(`libwayland-dev`, `libxkbcommon-dev`, `xorg-dev` ...). On macOS and
Windows GLFW builds out of the box.

## The convenience script

```sh
./compile.sh             # debug build into bin/debug/, then runs the game
./compile.sh --release   # release build into bin/release/, then runs it
./compile.sh --release -- some-arg  # arguments after `--` reach the game binary
```

The script:

1. Configures CMake into `bin/{debug,release}` with `Ninja` if available.
2. Builds the `monad-engine` target (which transitively builds the
   `Engine` static library plus every thirdparty dep).
3. Symlinks `compile_commands.json` to the repo root for clangd.
4. Launches the resulting binary.

## Direct CMake

For IDE integration or building only one target:

```sh
cmake -B bin/debug -S . -DCMAKE_BUILD_TYPE=Debug
cmake --build bin/debug                          # everything
cmake --build bin/debug --target Engine          # engine library only
cmake --build bin/debug --target monad-engine    # game executable
cmake --build bin/debug --target engine-docs     # this terminal (needs doxygen)
```

@attention `compile_commands.json` is exported at the repo root for
clangd / LSP integration. Re-run the CMake configure step whenever you
add new source files so clangd indexes them.

### Useful configure flags

| Flag                          | Effect                                                      |
| ----------------------------- | ----------------------------------------------------------- |
| `-DCMAKE_BUILD_TYPE=Debug`    | Includes debug info, disables optimisation.                 |
| `-DCMAKE_BUILD_TYPE=Release`  | `-O3` build.                                                |
| `-DMONAD_DOCS_ONLY=ON`        | Skips the engine/app build; configures only the docs target. CI uses this so it doesn't need GLFW system libs. |
| `-DBUILD_TESTING=OFF`         | Disables thirdparty self-tests (also a good idea on CI).    |

## Building the docs terminal

```sh
cmake -B bin/docs -S . -DMONAD_DOCS_ONLY=ON
cmake --build bin/docs --target engine-docs
```

HTML output appears at `bin/docs/docs/html/index.html`. Open it directly
or serve the folder over a local HTTP server if you want absolute-path
links to behave.

## Continuous integration

The `.github/workflows/docs.yml` workflow runs the same docs-only build
on every push to `master` and deploys the result to GitHub Pages. The
workflow installs `doxygen` and `graphviz`, configures CMake with
`-DMONAD_DOCS_ONLY=ON`, runs the `engine-docs` target, drops a
`.nojekyll`, and uploads the HTML directory as a Pages artifact.
