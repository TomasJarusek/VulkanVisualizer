
#include "VulkanPipelineLayout.h"

#include <ranges>

#include "VulkanConfig.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorSetLayout.h"


VulkanPipelineLayout::VulkanPipelineLayout(const VulkanDevice& device, std::span<const std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_)
{
    LOG_DBG(this->logger_) << "Creating pipeline layout." << std::endl;

    this->CreatePipelineLayout(device, descriptorSetLayouts);

    LOG_DBG(this->logger_) << "Pipeline layout created." << std::endl;
}

VulkanPipelineLayout::~VulkanPipelineLayout()
{
    LOG_DBG(this->logger_) << "Pipeline layout destroyed. " << std::endl;
}

vk::PipelineLayout VulkanPipelineLayout::Get() const
{
    return this->pipelineLayout_.get();
}

void VulkanPipelineLayout::CreatePipelineLayout(const VulkanDevice& device, std::span<const std::reference_wrapper<const VulkanDescriptorSetLayout>> descriptorSetLayouts)
{
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo{};

    // Vulkan handles extraction.
    std::vector<vk::DescriptorSetLayout> descriptorSetLayoutsHandles(descriptorSetLayouts | std::views::transform([](const auto& descriptorSetLayout) {return descriptorSetLayout.get().Get();}) | std::ranges::to<std::vector>());

    // Indexes of sets are incremented implicitly.
    pipelineLayoutCreateInfo
        .setSetLayouts(descriptorSetLayoutsHandles)
        .setPushConstantRangeCount(0);

    this->pipelineLayout_ = device.Get().createPipelineLayoutUnique(pipelineLayoutCreateInfo);
}
