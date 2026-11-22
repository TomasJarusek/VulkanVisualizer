
#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanInstance;
class VulkanDevice;


// https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/index.html

// VMA simplifies the memory allocation.
// Based on given VMA parameters, these Vulkan steps will be replaced by VMA.
// Create VkBuffer -> get requirements -> find memory type -> allocate VkDeviceMemory -> bind memory.
class VulkanVMA
{
private:

    Logger logger_;

    VmaAllocator vmaAllocator_;

public:

    // Constructor.
    VulkanVMA(const VulkanInstance& instance, const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanVMA();

    // Copy/move constructor/assignment.
    VulkanVMA(const VulkanVMA& vma) = delete;
    VulkanVMA(VulkanVMA&& vma) noexcept = delete;
    VulkanVMA& operator=(const VulkanVMA& vma) = delete;
    VulkanVMA& operator=(VulkanVMA&& vma) noexcept = delete;

    VmaAllocator Get() const;

    std::string ToString(VmaMemoryUsage vmaMemoryUsage) const;
    std::string ToString(VmaAllocationCreateFlags vmaAllocationCreateFlags) const;

private:

    void CreateAllocator(const VulkanInstance& instance, const VulkanDevice& device);

    void DestroyAllocator();
};
