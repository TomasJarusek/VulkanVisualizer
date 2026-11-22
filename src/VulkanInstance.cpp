
#include "VulkanInstance.h"

#include "Configuration/Validation.hpp"

#include "VulkanConfig.h"


VulkanInstance::VulkanInstance(const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_),
    applicationName_(visualizerConfig.instanceConfig_.applicationName_),
    minimumVulkanVersion_(visualizerConfig.instanceConfig_.minimumVulkanVersion_),
    showMessengerInfoMessages_(visualizerConfig.instanceConfig_.showMessengerInfoMessages_),
    showMessengerVerboseMessages_(visualizerConfig.instanceConfig_.showMessengerVerboseMessages_),
    showAvailableLayersAndExtensions_(visualizerConfig.instanceConfig_.showAvailableLayersAndExtensions_),
    enableValidationLayers_(visualizerConfig.instanceConfig_.enableValidationLayers_),
    useLayerSettings_(visualizerConfig.instanceConfig_.useLayerSettings_)
{
    LOG_DBG(this->logger_) << "Creating instance. "
        << "applicationName <" << this->applicationName_ << ">, "
        << "minimumVulkanVersion <" << std::to_string(vk::apiVersionMajor(this->minimumVulkanVersion_)) + "." + std::to_string(vk::apiVersionMinor(this->minimumVulkanVersion_)) + "." + std::to_string(vk::apiVersionPatch(this->minimumVulkanVersion_)) << ">, "
        << "showMessengerInfoMessages <" << std::format("{}", this->showMessengerInfoMessages_) << ">, "
        << "showMessengerVerboseMessages <" << std::format("{}", this->showMessengerVerboseMessages_) << ">, "
        << "showAvailableLayersAndExtensions <" << std::format("{}", this->showAvailableLayersAndExtensions_)<< ">, "
        << "enableValidationLayers <" << std::format("{}", this->enableValidationLayers_) << ">, "
        << "useLayerSettings <" << std::format("{}", this->useLayerSettings_) << ">" << std::endl;

    this->CheckLoaderVersion();

    this->RetrieveAvailableLayers();
    this->RetrieveAvailableExtensions();

    this->CreateInstance();

    this->CreateDebugMessenger();

    LOG_DBG(this->logger_) << "Instance created." << std::endl;
}

VulkanInstance::~VulkanInstance()
{
    LOG_DBG(this->logger_) << "Instance destroyed." << std::endl;
}

vk::Instance VulkanInstance::Get() const
{
    return this->instance_.get();
}

std::vector<vk::PhysicalDevice> VulkanInstance::GetAvailablePhysicalDevices() const
{
    return this->instance_.get().enumeratePhysicalDevices();
}

void VulkanInstance::CheckLoaderVersion() const
{
    if (vk::enumerateInstanceVersion() < this->minimumVulkanVersion_)
    {
        throw std::runtime_error("Exception: Loader does not support the required Vulkan version.");
    }
}

void VulkanInstance::RetrieveAvailableLayers()
{
   this->availableLayers_ = vk::enumerateInstanceLayerProperties();

   if (this->showAvailableLayersAndExtensions_ == true)
   {
       LOG_INF(this->logger_) << "Available layers:" << std::endl;

       for (const vk::LayerProperties& availableLayer : this->availableLayers_)
       {
           LOG_INF(this->logger_) << "  " << availableLayer.layerName << ", "
               << availableLayer.specVersion << ", "
               << availableLayer.implementationVersion << ", "
               << availableLayer.description << std::endl;
       }
   }
}

bool VulkanInstance::IsEveryRequiredLayerAvailable() const
{
    for (const std::string& layerName: this->requiredLayerNames_)
    {
        if (std::ranges::find_if(this->availableLayers_, [layerName](const auto& layer){return layer.layerName == layerName;}) == this->availableLayers_.end())
        {
            LOG_ERR(this->logger_) << "Required layer <"
                << layerName
                << "> wasn't found." << std::endl;

            return false;
        }
    }

    return true;
}

void VulkanInstance::RetrieveAvailableExtensions()
{
    this->availableExtensions_ = vk::enumerateInstanceExtensionProperties();

    if (this->showAvailableLayersAndExtensions_ == true)
    {
        LOG_INF(this->logger_) << "Available extensions:" << std::endl;
        for (const vk::ExtensionProperties& availableExtension : this->availableExtensions_)
        {
            LOG_INF(this->logger_) << "  " << availableExtension.extensionName << ", "
                << availableExtension.specVersion  << std::endl;
        }
    }
}

void VulkanInstance::RetrieveGLFWRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensionNames = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    for (uint32_t i = 0; i < glfwExtensionCount; ++i)
    {
        if (std::find(this->requiredExtensionNames_.begin(), this->requiredExtensionNames_.end(), glfwExtensionNames[i]) == this->requiredExtensionNames_.end())
        {
            this->requiredExtensionNames_.emplace_back(glfwExtensionNames[i]);
        }

        LOG_DBG(this->logger_) << "Extension <" << glfwExtensionNames[i] << "> is required by GLFW." << std::endl;
    }
}

