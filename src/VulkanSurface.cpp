
#include "VulkanSurface.h"

#include "VulkanConfig.h"
#include "VulkanWindow.h"
#include "VulkanInstance.h"


VulkanSurface::VulkanSurface(const VulkanWindow& window, const VulkanInstance& instance, const VulkanVisualizerConfig& visualizerConfig) :
    logger_(visualizerConfig.loggerConfig_.showMetaInformation_)
{
    LOG_DBG(this->logger_) << "Creating surface." << std::endl;

    this->CreateSurface(window, instance);

    LOG_DBG(this->logger_) << "Surface created." << std::endl;
}

VulkanSurface::~VulkanSurface()
{
    LOG_DBG(this->logger_) << "Surface destroyed." << std::endl;
}

vk::SurfaceKHR VulkanSurface::Get() const
{
    return this->surface_.get();
}

void VulkanSurface::CreateSurface(const VulkanWindow& window, const VulkanInstance& instance)
{
    VkSurfaceKHR rawSurface{};

    if (glfwCreateWindowSurface(instance.Get(), window.Get(), nullptr, &rawSurface) != VK_SUCCESS)
    {
        throw std::runtime_error("Exception: Surface could not be created.");
    }

    this->surface_ = vk::UniqueSurfaceKHR{rawSurface, instance.Get()};
}
