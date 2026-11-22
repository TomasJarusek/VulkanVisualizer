
#include "VulkanImage.h"

#include <vma/vk_mem_alloc.h>

#include <format>

#include "VulkanConfig.h"
#include "VulkanDevice.h"
#include "VulkanVMA.h"
#include "VulkanHelperEnums.h"
#include "VulkanHelperStructs.h"
#include "VulkanHelperFunctions.h"


VulkanImage::VulkanImage(const VulkanDevice& device, const VulkanVMA& vma, const std::pair<VulkanImageProperties, std::span<const VulkanImageViewProperties>>& imageAndViewsProperties, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    vma_(vma),
    imageUseCase_(imageAndViewsProperties.first.imageUseCase_)
{
    const VulkanImageProperties& imageProperties = imageAndViewsProperties.first;

    LOG_DBG(this->logger_) << "Creating image. "
        << "imageUseCase <" << GetPrintableValue(imageProperties.imageUseCase_) << ">, "
        << "imageType <" << GetPrintableValue(imageProperties.imageType_) << ">, "
        << "format <" << GetPrintableValue(imageProperties.format_) << ">, "
        << "extent <" << imageProperties.extent_.width << ", " << imageProperties.extent_.height << ", " << imageProperties.extent_.depth << ">, "
        << "mipLevels <" << imageProperties.mipLevels_ << ">, "
        << "arrayLayers <" << imageProperties.arrayLayers_ << ">, "
        << "sampleCountFlagBits <" << GetPrintableValue(imageProperties.sampleCountFlagBits_) << ">, "
        << "imageTiling <" << GetPrintableValue(imageProperties.imageTiling_) << ">, "
        << "imageUsageFlags <" << GetPrintableValue(imageProperties.imageUsageFlags_) << ">, "
        << "vmaMemoryUsage <" << vma_.get().ToString(imageProperties.vmaMemoryUsage_) << ">, "
        << "vmaAllocationCreateFlagBits <" << vma_.get().ToString(imageProperties.vmaAllocationCreateFlags_) << ">, "
        << "sharingMode <" << GetPrintableValue(imageProperties.sharingMode_) << ">, "
        << "queueFamilyIndexes <" << std::format("{}", imageProperties.queueFamilyIndexes_) << ">" << std::endl;

    const std::span<const VulkanImageViewProperties>& imageViewsProperties = imageAndViewsProperties.second;

    for (const VulkanImageViewProperties& imageViewProperties : imageViewsProperties)
    {
        LOG_DBG(this->logger_) << "  " << "Creating image view. "
            << "imageViewType <" << GetPrintableValue(imageViewProperties.imageViewType_) << ">, "
            << "format <" << GetPrintableValue(imageViewProperties.format_) << ">, "
            << "imageAspectFlags <" << GetPrintableValue(imageViewProperties.imageAspectFlags_) << ">, "
            << "baseMipLevel <" << imageViewProperties.baseMipLevel_ << ">, "
            << "levelCount <" << imageViewProperties.levelCount_ << ">, "
            << "baseArrayLayer <" << imageViewProperties.baseArrayLayer_ << ">, "
            << "layerCount <" << GetPrintableValue(imageViewProperties.layerCount_) << ">" << std::endl;
    }

    this->CreateImage(vma, imageProperties);

    // Unlikely that CPU would ever access image directly in host memory. Functionality still kept though.
    this->DetermineMemoryCoherence();


    this->CreateImageViews(device, imageViewsProperties);

    LOG_DBG(this->logger_) << "Image and views created. " << std::endl;
}

VulkanImage::~VulkanImage()
{
    // Views must be destroyed first.
    // They are represented by unique handles in a vector, so clear forces destruction.
    this->imageViews_.clear();

    this->DestroyImage();

    LOG_DBG(this->logger_) << "Image and views destroyed. " << std::endl;
}

vk::Image VulkanImage::Get() const
{
    return this->image_;
}

const std::vector<vk::UniqueImageView>& VulkanImage::GetImageViews() const
{
    return this->imageViews_;
}

bool VulkanImage::IsMapped() const
{
    return this->vmaAllocationInfo_.pMappedData != nullptr;
}

void VulkanImage::CreateImage(const VulkanVMA& vma, const VulkanImageProperties& imageProperties)
{
    VmaAllocationCreateInfo vmaAllocationCreateInfo{};

    vmaAllocationCreateInfo.usage = imageProperties.vmaMemoryUsage_;
    vmaAllocationCreateInfo.flags = imageProperties.vmaAllocationCreateFlags_;


    vk::ImageCreateInfo imageCreateInfo{};

    imageCreateInfo
        .setImageType(imageProperties.imageType_)
        .setFormat(imageProperties.format_)
        .setExtent(imageProperties.extent_)
        .setMipLevels(imageProperties.mipLevels_)
        .setArrayLayers(imageProperties.arrayLayers_)
        .setSamples(imageProperties.sampleCountFlagBits_)
        .setTiling(imageProperties.imageTiling_)
        .setUsage(imageProperties.imageUsageFlags_)
        .setSharingMode(imageProperties.sharingMode_)
        .setQueueFamilyIndices(imageProperties.queueFamilyIndexes_);

    VkImageCreateInfo imageCreateInfoC = static_cast<VkImageCreateInfo>(imageCreateInfo);


    VkImage image;

    VkResult result = vmaCreateImage(vma.Get(), &imageCreateInfoC, &vmaAllocationCreateInfo, &image, &this->vmaAllocation_, &this->vmaAllocationInfo_);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Exception: Error during VMA image creation.");
    }

    this->image_ = static_cast<vk::Image>(image);
}

void VulkanImage::DestroyImage()
{
    vmaDestroyImage(this->vma_.get().Get(), this->image_, this->vmaAllocation_);
}

void VulkanImage::CreateImageViews(const VulkanDevice& device, std::span<const VulkanImageViewProperties> imageViewsProperties)
{
    this->imageViews_.reserve(imageViewsProperties.size());

    for (const VulkanImageViewProperties& imageViewProperties : imageViewsProperties)
    {
        vk::ImageSubresourceRange imageSubresourceRange{};

        imageSubresourceRange
            .setAspectMask(imageViewProperties.imageAspectFlags_)
            .setBaseMipLevel(imageViewProperties.baseMipLevel_)
            .setLevelCount(imageViewProperties.levelCount_)
            .setBaseArrayLayer(imageViewProperties.baseArrayLayer_)
            .setLayerCount(imageViewProperties.layerCount_);

        vk::ImageViewCreateInfo imageViewCreateInfo{};

        imageViewCreateInfo
            .setImage(this->image_)
            .setViewType(imageViewProperties.imageViewType_)
            .setFormat(imageViewProperties.format_)
            .setSubresourceRange(imageSubresourceRange);

        this->imageViews_.push_back(device.Get().createImageViewUnique(imageViewCreateInfo));
    }
}

void VulkanImage::DetermineMemoryCoherence()
{
    VkMemoryPropertyFlags memoryPropertyFlags{};

    vmaGetMemoryTypeProperties(this->vma_.get().Get(), this->vmaAllocationInfo_.memoryType, &memoryPropertyFlags);

    this->memoryHostCoherent_ = (memoryPropertyFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
}
