
#include "VulkanPipeline.h"

#include "VulkanConfig.h"
#include "VulkanInternalConfig.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanShaderModule.h"
#include "VulkanPipelineLayout.h"
#include "VulkanVertex.h"


/*VulkanPipeline::VulkanPipeline(const VulkanDevice& device, const VulkanSwapchain& swapchain, const std::array<std::reference_wrapper<const VulkanShaderModule>, 2>& shaderModules, const VulkanPipelineLayout& pipelineLayout, const VulkanInternalConfig& internalConfig, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    rasterizationSamples_(visualizerConfig.pipelineConfig_.rasterizationSamples_),
    sampleShadingEnable_(visualizerConfig.pipelineConfig_.sampleShadingEnable_),
    minSampleShading_(visualizerConfig.pipelineConfig_.minSampleShading_)
{
    LOG_DBG(this->logger_) << "Creating pipeline. "
        << "rasterizationSamples <" << GetPrintableValue(this->rasterizationSamples_) << ">, "
        << "sampleShadingEnable <" << std::format("{}", this->sampleShadingEnable_) << ">, "
        << "minSampleShading <" << this->minSampleShading_ << ">" << std::endl;

    this->CreatePipeline(device, swapchain, shaderModules, pipelineLayout, internalConfig);

    LOG_DBG(this->logger_) << "Pipeline created." << std::endl;
}*/

VulkanPipeline::VulkanPipeline(const VulkanDevice& device, const vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_)
{
    LOG_DBG(this->logger_) << "Creating pipeline through pipeline builder. " << std::endl;

    this->CreatePipeline(device, graphicsPipelineCreateInfo);

    LOG_DBG(this->logger_) << "Pipeline created." << std::endl;
}

VulkanPipeline::~VulkanPipeline()
{
    LOG_DBG(this->logger_) << "Pipeline destroyed. " << std::endl;
}

vk::Pipeline VulkanPipeline::Get() const
{
    return this->pipeline_.get();
}

