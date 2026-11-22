
#include "VulkanDescriptorSet.h"

#include "VulkanConfig.h"
#include "VulkanDevice.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"


VulkanDescriptorSet::VulkanDescriptorSet(const VulkanDevice& device, const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_)
{
    LOG_DBG(this->logger_) << "Creating descriptor set." << std::endl;

    this->AllocateDescriptorSet(device, descriptorPool, descriptorSetLayout);

    LOG_DBG(this->logger_) << "Descriptor set created." << std::endl;
}

VulkanDescriptorSet::~VulkanDescriptorSet()
{
    LOG_DBG(this->logger_) << "Descriptor set destroyed. " << std::endl;
}

vk::DescriptorSet VulkanDescriptorSet::Get() const
{
    return this->descriptorSet_;
}

void VulkanDescriptorSet::AllocateDescriptorSet(const VulkanDevice& device, const VulkanDescriptorPool& descriptorPool, const VulkanDescriptorSetLayout& descriptorSetLayout)
{
    // This wrapper is designed to represent only one set based on one specific layout.

    const std::array<vk::DescriptorSetLayout, 1> descriptorSetLayouts = {descriptorSetLayout.Get()};

    vk::DescriptorSetAllocateInfo descriptorSetAllocateInfo{};

    descriptorSetAllocateInfo
        .setDescriptorPool(descriptorPool.Get())
        .setSetLayouts(descriptorSetLayouts);

    this->descriptorSet_ = device.Get().allocateDescriptorSets(descriptorSetAllocateInfo).front();
}
