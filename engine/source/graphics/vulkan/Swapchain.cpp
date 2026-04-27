#include "graphics/vulkan/Swapchain.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <string>

#include <GLFW/glfw3.h>

#include "graphics/vulkan/Context.h"

namespace mnd::vk
{
namespace
{
void Check(VkResult result, const char *what)
{
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("vk swapchain: ") + what + " failed (VkResult=" +
                                 std::to_string(static_cast<int>(result)) + ")");
    }
}

VkSurfaceFormatKHR PickFormat(const std::vector<VkSurfaceFormatKHR> &available)
{
    for (const auto &f : available)
    {
        if (f.format == VK_FORMAT_B8G8R8A8_SRGB &&
            f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return f;
        }
    }
    return available.front();
}

VkPresentModeKHR PickPresentMode(const std::vector<VkPresentModeKHR> &available)
{
    for (auto m : available)
    {
        if (m == VK_PRESENT_MODE_MAILBOX_KHR) return m;
    }
    return VK_PRESENT_MODE_FIFO_KHR;  // guaranteed to be available
}

VkExtent2D PickExtent(const VkSurfaceCapabilitiesKHR &caps, GLFWwindow *window)
{
    if (caps.currentExtent.width != UINT32_MAX)
    {
        return caps.currentExtent;
    }
    int w = 0;
    int h = 0;
    glfwGetFramebufferSize(window, &w, &h);
    VkExtent2D actual = {static_cast<uint32_t>(w), static_cast<uint32_t>(h)};
    actual.width  = std::clamp(actual.width,  caps.minImageExtent.width,  caps.maxImageExtent.width);
    actual.height = std::clamp(actual.height, caps.minImageExtent.height, caps.maxImageExtent.height);
    return actual;
}
}  // namespace

Swapchain::Swapchain(Context &ctx, GLFWwindow *window) : m_ctx(ctx), m_window(window)
{
    Build();
}

Swapchain::~Swapchain()
{
    Cleanup();
}

void Swapchain::Recreate()
{
    // Block while the window is minimised — Vulkan rejects 0x0 swapchains.
    int w = 0;
    int h = 0;
    glfwGetFramebufferSize(m_window, &w, &h);
    while (w == 0 || h == 0)
    {
        glfwWaitEvents();
        glfwGetFramebufferSize(m_window, &w, &h);
    }
    vkDeviceWaitIdle(m_ctx.GetDevice());
    Cleanup();
    Build();
}

void Swapchain::Build()
{
    auto phys    = m_ctx.GetPhysicalDevice();
    auto surface = m_ctx.GetSurface();

    VkSurfaceCapabilitiesKHR caps;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phys, surface, &caps);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, surface, &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phys, surface, &formatCount, formats.data());

    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys, surface, &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phys, surface, &presentModeCount, presentModes.data());

    VkSurfaceFormatKHR fmt    = PickFormat(formats);
    VkPresentModeKHR   pm     = PickPresentMode(presentModes);
    VkExtent2D         extent = PickExtent(caps, m_window);

    uint32_t imageCount = caps.minImageCount + 1;
    if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
    {
        imageCount = caps.maxImageCount;
    }

    VkSwapchainCreateInfoKHR info{};
    info.sType            = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    info.surface          = surface;
    info.minImageCount    = imageCount;
    info.imageFormat      = fmt.format;
    info.imageColorSpace  = fmt.colorSpace;
    info.imageExtent      = extent;
    info.imageArrayLayers = 1;
    info.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    info.preTransform     = caps.currentTransform;
    info.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    info.presentMode      = pm;
    info.clipped          = VK_TRUE;
    info.oldSwapchain     = VK_NULL_HANDLE;

    auto fams = m_ctx.GetQueueFamilies();
    uint32_t indices[] = { fams.graphics, fams.present };
    if (fams.graphics != fams.present)
    {
        info.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
        info.queueFamilyIndexCount = 2;
        info.pQueueFamilyIndices   = indices;
    }
    else
    {
        info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    Check(vkCreateSwapchainKHR(m_ctx.GetDevice(), &info, nullptr, &m_swapchain),
          "vkCreateSwapchainKHR");

    m_format = fmt.format;
    m_extent = extent;

    uint32_t actualCount = 0;
    vkGetSwapchainImagesKHR(m_ctx.GetDevice(), m_swapchain, &actualCount, nullptr);
    m_images.resize(actualCount);
    vkGetSwapchainImagesKHR(m_ctx.GetDevice(), m_swapchain, &actualCount, m_images.data());

    // Per-image views.
    m_imageViews.resize(m_images.size());
    for (size_t i = 0; i < m_images.size(); ++i)
    {
        VkImageViewCreateInfo iv{};
        iv.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        iv.image                           = m_images[i];
        iv.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
        iv.format                          = m_format;
        iv.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        iv.subresourceRange.baseMipLevel   = 0;
        iv.subresourceRange.levelCount     = 1;
        iv.subresourceRange.baseArrayLayer = 0;
        iv.subresourceRange.layerCount     = 1;
        Check(vkCreateImageView(m_ctx.GetDevice(), &iv, nullptr, &m_imageViews[i]),
              "vkCreateImageView");
    }

    // Render pass — single colour attachment, clear-load, present-layout-store.
    VkAttachmentDescription colour{};
    colour.format         = m_format;
    colour.samples        = VK_SAMPLE_COUNT_1_BIT;
    colour.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colour.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    colour.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colour.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colour.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
    colour.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colourRef{};
    colourRef.attachment = 0;
    colourRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments    = &colourRef;

    VkSubpassDependency dep{};
    dep.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dep.dstSubpass    = 0;
    dep.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dep.srcAccessMask = 0;
    dep.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo rp{};
    rp.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    rp.attachmentCount = 1;
    rp.pAttachments    = &colour;
    rp.subpassCount    = 1;
    rp.pSubpasses      = &subpass;
    rp.dependencyCount = 1;
    rp.pDependencies   = &dep;
    Check(vkCreateRenderPass(m_ctx.GetDevice(), &rp, nullptr, &m_renderPass),
          "vkCreateRenderPass");

    // Framebuffer per swapchain image view.
    m_framebuffers.resize(m_imageViews.size());
    for (size_t i = 0; i < m_imageViews.size(); ++i)
    {
        VkFramebufferCreateInfo fbi{};
        fbi.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbi.renderPass      = m_renderPass;
        fbi.attachmentCount = 1;
        fbi.pAttachments    = &m_imageViews[i];
        fbi.width           = m_extent.width;
        fbi.height          = m_extent.height;
        fbi.layers          = 1;
        Check(vkCreateFramebuffer(m_ctx.GetDevice(), &fbi, nullptr, &m_framebuffers[i]),
              "vkCreateFramebuffer");
    }
}

void Swapchain::Cleanup()
{
    auto device = m_ctx.GetDevice();
    for (auto fb : m_framebuffers) vkDestroyFramebuffer(device, fb, nullptr);
    m_framebuffers.clear();
    if (m_renderPass) { vkDestroyRenderPass(device, m_renderPass, nullptr); m_renderPass = VK_NULL_HANDLE; }
    for (auto v : m_imageViews) vkDestroyImageView(device, v, nullptr);
    m_imageViews.clear();
    if (m_swapchain) { vkDestroySwapchainKHR(device, m_swapchain, nullptr); m_swapchain = VK_NULL_HANDLE; }
    m_images.clear();
}

}  // namespace mnd::vk
