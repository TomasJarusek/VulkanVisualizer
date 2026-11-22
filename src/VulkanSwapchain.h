
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanWindow;
class VulkanSurface;
class VulkanDevice;
struct VulkanImageState;


class VulkanSwapchain
{
private:

    Logger logger_;

    // Device reference.
    std::reference_wrapper<const VulkanDevice> device_;

    // Chosen settings for a swapchain construction.
    vk::SurfaceFormatKHR surfaceFormat_;
    vk::PresentModeKHR presentMode_;
    vk::Extent2D extent_;

    // Swapchain and its images.
    vk::UniqueSwapchainKHR swapchain_;
    std::vector<vk::Image> swapchainImages_;

    // Swapchain image views.
    std::vector<vk::UniqueImageView> swapchainImageViews_;

    // One render finished semaphore per swapchain image.
    // https://docs.vulkan.org/guide/latest/swapchain_semaphore_reuse.html
    // presentKHR() does not signal anything when it is complete, a request for presentation is just sent out and
    // the driver/OS does whatever it wants. This means that there is no guarantee for us that the semaphore will be unused
    // by the time the main loop wants to reuse it for the next same frame in flight. But it should definitely be unused when
    // the image it was originally paired with is returned by acquireNextImage().
    std::vector<vk::UniqueSemaphore> renderFinishedSemaphores_;

public:

    // Constructor.
    VulkanSwapchain(const VulkanWindow& window, const VulkanSurface& surface, const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig);
    // Recreation constructor.
    VulkanSwapchain(const VulkanWindow& window, const VulkanSurface& surface, const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig, const VulkanSwapchain& oldSwapchain);
    // Destructor.
    ~VulkanSwapchain();

    // Copy/move constructor/assignment.
    VulkanSwapchain(const VulkanSwapchain& swapchain) = delete;
    VulkanSwapchain(VulkanSwapchain&& swapchain) noexcept = delete;
    VulkanSwapchain& operator=(const VulkanSwapchain& swapchain) = delete;
    VulkanSwapchain& operator=(VulkanSwapchain& swapchain) noexcept = delete;

    vk::SwapchainKHR Get() const;

    const vk::SurfaceFormatKHR& GetSurfaceFormat() const; 
    const vk::Extent2D& GetExtent() const;
    vk::ImageView GetSwapchainImageView(size_t swapchainImageIndex) const;
    vk::Semaphore GetSwapchainImageRFSemaphore(size_t swapchainImageIndex) const;

    // Acquires image. Used within draw loop.
    std::pair<vk::Result, uint32_t> AcquireNextImage(vk::Semaphore imageAvailableSemaphore) const;
    // Presents the image. Used within draw loop.
    vk::Result PresentImage(size_t swapchainImageIndex) const;

    // Inserts an image barrier.
    void InsertSwapchainImageBarrier(vk::CommandBuffer commandBuffer, size_t swapchainImageIndex, const VulkanImageState& srcImageState, const VulkanImageState& dstImageState) const;

    // Get aspect ratio based on the current extent.
    float GetAspectRatio() const;

private:

    void ChooseSurfaceFormat(const VulkanDevice& device);
    bool IsSurfaceFormatSupported(const VulkanDevice& device, const vk::SurfaceFormatKHR& surfaceFormatKHR);
    void ChoosePresentMode(const VulkanDevice& device);
    void ChooseExtent(const VulkanWindow& window, const VulkanDevice& device);

    void CreateSwapchain(const VulkanSurface& surface, const VulkanDevice& device, vk::SwapchainKHR oldSwapchainKHR = nullptr);
    void CreateSwapchainImageViews(const VulkanDevice& device);

    void CreateRenderFinishedSemaphores(const VulkanDevice& device);
};
