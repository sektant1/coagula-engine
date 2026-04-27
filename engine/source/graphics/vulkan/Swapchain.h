/**
 * @file Swapchain.h
 * @brief VkSwapchain + per-image views, render pass, framebuffers.
 *
 * Held separately from Context so a window resize can rebuild the swapchain
 * without tearing down the whole device. Renderer calls Recreate() when it
 * sees VK_ERROR_OUT_OF_DATE_KHR or VK_SUBOPTIMAL_KHR from the present path.
 */

#pragma once

#include <vector>

#include <vulkan/vulkan.h>

struct GLFWwindow;

namespace mnd::vk
{
class Context;

class Swapchain
{
public:
    Swapchain(Context &ctx, GLFWwindow *window);
    ~Swapchain();

    Swapchain(const Swapchain &)            = delete;
    Swapchain &operator=(const Swapchain &) = delete;

    /// Tear down + rebuild against the window's current size. Blocks on
    /// vkDeviceWaitIdle so call only between frames.
    void Recreate();

    [[nodiscard]] VkSwapchainKHR              Get()           const { return m_swapchain; }
    [[nodiscard]] VkRenderPass                GetRenderPass() const { return m_renderPass; }
    [[nodiscard]] VkExtent2D                  GetExtent()     const { return m_extent; }
    [[nodiscard]] VkFormat                    GetFormat()     const { return m_format; }
    [[nodiscard]] const std::vector<VkFramebuffer> &GetFramebuffers() const { return m_framebuffers; }
    [[nodiscard]] uint32_t                    ImageCount()    const { return static_cast<uint32_t>(m_images.size()); }

private:
    void Build();
    void Cleanup();

    Context     &m_ctx;
    GLFWwindow  *m_window     = nullptr;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkRenderPass   m_renderPass = VK_NULL_HANDLE;
    VkFormat       m_format     = VK_FORMAT_UNDEFINED;
    VkExtent2D     m_extent     = {0, 0};
    std::vector<VkImage>       m_images;
    std::vector<VkImageView>   m_imageViews;
    std::vector<VkFramebuffer> m_framebuffers;
};

}  // namespace mnd::vk
