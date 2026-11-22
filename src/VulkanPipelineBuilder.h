
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanPipeline;
class VulkanPipelineLayout;
class VulkanDevice;


class VulkanPipelineBuilder
{
private:

    Logger logger_;

    // Device reference.
    std::reference_wrapper<const VulkanDevice> device_;
    // Renderer config reference (passed to created pipelines).
    std::reference_wrapper<const VulkanVisualizerConfig> visualizerConfig_;

    // Shaders.
    std::vector<vk::PipelineShaderStageCreateInfo> pipelineShaderStageCreateInfos_;

    // Vertex input.
    std::vector<vk::VertexInputBindingDescription> vertexInputBindingDescriptions_;
    std::vector<vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions_;

    // Input assembly.
    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo_{};

    // Viewport and scissors.
    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo_{};

    // Rasterizer.
    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo_{};

    // Multisampling.
    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo_{};

    // Depth.
    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo_{};

    // Color.
    std::vector<vk::PipelineColorBlendAttachmentState> pipelineColorBlendAttachmentStates_;
    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo_{};

    // Dynamic states.
    std::vector<vk::DynamicState> dynamicStates_;

    // Dynamic rendering attachments.
    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo_{};

    // Pipeline layout reference.
    std::optional<std::reference_wrapper<const VulkanPipelineLayout>> pipelineLayout_;

public:

    // Constructor.
    VulkanPipelineBuilder(const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanPipelineBuilder();

    // Copy/move constructor/assignment.
    VulkanPipelineBuilder(const VulkanPipelineBuilder& pipelineBuilder) = delete;
    VulkanPipelineBuilder(VulkanPipelineBuilder&& pipelineBuilder) noexcept = delete;
    VulkanPipelineBuilder& operator=(const VulkanPipelineBuilder& pipelineBuilder) = delete;
    VulkanPipelineBuilder& operator=(VulkanPipelineBuilder& pipelineBuilder) noexcept = delete;

    void AddPipelineShaderStageCreateInfo(const vk::PipelineShaderStageCreateInfo& pipelineShaderStageCreateInfo);
    void ClearPipelineShaderStageCreateInfos();
    
    void AddVertexInputBindingDescription(const vk::VertexInputBindingDescription& vertexInputBindingDescription);
    void ClearVertexInputBindingDescriptions();

    void AddVertexInputAttributeDescription(const vk::VertexInputAttributeDescription& vertexInputAttributeDescription);
    void AddVertexInputAttributeDescriptions(std::span<const vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions);
    void ClearVertexInputAttributeDescriptions();
    
    void SetPipelineInputAssemblyStateCreateInfo(const vk::PipelineInputAssemblyStateCreateInfo& pipelineInputAssemblyStateCreateInfo);

    void SetPipelineViewportStateCreateInfo(const vk::PipelineViewportStateCreateInfo& pipelineViewportStateCreateInfo);

    void SetPipelineRasterizationStateCreateInfo(const vk::PipelineRasterizationStateCreateInfo& pipelineRasterizationStateCreateInfo);

    void SetPipelineMultisampleStateCreateInfo(const vk::PipelineMultisampleStateCreateInfo& pipelineMultisampleStateCreateInfo);

    void SetPipelineDepthStencilStateCreateInfo(const vk::PipelineDepthStencilStateCreateInfo& pipelineDepthStencilStateCreateInfo);

    void AddPipelineColorBlendAttachmentState(const vk::PipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState);
    void ClearPipelineColorBlendAttachmentStates();
    void SetPipelineColorBlendStateCreateInfo(const vk::PipelineColorBlendStateCreateInfo& pipelineColorBlendStateCreateInfo);

    void AddDynamicState(const vk::DynamicState& dynamicState);
    void ClearDynamicStates();

    void SetPipelineRenderingCreateInfo(const vk::PipelineRenderingCreateInfo& pipelineRenderingCreateInfo);

    void SetPipelineLayout(const VulkanPipelineLayout& pipelineLayout);

    std::unique_ptr<VulkanPipeline> Build();

private:
};
