
#include "VulkanDescriptorSetLayout.h"

#include "VulkanConfig.h"
#include "VulkanDevice.h"


VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VulkanDevice& device, std::span<const vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_)
{
    LOG_DBG(this->logger_) << "Creating descriptor set layout." << std::endl;

    this->CreateDescriptorSetLayout(device, descriptorSetLayoutBindings);

    LOG_DBG(this->logger_) << "Descriptor set layout created." << std::endl;
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
    LOG_DBG(this->logger_) << "Descriptor set layout destroyed. " << std::endl;
}

vk::DescriptorSetLayout VulkanDescriptorSetLayout::Get() const
{
    return this->descriptorSetLayout_.get();
}

void VulkanDescriptorSetLayout::CreateDescriptorSetLayout(const VulkanDevice& device, std::span<const vk::DescriptorSetLayoutBinding> descriptorSetLayoutBindings)
{
    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};

    descriptorSetLayoutCreateInfo
        .setBindings(descriptorSetLayoutBindings);

    this->descriptorSetLayout_ = device.Get().createDescriptorSetLayoutUnique(descriptorSetLayoutCreateInfo);
}
