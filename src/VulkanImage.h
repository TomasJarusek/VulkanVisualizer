
#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanDevice;
class VulkanVMA;
enum class VulkanImageUseCase;
struct VulkanImageProperties;
struct VulkanImageViewProperties;


class VulkanImage
{
private:

    Logger logger_;

    // VMA reference.
    std::reference_wrapper<const VulkanVMA> vma_;

    VulkanImageUseCase imageUseCase_;

    // Vulkan image handle.
    vk::Image image_;
    // Vulkan image view handles.
    std::vector<vk::UniqueImageView> imageViews_;
    
    // VMA allocation handle.
    VmaAllocation vmaAllocation_{};
    // Information about allocation.
    VmaAllocationInfo vmaAllocationInfo_{};

    // Memory host coherence flag.
    bool memoryHostCoherent_ = false;

public:

    // Constructor.
    VulkanImage(const VulkanDevice& device, const VulkanVMA& vma, const std::pair<VulkanImageProperties, std::span<const VulkanImageViewProperties>>& imageAndViewsProperties, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanImage();

    // Copy/move constructor/assignment.
    VulkanImage(const VulkanImage& image) = delete;
    VulkanImage(VulkanImage&& image) noexcept = delete;
    VulkanImage& operator=(const VulkanImage& image) = delete;
    VulkanImage& operator=(VulkanImage&& image) noexcept = delete;

    vk::Image Get() const;
    const std::vector<vk::UniqueImageView>& GetImageViews() const;

    // Checks if the image is mapped.
    bool IsMapped() const;

private:

    // Creates image.
    // Possible inputs:
    // vk::ImageUsageFlags imageUsageFlags_;
    //   vk::ImageUsageFlagBits::eDepthStencilAttachment - Depth attachment.
    // VmaMemoryUsage vmaMemoryUsage_;
    //   VMA_MEMORY_USAGE_AUTO - VMA chooses the optimal memory for an image.
    // vk::SharingMode sharingMode_;
    //   vk::SharingMode::eExclusive - Only one queue family can use the image at a time. List of queue family indexes stays empty.
    //   vk::SharingMode::eConcurrent - Multiple queue families can use the image simultaneously. List of queue family indexes must be provided.
    void CreateImage(const VulkanVMA& vma, const VulkanImageProperties& imageProperties);

    // Destroys image.
    void DestroyImage();

    // Determines if the memory provided by VMA is coherent.
    void DetermineMemoryCoherence();

    // Creates image views.
    // Possible inputs:
    // vk::ImageViewType imageViewType_;
    //   vk::ImageAspectFlagBits::eDepth - Depth view.
    void CreateImageViews(const VulkanDevice& device, std::span<const VulkanImageViewProperties> imageViewsProperties);
};
