
#include "VulkanDevice.h"

#include <vulkan/vk_enum_string_helper.h>

#include <ranges>
#include <unordered_set>

#include "VulkanConfig.h"
#include "VulkanInstance.h"
#include "VulkanSurface.h"
#include "VulkanHelperFunctions.h"


VulkanDevice::VulkanDevice(const VulkanInstance& instance, const VulkanSurface& surface, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    minimumVulkanVersion_(visualizerConfig.deviceConfig_.minimumVulkanVersion_),
    showAvailablePhysicalDevices_(visualizerConfig.deviceConfig_.showAvailablePhysicalDevices_),
    requiredMSAASampleCount_(visualizerConfig.deviceConfig_.requiredMSAASampleCount_),
    sampleRateShadingEnabled_(visualizerConfig.deviceConfig_.sampleRateShadingEnabled_),
    surface_(surface)
{
    LOG_DBG(this->logger_) << "Creating device. "
        << "minimumVulkanVersion <" << this->ApiVersionToString(this->minimumVulkanVersion_) << ">, "
        << "showAvailablePhysicalDevices <" << std::format("{}", this->showAvailablePhysicalDevices_) << ">, "
        << "requiredMSAASampleCount <" << GetPrintableValue(this->requiredMSAASampleCount_) << ">, "
        << "sampleRateShadingEnabled <" <<  std::format("{}", this->sampleRateShadingEnabled_) << ">" << std::endl;

    this->PickPhysicalDevice(instance, surface);

    this->CreateDevice();

    LOG_DBG(this->logger_) << "Device created. " << std::endl;
}

VulkanDevice::~VulkanDevice()
{
    LOG_DBG(this->logger_) << "Device destroyed. " << std::endl;
}

vk::PhysicalDevice VulkanDevice::GetPhysicalDevice() const
{
    return this->physicalDevice_;
}

const VulkanDevice::PhysicalDeviceInfo& VulkanDevice::GetPhysicalDeviceInfo() const
{
    return this->physicalDeviceInfo_;
}

vk::Device VulkanDevice::Get() const
{
    return this->device_.get();
}

bool VulkanDevice::DoGraphicsAndPresentationQueuesShareQueueFamily() const
{
    return this->physicalDeviceInfo_.graphicsQueueFamilyIndex_ == this->physicalDeviceInfo_.presentationQueueFamilyIndex_;
}

bool VulkanDevice::DoTransferAndGraphicsQueuesShareQueueFamily() const
{
    return this->physicalDeviceInfo_.transferQueueFamilyIndex_ == this->physicalDeviceInfo_.graphicsQueueFamilyIndex_;
}

uint32_t VulkanDevice::GetGraphicsQueueFamilyIndex() const
{
    return this->physicalDeviceInfo_.graphicsQueueFamilyIndex_;
}

uint32_t VulkanDevice::GetPresentationQueueFamilyIndex() const
{
    return this->physicalDeviceInfo_.presentationQueueFamilyIndex_;
}

uint32_t VulkanDevice::GetTransferQueueFamilyIndex() const
{
    return this->physicalDeviceInfo_.transferQueueFamilyIndex_;
}

vk::Queue VulkanDevice::GetGraphicsQueue() const
{
    return this->graphicsQueue_;
}

vk::Queue VulkanDevice::GetTransferQueue() const
{
    return this->transferQueue_;
}

vk::Queue VulkanDevice::GetPresentationQueue() const
{
    return this->presentationQueue_;
}

void VulkanDevice::WaitIdle()
{
    this->device_.get().waitIdle();
}

void VulkanDevice::RecheckSurfaceRelatedRequirements()
{
    // getSurfaceSupportKHR (related to a presentation queue) does not have to be rechecked. It should not be possible
    // for it to change based on vk::Result::eErrorOutOfDateKHR or vk::Result::eSuboptimalKHR.

    // During recreation:
    // physicalDeviceInfo that already exists for the chosen physical device will be updated.
    // Picked physical device won't change, so the chosen one is used.
    // vk::Surface changed internally, but the reference is still valid.
    if (this->CheckSurfaceRelatedRequirements(this->physicalDeviceInfo_, this->physicalDevice_, this->surface_) == false)
    {
        throw std::runtime_error("Exception: Physical device cannot support the new surface related requirements.");
    }
}

vk::Format VulkanDevice::GetSupportedFormat(std::span<const vk::Format> acceptableFormats, vk::ImageTiling usedImageTiling, vk::FormatFeatureFlags requiredFormatFeatureFlags) const
{
    for (const vk::Format& format : acceptableFormats)
    {
        vk::FormatProperties formatProperties = this->physicalDevice_.getFormatProperties(format);

        if (((usedImageTiling == vk::ImageTiling::eLinear) && ((formatProperties.linearTilingFeatures & requiredFormatFeatureFlags) == requiredFormatFeatureFlags)) ||
            ((usedImageTiling == vk::ImageTiling::eOptimal) && ((formatProperties.optimalTilingFeatures & requiredFormatFeatureFlags) == requiredFormatFeatureFlags)))
        {
            return format;
        }
    }

    return vk::Format::eUndefined;
}

std::string VulkanDevice::ApiVersionToString(uint32_t apiVersion) const
{
    return std::to_string(vk::apiVersionMajor(apiVersion)) + "." + std::to_string(vk::apiVersionMinor(apiVersion)) + "." + std::to_string(vk::apiVersionPatch(apiVersion));
}

std::string VulkanDevice::VendorIDToString(uint32_t vendorID) const
{
    switch (vendorID)
    {
        case 0x10DE:
            return "NVIDIA";
        case 0x1002:
            return "AMD";
        case 0x8086:
            return "Intel";
        case 0x13B5:
            return "ARM";
        case 0x106B:
            return "Apple";
        default:
            return "Unknown";
    }
}

void VulkanDevice::ShowAvailablePhysicalDevices(const std::vector<vk::PhysicalDevice>& availablePhysicalDevices, const VulkanSurface& surface) const
{
    LOG_INF(this->logger_) << "Available physical devices:" << std::endl;
    
    for (const vk::PhysicalDevice& physicalDevice : availablePhysicalDevices)
    {
        // vk::PhysicalDeviceProperties2 is a wrapper around vk::PhysicalDeviceProperties with other structures being available via pNext.
        // By chaining at least two parameters in the function template, the required structures are filled automatically.
        // const auto physicalDeviceProperties = physicalDevice.getProperties2<vk::PhysicalDeviceProperties2,vk::PhysicalDeviceVulkan12Properties>();
        // The base properties are enough for this use case.
        const vk::PhysicalDeviceProperties2 physicalDeviceProperties = physicalDevice.getProperties2();

        LOG_INF(this->logger_) << "  " << physicalDeviceProperties.properties.deviceName << std::endl;
        LOG_INF(this->logger_) << "    " << "API version <" << this->ApiVersionToString(physicalDeviceProperties.properties.apiVersion) << ">" << std::endl;
        LOG_INF(this->logger_) << "    " << "Driver version <" << physicalDeviceProperties.properties.driverVersion << ">" << std::endl;
        LOG_INF(this->logger_) << "    " << "Device ID <" << physicalDeviceProperties.properties.deviceID << ">" << std::endl;
        LOG_INF(this->logger_) << "    " << "Device type <" << GetPrintableValue(physicalDeviceProperties.properties.deviceType) << ">" << std::endl;
        LOG_INF(this->logger_) << "    " << "Vendor ID <" << this->VendorIDToString(physicalDeviceProperties.properties.vendorID) << ">" << std::endl;
        LOG_INF(this->logger_) << "    " << "Color sample counts <" << GetPrintableValue(physicalDeviceProperties.properties.limits.framebufferColorSampleCounts) << ">" << std::endl;
        LOG_INF(this->logger_) << "    " << "Depth sample counts <" << GetPrintableValue(physicalDeviceProperties.properties.limits.framebufferDepthSampleCounts) << ">" << std::endl;
        LOG_INF(this->logger_) << "    " << "Available queue families:" << std::endl;

        for (auto const& [index, family] : std::views::enumerate(physicalDevice.getQueueFamilyProperties()))
        {
            std::string familyInfo = "      " + std::to_string(index) + ": ";

            familyInfo += "queue count <" + std::to_string(family.queueCount) + ">, ";

            familyInfo += "flags <";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics ? GetPrintableValue(vk::QueueFlagBits::eGraphics) + "," : "";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer ? GetPrintableValue(vk::QueueFlagBits::eTransfer) + "," : "";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute ? GetPrintableValue(vk::QueueFlagBits::eCompute) + "," : "";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eSparseBinding) == vk::QueueFlagBits::eSparseBinding ? GetPrintableValue(vk::QueueFlagBits::eSparseBinding) + "," : "";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eProtected) == vk::QueueFlagBits::eProtected ? GetPrintableValue(vk::QueueFlagBits::eProtected) + "," : "";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eVideoDecodeKHR) == vk::QueueFlagBits::eVideoDecodeKHR ? GetPrintableValue(vk::QueueFlagBits::eVideoDecodeKHR) + "," : "";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eVideoEncodeKHR) == vk::QueueFlagBits::eVideoEncodeKHR ? GetPrintableValue(vk::QueueFlagBits::eVideoEncodeKHR) + "," : "";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eOpticalFlowNV) == vk::QueueFlagBits::eOpticalFlowNV ? GetPrintableValue(vk::QueueFlagBits::eOpticalFlowNV) + "," : "";
            familyInfo += (family.queueFlags & vk::QueueFlagBits::eDataGraphARM) == vk::QueueFlagBits::eDataGraphARM ? GetPrintableValue(vk::QueueFlagBits::eDataGraphARM) + "," : "";
            familyInfo += physicalDevice.getSurfaceSupportKHR(static_cast<int32_t>(index), surface.Get()) == vk::True ? "Presentation," : "";
            
            if (familyInfo.back() == ',')
            {
                familyInfo.pop_back();
            }

            familyInfo += ">";

            LOG_INF(this->logger_) << familyInfo << std::endl;
        }

        vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(surface.Get());

        LOG_INF(this->logger_) << "    " << "Surface capabilities:" << std::endl;
        LOG_INF(this->logger_) << "      " << "currentExtent.width <" << surfaceCapabilities.currentExtent.width << ">" << std::endl;
        LOG_INF(this->logger_) << "      " << "currentExtent.height <" << surfaceCapabilities.currentExtent.height << ">" << std::endl;
        LOG_INF(this->logger_) << "      " << "minImageExtent.width <" << surfaceCapabilities.minImageExtent.width << ">" << std::endl;
        LOG_INF(this->logger_) << "      " << "minImageExtent.height <" << surfaceCapabilities.minImageExtent.height << ">" << std::endl;
        LOG_INF(this->logger_) << "      " << "minImageCount <" << surfaceCapabilities.minImageCount << ">" << std::endl;
        LOG_INF(this->logger_) << "      " << "maxImageCount <" << surfaceCapabilities.maxImageCount << ">" << std::endl;

        std::vector<vk::SurfaceFormatKHR> surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface.Get());

        LOG_INF(this->logger_) << "    " << "Available surface formats:" << std::endl;
        for (const vk::SurfaceFormatKHR& surfaceFormat : surfaceFormats)
        {
             LOG_INF(this->logger_) << "      " << GetPrintableValue(surfaceFormat.format) << ", " << GetPrintableValue(surfaceFormat.colorSpace) << std::endl;
        }

        std::vector<vk::PresentModeKHR> presentModes = physicalDevice.getSurfacePresentModesKHR(surface.Get());

        LOG_INF(this->logger_) << "    " << "Available present modes:" << std::endl;
        for (const vk::PresentModeKHR& presentMode : presentModes)
        {
            LOG_INF(this->logger_) << "      " << GetPrintableValue(presentMode) << std::endl;
        }
    }
}

