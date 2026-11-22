
#pragma once

#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanVMA;
enum class VulkanBufferUseCase;
struct VulkanBufferProperties;


class VulkanBuffer
{
private:

    Logger logger_;

    // VMA reference.
    std::reference_wrapper<const VulkanVMA> vma_;

    VulkanBufferUseCase bufferUseCase_;

    // Vulkan buffer handle.
    vk::Buffer buffer_;
    // VMA allocation handle.
    VmaAllocation vmaAllocation_{};
    // Information about allocation.
    VmaAllocationInfo vmaAllocationInfo_{};

    // Memory host coherence flag.
    bool memoryHostCoherent_ = false;

public:

    // Constructor.
    VulkanBuffer(const VulkanVMA& vma, const VulkanBufferProperties& bufferProperties, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanBuffer();

    // Copy/move constructor/assignment.
    VulkanBuffer(const VulkanBuffer& buffer) = delete;
    VulkanBuffer(VulkanBuffer&& buffer) noexcept = delete;
    VulkanBuffer& operator=(const VulkanBuffer& buffer) = delete;
    VulkanBuffer& operator=(VulkanBuffer&& buffer) noexcept = delete;

    vk::Buffer Get() const;

    // Writes data into a buffer.
    template <typename T>
    void WriteData(std::span<const T> data, vk::DeviceSize offset = 0);

    // Checks if the buffer is mapped.
    bool IsMapped() const;

private:

    // Creates buffer.
    // Possible inputs:
    // vk::BufferUsageFlags bufferUsageFlags_;
    //   vk::BufferUsageFlagBits::eTransferSrc - Source of a transfer operation.
    //   vk::BufferUsageFlagBits::eTransferDst - Destination of a transfer operation.
    //   vk::BufferUsageFlagBits::eVertexBuffer - Vertex buffer.
    //   vk::BufferUsageFlagBits::eIndexBuffer - Index buffer.
    //   vk::BufferUsageFlagBits::eUniformBuffer - Uniform buffer.
    // VmaMemoryUsage vmaMemoryUsage_;
    //   VMA_MEMORY_USAGE_AUTO_PREFER_HOST - Host accessible memory.
    //   VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE - Device memory.
    // VmaAllocationCreateFlagBits vmaAllocationCreateFlagBits_;
    //   VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT - Sequential write of data into the buffer. Used for example with memcpy.
    //   VMA_ALLOCATION_CREATE_MAPPED_BIT - Memory will be persistently mapped.
    // vk::SharingMode sharingMode_;
    //   vk::SharingMode::eExclusive - Only one queue family can use the buffer at a time. List of queue family indexes stays empty.
    //   vk::SharingMode::eConcurrent - Multiple queue families can use the buffer simultaneously. List of queue family indexes must be provided.
    void CreateBuffer(const VulkanVMA& vma, const VulkanBufferProperties& bufferProperties);

    // Destroys buffer.
    void DestroyBuffer();

    // Determines if the memory provided by VMA is coherent.
    void DetermineMemoryCoherence();
};

template <typename T>
void VulkanBuffer::WriteData(std::span<const T> data, vk::DeviceSize offset)
{
    // No alignment is taken into account here. Bytes are manipulated exclusively.

    const size_t size = data.size_bytes();

    // void* should't be altered with pointer arithmetic. Cast to std::byte* is used.
    std::memcpy(static_cast<std::byte*>(this->vmaAllocationInfo_.pMappedData) + offset, data.data(), size);
 
    if (this->memoryHostCoherent_ == false)
    {
        vmaFlushAllocation(this->vma_.get().Get(), this->vmaAllocation_, offset, size);
    }
}