/*void VulkanPipeline::CreatePipeline(const VulkanDevice& device, const VulkanSwapchain& swapchain, const std::array<std::reference_wrapper<const VulkanShaderModule>, 2>& shaderModules, const VulkanPipelineLayout& pipelineLayout, const VulkanInternalConfig& internalConfig)
{
    // Shaders.
    vk::PipelineShaderStageCreateInfo vertexPipelineShaderStageCreateInfo{};

    vertexPipelineShaderStageCreateInfo
        .setStage(vk::ShaderStageFlagBits::eVertex)
        .setModule(shaderModules[0].get().Get())
        .setPName("main");

    vk::PipelineShaderStageCreateInfo fragmentPipelineShaderStageCreateInfo{};

    fragmentPipelineShaderStageCreateInfo
        .setStage(vk::ShaderStageFlagBits::eFragment)
        .setModule(shaderModules[1].get().Get())
        .setPName("main");

    const std::array<vk::PipelineShaderStageCreateInfo, 2> pipelineShaderStageCreateInfos = {vertexPipelineShaderStageCreateInfo, fragmentPipelineShaderStageCreateInfo};

    // Vertex input.
    const auto vertexInputBindingDescription = VulkanVertex::GetVertexInputBindingDescription(0);
    const auto vertexInputAttributeDescriptions = VulkanVertex::GetVertexInputAttributeDescription(0);

    vk::PipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};

    pipelineVertexInputStateCreateInfo
        .setVertexBindingDescriptions(vertexInputBindingDescription)
        .setVertexAttributeDescriptions(vertexInputAttributeDescriptions);

    // Input assembly.
    vk::PipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};

    pipelineInputAssemblyStateCreateInfo
        .setTopology(vk::PrimitiveTopology::eTriangleList);

    // Viewport and scissors.
    vk::PipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};

    pipelineViewportStateCreateInfo
        .setViewportCount(1)
        .setScissorCount(1);

    // Rasterizer.
    vk::PipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};

    pipelineRasterizationStateCreateInfo
        .setDepthClampEnable(vk::False)
        .setRasterizerDiscardEnable(vk::False)
        .setPolygonMode(vk::PolygonMode::eFill)
        .setCullMode(vk::CullModeFlagBits::eBack)
        .setFrontFace(vk::FrontFace::eCounterClockwise)
        .setDepthBiasEnable(vk::False)
        .setLineWidth(1.0f);

    // Multisampling.
    vk::PipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};

    pipelineMultisampleStateCreateInfo
        .setRasterizationSamples(this->rasterizationSamples_)
        .setSampleShadingEnable(static_cast<vk::Bool32>(this->sampleShadingEnable_))
        .setMinSampleShading(this->minSampleShading_);

    // Depth.
    vk::PipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};

    pipelineDepthStencilStateCreateInfo
        .setDepthTestEnable(vk::True)
        .setDepthWriteEnable(vk::True)
        .setDepthCompareOp(vk::CompareOp::eLess)
        .setDepthBoundsTestEnable(vk::False)
        .setStencilTestEnable(vk::False);

    // Color.
    vk::PipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};

    pipelineColorBlendAttachmentState
        .setBlendEnable(vk::False)
        .setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);

    vk::PipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};

    pipelineColorBlendStateCreateInfo
        .setLogicOpEnable(vk::False)
        .setLogicOp(vk::LogicOp::eCopy)
        .setAttachments(pipelineColorBlendAttachmentState);

    // Dynamic states.
    std::array<vk::DynamicState, 2> dynamicStates = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};

    vk::PipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};

    pipelineDynamicStateCreateInfo
        .setDynamicStates(dynamicStates);


    // Creation of the pipeline itself.
    // Dynamic rendering is used, so no render passes are specified.

    // Dynamic rendering attachments.
    vk::PipelineRenderingCreateInfo pipelineRenderingCreateInfo{};

    pipelineRenderingCreateInfo
        .setColorAttachmentFormats(swapchain.GetSurfaceFormat().format)
        .setDepthAttachmentFormat(internalConfig.depthFormat_);

    // Graphics pipeline.
    vk::GraphicsPipelineCreateInfo graphicsPipelineCreateInfo{};

    graphicsPipelineCreateInfo
        .setStages(pipelineShaderStageCreateInfos)
        .setPVertexInputState(&pipelineVertexInputStateCreateInfo)
        .setPInputAssemblyState(&pipelineInputAssemblyStateCreateInfo)
        .setPViewportState(&pipelineViewportStateCreateInfo)
        .setPRasterizationState(&pipelineRasterizationStateCreateInfo)
        .setPMultisampleState(&pipelineMultisampleStateCreateInfo)
        .setPDepthStencilState(&pipelineDepthStencilStateCreateInfo)
        .setPColorBlendState(&pipelineColorBlendStateCreateInfo)
        .setPDynamicState(&pipelineDynamicStateCreateInfo)
        .setLayout(pipelineLayout.Get())
        .setRenderPass(nullptr)
        .setPNext(&pipelineRenderingCreateInfo);

    // Return values other than success and failure are possible.
    // Anything but a success will be treated as a failure.
    auto result = device.Get().createGraphicsPipelineUnique(nullptr, graphicsPipelineCreateInfo);

    if (result.result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Exception: Pipeline was not created successfully. Result was < " + GetPrintableValue(result.result) +  ">.");
    }

    this->pipeline_ = std::move(result.value);
}*/

void VulkanPipeline::CreatePipeline(const VulkanDevice& device, const vk::GraphicsPipelineCreateInfo& graphicsPipelineCreateInfo)
{
    // Return values other than success and failure are possible.
    // Anything but a success will be treated as a failure.
    auto result = device.Get().createGraphicsPipelineUnique(nullptr, graphicsPipelineCreateInfo);

    if (result.result != vk::Result::eSuccess)
    {
        throw std::runtime_error("Exception: Pipeline was not created successfully. Result was < " + GetPrintableValue(result.result) +  ">.");
    }

    this->pipeline_ = std::move(result.value);
}
