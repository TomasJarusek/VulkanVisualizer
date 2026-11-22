
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanDevice;
class VulkanDescriptorPool;
class VulkanDescriptorSetLayout;


// This is the actual descriptor machinery.
// It is allocated from the pool and corresponds to a specific single layout.
// No buffer/image/... is bound yet.
class VulkanDescriptorSet
{
private:

    Logger logger_;

    // Not a unique handle. Lifetime is tied to a descriptor pool.
    vk::DescriptorSet descriptorSet_;

public:

    // Constructor.
    VulkanDescriptorSet(const VulkanDevice& device, const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanDescriptorSet();

    // Copy/move constructor/assignment.
    VulkanDescriptorSet(const VulkanDescriptorSet& descriptorSet) = delete;
    VulkanDescriptorSet(VulkanDescriptorSet&& descriptorSet) noexcept = delete;
    VulkanDescriptorSet& operator=(const VulkanDescriptorSet& descriptorSet) = delete;
    VulkanDescriptorSet& operator=(VulkanDescriptorSet& descriptorSet) noexcept = delete;

    vk::DescriptorSet Get() const;

private:

    void AllocateDescriptorSet(const VulkanDevice& device, const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout);
};
