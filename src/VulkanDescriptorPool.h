
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanDevice;
class VulkanDescriptorSet;


class VulkanDescriptorPool
{
private:

    Logger logger_;

    vk::UniqueDescriptorPool descriptorPool_;

public:

    // Constructor.
    VulkanDescriptorPool(const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanDescriptorPool();

    // Copy/move constructor/assignment.
    VulkanDescriptorPool(const VulkanDescriptorPool& descriptorPool) = delete;
    VulkanDescriptorPool(VulkanDescriptorPool&& descriptorPool) noexcept = delete;
    VulkanDescriptorPool& operator=(const VulkanDescriptorPool& descriptorPool) = delete;
    VulkanDescriptorPool& operator=(VulkanDescriptorPool& descriptorPool) noexcept = delete;

    vk::DescriptorPool Get() const;

private:

    void CreateDescriptorPool(const VulkanDevice& device, size_t framesInFlightCount);
};
