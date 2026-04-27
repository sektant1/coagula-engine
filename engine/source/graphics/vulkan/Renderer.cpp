#include "graphics/vulkan/Renderer.h"

#include <stdexcept>
#include <string>

#include "graphics/vulkan/Context.h"
#include "graphics/vulkan/Swapchain.h"

namespace mnd::vk
{
namespace
{
void Check(VkResult result, const char *what)
{
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(std::string("vk renderer: ") + what + " failed (VkResult=" +
                                 std::to_string(static_cast<int>(result)) + ")");
    }
}
}  // namespace

Renderer::Renderer(Context &ctx, Swapchain &swap) : m_ctx(ctx), m_swap(swap)
{
    CreateCommandPool();
    AllocateCommandBuffers();
    CreateSyncObjects();
}

Renderer::~Renderer()
{
    auto device = m_ctx.GetDevice();
    for (uint32_t i = 0; i < kFramesInFlight; ++i)
    {
        if (m_imageAvailable[i]) vkDestroySemaphore(device, m_imageAvailable[i], nullptr);
        if (m_renderFinished[i]) vkDestroySemaphore(device, m_renderFinished[i], nullptr);
        if (m_inFlight[i])       vkDestroyFence(device, m_inFlight[i], nullptr);
    }
    if (m_cmdPool)
    {
        vkDestroyCommandPool(device, m_cmdPool, nullptr);
    }
}

void Renderer::WaitIdle() const
{
    vkDeviceWaitIdle(m_ctx.GetDevice());
}

void Renderer::CreateCommandPool()
{
    VkCommandPoolCreateInfo info{};
    info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    info.queueFamilyIndex = m_ctx.GetQueueFamilies().graphics;
    Check(vkCreateCommandPool(m_ctx.GetDevice(), &info, nullptr, &m_cmdPool),
          "vkCreateCommandPool");
}

void Renderer::AllocateCommandBuffers()
{
    VkCommandBufferAllocateInfo info{};
    info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    info.commandPool        = m_cmdPool;
    info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    info.commandBufferCount = kFramesInFlight;
    Check(vkAllocateCommandBuffers(m_ctx.GetDevice(), &info, m_cmdBuffers.data()),
          "vkAllocateCommandBuffers");
}

void Renderer::CreateSyncObjects()
{
    VkSemaphoreCreateInfo sem{};
    sem.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    VkFenceCreateInfo fen{};
    fen.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fen.flags = VK_FENCE_CREATE_SIGNALED_BIT;  // start signalled so frame 0 doesn't deadlock

    auto device = m_ctx.GetDevice();
    for (uint32_t i = 0; i < kFramesInFlight; ++i)
    {
        Check(vkCreateSemaphore(device, &sem, nullptr, &m_imageAvailable[i]),
              "vkCreateSemaphore (imageAvailable)");
        Check(vkCreateSemaphore(device, &sem, nullptr, &m_renderFinished[i]),
              "vkCreateSemaphore (renderFinished)");
        Check(vkCreateFence(device, &fen, nullptr, &m_inFlight[i]),
              "vkCreateFence");
    }
}

void Renderer::RecordCommandBuffer(VkCommandBuffer cb, uint32_t imageIndex,
                                   float r, float g, float b)
{
    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    Check(vkBeginCommandBuffer(cb, &begin), "vkBeginCommandBuffer");

    VkClearValue clear{};
    clear.color = { { r, g, b, 1.0f } };

    VkRenderPassBeginInfo rp{};
    rp.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp.renderPass        = m_swap.GetRenderPass();
    rp.framebuffer       = m_swap.GetFramebuffers()[imageIndex];
    rp.renderArea.offset = { 0, 0 };
    rp.renderArea.extent = m_swap.GetExtent();
    rp.clearValueCount   = 1;
    rp.pClearValues      = &clear;

    vkCmdBeginRenderPass(cb, &rp, VK_SUBPASS_CONTENTS_INLINE);
    // No draw calls yet — this is intentionally a clear-only frame so the
    // scaffold can be exercised before pipelines/shaders land.
    vkCmdEndRenderPass(cb);

    Check(vkEndCommandBuffer(cb), "vkEndCommandBuffer");
}

bool Renderer::DrawFrame(float clearR, float clearG, float clearB)
{
    auto device = m_ctx.GetDevice();
    vkWaitForFences(device, 1, &m_inFlight[m_currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = 0;
    VkResult acquire    = vkAcquireNextImageKHR(device,
                                                m_swap.Get(),
                                                UINT64_MAX,
                                                m_imageAvailable[m_currentFrame],
                                                VK_NULL_HANDLE,
                                                &imageIndex);
    if (acquire == VK_ERROR_OUT_OF_DATE_KHR)
    {
        return false;
    }
    if (acquire != VK_SUCCESS && acquire != VK_SUBOPTIMAL_KHR)
    {
        Check(acquire, "vkAcquireNextImageKHR");
    }

    vkResetFences(device, 1, &m_inFlight[m_currentFrame]);
    vkResetCommandBuffer(m_cmdBuffers[m_currentFrame], 0);
    RecordCommandBuffer(m_cmdBuffers[m_currentFrame], imageIndex, clearR, clearG, clearB);

    VkSemaphore          waitSems[]   = { m_imageAvailable[m_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSemaphore          sigSems[]    = { m_renderFinished[m_currentFrame] };

    VkSubmitInfo submit{};
    submit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit.waitSemaphoreCount   = 1;
    submit.pWaitSemaphores      = waitSems;
    submit.pWaitDstStageMask    = waitStages;
    submit.commandBufferCount   = 1;
    submit.pCommandBuffers      = &m_cmdBuffers[m_currentFrame];
    submit.signalSemaphoreCount = 1;
    submit.pSignalSemaphores    = sigSems;
    Check(vkQueueSubmit(m_ctx.GetGraphicsQueue(), 1, &submit, m_inFlight[m_currentFrame]),
          "vkQueueSubmit");

    VkSwapchainKHR     swap = m_swap.Get();
    VkPresentInfoKHR   present{};
    present.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores    = sigSems;
    present.swapchainCount     = 1;
    present.pSwapchains        = &swap;
    present.pImageIndices      = &imageIndex;

    VkResult presentRes = vkQueuePresentKHR(m_ctx.GetPresentQueue(), &present);
    m_currentFrame      = (m_currentFrame + 1) % kFramesInFlight;
    if (presentRes == VK_ERROR_OUT_OF_DATE_KHR || presentRes == VK_SUBOPTIMAL_KHR)
    {
        return false;
    }
    Check(presentRes, "vkQueuePresentKHR");
    return true;
}

}  // namespace mnd::vk
