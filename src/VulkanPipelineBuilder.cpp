
#include "VulkanPipelineBuilder.h"

#include "VulkanConfig.h"
#include "VulkanPipeline.h"
#include "VulkanPipelineLayout.h"
#include "VulkanDevice.h"


VulkanPipelineBuilder::VulkanPipelineBuilder(const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    device_(device),
    visualizerConfig_(visualizerConfig)
{
    LOG_DBG(this->logger_) << "Creating pipeline builder." << std::endl;

    LOG_DBG(this->logger_) << "Pipeline builder created." << std::endl;
}

VulkanPipelineBuilder::~VulkanPipelineBuilder()
{
    LOG_DBG(this->logger_) << "Pipeline builder destroyed. " << std::endl;
}

void VulkanPipelineBuilder::AddPipelineShaderStageCreateInfo(const vk::PipelineShaderStageCreateInfo& pipelineShaderStageCreateInfo)
{
    this->pipelineShaderStageCreateInfos_.push_back(pipelineShaderStageCreateInfo);
}

void VulkanPipelineBuilder::ClearPipelineShaderStageCreateInfos()
{
    this->pipelineShaderStageCreateInfos_.clear();
}

void VulkanPipelineBuilder::AddVertexInputBindingDescription(const vk::VertexInputBindingDescription& vertexInputBindingDescription)
{
    this->vertexInputBindingDescriptions_.push_back(vertexInputBindingDescription);
}

void VulkanPipelineBuilder::ClearVertexInputBindingDescriptions()
{
    this->vertexInputBindingDescriptions_.clear();
}

void VulkanPipelineBuilder::AddVertexInputAttributeDescription(const vk::VertexInputAttributeDescription& vertexInputAttributeDescription)
{
    this->vertexInputAttributeDescriptions_.push_back(vertexInputAttributeDescription);
}

void VulkanPipelineBuilder::AddVertexInputAttributeDescriptions(std::span<const vk::VertexInputAttributeDescription> vertexInputAttributeDescriptions)
{
    for (const vk::VertexInputAttributeDescription& vertexInputAttributeDescription : vertexInputAttributeDescriptions)
    {
        this->vertexInputAttributeDescriptions_.push_back(vertexInputAttributeDescription);
    }
}

void VulkanPipelineBuilder::ClearVertexInputAttributeDescriptions()
{
    this->vertexInputAttributeDescriptions_.clear();
}

void VulkanPipelineBuilder::SetPipelineInputAssemblyStateCreateInfo(const vk::PipelineInputAssemblyStateCreateInfo& pipelineInputAssemblyStateCreateInfo)
{
    this->pipelineInputAssemblyStateCreateInfo_ = pipelineInputAssemblyStateCreateInfo;
}

void VulkanPipelineBuilder::SetPipelineViewportStateCreateInfo(const vk::PipelineViewportStateCreateInfo& pipelineViewportStateCreateInfo)
{
    this->pipelineViewportStateCreateInfo_ = pipelineViewportStateCreateInfo;
}

void VulkanPipelineBuilder::SetPipelineRasterizationStateCreateInfo(const vk::PipelineRasterizationStateCreateInfo& pipelineRasterizationStateCreateInfo)
{
    this->pipelineRasterizationStateCreateInfo_ = pipelineRasterizationStateCreateInfo;
}

void VulkanPipelineBuilder::SetPipelineMultisampleStateCreateInfo(const vk::PipelineMultisampleStateCreateInfo& pipelineMultisampleStateCreateInfo)
{
    this->pipelineMultisampleStateCreateInfo_ = pipelineMultisampleStateCreateInfo;
}

void VulkanPipelineBuilder::SetPipelineDepthStencilStateCreateInfo(const vk::PipelineDepthStencilStateCreateInfo& pipelineDepthStencilStateCreateInfo)
{
    this->pipelineDepthStencilStateCreateInfo_ = pipelineDepthStencilStateCreateInfo;
}

void VulkanPipelineBuilder::AddPipelineColorBlendAttachmentState(const vk::PipelineColorBlendAttachmentState& pipelineColorBlendAttachmentState)
{
    this->pipelineColorBlendAttachmentStates_.push_back(pipelineColorBlendAttachmentState);
}

void VulkanPipelineBuilder::ClearPipelineColorBlendAttachmentStates()
{
    this->pipelineColorBlendAttachmentStates_.clear();
}

void VulkanPipelineBuilder::SetPipelineColorBlendStateCreateInfo(const vk::PipelineColorBlendStateCreateInfo& pipelineColorBlendStateCreateInfo)
{
    this->pipelineColorBlendStateCreateInfo_ = pipelineColorBlendStateCreateInfo;
}

void VulkanPipelineBuilder::AddDynamicState(const vk::DynamicState& dynamicState)
{
    this->dynamicStates_.push_back(dynamicState);
}

void VulkanPipelineBuilder::ClearDynamicStates()
{
    this->dynamicStates_.clear();
}

void VulkanPipelineBuilder::SetPipelineRenderingCreateInfo(const vk::PipelineRenderingCreateInfo& pipelineRenderingCreateInfo)
{
    this->pipelineRenderingCreateInfo_ = pipelineRenderingCreateInfo;
}

void VulkanPipelineBuilder::SetPipelineLayout(const VulkanPipelineLayout& pipelineLayout)
{
    this->pipelineLayout_ = pipelineLayout;
}

std::unique_ptr<VulkanPipeline> VulkanPipelineBuilder::Build()
{
    // Vertex input.
    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};

    pipelineVertexInputStateCreateInfo
        .setVertexBindingDescriptions(this->vertexInputBindingDescriptions_)
        .setVertexAttributeDescriptions(this->vertexInputAttributeDescriptions_);

    // Color.
    this->pipelineColorBlendStateCreateInfo_
        .setAttachments(this->pipelineColorBlendAttachmentStates_);

    // Dynamic states.
    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};

    pipelineDynamicStateCreateInfo
        .setDynamicStates(this->dynamicStates_);

    // Graphics pipeline.
    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};

    graphicsPipelineCreateInfo
        .setStages(this->pipelineShaderStageCreateInfos_)
        .setPVertexInputState(&pipelineVertexInputStateCreateInfo)
        .setPInputAssemblyState(&this->pipelineInputAssemblyStateCreateInfo_)
        .setPViewportState(&this->pipelineViewportStateCreateInfo_)
        .setPRasterizationState(&this->pipelineRasterizationStateCreateInfo_)
        .setPMultisampleState(&this->pipelineMultisampleStateCreateInfo_)
        .setPDepthStencilState(&this->pipelineDepthStencilStateCreateInfo_)
        .setPColorBlendState(&this->pipelineColorBlendStateCreateInfo_)
        .setPDynamicState(&pipelineDynamicStateCreateInfo)
        .setLayout(this->pipelineLayout_.value().get().Get())
        .setRenderPass(nullptr)
        .setPNext(&this->pipelineRenderingCreateInfo_);

    return std::make_unique<VulkanPipeline>(this->device_.get(), graphicsPipelineCreateInfo, this->visualizerConfig_.get());
}
