
#pragma once

#include <vulkan/vulkan.hpp>

#include <array>

#include "Logger.h"


class VulkanVisualizerConfig;
struct VulkanInternalConfig;
class VulkanDevice;
class VulkanSwapchain;
class VulkanShaderModule;
class VulkanPipelineLayout;


class VulkanPipeline
{
private:

    Logger logger_;

    vk::UniquePipeline pipeline_;

    vk::SampleCountFlagBits rasterizationSamples_ = vk::SampleCountFlagBits::e1;
    bool sampleShadingEnable_ = false;
    float minSampleShading_ = 0.0f;

public:

    // Constructor.
    // VulkanPipeline(const VulkanDevice& device, const VulkanSwapchain& swapchain, const std::array<std::reference_wrapper<const VulkanShaderModule>, 2>& shaderModules, const VulkanPipelineLayout& pipelineLayout, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig);
    VulkanPipeline(const VulkanDevice& device, const vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanPipeline();

    // Copy/move constructor/assignment.
    VulkanPipeline(const VulkanPipeline& pipeline) = delete;
    VulkanPipeline(VulkanPipeline&& pipeline) noexcept = delete;
    VulkanPipeline& operator=(const VulkanPipeline& pipeline) = delete;
    VulkanPipeline& operator=(VulkanPipeline& pipeline) noexcept = delete;

    vk::Pipeline Get() const;

private:

    // void CreatePipeline(const VulkanDevice& device, const VulkanSwapchain& swapchain, const std::array<std::reference_wrapper<const VulkanShaderModule>, 2>& shaderModules, const VulkanPipelineLayout& pipelineLayout, const VulkanInternalConfig& internalConfig);
    void CreatePipeline(const VulkanDevice& device, const vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo);
};
