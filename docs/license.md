@page license License
@tableofcontents

# License

Coagula Engine is an **educational project**. It ships without warranty of
any kind, express or implied. Use it to learn, fork it, break it — but do
not rely on it for anything you cannot afford to lose.

## Engine code

All first-party code under `source/`, `engine/source/`, `assets/`,
`scripts/`, and `docs/` is provided for learning and experimentation. No
formal open-source license is attached yet; treat it as "source-available,
educational use".

If you want to reuse pieces in your own project, open an issue or reach
out — a proper permissive license (MIT / zlib) is the likely direction.

## Vendored third-party libraries

Each lives under `engine/thirdparty/` with its own upstream license. Those
licenses apply to their respective sources and **override** the note
above:

| Library | Location                          | License             |
|---------|-----------------------------------|---------------------|
| GLFW    | `engine/thirdparty/glfw-3.4/`     | zlib/libpng         |
| GLEW    | `engine/thirdparty/glew/`         | Modified BSD + MIT  |
| Bullet  | `engine/thirdparty/bullet/`       | zlib                |
| glm     | `engine/thirdparty/glm/`          | MIT (or Happy Bunny)|

Check the `LICENSE` / `COPYING` file inside each directory for the exact,
authoritative terms.

## Assets

Models and textures under `assets/models/` and `assets/textures/` may have
their own licenses from their original authors. Attribution lives next to
the asset where available. Do not redistribute commercially without
verifying.

## No warranty

> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
> OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
> MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
