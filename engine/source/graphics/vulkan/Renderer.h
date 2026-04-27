/**
 * @file Renderer.h
 * @brief Per-frame Vulkan command-buffer recording + presentation.
 *
 * Skeleton renderer for the preview path. Records a single render pass that
 * clears the swapchain image to a configurable colour, submits to the
 * graphics queue, and presents. Two frames in flight (FRAMES_IN_FLIGHT = 2)
 * with the standard image-available / render-finished semaphore + in-flight
 * fence triple.
 */

#pragma once

#include <array>

#include <vulkan/vulkan.h>

namespace mnd::vk
{
class Context;
class Swapchain;

class Renderer
{
public:
    static constexpr uint32_t kFramesInFlight = 2;

    Renderer(Context &ctx, Swapchain &swap);
    ~Renderer();

    Renderer(const Renderer &)            = delete;
    Renderer &operator=(const Renderer &) = delete;

    /// Record + submit + present one frame. Returns false if the swapchain
    /// went out-of-date and the caller should rebuild it before calling again.
    bool DrawFrame(float clearR, float clearG, float clearB);

    /// Block until the GPU is idle. Call before destroying anything bound
    /// to the swapchain or device.
    void WaitIdle() const;

private:
    void CreateCommandPool();
    void AllocateCommandBuffers();
    void CreateSyncObjects();
    void RecordCommandBuffer(VkCommandBuffer cb, uint32_t imageIndex,
                             float r, float g, float b);

    Context   &m_ctx;
    Swapchain &m_swap;

    VkCommandPool                                            m_cmdPool = VK_NULL_HANDLE;
    std::array<VkCommandBuffer, kFramesInFlight>             m_cmdBuffers{};
    std::array<VkSemaphore, kFramesInFlight>                 m_imageAvailable{};
    std::array<VkSemaphore, kFramesInFlight>                 m_renderFinished{};
    std::array<VkFence, kFramesInFlight>                     m_inFlight{};

    uint32_t m_currentFrame = 0;
};

}  // namespace mnd::vk
