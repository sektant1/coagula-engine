/**
 * @file RenderSettings.h
 * @ingroup mnd_graphics
 * @brief Per-frame renderer tuning knobs (PSX-style pixelation, fog, ambient).
 *
 * One instance lives on mnd::Engine and is forwarded to the
 * mnd::RenderQueue and PostFX shader as uniforms. Tweak it from the
 * Editor's "Render" panel or imperatively at runtime to switch between
 * crisp full-resolution and a coarse internal-resolution pixelated look.
 */

#pragma once

#include "Constants.h"
#include "Types.h"

namespace mnd
{

/**
 * @ingroup mnd_graphics
 * @brief Renderer tuning struct passed to the post-process shader each frame.
 */
struct RenderSettings
{
    /// When true the scene renders to an internal-resolution FBO and is
    /// then nearest-blit upscaled to the window — gives a chunky, pixely look.
    bool useInternalRes = true;
    int  internalW      = kDefaultInternalWidth;   ///< Internal target width  in pixels.
    int  internalH      = kDefaultInternalHeight;  ///< Internal target height in pixels.

    /// RGBA value used by `glClear` at the start of each frame.
    vec4 clearColor = vec4(0.0F, 0.0F, 0.0F, 1.0F);

    f32  snapX          = 320.0F;          ///< Vertex snapping grid X (PS1 wobble).
    f32  snapY          = 240.0F;          ///< Vertex snapping grid Y.
    f32  fogStart       = 0.0F;            ///< Fog near distance (0 disables fog).
    f32  fogEnd         = 0.0F;            ///< Fog far distance.
    f32  ambient        = kDefaultAmbient; ///< Global ambient term added to lit pixels.
    vec3 lightDir       = vec3(0.0F, -1.0F, 0.0F); ///< Sun/directional light vector.
    f32  colorDepth     = 0.0F;  ///< levels per channel; <= 1 disables quantize
    f32  ditherStrength = 0.0F;  ///< Bayer dither amount applied after quantization.
};

}  // namespace mnd