bool VulkanInstance::IsEveryRequiredExtensionAvailable() const
{
    for (const std::string& extensionName: this->requiredExtensionNames_)
    {
        if (std::ranges::find_if(this->availableExtensions_, [extensionName](const auto& extension){return extension.extensionName == extensionName;}) == this->availableExtensions_.end())
        {
            LOG_ERR(this->logger_) << "Required extension <"
                << extensionName
                << "> wasn't found." << std::endl;

            return false;
        }
    }
    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanInstance::DebugMessengerCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity, [[maybe_unused]] vk::DebugUtilsMessageTypeFlagsEXT messageType, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    const VulkanInstance& instance = *(reinterpret_cast<VulkanInstance*>(pUserData));
    const Logger& logger = instance.logger_;

    // BestPractices-PipelineBarrier-unneeded-QFOT
    std::array<int32_t, 1> suppressedMessages = {1402469115};

    // Callback handling is skipped if a suppressed message is detected.
    if (std::ranges::find(suppressedMessages, pCallbackData->messageIdNumber) != suppressedMessages.end())
    {
        return VK_FALSE;
    }

    if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eError)
    {
        LOG_ERR(logger) << "Validation layer: " << std::endl;
        LOG_ERR(logger) << pCallbackData->pMessage << std::endl;

        if (pCallbackData->objectCount > 0 && (pCallbackData->pObjects)[0].pObjectName != nullptr)
        {
            LOG_ERR(logger) << "Related Vulkan objects: " << std::endl;
            for (size_t i = 0; i < pCallbackData->objectCount; ++i)
            {
                if ((pCallbackData->pObjects)[i].pObjectName != nullptr)
                {
                    LOG_ERR(logger) << "  " << (pCallbackData->pObjects)[i].pObjectName << std::endl;
                }
            }
        }

        LOG_ERR(logger) << std::endl;
    }
    else if (messageSeverity & vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        LOG_WRN(logger) << "Validation layer: " << std::endl;
        LOG_WRN(logger) << pCallbackData->pMessage << std::endl;

        if (pCallbackData->objectCount > 0 && (pCallbackData->pObjects)[0].pObjectName != nullptr)
        {
            LOG_WRN(logger) << "Related Vulkan objects: " << std::endl;
            for (size_t i = 0; i < pCallbackData->objectCount; ++i)
            {
                if ((pCallbackData->pObjects)[i].pObjectName != nullptr)
                {
                    LOG_WRN(logger) << "  " << (pCallbackData->pObjects)[i].pObjectName << std::endl;
                }
            }
        }

        LOG_WRN(logger) << std::endl;
    }
    else
    {
        LOG_INF(logger) << "Validation layer: " << std::endl;
        LOG_INF(logger) << pCallbackData->pMessage << std::endl;

        if (pCallbackData->objectCount > 0 && (pCallbackData->pObjects)[0].pObjectName != nullptr)
        {
            LOG_INF(logger) << "Related Vulkan objects: " << std::endl;
            for (size_t i = 0; i < pCallbackData->objectCount; ++i)
            {
                if ((pCallbackData->pObjects)[i].pObjectName != nullptr)
                {
                    LOG_INF(logger) << "  " << (pCallbackData->pObjects)[i].pObjectName << std::endl;
                }
            }
        }

        LOG_INF(logger) << std::endl;
    }

    // VK_FALSE means do not end execution.
    return VK_FALSE;
}

