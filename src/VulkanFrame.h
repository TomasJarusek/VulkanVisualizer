
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"
#include "VulkanCommandPool.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "VulkanDescriptorSet.h"


class VulkanVisualizerConfig;
struct VulkanInternalConfig;
class VulkanDevice;
class VulkanSwapchain;
class VulkanDescriptorSetLayout;
class VulkanDescriptorPool;


class VulkanGraphicsFrameContext
{
private:

    Logger logger_;

    // Device reference.
    std::reference_wrapper<const VulkanDevice> device_;
    // VMA reference.
    std::reference_wrapper<const VulkanVMA> vma_;

    VulkanCommandPool commandPool_;

    VulkanBuffer vertexBuffer_;
    VulkanBuffer indexBuffer_;

    VulkanBuffer uniformBuffer_;

    std::unique_ptr<VulkanImage> depthImage_;
    std::unique_ptr<VulkanImage> msaaImage_;

    VulkanDescriptorSet globalDataDescriptorSet_;
    std::vector<vk::DescriptorSet> descriptorSetsHandles_;

public:

    // Constructor.
    VulkanGraphicsFrameContext(const VulkanDevice& device, const VulkanSwapchain& swapchain, const VulkanVMA& vma, const VulkanDescriptorPool& descriptorPool, std::span<const std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanGraphicsFrameContext();

    // Copy/move constructor/assignment.
    VulkanGraphicsFrameContext(const VulkanGraphicsFrameContext& graphicsFrameContext) = delete;
    VulkanGraphicsFrameContext(VulkanGraphicsFrameContext&& graphicsFrameContext) noexcept = delete;
    VulkanGraphicsFrameContext& operator=(const VulkanGraphicsFrameContext& graphicsFrameContext) = delete;
    VulkanGraphicsFrameContext& operator=(VulkanGraphicsFrameContext& graphicsFrameContext) noexcept = delete;

    VulkanCommandPool& GetCommandPool();

    VulkanBuffer& GetVertexBuffer();
    VulkanBuffer& GetIndexBuffer();

    VulkanBuffer& GetUniformBuffer();

    VulkanImage& GetDepthImage();
    void RecreateDepthImage(const VulkanSwapchain& swapchain, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig);

    VulkanImage& GetMSAAImage();
    void RecreateMSAAImage(const VulkanSwapchain& swapchain, const VulkanVisualizerConfig& visualizerConfig);

    const std::vector<vk::DescriptorSet>& GetDescriptorSetsHandles() const;

private:

    void UpdateDescriptorSets(const VulkanDevice& device) const;

    VulkanBufferProperties SetupVertexBufferProperties(const VulkanVisualizerConfig& visualizerConfig) const;
    VulkanBufferProperties SetupIndexBufferProperties(const VulkanVisualizerConfig& visualizerConfig) const;
    VulkanBufferProperties SetupUniformBufferProperties() const;

    std::pair<VulkanImageProperties, std::array<VulkanImageViewProperties, 1>> SetupDepthImageAndViewsProperties(const VulkanSwapchain& swapchain, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig) const;
    std::pair<VulkanImageProperties, std::array<VulkanImageViewProperties, 1>> SetupMSAAImageAndViewsProperties(const VulkanSwapchain& swapchain, const VulkanVisualizerConfig& visualizerConfig) const;
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanTransferFrameContext
{
private:

    Logger logger_;

    VulkanCommandPool commandPool_;

    VulkanBuffer stagingBuffer_;

public:

    // Constructor.
    VulkanTransferFrameContext(const VulkanDevice& device, const VulkanVMA& vma, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanTransferFrameContext();

    // Copy/move constructor/assignment.
    VulkanTransferFrameContext(const VulkanTransferFrameContext& transferFrameContext) = delete;
    VulkanTransferFrameContext(VulkanTransferFrameContext&& transferFrameContext) noexcept = delete;
    VulkanTransferFrameContext& operator=(const VulkanTransferFrameContext& transferFrameContext) = delete;
    VulkanTransferFrameContext& operator=(VulkanTransferFrameContext& transferFrameContext) noexcept = delete;

    VulkanCommandPool& GetCommandPool();

    VulkanBuffer& GetStagingBuffer();

private:

    VulkanBufferProperties SetupStagingBufferProperties(const VulkanVisualizerConfig& visualizerConfig) const;
};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

class VulkanFrameContext
{
private:

    Logger logger_;

    // True if the next frame will be the first frame.
    bool firstFrame_ = true;

    VulkanGraphicsFrameContext graphicsFrameContext_;
    VulkanTransferFrameContext transferFrameContext_;

    // Draw loop base synchronization primitives.
    vk::UniqueFence frameInFlightFence_;
    vk::UniqueSemaphore imageAvailableSemaphore_;

    // Transfer - graphics synchronization primitives.
    vk::UniqueSemaphore transferFinishedSemaphore_;
    vk::UniqueSemaphore graphicsFinishedSemaphore_;
public:

    // Constructor.
    VulkanFrameContext(const VulkanDevice& device, const VulkanSwapchain& swapchain, const VulkanVMA& vma, const VulkanDescriptorPool& descriptorPool, std::span<const std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanFrameContext();

    // Copy/move constructor/assignment.
    VulkanFrameContext(const VulkanFrameContext& frameContext) = delete;
    VulkanFrameContext(VulkanFrameContext&& frameContext) noexcept = delete;
    VulkanFrameContext& operator=(const VulkanFrameContext& frameContext) = delete;
    VulkanFrameContext& operator=(VulkanFrameContext& frameContext) noexcept = delete;

    void FirstFrameHandled();
    bool HandlingFirstFrame() const;

    VulkanGraphicsFrameContext& GetGraphicsFrameContext();
    VulkanTransferFrameContext& GetTransferFrameContext();

    vk::Fence GetFrameInFlightFence() const;
    vk::Semaphore GetImageAvailableSemaphore() const;
    vk::Semaphore GetTransferFinishedSemaphore() const;
    vk::Semaphore GetGraphicsFinishedSemaphore() const;

private:

    void CreateFences(const VulkanDevice& device);
    void CreateSemaphores(const VulkanDevice& device);
};
