
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanDevice;


class VulkanCommandPool
{
private:

    Logger logger_;

    // Device reference.
    std::reference_wrapper<const VulkanDevice> device_;

    vk::UniqueCommandPool commandPool_;

    vk::UniqueCommandBuffer primaryCommandBuffer_;

public:

    // Constructor.
    VulkanCommandPool(const VulkanDevice& device, uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags commandPoolCreateFlags, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanCommandPool();

    // Copy/move constructor/assignment.
    VulkanCommandPool(const VulkanCommandPool& commandPool) = delete;
    VulkanCommandPool(VulkanCommandPool&& commandPool) noexcept = delete;
    VulkanCommandPool& operator=(const VulkanCommandPool& commandPool) = delete;
    VulkanCommandPool& operator=(VulkanCommandPool& commandPool) noexcept = delete;

    vk::CommandPool Get() const;
    vk::CommandBuffer GetPrimaryCommandBuffer() const;

    // Resets command pool.
    void Reset();

private:

    void CreateCommandPool(const VulkanDevice& device, uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags commandPoolCreateFlags);

    void AllocatePrimaryCommandBuffer(const VulkanDevice& device);
};
