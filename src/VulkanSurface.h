
#pragma once

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.hpp>

#include "Logger.h"


class VulkanVisualizerConfig;
class VulkanWindow;
class VulkanInstance;


class VulkanSurface
{
private:

    Logger logger_;

    vk::UniqueSurfaceKHR surface_;

public:

    // Constructor.
    VulkanSurface(const VulkanWindow& window, const VulkanInstance& instance, const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanSurface();

    // Copy/move constructor/assignment.
    VulkanSurface(const VulkanSurface& surface) = delete;
    VulkanSurface(VulkanSurface&& surface) noexcept = delete;
    VulkanSurface& operator=(const VulkanSurface& surface) = delete;
    VulkanSurface& operator=(VulkanSurface&& surface) noexcept = delete;

    // Returns Vulkan surface.
    vk::SurfaceKHR Get() const;

private:

    // Creates surface.
    void CreateSurface(const VulkanWindow& window, const VulkanInstance& instance);
};