void VulkanInstance::CreateInstance()
{
    // Required layers.

    if (this->enableValidationLayers_ == true)
    {
        // Validation layer functionality.
        const std::string requiredLayerName = "VK_LAYER_KHRONOS_validation";

        if (std::find(this->requiredLayerNames_.begin(), this->requiredLayerNames_.end(), requiredLayerName) == this->requiredLayerNames_.end())
        {
            this->requiredLayerNames_.push_back(requiredLayerName);       
        }

        LOG_DBG(this->logger_) << "Layer <" << requiredLayerName << "> is required by validation layers." << std::endl;
    }

    // If there are any required layers, check if they are all available.
    if (this->requiredLayerNames_.empty() == false)
    {
        if (this->IsEveryRequiredLayerAvailable() == false)
        {
            throw std::runtime_error("Exception: One or more required layers aren't available.");
        }
    }

    std::transform(this->requiredLayerNames_.begin(), this->requiredLayerNames_.end(), std::back_inserter(this->requiredLayerNamesCStr_), [](const std::string& string){return string.c_str();});


    // Required extensions.

    this->RetrieveGLFWRequiredExtensions();

    if (this->enableValidationLayers_ == true)
    {
        // Provides debug callback API to receive validation messages.
        const std::string requiredExtensionName = vk::EXTDebugUtilsExtensionName;

        if (std::find(this->requiredExtensionNames_.begin(), this->requiredExtensionNames_.end(), requiredExtensionName) == this->requiredExtensionNames_.end())
        {
            this->requiredExtensionNames_.push_back(requiredExtensionName);
        }

        LOG_DBG(this->logger_) << "Extension <" << requiredExtensionName << "> is required by validation layers." << std::endl;
    }

    // If there are any required extensions, check if they are all available.
    if (this->requiredExtensionNames_.empty() == false)
    {
        if (this->IsEveryRequiredExtensionAvailable() == false)
        {
            throw std::runtime_error("Exception: One or more required extensions aren't available.");
        }
    }

    std::transform(this->requiredExtensionNames_.begin(), this->requiredExtensionNames_.end(), std::back_inserter(this->requiredExtensionNamesCStr_), [](const std::string& string){return string.c_str();});


    // Instance creation.

    vk::ApplicationInfo applicationInfo{};

    applicationInfo
        .setPApplicationName(this->applicationName_.c_str())
        .setApplicationVersion(vk::makeVersion(1, 0, 0))
        .setApiVersion(this->minimumVulkanVersion_);


    vk::InstanceCreateInfo instanceCreateInfo{};

    instanceCreateInfo
        .setPApplicationInfo(&applicationInfo)
        .setPEnabledLayerNames(this->requiredLayerNamesCStr_)
        .setPEnabledExtensionNames(this->requiredExtensionNamesCStr_);

    // Debug messenger that will be used only during an instance creation. 
    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};

    // Layer settings.
    LayerSettings layerSettingsBase;
    std::vector<vk::LayerSettingEXT> layerSettings;
    vk::LayerSettingsCreateInfoEXT layerSettingsCreateInfo{};

    if (this->enableValidationLayers_ == true)
    {
        debugMessengerCreateInfo
            .setMessageSeverity(
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                (this->showMessengerInfoMessages_ == true ? vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo : vk::DebugUtilsMessageSeverityFlagBitsEXT{}) |
                (this->showMessengerVerboseMessages_ == true ? vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose : vk::DebugUtilsMessageSeverityFlagBitsEXT{}))
            .setMessageType(
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
            .setPfnUserCallback(DebugMessengerCallback)
            .setPUserData(this);

        // instanceCreateInfo -> debugMessengerCreateInfo
        instanceCreateInfo
            .setPNext(&debugMessengerCreateInfo);

        if (this->useLayerSettings_ == true)
        {
            // Layer settings can be set up in several ways.
            // https://vulkan.lunarg.com/doc/view/latest/windows/layer_configuration.html
            // https://github.com/KhronosGroup/Vulkan-Utility-Libraries/blob/main/docs/Configuring-Vulkan-Layers-Whitepaper.pdf
            // My goal was to enable best practices and synchronization validation.
            // 1) Vulkan Configurator app. Works when the app is running. Intercepts any Vulkan program.
            // Temporary files in appdata folder are generated.
            // "...\AppData\Local\LunarG\vulkan\vk_loader_settings.json
            // "...\AppData\Local\LunarG\vkconfig\override\vk_layer_settings.txt
            // 2) Environment variables. I did not try that.
            // 3) Vulkan API. Validation.hpp file was generated using the Vulkan Configurator (right click on "Validation" layer).
            // Then the settings can be manually loaded during an instance creation.

            // I've also tried using vk::ValidationFeaturesEXT with vk::ValidationFeatureEnableEXT, but I am not sure how exactly that works.
            // It enabled the features, but it also enabled GPU Assisted validation for some reason even though I did not set it explicitly.

            layerSettings = layerSettingsBase.settings();

            layerSettingsCreateInfo
                .setSettings(layerSettings);

            // instanceCreateInfo -> debugMessengerCreateInfo -> layerSettingsCreateInfo
            debugMessengerCreateInfo
                .setPNext(&layerSettingsCreateInfo);
        }
    }

    this->instance_ = vk::createInstanceUnique(instanceCreateInfo);

    // Initialization of instance related function pointers of Vulkan functions.
    // This also initializes all functions that are part of enabled extensions
    // like for example the vk::createDebugUtilsMessengerEXT from vk::EXTDebugUtilsExtensionName.
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*(this->instance_));
}

void VulkanInstance::CreateDebugMessenger()
{
    if (this->enableValidationLayers_ == true)
    {
        // Setup of a debug messenger that will be used for the rest of the application's lifetime.
        vk::DebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo{};

        debugMessengerCreateInfo
            .setMessageSeverity(
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                (this->showMessengerInfoMessages_ == true ? vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo : vk::DebugUtilsMessageSeverityFlagBitsEXT{}) |
                (this->showMessengerVerboseMessages_ == true ? vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose : vk::DebugUtilsMessageSeverityFlagBitsEXT{}))
            .setMessageType(
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
            .setPfnUserCallback(DebugMessengerCallback)
            .setPUserData(this);

        this->debugMessenger_ = this->instance_->createDebugUtilsMessengerEXTUnique(debugMessengerCreateInfo);
    }
}
