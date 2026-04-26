/**
 * @file RenderTarget.h
 * @ingroup mnd_graphics
 * @brief Offscreen FBO with colour, view-normal and depth attachments.
 *
 * Used to implement the engine's "internal resolution" pixelation
 * effect. Render the scene at a small size (e.g. 320x240) into the
 * RenderTarget, then BlitNearest() it into the window framebuffer. The
 * non-filtered upscale produces crisp PSX-era chunky pixels.
 *
 * The second colour attachment receives view-space normals encoded as
 * `N*0.5+0.5` (RGBA8) so the outline post-pass can do depth+normal edge
 * detection. Depth is a sampleable texture rather than a renderbuffer
 * for the same reason.
 */

#pragma once

#include "GL/glew.h"

namespace mnd
{

/**
 * @ingroup mnd_graphics
 * @brief Offscreen FBO with MRT colour + sampleable depth for low-resolution rendering.
 *
 * Attachment layout matches lit_pixel.frag:
 *   COLOR_ATTACHMENT0 -> shaded RGBA8 colour
 *   COLOR_ATTACHMENT1 -> view-space normals (encoded N*0.5+0.5, RGBA8)
 *   DEPTH_ATTACHMENT  -> DEPTH_COMPONENT24 texture (sampleable)
 */
class RenderTarget
{
public:
    bool Create(int w, int h);
    void Resize(int w, int h);
    void Destroy();

    void        Bind();
    static void BindDefault(int winW, int winH);

    GLuint ColorTex()  const { return m_color; }
    GLuint NormalTex() const { return m_normal; }
    GLuint DepthTex()  const { return m_depth; }
    int    Width() const { return m_w; }
    int    Height() const { return m_h; }
    bool   IsValid() const { return m_fbo != 0; }

private:
    GLuint m_fbo    = 0;
    GLuint m_color  = 0;
    GLuint m_normal = 0;
    GLuint m_depth  = 0;
    int    m_w      = 0;
    int    m_h      = 0;
};

/// Blit modes for BlitNearest.
enum class BlitMode : int
{
    Color     = 0,  ///< Sample RGB straight through (default).
    SplatRed  = 1,  ///< Output (r,r,r,1) — used for visualising depth.
    DecodeNrm = 2,  ///< Output rgb as-is from a packed-normal texture (no remap).
};

/// Fullscreen nearest-neighbor blit of a 2D texture into the currently bound framebuffer.
/// Lazily compiles its own vertex-less shader on first call. Safe to call every frame.
void BlitNearest(GLuint srcTexture, int dstW, int dstH, BlitMode mode = BlitMode::Color);

}  // namespace mnd
