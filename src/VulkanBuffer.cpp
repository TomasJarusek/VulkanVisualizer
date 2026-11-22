
#include "VulkanBuffer.h"

#include <vma/vk_mem_alloc.h>

#include <format>

#include "VulkanConfig.h"
#include "VulkanVMA.h"
#include "VulkanHelperEnums.h"
#include "VulkanHelperStructs.h"
#include "VulkanHelperFunctions.h"


VulkanBuffer::VulkanBuffer(const VulkanVMA& vma, const VulkanBufferProperties& bufferProperties, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    vma_(vma),
    bufferUseCase_(bufferProperties.bufferUseCase_)
{
    LOG_DBG(this->logger_) << "Creating buffer. "
        << "bufferUseCase <" << GetPrintableValue(bufferProperties.bufferUseCase_) << ">, "
        << "deviceSize <" << bufferProperties.deviceSize_ << ">, "
        << "bufferUsageFlags <" << GetPrintableValue(bufferProperties.bufferUsageFlags_) << ">, "
        << "vmaMemoryUsage <" << vma_.get().ToString(bufferProperties.vmaMemoryUsage_) << ">, "
        << "vmaAllocationCreateFlagBits <" << vma_.get().ToString(bufferProperties.vmaAllocationCreateFlags_) << ">, "
        << "sharingMode <" << GetPrintableValue(bufferProperties.sharingMode_) << ">, "
        << "queueFamilyIndexes <" << std::format("{}", bufferProperties.queueFamilyIndexes_) << ">" << std::endl;

    this->CreateBuffer(vma, bufferProperties);

    this->DetermineMemoryCoherence();

    LOG_DBG(this->logger_) << "Buffer created. " << std::endl;
}

VulkanBuffer::~VulkanBuffer()
{
    this->DestroyBuffer();

    LOG_DBG(this->logger_) << "Buffer destroyed. " << std::endl;
}

vk::Buffer VulkanBuffer::Get() const
{
    return this->buffer_;
}

bool VulkanBuffer::IsMapped() const
{
    return this->vmaAllocationInfo_.pMappedData != nullptr;
}

void VulkanBuffer::CreateBuffer(const VulkanVMA& vma, const VulkanBufferProperties& bufferProperties)
{
    VmaAllocationCreateInfo vmaAllocationCreateInfo{};

    vmaAllocationCreateInfo.usage = bufferProperties.vmaMemoryUsage_;
    vmaAllocationCreateInfo.flags = bufferProperties.vmaAllocationCreateFlags_;


    vk::BufferCreateInfo bufferCreateInfo{};
    
    bufferCreateInfo
        .setSize(bufferProperties.deviceSize_)
        .setUsage(bufferProperties.bufferUsageFlags_)
        .setSharingMode(bufferProperties.sharingMode_)
        .setQueueFamilyIndices(bufferProperties.queueFamilyIndexes_);

    VkBufferCreateInfo bufferCreateInfoC = static_cast<VkBufferCreateInfo>(bufferCreateInfo);
    

    VkBuffer buffer;

    VkResult result = vmaCreateBuffer(vma.Get(), &bufferCreateInfoC, &vmaAllocationCreateInfo, &buffer, &this->vmaAllocation_, &this->vmaAllocationInfo_);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Exception: Error during VMA buffer creation.");
    }

    this->buffer_ = static_cast<vk::Buffer>(buffer);
}

void VulkanBuffer::DestroyBuffer()
{
    vmaDestroyBuffer(this->vma_.get().Get(), this->buffer_, this->vmaAllocation_);
}

void VulkanBuffer::DetermineMemoryCoherence()
{
    VkMemoryPropertyFlags memoryPropertyFlags{};

    vmaGetMemoryTypeProperties(this->vma_.get().Get(), this->vmaAllocationInfo_.memoryType, &memoryPropertyFlags);

    this->memoryHostCoherent_ = (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
}
