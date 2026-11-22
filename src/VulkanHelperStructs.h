
#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "VulkanHelperEnums.h"


// Represents current image state.
struct VulkanImageState
{
    vk::PipelineStageFlags2 stage_;
    vk::AccessFlags2 access_;
    vk::ImageLayout layout_;
    int32_t queueFamilyIndex_;

    // Standard constructor.
    VulkanImageState(vk::PipelineStageFlags2 stage, vk::AccessFlags2 access, vk::ImageLayout layout, int32_t queueFamilyIndex) :
        stage_(stage),
        access_(access),
        layout_(layout),
        queueFamilyIndex_(queueFamilyIndex)
    {
    };

    // No queue family index is set, so it will default to vk::QueueFamilyIgnored.
    VulkanImageState(vk::PipelineStageFlags2 stage, vk::AccessFlags2 access, vk::ImageLayout layout) :
        stage_(stage),
        access_(access),
        layout_(layout),
        queueFamilyIndex_(vk::QueueFamilyIgnored)
    {
    };
};

// Represents current buffer state.
struct VulkanBufferState
{
    vk::PipelineStageFlags2 stage_;
    vk::AccessFlags2 access_;
    int32_t queueFamilyIndex_;

    // Standard constructor.
    VulkanBufferState(vk::PipelineStageFlags2 stage, vk::AccessFlags2 access, int32_t queueFamilyIndex) :
        stage_(stage),
        access_(access),
        queueFamilyIndex_(queueFamilyIndex)
    {
    };

    // No queue family index is set, so it will default to vk::QueueFamilyIgnored.
    VulkanBufferState(vk::PipelineStageFlags2 stage, vk::AccessFlags2 access) :
        stage_(stage),
        access_(access),
        queueFamilyIndex_(vk::QueueFamilyIgnored)
    {
    };
};

// Properties that are required to create new buffer.
struct VulkanBufferProperties
{
    VulkanBufferUseCase bufferUseCase_;

    vk::DeviceSize deviceSize_;
    vk::BufferUsageFlags bufferUsageFlags_;

    VmaMemoryUsage vmaMemoryUsage_;
    VmaAllocationCreateFlags vmaAllocationCreateFlags_;

    vk::SharingMode sharingMode_;
    std::vector<std::uint32_t> queueFamilyIndexes_;

    // Sharing mode vk::SharingMode::eExclusive.
    // vk::SharingMode::eExclusive does not require queue indexes.
    VulkanBufferProperties(VulkanBufferUseCase bufferUseCase, vk::DeviceSize deviceSize, vk::BufferUsageFlags bufferUsageFlags, VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationCreateFlags, vk::SharingMode sharingMode) :
        bufferUseCase_(bufferUseCase),
        deviceSize_(deviceSize),
        bufferUsageFlags_(bufferUsageFlags),
        vmaMemoryUsage_(vmaMemoryUsage),
        vmaAllocationCreateFlags_(vmaAllocationCreateFlags),
        sharingMode_(sharingMode)
    {     
        if (deviceSize == 0)
        {
            throw std::runtime_error("Exception: Size of buffer cannot be 0.");
        }

        if (sharingMode != vk::SharingMode::eExclusive)
        {
            throw std::runtime_error("Exception: Constructor requires sharing mode to be vk::SharingMode::eExclusive.");
        }
    };

    // Sharing mode vk::SharingMode::eExclusive.
    // vk::SharingMode::eConcurrent requires list of queue indexes.
    VulkanBufferProperties(VulkanBufferUseCase bufferUseCase, vk::DeviceSize deviceSize, vk::BufferUsageFlags bufferUsageFlags, VmaMemoryUsage vmaMemoryUsage, VmaAllocationCreateFlags vmaAllocationCreateFlags, vk::SharingMode sharingMode, const std::vector<std::uint32_t>& queueFamilyIndexes) :
        bufferUseCase_(bufferUseCase),
        deviceSize_(deviceSize),
        bufferUsageFlags_(bufferUsageFlags),
        vmaMemoryUsage_(vmaMemoryUsage),
        vmaAllocationCreateFlags_(vmaAllocationCreateFlags),
        sharingMode_(sharingMode),
        queueFamilyIndexes_(queueFamilyIndexes)
    {
        if (deviceSize == 0)
        {
            throw std::runtime_error("Exception: Size of buffer cannot be 0.");
        }

        if (sharingMode != vk::SharingMode::eConcurrent)
        {
            throw std::runtime_error("Exception: Constructor requires sharing mode to be vk::SharingMode::eConcurrent.");
        }

        if (queueFamilyIndexes.empty() == true)
        {
            throw std::runtime_error("Exception: vk::SharingMode::eConcurrent constructor requires queueFamilyIndexes to be filled.");
        }
    };
};

// Properties that are required to create a new image.
struct VulkanImageProperties
{
    VulkanImageUseCase imageUseCase_;

    vk::ImageType imageType_;
    vk::Format format_;
    vk::Extent3D extent_;

    std::uint32_t mipLevels_;
    std::uint32_t arrayLayers_;

    vk::SampleCountFlagBits sampleCountFlagBits_;
    vk::ImageTiling imageTiling_;
    vk::ImageUsageFlags imageUsageFlags_;

