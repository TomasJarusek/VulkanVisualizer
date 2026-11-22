
#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

#include "Logger.h"


class VulkanVisualizerConfig;


class VulkanInstance
{
private:

    Logger logger_;

    std::string applicationName_ = "Unnamed";
    uint32_t minimumVulkanVersion_ = vk::makeVersion(1, 0, 0);

    // Logging flags.
    bool showMessengerInfoMessages_ = false;
    bool showMessengerVerboseMessages_ = false;
    bool showAvailableLayersAndExtensions_ = false;

    // Validation layers flags.
    bool enableValidationLayers_ = false;
    bool useLayerSettings_ = false;

    // Layers.
    std::vector<vk::LayerProperties> availableLayers_;
    std::vector<std::string> requiredLayerNames_;
    std::vector<const char*> requiredLayerNamesCStr_;

    // Extensions.
    std::vector<vk::ExtensionProperties> availableExtensions_;
    std::vector<std::string> requiredExtensionNames_;
    std::vector<const char*> requiredExtensionNamesCStr_;

    // Instance.
    vk::UniqueInstance instance_;

    // Debug messenger.
    vk::UniqueDebugUtilsMessengerEXT debugMessenger_;

public:

    // Constructor.
    VulkanInstance(const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanInstance();

    // Copy/move constructor/assignment.
    VulkanInstance(const VulkanInstance& instance) = delete;
    VulkanInstance(VulkanInstance&& instance) noexcept = delete;
    VulkanInstance& operator=(const VulkanInstance& instance) = delete;
    VulkanInstance& operator=(VulkanInstance&& instance) noexcept = delete;

    // Returns Vulkan instance.
    vk::Instance Get() const;

    // Returns physical devices available to the instance.
    std::vector<vk::PhysicalDevice> GetAvailablePhysicalDevices() const;

private:

    // Checks if the Vulkan loader has a sufficiently high version.
    void CheckLoaderVersion() const;

    // Retrieves all available layers.
    void RetrieveAvailableLayers();
    // Checks if every required layer is available.
    bool IsEveryRequiredLayerAvailable() const;

    // Retrieves all available extensions.
    void RetrieveAvailableExtensions();
    // Retrieves extensions required by GLFW.
    void RetrieveGLFWRequiredExtensions();
    // Checks if every required extension is available.
    bool IsEveryRequiredExtensionAvailable() const;

    // Debug messenger callback. It is used to display the validation layer messages.
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessengerCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, [[maybe_unused]] vk::DebugUtilsMessageTypeFlagsEXT messageType, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

    // Creates instance.
    void CreateInstance();

    // Creates standalone debug messenger.
    void CreateDebugMessenger();
};
