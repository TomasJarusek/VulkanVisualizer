
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanDevice;
class VulkanDescriptorSetLayout;

class VulkanPipelineLayout
{
private:

    Logger logger_;

    vk::UniquePipelineLayout pipelineLayout_;

public:

    // Constructor.
    VulkanPipelineLayout(const VulkanDevice& device, std::span<const std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanPipelineLayout();

    // Copy/move constructor/assignment.
    VulkanPipelineLayout(const VulkanPipelineLayout& pipelineLayout) = delete;
    VulkanPipelineLayout(VulkanPipelineLayout&& pipelineLayout) noexcept = delete;
    VulkanPipelineLayout& operator=(const VulkanPipelineLayout& pipelineLayout) = delete;
    VulkanPipelineLayout& operator=(VulkanPipelineLayout& pipelineLayout) noexcept = delete;

    vk::PipelineLayout Get() const;

private:

    void CreatePipelineLayout(const VulkanDevice& device, std::span<const std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts);
};