    VmaMemoryUsage vmaMemoryUsage_;
    VmaAllocationCreateFlags vmaAllocationCreateFlags_;

    vk::SharingMode sharingMode_;
    std::vector<std::uint32_t> queueFamilyIndexes_;

    VulkanImageProperties(VulkanImageUseCase imageUseCase, vk::Format format, vk::Extent3D extent, vk::ImageUsageFlags imageUsageFlags, vk::SharingMode sharingMode, std::uint32_t mipLevels = 1, std::uint32_t arrayLayers = 1, vk::SampleCountFlagBits sampleCountFlagBits = vk::SampleCountFlagBits::e1, vk::ImageTiling imageTiling = vk::ImageTiling::eOptimal, vk::ImageType imageType = vk::ImageType::e2D, VmaMemoryUsage vmaMemoryUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaAllocationCreateFlags = 0) :
        imageUseCase_(imageUseCase),
        imageType_(imageType),
        format_(format),
        extent_(extent),
        mipLevels_(mipLevels),
        arrayLayers_(arrayLayers),
        sampleCountFlagBits_(sampleCountFlagBits),
        imageTiling_(imageTiling),
        imageUsageFlags_(imageUsageFlags),
        vmaMemoryUsage_(vmaMemoryUsage),
        vmaAllocationCreateFlags_(vmaAllocationCreateFlags),
        sharingMode_(sharingMode)
    {
        if (extent.width == 0 || extent.height == 0 || extent.depth == 0)
        {
            throw std::runtime_error( "Exception: Image extent cannot contain dimension with a value of 0.");
        }

        if (mipLevels == 0)
        {
            throw std::runtime_error("Exception: Image must have at least one mip level.");
        }

        if (arrayLayers == 0)
        {
            throw std::runtime_error("Exception: Image must have at least one array layer.");
        }

        if (sharingMode != vk::SharingMode::eExclusive)
        {
            throw std::runtime_error("Exception: Constructor requires sharing mode to be vk::SharingMode::eExclusive.");
        }
    }

    VulkanImageProperties(VulkanImageUseCase imageUseCase, vk::Format format, vk::Extent3D extent, vk::ImageUsageFlags imageUsageFlags, vk::SharingMode sharingMode, std::vector<std::uint32_t>& queueFamilyIndexes, std::uint32_t mipLevels = 1, std::uint32_t arrayLayers = 1, vk::SampleCountFlagBits sampleCountFlagBits = vk::SampleCountFlagBits::e1, vk::ImageTiling imageTiling = vk::ImageTiling::eOptimal, vk::ImageType imageType = vk::ImageType::e2D, VmaMemoryUsage vmaMemoryUsage = VMA_MEMORY_USAGE_AUTO, VmaAllocationCreateFlags vmaAllocationCreateFlags = 0) :
        imageUseCase_(imageUseCase),
        imageType_(imageType),
        format_(format),
        extent_(extent),
        mipLevels_(mipLevels),
        arrayLayers_(arrayLayers),
        sampleCountFlagBits_(sampleCountFlagBits),
        imageTiling_(imageTiling),
        imageUsageFlags_(imageUsageFlags),
        vmaMemoryUsage_(vmaMemoryUsage),
        vmaAllocationCreateFlags_(vmaAllocationCreateFlags),
        sharingMode_(sharingMode),
        queueFamilyIndexes_(queueFamilyIndexes)
    {
        if (extent.width == 0 || extent.height == 0 || extent.depth == 0)
        {
            throw std::runtime_error("Exception: Image extent cannot contain dimension with a value of 0.");
        }

        if (mipLevels == 0)
        {
            throw std::runtime_error("Exception: Image must have at least one mip level.");
        }

        if (arrayLayers == 0)
        {
            throw std::runtime_error("Exception: Image must have at least one array layer.");
        }

        if (sharingMode != vk::SharingMode::eConcurrent)
        {
            throw std::runtime_error("Exception: Constructor requires sharing mode to be vk::SharingMode::eConcurrent.");
        }

        if (queueFamilyIndexes.empty() == true)
        {
            throw std::runtime_error("Exception: vk::SharingMode::eConcurrent constructor requires queueFamilyIndexes to be filled.");
        }
    }
};

// Properties that are required to create a new image view.
struct VulkanImageViewProperties
{
    vk::ImageViewType imageViewType_;
    vk::Format format_;
    vk::ImageAspectFlags imageAspectFlags_;

    std::uint32_t baseMipLevel_;
    std::uint32_t levelCount_;

    std::uint32_t baseArrayLayer_;
    std::uint32_t layerCount_;

    VulkanImageViewProperties(vk::ImageAspectFlags imageAspectFlags, vk::Format format, std::uint32_t baseMipLevel = 0, std::uint32_t levelCount = 1, std::uint32_t baseArrayLayer = 0, std::uint32_t layerCount = 1, vk::ImageViewType imageViewType = vk::ImageViewType::e2D) :
        imageViewType_(imageViewType),
        format_(format),
        imageAspectFlags_(imageAspectFlags),
        baseMipLevel_(baseMipLevel),
        levelCount_(levelCount),
        baseArrayLayer_(baseArrayLayer),
        layerCount_(layerCount)
    {
    }
};
