
#include "VulkanVMA.h"

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#include "VulkanConfig.h"
#include "VulkanInstance.h"
#include "VulkanDevice.h"


VulkanVMA::VulkanVMA(const VulkanInstance& instance, const VulkanDevice& device, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_)
{
    LOG_DBG(this->logger_) << "Creating Vulkan Memory Allocator." << std::endl;

    this->CreateAllocator(instance, device);

    LOG_DBG(this->logger_) << "Vulkan Memory Allocator created." << std::endl;
}

VulkanVMA::~VulkanVMA()
{
    this->DestroyAllocator();

    LOG_DBG(this->logger_) << "Vulkan Memory Allocator destroyed." << std::endl;
}

VmaAllocator VulkanVMA::Get() const
{
    return this->vmaAllocator_;
}

std::string VulkanVMA::ToString(VmaMemoryUsage vmaMemoryUsage) const
{
    switch (vmaMemoryUsage)
    {
        case VMA_MEMORY_USAGE_UNKNOWN:
            return "Unknown";

        case VMA_MEMORY_USAGE_AUTO:
            return "Auto";

        case VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE:
            return "AutoPreferDevice";

        case VMA_MEMORY_USAGE_AUTO_PREFER_HOST:
            return "AutoPreferHost";

        case VMA_MEMORY_USAGE_GPU_ONLY:
            return "GpuOnly";

        case VMA_MEMORY_USAGE_CPU_ONLY:
            return "CpuOnly";

        case VMA_MEMORY_USAGE_CPU_TO_GPU:
            return "CpuToGpu";

        case VMA_MEMORY_USAGE_GPU_TO_CPU:
            return "GpuToCpu";

        default:
            return "Unknown";
    }
}

std::string VulkanVMA::ToString(VmaAllocationCreateFlags vmaAllocationCreateFlags) const
{
    std::string result = "{";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_MAPPED_BIT)
        result += " Mapped |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT)
        result += " HostAccessSequentialWrite |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT)
        result += " HostAccessRandom |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_HOST_ACCESS_ALLOW_TRANSFER_INSTEAD_BIT)
        result += " HostAccessAllowTransferInstead |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT)
        result += " DedicatedMemory |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_NEVER_ALLOCATE_BIT)
        result += " NeverAllocate |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_WITHIN_BUDGET_BIT)
        result += " WithinBudget |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_CAN_ALIAS_BIT)
        result += " CanAlias |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT)
        result += " StrategyMinMemory |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_STRATEGY_MIN_TIME_BIT)
        result += " StrategyMinTime |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_STRATEGY_MIN_OFFSET_BIT)
        result += " StrategyMinOffset |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_STRATEGY_BEST_FIT_BIT)
        result += " StrategyBestFit |";

    if (vmaAllocationCreateFlags & VMA_ALLOCATION_CREATE_STRATEGY_FIRST_FIT_BIT)
        result += " StrategyFirstFit |";

    if (result.size() > 1)
    {
        result.back() = '}';
    }
    else
    {
        result = "{}";
    }

    return result;
}

void VulkanVMA::CreateAllocator(const VulkanInstance& instance, const VulkanDevice& device)
{
    // VMA uses C style API.

    VmaVulkanFunctions vmaVulkanFunctions{};

    // VMA needs addresses of vkGetInstanceProcAddr() and vkGetDeviceProcAddr() functions.
    // Vulkan was dynamically loaded in this project, so the pointers are stored in the dispatcher object.
    const auto& dispatcher = VULKAN_HPP_DEFAULT_DISPATCHER;

    vmaVulkanFunctions.vkGetInstanceProcAddr = dispatcher.vkGetInstanceProcAddr;
    vmaVulkanFunctions.vkGetDeviceProcAddr = dispatcher.vkGetDeviceProcAddr;

    VmaAllocatorCreateInfo vmaAllocatorCreateInfo{};

    vmaAllocatorCreateInfo.instance = instance.Get();
    vmaAllocatorCreateInfo.physicalDevice = device.GetPhysicalDevice();
    vmaAllocatorCreateInfo.device = device.Get();
    vmaAllocatorCreateInfo.pVulkanFunctions = &vmaVulkanFunctions;

    const VkResult result = vmaCreateAllocator(&vmaAllocatorCreateInfo, &this->vmaAllocator_);

    if (result != VK_SUCCESS)
    { 
        throw std::runtime_error("Exception: Failed to create Vulkan Memory Allocator.");
    }
}

void VulkanVMA::DestroyAllocator()
{
    vmaDestroyAllocator(this->vmaAllocator_);
}
