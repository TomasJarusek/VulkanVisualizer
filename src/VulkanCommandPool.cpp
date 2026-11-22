
#include "VulkanCommandPool.h"

#include "VulkanConfig.h"
#include "VulkanDevice.h"
#include "VulkanHelperFunctions.h"

VulkanCommandPool::VulkanCommandPool(const VulkanDevice& device, uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags commandPoolCreateFlags, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    device_(device)
{
    LOG_DBG(this->logger_) << "Creating command pool. "
        << "queueFamilyIndex <" << queueFamilyIndex << ">, "
        << "commandPoolCreateFlags <" << GetPrintableValue(commandPoolCreateFlags) << ">" << std::endl;

    this->CreateCommandPool(device, queueFamilyIndex, commandPoolCreateFlags);

    this->AllocatePrimaryCommandBuffer(device);

    LOG_DBG(this->logger_) << "Command pool created." << std::endl;
}

VulkanCommandPool::~VulkanCommandPool()
{
    LOG_DBG(this->logger_) << "Command pool destroyed. " << std::endl;
}

vk::CommandPool VulkanCommandPool::Get() const
{
    return this->commandPool_.get();
}

vk::CommandBuffer VulkanCommandPool::GetPrimaryCommandBuffer() const
{
    return this->primaryCommandBuffer_.get();
}

void VulkanCommandPool::Reset()
{
    this->device_.get().Get().resetCommandPool(this->commandPool_.get());
}

void VulkanCommandPool::CreateCommandPool(const VulkanDevice& device, uint32_t queueFamilyIndex, vk::CommandPoolCreateFlags commandPoolCreateFlags)
{
    vk::CommandPoolCreateInfo commandPoolCreateInfo{};

    commandPoolCreateInfo
        .setQueueFamilyIndex(queueFamilyIndex)
        .setFlags(commandPoolCreateFlags);

    this->commandPool_ = device.Get().createCommandPoolUnique(commandPoolCreateInfo);
}

void VulkanCommandPool::AllocatePrimaryCommandBuffer(const VulkanDevice& device)
{
    vk::CommandBufferAllocateInfo commandBufferAllocateInfo{};

    commandBufferAllocateInfo
        .setCommandPool(this->commandPool_.get())
        .setLevel(vk::CommandBufferLevel::ePrimary)
        .setCommandBufferCount(1);

    this->primaryCommandBuffer_ = std::move((device.Get().allocateCommandBuffersUnique(commandBufferAllocateInfo)).front());
}
