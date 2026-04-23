# Docs Image Assets

Placeholder SVGs for the Coagula Engine docs. Theme: PS1 *MediEvil* — Gallowmere graveyard, Zarok's tower, Sir Dan's forge. Swap any file in place; paths are referenced from `custom.css`, `header.html`, and `homepage.md`.

| File | Slot | Reference | Recommended final |
|------|------|-----------|-------------------|
| `banner-placeholder.svg` | Title-bar backdrop (`#titlearea`) | Gallowmere graveyard at night — moon, gravestones, bare trees, fog | 1600×220 PNG or SVG |
| `sidebar-sigil.svg` | Sidebar top ornament | Skull-and-crossbones sigil with glowing green eye-sockets | 64×64 SVG |
| `scroll-divider.svg` | Section divider on mainpage + long pages | Ornamental line with central purple gem + four-point star | 800×32 SVG |
| `section-crypt.svg` | "Build & Run" section illustration | Stone crypt doorway, iron bands, glowing keyhole, wall torches | 480×180 |
| `section-tower.svg` | "API Overview" section illustration | Zarok's tower silhouette, purple lightning, green-lit windows | 480×180 |
| `section-forge.svg` | "Tutorials" section illustration | Sir Dan's sword on an anvil by a lit forge | 480×180 |
| `cursor-rune.svg` | Optional custom cursor (via CSS) | Small rune-pointer (arrow + lightning bolt) | 24×24 |

## Replacing a placeholder

1. Drop the new file at the same path with the same filename — or update the reference.
2. Keep roughly the same aspect ratio so layout doesn't shift.
3. Prefer SVG for crisp scaling; PNG is fine for painterly art.
4. Doxygen copies this whole `images/` directory to the generated site via `HTML_EXTRA_FILES` (see `Doxyfile.in`). `IMAGE_PATH` is also set so `@image html filename.svg` works from any markdown page.

## Licensing note

Placeholders above are original CC0 stand-ins. Do **not** commit copyrighted *MediEvil* assets (screenshots, logos, Sir Dan art) — use them as thematic reference only, not as content.
