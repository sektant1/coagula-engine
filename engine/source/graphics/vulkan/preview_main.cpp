/**
 * @file preview_main.cpp
 * @brief Standalone smoke-test for the Vulkan preview scaffold.
 *
 * Builds when MONAD_VULKAN_PREVIEW=ON. Not linked into the main game; this
 * executable exists so we can iterate on the Vulkan backend without touching
 * the working OpenGL render path.
 *
 * Run:  bin/debug/vulkan_preview
 * Quit: ESC, or close the window.
 */

#include <cmath>
#include <cstdio>
#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "graphics/vulkan/Context.h"
#include "graphics/vulkan/Renderer.h"
#include "graphics/vulkan/Swapchain.h"

namespace
{
bool g_framebufferResized = false;

void OnResize(GLFWwindow * /*win*/, int /*w*/, int /*h*/)
{
    g_framebufferResized = true;
}
}  // namespace

int main()
{
    if (!glfwInit())
    {
        fprintf(stderr, "glfwInit failed\n");
        return 1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow *win = glfwCreateWindow(1280, 720, "monad-engine vulkan preview", nullptr, nullptr);
    if (!win)
    {
        fprintf(stderr, "glfwCreateWindow failed (does the system support Vulkan?)\n");
        glfwTerminate();
        return 1;
    }
    glfwSetFramebufferSizeCallback(win, OnResize);

    try
    {
        // Validation on by default for the preview tool — it's the only way
        // to catch the kind of mistakes a scaffold accumulates.
        mnd::vk::Context   ctx(win, /*enableValidation=*/true);
        mnd::vk::Swapchain swap(ctx, win);
        mnd::vk::Renderer  renderer(ctx, swap);

        printf("[vk preview] device + swapchain online (%u images, %ux%u)\n",
               swap.ImageCount(), swap.GetExtent().width, swap.GetExtent().height);

        double t0 = glfwGetTime();
        while (!glfwWindowShouldClose(win))
        {
            glfwPollEvents();
            if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            {
                glfwSetWindowShouldClose(win, GLFW_TRUE);
            }

            // Animated clear colour so a successful frame is visually obvious.
            float t = static_cast<float>(glfwGetTime() - t0);
            float r = 0.5f + 0.5f * std::sin(t * 0.7f);
            float g = 0.5f + 0.5f * std::sin(t * 0.9f + 2.0f);
            float b = 0.5f + 0.5f * std::sin(t * 1.1f + 4.0f);

            bool ok = renderer.DrawFrame(r, g, b);
            if (!ok || g_framebufferResized)
            {
                swap.Recreate();
                g_framebufferResized = false;
            }
        }
        renderer.WaitIdle();
    }
    catch (const std::exception &ex)
    {
        fprintf(stderr, "[vk preview] fatal: %s\n", ex.what());
        glfwDestroyWindow(win);
        glfwTerminate();
        return 1;
    }

    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