void VulkanDevice::RetrievePhysicalDeviceVulkanVersion(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice) const
{
    physicalDeviceInfo.vulkanVersion_ = physicalDevice.getProperties2().properties.apiVersion;

    LOG_DBG(this->logger_) << "  " << "Vulkan version is <" << this->ApiVersionToString(physicalDeviceInfo.vulkanVersion_) << ">." << std::endl;
}

bool VulkanDevice::DoesPhysicalDeviceSupportRequiredVulkanVersion(const PhysicalDeviceInfo& physicalDeviceInfo) const
{
    return physicalDeviceInfo.vulkanVersion_ >= this->minimumVulkanVersion_;
}

std::optional<uint32_t> VulkanDevice::FindQueueFamilySatisfyingRequirements(vk::PhysicalDevice physicalDevice, const VulkanSurface& surface, std::optional<bool> supportsGraphics, std::optional<bool> supportsTransfer, std::optional<bool> supportsCompute, std::optional<bool> supportsPresentation) const
{
    for (auto const& [index, family] : std::views::enumerate(physicalDevice.getQueueFamilyProperties()))
    {
        if (supportsGraphics.has_value() == true)
        {
            if (!(supportsGraphics.value() == ((family.queueFlags & vk::QueueFlagBits::eGraphics) == vk::QueueFlagBits::eGraphics)))
            {
                continue;
            }
        }

        if (supportsTransfer.has_value() == true)
        {
            if (!(supportsTransfer.value() == ((family.queueFlags & vk::QueueFlagBits::eTransfer) == vk::QueueFlagBits::eTransfer)))
            {
                continue;
            }
        }

        if (supportsCompute.has_value() == true)
        {
            if (!(supportsCompute.value() == ((family.queueFlags & vk::QueueFlagBits::eCompute) == vk::QueueFlagBits::eCompute)))
            {
                continue;
            }
        }

        if (supportsPresentation.has_value() == true)
        {
            if (!(supportsPresentation.value() == (physicalDevice.getSurfaceSupportKHR(static_cast<int32_t>(index), surface.Get()) == vk::True)))
            {
                continue;
            }
        }

        return std::make_optional(static_cast<int32_t>(index));
    }

    return {};
}

