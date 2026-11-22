
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanDevice;


// Descriptor set layout reflects shader's resource inputs (layout(location = 0)..., ...).
// Each resource input must belong to a corresponding descriptor set layout.
// One shader can have multiple descriptor set layouts.
// This is finalized in the pipeline layout, where each descriptor set layout is tied to a set.
// This is only interface definition, it doesn't know anything about the actual objects.
class VulkanDescriptorSetLayout
{
private:

    Logger logger_;

    vk::UniqueDescriptorSetLayout descriptorSetLayout_;

public:

    // Constructor.
    VulkanDescriptorSetLayout(const VulkanDevice& device, std::span<const vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanDescriptorSetLayout();

    // Copy/move constructor/assignment.
    VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout& pipelineLayout) = delete;
    VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&& pipelineLayout) noexcept = delete;
    VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout& pipelineLayout) = delete;
    VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout& pipelineLayout) noexcept = delete;

    vk::DescriptorSetLayout Get() const;

private:

    void CreateDescriptorSetLayout(const VulkanDevice& device, std::span<const vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings);
};
