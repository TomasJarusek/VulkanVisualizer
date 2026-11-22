
#include "VulkanDescriptorPool.h"

#include "VulkanConfig.h"
#include "VulkanDevice.h"


VulkanDescriptorPool::VulkanDescriptorPool(const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_)
{
    LOG_DBG(this->logger_) << "Creating descriptor pool." << std::endl;

    this->CreateDescriptorPool(device, visualizerConfig.internalConfig_.framesInFlightCount_);

    LOG_DBG(this->logger_) << "Descriptor pool created." << std::endl;
}

VulkanDescriptorPool::~VulkanDescriptorPool()
{
    LOG_DBG(this->logger_) << "Descriptor pool destroyed. " << std::endl;
}

vk::DescriptorPool VulkanDescriptorPool::Get() const
{
    return this->descriptorPool_.get();
}

void VulkanDescriptorPool::CreateDescriptorPool(const VulkanDevice& device, size_t framesInFlightCount)
{
    // DescriptorPoolSize specifies the independent absolute amount of a specific descriptor type.
    vk::DescriptorPoolSize uniformBufferDescriptorPoolSize{};

    uniformBufferDescriptorPoolSize
        .setType(vk::DescriptorType::eUniformBuffer)
        .setDescriptorCount(static_cast<uint32_t>(framesInFlightCount));

    std::array<vk::DescriptorPoolSize, 1> descriptorPoolSizes = {uniformBufferDescriptorPoolSize};

    // MaxSets specifies the maximum number od descriptor sets that can be allocated from this pool.
    // But this count depends on the architecture of the renderer. Some things might be duplicated based on frames in flight and others might not.
    // 1 layout represents baseline for 1 set (or more) and if it will be used per frame in flight, 1*framesInFlightCount sets would be needed.
    vk::DescriptorPoolCreateInfo descriptorPoolCreateInfo{};

    descriptorPoolCreateInfo
        // One set per frame in flight.
        .setMaxSets(static_cast<uint32_t>(framesInFlightCount))
        .setPoolSizes(descriptorPoolSizes);

    this->descriptorPool_ = device.Get().createDescriptorPoolUnique(descriptorPoolCreateInfo);
}
