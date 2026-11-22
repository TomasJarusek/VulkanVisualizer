
#pragma once

#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanInstance;
class VulkanSurface;


class VulkanDevice
{
private:

    Logger logger_;

    bool showAvailablePhysicalDevices_ = true;
    uint32_t minimumVulkanVersion_ = vk::makeVersion(1, 0, 0);

    vk::SampleCountFlagBits requiredMSAASampleCount_ = vk::SampleCountFlagBits::e1;
    bool sampleRateShadingEnabled_ = false;

    std::vector<std::string> requiredPhysicalDeviceExtensionNames_ = {vk::KHRSwapchainExtensionName};
    std::vector<const char*> requiredPhysicalDeviceExtensionNamesCStr_ = {vk::KHRSwapchainExtensionName};

    // Holds additional information about physical device.
    struct PhysicalDeviceInfo
    {
        // Basic info.
        std::string name_;
        uint32_t vulkanVersion_ = vk::makeVersion(1, 0, 0);

        // Extensions, features and properties.
        std::vector<vk::ExtensionProperties> extensionProperties_;
        vk::PhysicalDeviceFeatures physicalDeviceFeatures_;
        vk::PhysicalDeviceProperties physicalDeviceProperties_;

        // Queues info.
        uint32_t graphicsQueueFamilyIndex_ = 0;
        uint32_t presentationQueueFamilyIndex_ = 0;
        uint32_t transferQueueFamilyIndex_ = 0;

        // Available capabilities.
        vk::SurfaceCapabilitiesKHR surfaceCapabilities_{};
        std::vector<vk::SurfaceFormatKHR> surfaceFormats_;
        std::vector<vk::PresentModeKHR> presentModes_;
    };

    // Surface reference.
    std::reference_wrapper<const VulkanSurface> surface_;

    // Physical device handle.
    vk::PhysicalDevice physicalDevice_;
    // Its retrieved properties.
    PhysicalDeviceInfo physicalDeviceInfo_;

    // Device.
    vk::UniqueDevice device_;

    // Retrieved queues.
    vk::Queue graphicsQueue_;
    vk::Queue presentationQueue_;
    vk::Queue transferQueue_;

public:

    // Constructor.
    VulkanDevice(const VulkanInstance& instance, const VulkanSurface& surface, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanDevice();

    // Copy/move constructor/assignment.
    VulkanDevice(const VulkanDevice& device) = delete;
    VulkanDevice(VulkanDevice&& device) noexcept = delete;
    VulkanDevice& operator=(const VulkanDevice& device) = delete;
    VulkanDevice& operator=(VulkanDevice&& device) noexcept = delete;

    vk::PhysicalDevice GetPhysicalDevice() const;
    const VulkanDevice::PhysicalDeviceInfo& GetPhysicalDeviceInfo() const;

    vk::Device Get() const;

    bool DoGraphicsAndPresentationQueuesShareQueueFamily() const;
    bool DoTransferAndGraphicsQueuesShareQueueFamily() const;

    uint32_t GetGraphicsQueueFamilyIndex() const;
    uint32_t GetPresentationQueueFamilyIndex() const;
    uint32_t GetTransferQueueFamilyIndex() const;

    vk::Queue GetGraphicsQueue() const;
    vk::Queue GetTransferQueue() const;
    vk::Queue GetPresentationQueue() const;

    void WaitIdle();

    // Checks and updates surface related physical device requirements.
    // Used during a swapchain recreation.
    void RecheckSurfaceRelatedRequirements();

    // Takes in a list of formats. The formats are iterated over in order.
    // Returns first format that supports every required flag for a given tiling.
    // Otherwise vk::Format::eUndefined is returned. 
    vk::Format GetSupportedFormat(std::span<const vk::Format> acceptableFormats, vk::ImageTiling usedImageTiling, vk::FormatFeatureFlags requiredFormatFeatureFlags) const;

private:

    std::string ApiVersionToString(uint32_t apiVersion) const;
    std::string VendorIDToString(uint32_t vendorID) const;

    void ShowAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& physicalDevices, const VulkanSurface& surface) const;

    void RetrievePhysicalDeviceVulkanVersion(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice) const;
    bool DoesPhysicalDeviceSupportRequiredVulkanVersion(const PhysicalDeviceInfo& physicalDeviceInfo) const;

    std::optional<uint32_t> FindQueueFamilySatisfyingRequirements(vk::PhysicalDevice physicalDevice, const VulkanSurface& surface, std::optional<bool> supportsGraphics, std::optional<bool> supportsTransfer, std::optional<bool> supportsCompute, std::optional<bool> supportsPresentation) const;

    bool RetrieveNecessaryQueueFamilyIndexes(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const;

    void RetrieveExtensionProperties(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice) const;
    bool IsEveryRequiredExtensionSupported(const PhysicalDeviceInfo& physicalDeviceInfo) const;

    void RetrievePhysicalDeviceFeatures(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice) const;
    bool IsEveryPhysicalDeviceFeatureSupported(const PhysicalDeviceInfo& physicalDeviceInfo) const;

    void RetrieveSurfaceCapabilities(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const;

    void RetrieveSurfaceFormats(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const;
    bool IsAnySurfaceFormatAvailable(const PhysicalDeviceInfo& physicalDeviceInfo) const;

    void RetrievePresentModes(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const;
    bool IsAnyPresentModeAvailable(const PhysicalDeviceInfo& physicalDeviceInfo) const;

    void RetrievePhysicalDeviceProperties(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice) const;
    bool DoesPhysicalDeviceSupportRequiredMSAASampleCount(const PhysicalDeviceInfo& physicalDeviceInfo) const;
    bool DoesPhysicalDeviceSupportSampleRateShading(const PhysicalDeviceInfo& physicalDeviceInfo) const;

    // Logical grouping of surface related checks. Meant to be reused during surface recreation.
    bool CheckSurfaceRelatedRequirements(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const;

    void PickPhysicalDevice(const VulkanInstance& instance, const VulkanSurface& surface);

    void CreateDevice();
};
