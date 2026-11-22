
#pragma once

#include <GLFW/glfw3.h>

#include "Logger.h"
#include "VulkanHelperEnums.h"


class VulkanVisualizerConfig;


class VulkanWindow
{
private:

    Logger logger_;

    // GLFW window struct.
    GLFWwindow* glfwWindow_ = nullptr;
    // GLFW window properties.
    std::string glfwWindowName_ = "Unnamed";
    VulkanWindowMode glfwWindowMode_ = VulkanWindowMode::Window;
    int glfwWindowWidth_ = 1280;
    int glfwWindowHeight_ = 720;
    int glfwWindowX_ = 100;
    int glfwWindowY_ = 100;

    // Framebuffer resized flag.
    bool framebufferResized_ = false;

public:

    // Constructor.
    VulkanWindow(const VulkanVisualizerConfig& visualizerConfig);
    // Destructor.
    ~VulkanWindow();

    // Copy/move constructor/assignment.
    VulkanWindow(const VulkanWindow& window) = delete;
    VulkanWindow(VulkanWindow&& window) noexcept = delete;
    VulkanWindow& operator=(const VulkanWindow& window) = delete;
    VulkanWindow& operator=(VulkanWindow&& window) noexcept = delete;

    // Returns GLFW window pointer.
    GLFWwindow* Get() const;

    // Sets window mode.
    void SetWindowMode(VulkanWindowMode windowMode, int windowWidth, int windowHeight, int windowX, int windowY);

    // Setter for the framebuffer resized flag.
    void SetFramebufferResized(bool framebufferResized);

    // Getter for the framebuffer resized flag.
    bool GetFramebufferResized() const;

    // Returns current window size.
    std::pair<int, int> GetWindowSize() const;
    // Returns current framebuffer size.
    std::pair<int, int> GetFramebufferSize() const;
    // Returns current window position.
    std::pair<int, int> GetWindowPosition() const;
    // Returns current primary monitor position.
    std::pair<int, int> GetPrimaryMonitorPosition() const;

    bool WindowShouldClose() const;

    void HandleMinimization() const;

private:

    std::pair<int, int> CalculateCenteredWindowCoordinates(int windowWidth, int windowHeight) const;

    // Initializes GLFW and creates window.
    void CreateGLFWWindow();

    // Destroys GLFW window and frees resources.
    void DestroyGLFWWindow();
};