bool VulkanDevice::RetrieveNecessaryQueueFamilyIndexes(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const
{
    std::optional<uint32_t> queueFamilyIndex;

    // Graphics queue family.
    // 1) Combined graphics and presentation.
    queueFamilyIndex = this->FindQueueFamilySatisfyingRequirements(physicalDevice, surface, true, {}, {}, true);
    if (queueFamilyIndex.has_value() == true)
    {
        physicalDeviceInfo.graphicsQueueFamilyIndex_ = queueFamilyIndex.value();
        LOG_DBG(this->logger_) << "  " << "Queue family with index <" << queueFamilyIndex.value() << "> is taken as a graphics queue family. (graphics + presentation)" << std::endl;
    }
    else
    {
        // 2) Graphics.
        queueFamilyIndex = this->FindQueueFamilySatisfyingRequirements(physicalDevice, surface, true, {}, {}, {});
        if (queueFamilyIndex.has_value() == true)
        {
            physicalDeviceInfo.graphicsQueueFamilyIndex_ = queueFamilyIndex.value();
            LOG_DBG(this->logger_) << "  " << "Queue family with index <" << queueFamilyIndex.value() << "> is taken as a graphics queue family. (graphics)" << std::endl;
        }
        else
        {
            return false;
        }
    }

    // Presentation queue family.
    // 1) Combined presentation and graphics.
    queueFamilyIndex = this->FindQueueFamilySatisfyingRequirements(physicalDevice, surface, true, {}, {}, true);
    if (queueFamilyIndex.has_value() == true)
    {
        physicalDeviceInfo.presentationQueueFamilyIndex_ = queueFamilyIndex.value();
        LOG_DBG(this->logger_) << "  " << "Queue family with index <" << queueFamilyIndex.value() << "> is taken as a presentation queue family. (presentation + graphics)" << std::endl;
    }
    else
    {
        // 2) Presentation.
        queueFamilyIndex = this->FindQueueFamilySatisfyingRequirements(physicalDevice, surface, {}, {}, {}, true);
        if (queueFamilyIndex.has_value() == true)
        {
            physicalDeviceInfo.presentationQueueFamilyIndex_ = queueFamilyIndex.value();
            LOG_DBG(this->logger_) << "  " << "Queue family with index <" << queueFamilyIndex.value() << "> is taken as a presentation queue family. (presentation)" << std::endl;
        }
        else
        {
            return false;
        }
    }

    // Transfer queue family.
    // 1) Transfer with no compute or graphics.
    queueFamilyIndex = this->FindQueueFamilySatisfyingRequirements(physicalDevice, surface, false, true, false, {});
    if (queueFamilyIndex.has_value() == true)
    {
        physicalDeviceInfo.transferQueueFamilyIndex_ = queueFamilyIndex.value();
        LOG_DBG(this->logger_) << "  " << "Queue family with index <" << queueFamilyIndex.value() << "> is taken as a transfer queue family. (transfer + not graphics + not compute)" << std::endl;
    }
    else
    {
        // 2) Transfer with no graphics.
        queueFamilyIndex = this->FindQueueFamilySatisfyingRequirements(physicalDevice, surface, false, true, {}, {});
        if (queueFamilyIndex.has_value() == true)
        {
            physicalDeviceInfo.transferQueueFamilyIndex_ = queueFamilyIndex.value();
            LOG_DBG(this->logger_) << "  " << "Queue family with index <" << queueFamilyIndex.value() << "> is taken as a transfer queue family. (transfer + not graphics)" << std::endl;
        }
        else
        {
            // 3) Transfer.
            queueFamilyIndex = this->FindQueueFamilySatisfyingRequirements(physicalDevice, surface, {}, true, {}, {});
            if (queueFamilyIndex.has_value() == true)
            {
                physicalDeviceInfo.transferQueueFamilyIndex_ = queueFamilyIndex.value();
                LOG_DBG(this->logger_) << "  " << "Queue family with index <" << queueFamilyIndex.value() << "> is taken as a transfer queue family. (transfer)" << std::endl;
            }
            else
            {
                return false;
            }
        }
    }

    return true;
}

void VulkanDevice::RetrieveExtensionProperties(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice) const
{
    physicalDeviceInfo.extensionProperties_ = physicalDevice.enumerateDeviceExtensionProperties();
}

bool VulkanDevice::IsEveryRequiredExtensionSupported(const PhysicalDeviceInfo& physicalDeviceInfo) const
{
    for (const std::string& requiredExtensionName :this->requiredPhysicalDeviceExtensionNames_)
    {
        if (std::ranges::find_if(physicalDeviceInfo.extensionProperties_, [requiredExtensionName](const auto& extensionProperty){return extensionProperty.extensionName == requiredExtensionName;}) == physicalDeviceInfo.extensionProperties_.end())
        {
            LOG_DBG(this->logger_) << "  " << "Required device extension <" << requiredExtensionName << "> is not supported." << std::endl;

            return false;
        }
    }

    return true;
}

void VulkanDevice::RetrievePhysicalDeviceFeatures(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice) const
{
    physicalDeviceInfo.physicalDeviceFeatures_ = physicalDevice.getFeatures();
}

bool VulkanDevice::IsEveryPhysicalDeviceFeatureSupported([[maybe_unused]] const PhysicalDeviceInfo& physicalDeviceInfo) const
{
    // No requirements so far.

    // if (physicalDeviceInfo.physicalDeviceFeatures_........ == vk::True)
    // {
    //     return true;
    // }
    // else
    // {
    //     return false;
    // }

    return true;
}

void VulkanDevice::RetrieveSurfaceCapabilities(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const
{
    physicalDeviceInfo.surfaceCapabilities_ = physicalDevice.getSurfaceCapabilitiesKHR(surface.Get());
}

void VulkanDevice::RetrieveSurfaceFormats(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const
{
    physicalDeviceInfo.surfaceFormats_ = physicalDevice.getSurfaceFormatsKHR(surface.Get());
}

bool VulkanDevice::IsAnySurfaceFormatAvailable(const PhysicalDeviceInfo& physicalDeviceInfo) const
{
    return physicalDeviceInfo.surfaceFormats_.empty();
}

void VulkanDevice::RetrievePresentModes(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const
{
    physicalDeviceInfo.presentModes_ = physicalDevice.getSurfacePresentModesKHR(surface.Get());
}

bool VulkanDevice::IsAnyPresentModeAvailable(const PhysicalDeviceInfo& physicalDeviceInfo) const
{
    return physicalDeviceInfo.presentModes_.empty();
}

void VulkanDevice::RetrievePhysicalDeviceProperties(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice) const
{
    physicalDeviceInfo.physicalDeviceProperties_ = physicalDevice.getProperties();
}

bool VulkanDevice::DoesPhysicalDeviceSupportRequiredMSAASampleCount(const PhysicalDeviceInfo& physicalDeviceInfo) const
{
    // Bit fields are and-ed together to get valid flag bits for both color and depth.
    vk::SampleCountFlags sampleCountFlags = physicalDeviceInfo.physicalDeviceProperties_.limits.framebufferColorSampleCounts & physicalDeviceInfo.physicalDeviceProperties_.limits.framebufferDepthSampleCounts;

    if ((sampleCountFlags & this->requiredMSAASampleCount_) == this->requiredMSAASampleCount_)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool VulkanDevice::DoesPhysicalDeviceSupportSampleRateShading(const PhysicalDeviceInfo& physicalDeviceInfo) const
{
    return physicalDeviceInfo.physicalDeviceFeatures_.sampleRateShading == vk::True;
}

bool VulkanDevice::CheckSurfaceRelatedRequirements(PhysicalDeviceInfo& physicalDeviceInfo, vk::PhysicalDevice physicalDevice, const VulkanSurface& surface) const
{
    // Surface capabilities are retrieved, no requirements.
    this->RetrieveSurfaceCapabilities(physicalDeviceInfo, physicalDevice, surface);

    // Surface formats are retrieved. Any will suffice for now.
    this->RetrieveSurfaceFormats(physicalDeviceInfo, physicalDevice, surface);
    if (this->IsAnySurfaceFormatAvailable(physicalDeviceInfo) == true)
    {
        LOG_DBG(this->logger_) << "  " << "No surface format is available." << std::endl;
        return false;
    }

    // Present modes are retrieved. Any will suffice for now.
    this->RetrievePresentModes(physicalDeviceInfo, physicalDevice, surface);
    if (this->IsAnyPresentModeAvailable(physicalDeviceInfo) == true)
    {
        LOG_DBG(this->logger_) << "  " << "No present mode is available." << std::endl;
        return false;
    }

    return true;
}

void VulkanDevice::PickPhysicalDevice(const VulkanInstance& instance, const VulkanSurface& surface)
{
    const std::vector<vk::PhysicalDevice> availablePhysicalDevices = instance.GetAvailablePhysicalDevices();

    if (availablePhysicalDevices.empty() == true)
    {
        throw std::runtime_error("Exception: No physical devices are available.");
    }

    if (this->showAvailablePhysicalDevices_ == true)
    {
        this->ShowAvailablePhysicalDevices(availablePhysicalDevices, surface);
    }

    // First, the devices that can't be used will be filtered out.
    std::vector<std::pair<vk::PhysicalDevice, PhysicalDeviceInfo>> suitablePhysicalDevices;

    for (const vk::PhysicalDevice& physicalDevice : availablePhysicalDevices)
    {
        PhysicalDeviceInfo physicalDeviceInfo{};

        physicalDeviceInfo.name_ = physicalDevice.getProperties2().properties.deviceName.data();
        LOG_DBG(this->logger_) << "Physical device <" << physicalDeviceInfo.name_ << "> is being considered." << std::endl;   

        // API version must be at least the minimum required.
        this->RetrievePhysicalDeviceVulkanVersion(physicalDeviceInfo, physicalDevice);
        if (this->DoesPhysicalDeviceSupportRequiredVulkanVersion(physicalDeviceInfo) == false)
        {
            LOG_DBG(this->logger_) << "  " << "Api version is not sufficient." << std::endl;
            continue;
        }

        // All required queue families must be retrieved.
        if (this->RetrieveNecessaryQueueFamilyIndexes(physicalDeviceInfo, physicalDevice, surface) == false)
        {
            LOG_DBG(this->logger_) << "  " << "One or more necessary queue families could not be retrieved." << std::endl;
            continue;
        }

        // All required extensions must be supported.
        this->RetrieveExtensionProperties(physicalDeviceInfo, physicalDevice);
        if (this->IsEveryRequiredExtensionSupported(physicalDeviceInfo) == false)
        {
            LOG_DBG(this->logger_) << "  " << "One or more required device extensions aren't supported." << std::endl;
            continue;
        }

        // All required features must be supported.
        this->RetrievePhysicalDeviceFeatures(physicalDeviceInfo, physicalDevice);
        if (this->IsEveryPhysicalDeviceFeatureSupported(physicalDeviceInfo) == false)
        {
            LOG_DBG(this->logger_) << "  " << "One or more required features aren't supported." << std::endl;
            continue;
        }

        // Surface related checks are logically bundled into a standalone function.
        if (this->CheckSurfaceRelatedRequirements(physicalDeviceInfo, physicalDevice, surface) == false)
        {
            continue;
        }

        // Every property related requirement must be satisfied.
        this->RetrievePhysicalDeviceProperties(physicalDeviceInfo, physicalDevice);
        if (this->DoesPhysicalDeviceSupportRequiredMSAASampleCount(physicalDeviceInfo) == false)
        {
            LOG_DBG(this->logger_) << "  " << "Required MSAA sample count is not supported." << std::endl;
            continue;
        }

        if (this->sampleRateShadingEnabled_ == true && this->DoesPhysicalDeviceSupportSampleRateShading(physicalDeviceInfo) == false)
        {
            LOG_DBG(this->logger_) << "  " << "Sample shading is not supported." << std::endl;
            continue;
        }

        suitablePhysicalDevices.emplace_back(physicalDevice, physicalDeviceInfo);
    }

    if (suitablePhysicalDevices.empty() == true)
    {
        throw std::runtime_error("Exception: No suitable physical devices are available.");
    }

    // Then, the suitable devices are sorted.
    // Discrete device has priority.
    // If there are multiple, then the one with the highest Vulkan version will be picked.
    auto physicalDeviceComparator = [](const std::pair<vk::PhysicalDevice, PhysicalDeviceInfo>& firstInfo, const std::pair<vk::PhysicalDevice, PhysicalDeviceInfo>& secondInfo)
    {
        if (firstInfo.second.physicalDeviceProperties_.deviceType == secondInfo.second.physicalDeviceProperties_.deviceType)
        {
            return firstInfo.second.vulkanVersion_ >= secondInfo.second.vulkanVersion_;
        }
        else
        {
            if (firstInfo.second.physicalDeviceProperties_.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return true;
            }
            else if (secondInfo.second.physicalDeviceProperties_.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                return false;
            }
            else
            {
                return false;
            }
        }
    };

    std::ranges::sort(suitablePhysicalDevices, physicalDeviceComparator);

    // Picked physical device and its info will be stored.
    this->physicalDevice_ = suitablePhysicalDevices.front().first;
    this->physicalDeviceInfo_ = suitablePhysicalDevices.front().second;

    LOG_DBG(this->logger_) << "Physical device <" << this->physicalDeviceInfo_.name_ << "> was picked." << std::endl;
}

void VulkanDevice::CreateDevice()
{
    // Unique queue family indexes.
    const std::unordered_set<uint32_t> uniqueQueueFamilyIndexes{this->physicalDeviceInfo_.graphicsQueueFamilyIndex_, this->physicalDeviceInfo_.presentationQueueFamilyIndex_, this->physicalDeviceInfo_.transferQueueFamilyIndex_};

    // For each unique queue family index, a new queue will be created.
    // Multiple queues are not necessary, one will suffice.
    // Even if for example graphics and presentation share a queue family, only one queue will be created.
    // Both the graphics and presentation handles will refer to the same queue.
    const std::array<float, 1> queuePriorities = {1.0f};
    std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfos;

    for (const auto& queueFamilyIndex : uniqueQueueFamilyIndexes)
    {
        vk::DeviceQueueCreateInfo deviceQueueCreateInfo{};

        deviceQueueCreateInfo
            .setQueueFamilyIndex(queueFamilyIndex)
            .setQueueCount(static_cast<uint32_t>(queuePriorities.size()))
            .setQueuePriorities(queuePriorities);

        deviceQueueCreateInfos.push_back(deviceQueueCreateInfo);
    }

    // Base physical device features.
    // Might be necessary to check if they are supported!
    vk::PhysicalDeviceFeatures physicalDeviceFeatures{};

    physicalDeviceFeatures
        .setSampleRateShading(static_cast<vk::Bool32>(this->sampleRateShadingEnabled_));

    // Synchronization features.
    // Support is implicitly checked by a Vulkan version condition (>= 1.3).
    vk::PhysicalDeviceSynchronization2Features physicalDeviceSynchronization2Features{};

    physicalDeviceSynchronization2Features
        .setSynchronization2(vk::True);

    // Dynamic rendering features.
    // Support is implicitly checked by a Vulkan version condition (>= 1.3).
    vk::PhysicalDeviceDynamicRenderingFeatures physicalDeviceDynamicRenderingFeatures{};

    physicalDeviceDynamicRenderingFeatures
        .setDynamicRendering(vk::True)
        .setPNext(&physicalDeviceSynchronization2Features);

    // Device creation.
    vk::DeviceCreateInfo deviceCreateInfo{};

    deviceCreateInfo
        .setPEnabledExtensionNames(this->requiredPhysicalDeviceExtensionNamesCStr_)
        .setQueueCreateInfos(deviceQueueCreateInfos)
        .setPEnabledFeatures(&physicalDeviceFeatures)
        .setPNext(&physicalDeviceDynamicRenderingFeatures);

    this->device_ = this->physicalDevice_.createDeviceUnique(deviceCreateInfo);

    // Initialization of device related function pointers of Vulkan functions.
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*(this->device_));

    // Queue handles extraction.
    // No destruction necessary, they implicitly belong to the device.
    // One queue per family as mentioned during setup -> zeroth index is used each time.
    this->graphicsQueue_ = this->device_->getQueue(this->physicalDeviceInfo_.graphicsQueueFamilyIndex_, 0);
    this->presentationQueue_ = this->device_->getQueue(this->physicalDeviceInfo_.presentationQueueFamilyIndex_, 0);
    this->transferQueue_ = this->device_->getQueue(this->physicalDeviceInfo_.transferQueueFamilyIndex_, 0);
}
